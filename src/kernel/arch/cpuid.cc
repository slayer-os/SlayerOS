#include <arch/cpuid.h>
#include <stdint.h>

extern "C" void cpuid(uint32_t eax_in, uint32_t ecx_in,
                      uint32_t* eax, uint32_t* ebx,
                      uint32_t* ecx, uint32_t* edx) {
    asm volatile("cpuid"
                 : "=a"(*eax), "=b"(*ebx),
                   "=c"(*ecx), "=d"(*edx)
                 : "a"(eax_in), "c"(ecx_in));
}

const char *CPUID::cpu_vendor() {
  static char vendor[13];
  uint32_t eax, ebx, ecx, edx;
  cpuid(0, 0, &eax, &ebx, &ecx, &edx);
  ((uint32_t*)vendor)[0] = ebx;
  ((uint32_t*)vendor)[1] = edx;
  ((uint32_t*)vendor)[2] = ecx;
  vendor[12] = '\0';
  return vendor;
}

const char *CPUID::cpu_brand() {
  static char brand[49];
  uint32_t eax, ebx, ecx, edx;
  for (int i = 0; i < 3; i++) {
      cpuid(0x80000002 + i, 0, &eax, &ebx, &ecx, &edx);
      ((uint32_t*)brand)[i * 4 + 0] = eax;
      ((uint32_t*)brand)[i * 4 + 1] = ebx;
      ((uint32_t*)brand)[i * 4 + 2] = ecx;
      ((uint32_t*)brand)[i * 4 + 3] = edx;
  }
  brand[48] = '\0';
  return brand;
}
