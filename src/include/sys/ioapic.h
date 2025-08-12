#pragma once
#include <klib/types.h>
#include <uacpi/acpi.h>
#include <uacpi/uacpi.h>
#include <uacpi/status.h>
#include <uacpi/tables.h>

namespace Sys::IOAPIC {
  void write(u64 base, u32 reg, u32 value);
  u32  read(u64 base, u32 reg);
  void set(u8 vec, u8 irq, u64 flags, u64 lapic);
  void dump_entries();
  void init();
}
