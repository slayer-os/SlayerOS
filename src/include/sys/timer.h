#pragma once
#include <klib/types.h>

namespace Sys {
namespace Timer {
  enum class Type {
    NONE,
    PIT,
    APIC_TIMER,
    HPET
  };

  bool init();
  
  Type get_active_timer();
  u64 get_frequency();
  const char* get_timer_name();
  
  u64 get_counter();
  u64 get_uptime_ns();
  u64 get_uptime_us();
  u64 get_uptime_ms();
  
  void sleep_ns(u64 nanoseconds);
  void sleep_us(u64 microseconds);
  void sleep_ms(u64 milliseconds);
  
  void setup_periodic_interrupt(u32 frequency_hz);
  void handle_timer_interrupt();
  
  bool supports_high_precision();
  u64 get_resolution_ns();
}
}
