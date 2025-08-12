#include <arch/cpuid.h>
#include <klib/types.h>

extern "C" void cpuid(u32 eax_in, u32 ecx_in,
                      u32* eax, u32* ebx,
                      u32* ecx, u32* edx) {
    __asm__ __volatile__("cpuid"
                 : "=a"(*eax), "=b"(*ebx),
                   "=c"(*ecx), "=d"(*edx)
                 : "a"(eax_in), "c"(ecx_in));
}

const char *CPUID::cpu_vendor() {
  static char vendor[13];
  u32 eax, ebx, ecx, edx;
  cpuid(0, 0, &eax, &ebx, &ecx, &edx);
  ((u32*)vendor)[0] = ebx;
  ((u32*)vendor)[1] = edx;
  ((u32*)vendor)[2] = ecx;
  vendor[12] = '\0';
  return vendor;
}

const char *CPUID::cpu_brand() {
  static char brand[49];
  u32 eax, ebx, ecx, edx;
  for (int i = 0; i < 3; i++) {
      cpuid(0x80000002 + i, 0, &eax, &ebx, &ecx, &edx);
      ((u32*)brand)[i * 4 + 0] = eax;
      ((u32*)brand)[i * 4 + 1] = ebx;
      ((u32*)brand)[i * 4 + 2] = ecx;
      ((u32*)brand)[i * 4 + 3] = edx;
  }
  brand[48] = '\0';
  return brand;
}

u64 CPUID::tsc_freq() {
  u32 eax, ebx, ecx, edx;
  cpuid(0x15, 0, &eax, &ebx, &ecx, &edx);
  if (eax && ebx) {
    u64 base = ecx ? ecx : 1;
    return (u64)base * ebx / eax;
  }
  return 0; // fallback
}
