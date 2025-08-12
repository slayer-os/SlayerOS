#pragma once
#include <klib/types.h>

namespace IDT {
  constexpr u8 INT_GATE  = 0x8E;
  constexpr u8 TRAP_GATE = 0x8F;

  struct [[gnu::packed]] Table {
    u16 size;
    u64 offset;
  };

  struct [[gnu::packed]] Gate {
    u16 offset_low;
    u16 selector;
    u8 ist;
    u8 type_attr;
    u16 offset_mid;
    u32 offset_high;
    u32 zero;
  };

  void register_entry(u8 index, u64 offset, u16 selector, u8 type);
  void register_entry_with_ist(u8 index, u64 offset, u16 selector, u8 type, u8 ist);
  void init();
}
