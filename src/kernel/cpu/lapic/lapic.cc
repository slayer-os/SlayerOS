#include <cpu/lapic.h>
#include <cpu/pit.h>
#include <mem/paging.h>
#include <cpu/isr.h>
#include <dbg/log.h>

#define LAPIC_TIMER_LVT 0x320
#define LAPIC_TIMER_INITIAL_COUNT 0x380
#define LAPIC_TIMER_CURRENT_COUNT 0x390
#define LAPIC_TIMER_DIVIDE_CONFIG 0x3E0

static u64 apic_timer_frequency = 0;
static u32 apic_timer_ticks_per_ms = 0;

inline void _wrmsr(u32 msr, u64 value) {
  u32 lo = (u32)(value & 0xFFFFFFFF); 
  u32 hi = (u32)(value >> 32);
  __asm__ __volatile__ ("wrmsr" : : "c"(msr), "a"(lo), "d"(hi));
}

inline u64 _rdmsr(u32 msr) {
  u32 lo, hi;
  __asm__ __volatile__ ("rdmsr" : "=a"(lo), "=d"(hi) : "c"(msr));
  return ((u64)hi << 32) | lo; 
}

u64 LAPIC::base() {
  return (u64)PHYS2VIRT(_rdmsr(0x1B) & 0xFFFFF000);
}

u32 LAPIC::read(u32 reg) {
  return *(volatile u32*)(LAPIC::base() + reg);
}

void LAPIC::write(u32 reg, u32 value) {
  *(volatile u32*)(LAPIC::base() + reg) = value;
}

u32 LAPIC::id() {
  return LAPIC::read(0x20) >> 24;
}

void LAPIC::eoi() {
  LAPIC::write(0xB0, 0);
}

void LAPIC::init() {
  u64 msr_val = _rdmsr(0x1B);
  _wrmsr(0x1B, msr_val | 0x800); // Enable LAPIC
  u64 phys_addr = msr_val & 0xFFFFF000;
  
  Mem::Paging::map((void*)phys_addr, PHYS2VIRT(phys_addr), Mem::Paging::PAGE_PRESENT | Mem::Paging::PAGE_WRITE);
  
  // Set spurious vector
  LAPIC::write(0xf0, 0xff | 0x100);
  
  Log::success("LAPIC initialized");
}

extern "C" void spurious_handler(isr_frame *) {
}

void LAPIC::Timer::init(u8 vector, Mode mode, Divider divider) {
  disable();
  set_divider(divider);
  
  u32 lvt_value = vector;
  
  switch (mode) {
    case ONE_SHOT:
      break;
    case PERIODIC:
      lvt_value |= (1 << 17);
      break;
    case TSC_DEADLINE:
      lvt_value |= (2 << 17);
      break;
  }
  
  LAPIC::write(LAPIC_TIMER_LVT, lvt_value);
  
  apic_timer_frequency = calibrate_frequency();
  apic_timer_ticks_per_ms = apic_timer_frequency / 1000;
  
  enable();
  
  Log::success("APIC Timer initialized - Frequency: %lld Hz", apic_timer_frequency);
}

void LAPIC::Timer::set_initial_count(u32 count) {
  LAPIC::write(LAPIC_TIMER_INITIAL_COUNT, count);
}

u32 LAPIC::Timer::get_current_count() {
  return LAPIC::read(LAPIC_TIMER_CURRENT_COUNT);
}

void LAPIC::Timer::set_divider(Divider divider) {
  LAPIC::write(LAPIC_TIMER_DIVIDE_CONFIG, divider);
}

void LAPIC::Timer::enable() {
  u32 lvt = LAPIC::read(LAPIC_TIMER_LVT);
  lvt &= ~(1 << 16);
  LAPIC::write(LAPIC_TIMER_LVT, lvt);
}

void LAPIC::Timer::disable() {
  u32 lvt = LAPIC::read(LAPIC_TIMER_LVT);
  lvt |= (1 << 16);
  LAPIC::write(LAPIC_TIMER_LVT, lvt);
}

void LAPIC::Timer::mask() {
  disable();
}

void LAPIC::Timer::unmask() {
  enable();
}

u64 LAPIC::Timer::calibrate_frequency() {
  const u32 CALIBRATION_MS = 10;
  
  set_divider(DIV_BY_1);
  
  u32 lvt_backup = LAPIC::read(LAPIC_TIMER_LVT);
  LAPIC::write(LAPIC_TIMER_LVT, (1 << 16));
  
  const u32 MAX_COUNT = 0xFFFFFFFF;
  set_initial_count(MAX_COUNT);
  
  u32 start_count = get_current_count();
  u64 start_pit_ticks = PIT::get_ticks();
  
  u64 target_pit_ticks = start_pit_ticks + CALIBRATION_MS;
  while (PIT::get_ticks() < target_pit_ticks) {
    __asm__ __volatile__("pause");
  }
  
  u32 end_count = get_current_count();
  u64 actual_pit_ticks = PIT::get_ticks() - start_pit_ticks;
  
  if (actual_pit_ticks == 0) {
    Log::warning("PIT ticks didn't advance during calibration");
    LAPIC::write(LAPIC_TIMER_LVT, lvt_backup);
    return 0;
  }
  
  u32 apic_ticks = start_count - end_count;
  u64 frequency = ((u64)apic_ticks * 1000) / actual_pit_ticks;
  
  Log::info("APIC Timer calibration: %d ticks in %lld ms = %lld Hz", 
            apic_ticks, actual_pit_ticks, frequency);
  
  LAPIC::write(LAPIC_TIMER_LVT, lvt_backup);
  
  return frequency;
}

void LAPIC::Timer::set_frequency(u32 target_hz) {
  if (apic_timer_frequency == 0) {
    Log::critical("APIC Timer not calibrated");
    return;
  }
  
  u32 divider_value = 1;
  u64 count = apic_timer_frequency / target_hz;
  
  if (count > 0xFFFFFFFF) {
    for (int i = 1; i <= 128; i *= 2) {
      count = apic_timer_frequency / (target_hz * i);
      if (count <= 0xFFFFFFFF) {
        divider_value = i;
        break;
      }
    }
  }
  
  if (count > 0xFFFFFFFF) {
    Log::warning("APIC Timer count too large even with max divider: %lld", count);
    count = 0xFFFFFFFF;
    divider_value = 128;
  }
  
  Divider div;
  switch (divider_value) {
    case 1: div = DIV_BY_1; break;
    case 2: div = DIV_BY_2; break;
    case 4: div = DIV_BY_4; break;
    case 8: div = DIV_BY_8; break;
    case 16: div = DIV_BY_16; break;
    case 32: div = DIV_BY_32; break;
    case 64: div = DIV_BY_64; break;
    case 128: div = DIV_BY_128; break;
    default: div = DIV_BY_1; break;
  }
  
  Log::info("APIC Timer config: freq=%lld Hz, target=%d Hz, div=%d, count=%lld", 
            apic_timer_frequency, target_hz, divider_value, count);
  
  disable();
  set_divider(div);
  set_initial_count((u32)count);
  
  u32 new_lvt = 0x21 | (1 << 17);
  LAPIC::write(LAPIC_TIMER_LVT, new_lvt);
  
  enable();
  
  Log::info("APIC Timer LVT configured: 0x%x", new_lvt);
  Log::info("Timer enabled - LVT after enable: 0x%x", LAPIC::read(LAPIC_TIMER_LVT));
  Log::info("Initial count set to: %d", (u32)count);
}
