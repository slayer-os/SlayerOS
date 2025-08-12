#include <sys/timer.h>
#include <cpu/pit.h>
#include <cpu/irq.h>
#include <dbg/log.h>

static Sys::Timer::Type active_timer = Sys::Timer::Type::NONE;
static u64 timer_frequency = 0;
static const u32 PIT_TARGET_FREQUENCY = 1000; // 1000 Hz = 1ms intervals


bool Sys::Timer::init() {
  PIT::init(PIT_TARGET_FREQUENCY);
  active_timer = Sys::Timer::Type::PIT;
  timer_frequency = PIT_TARGET_FREQUENCY;
  
  return true;
}

Sys::Timer::Type Sys::Timer::get_active_timer() {
  return active_timer;
}

u64 Sys::Timer::get_frequency() {
  return timer_frequency;
}


const char *Sys::Timer::get_timer_name() {
  switch (active_timer) {
    case Sys::Timer::Type::PIT:
      return "PIT";
    case Sys::Timer::Type::APIC_TIMER:
      return "APIC";
    case Sys::Timer::Type::HPET:
      return "HPET";
  }
  return "None";
}

u64 Sys::Timer::get_counter() {
  switch (active_timer) {
    case Sys::Timer::Type::PIT:
      return PIT::get_ticks();
  }
  return 0;
}

u64 Sys::Timer::get_uptime_ns() {
  switch (active_timer) {
    case Sys::Timer::Type::PIT:
      return PIT::get_uptime_us() * 1000;
  }
  return 0;
}

u64 Sys::Timer::get_uptime_us() {
  switch (active_timer) {
    case Sys::Timer::Type::PIT:
      return PIT::get_uptime_us();
  }
  return 0;
}

u64 Sys::Timer::get_uptime_ms() {
  switch (active_timer) {
    case Sys::Timer::Type::PIT:
      return PIT::get_uptime_ms();
  }
  return 0;
}

void Sys::Timer::sleep_ns(u64 nanoseconds) {
  u64 microseconds = (nanoseconds + 999) / 1000;
  if (microseconds == 0) microseconds = 1; // minimum 1 us
  switch (active_timer) {
    case Sys::Timer::Type::PIT:
      PIT::sleep_us(microseconds);
      break;
  }
}

void Sys::Timer::sleep_us(u64 microseconds) {
  switch (active_timer) {
    case Sys::Timer::Type::PIT:
      PIT::sleep_us(microseconds);
      break;
  }
}

void Sys::Timer::sleep_ms(u64 milliseconds) {
  switch (active_timer) {
    case Sys::Timer::Type::PIT:
      PIT::sleep_ms(milliseconds);
      break;
  }
}


