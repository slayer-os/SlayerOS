#pragma once
#include <klib/types.h>

namespace PIT {
  void init(u32 frequency);
  void set_frequency(u32 frequency);
  void tick();
  u64 get_ticks();
  u64 get_uptime_ms();
  u64 get_uptime_us();
  void sleep_ms(u32 milliseconds);
  void sleep_us(u32 microseconds);
}
