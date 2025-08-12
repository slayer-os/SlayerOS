#pragma once
#include <klib/types.h>

namespace CPUID {
  const char *cpu_vendor();
  const char *cpu_brand();
  u64 tsc_freq();
}
