#pragma once
#include <klib/types.h>

namespace GDT {
  struct [[gnu::packed]] TablePtr {
    u16 limit;
    u64 base;
  };

  void init();
}
