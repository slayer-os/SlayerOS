#pragma once
#include <klib/types.h>
#include <cpu/isr.h>

struct irq_frame {
  u64 r15, r14, r13, r12, r11, r10, r9, r8;
  u64 rbp, rsi, rdi, rdx, rcx, rbx, rax;
  u64 error;
  u64 rip;
  u64 cs;
  u64 rflags;
  u64 rsp;
  u64 ss;
};

namespace IRQ {
  void register_entry(u8 irq, void (*handler)(struct irq_frame*));
  void init();
}
