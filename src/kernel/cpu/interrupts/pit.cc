#include <cpu/pit.h>
#include <klib/types.h>
#include <dbg/log.h>
#include <drivers/io.h>

#define PIT_FREQUENCY 1193182
#define PIT_COMMAND_PORT 0x43
#define PIT_DATA_PORT_0 0x40

static u64 ticks_since_boot = 0;
static u32 timer_frequency = 0;

void PIT::init(u32 frequency) {
    timer_frequency = frequency;
    PIT::set_frequency(frequency);
    Log::success("PIT timer initialized at %d Hz", frequency);
}

void PIT::set_frequency(u32 frequency) {
    u32 divisor = PIT_FREQUENCY / frequency;
    
    Drivers::IO::outb(PIT_COMMAND_PORT, 0x36);
    
    Drivers::IO::outb(PIT_DATA_PORT_0, (u8)(divisor & 0xFF));
    Drivers::IO::outb(PIT_DATA_PORT_0, (u8)((divisor >> 8) & 0xFF));
}

void PIT::tick() {
    ticks_since_boot++;
}

u64 PIT::get_ticks() {
    return ticks_since_boot;
}

u64 PIT::get_uptime_ms() {
    if (timer_frequency == 0) return 0;
    return (ticks_since_boot * 1000) / timer_frequency;
}

u64 PIT::get_uptime_us() {
    if (timer_frequency == 0) return 0;
    return (ticks_since_boot * 1000000) / timer_frequency;
}

void PIT::sleep_ms(u32 milliseconds) {
    if (timer_frequency == 0) return;
    u64 target_ticks = ticks_since_boot + (milliseconds * timer_frequency) / 1000;
    while (ticks_since_boot < target_ticks) {
        __asm__ __volatile__ ("hlt");
    }
}

void PIT::sleep_us(u32 microseconds) {
    if (timer_frequency == 0) return;
    u64 target_ticks = ticks_since_boot + (microseconds * timer_frequency) / 1000000;
    while (ticks_since_boot < target_ticks) {
        __asm__ __volatile__ ("hlt");
    }
}
