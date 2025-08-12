#pragma once
#include <klib/types.h>

namespace TSS {
  struct [[gnu::packed]] TaskStateSegment {
    u32 reserved1;
    u64 rsp0;
    u64 rsp1;
    u64 rsp2;
    u64 reserved2;
    u64 ist1;
    u64 ist2;
    u64 ist3;
    u64 ist4;
    u64 ist5;
    u64 ist6;
    u64 ist7;
    u64 reserved3;
    u16 reserved4;
    u16 iopb_offset;
  };

  struct [[gnu::packed]] Descriptor {
    u16 limit_low;
    u16 base_low;
    u8  base_mid;
    u8  access;
    u8  granularity;
    u8  base_high;
    u32 base_ext;
    u32 reserved;
  };

  enum ISTIndex {
    IST_NONE = 0,
    IST_EXCEPTION = 1,
    IST_IRQ = 2,
  };

  void init();
  void init_stacks();
  void set_kernel_stack(u64 stack_ptr);
  u64 get_ist_stack(ISTIndex index);
  void load_tss();
}
