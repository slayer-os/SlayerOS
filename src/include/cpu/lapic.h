#pragma once
#include <klib/types.h>
#include <cpu/isr.h>

namespace LAPIC {
  u64  base();
  u32  read(u32 reg);
  void write(u32 reg, u32 value);
  u32  id();
  void eoi();
  void init();
  
  namespace Timer {
    enum Mode {
      ONE_SHOT = 0,
      PERIODIC = 1,
      TSC_DEADLINE = 2
    };
    
    enum Divider {
      DIV_BY_2 = 0b0000,
      DIV_BY_4 = 0b0001,
      DIV_BY_8 = 0b0010,
      DIV_BY_16 = 0b0011,
      DIV_BY_32 = 0b1000,
      DIV_BY_64 = 0b1001,
      DIV_BY_128 = 0b1010,
      DIV_BY_1 = 0b1011
    };
    
    void init(u8 vector, Mode mode = PERIODIC, Divider divider = DIV_BY_1);
    void set_initial_count(u32 count);
    u32 get_current_count();
    void set_divider(Divider divider);
    void enable();
    void disable();
    void mask();
    void unmask();
    u64 calibrate_frequency();
    void set_frequency(u32 target_hz);
  }
}

extern "C" void spurious_handler(isr_frame *f);
