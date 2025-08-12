#include <arch/sse.h>

bool __attribute__((naked)) SSE::enable() {
__asm__ __volatile__(
    "push %rbx\n"
    "mov %cs, %ax\n"
    "and $3, %ax\n"
    "cmp $0, %ax\n"
    "jne fl\n"
    
    "mov $1, %eax\n"
    "cpuid\n"
    "bt $25, %edx\n"
    "jnc fl\n"

    "test $1, %ecx\n"
    "jz fl\n"
    "mov %ecx, %ebx\n"
    "test $(1 << 19), %ebx\n"
    "jz fl\n"
    "test $(1 << 20), %ebx\n"
    "jz fl\n"
    
    "mov %cr0, %rax\n"
    "and $~(1<<2), %rax\n"
    "or $(1<<1), %rax\n"
    "mov %rax, %cr0\n"

    "mov %cr4, %rax\n"
    "or $((1<<9) | (1<<10)), %rax\n"
    "mov %rax, %cr4\n"
    "xor %eax, %eax\n"
    "mov $1, %al\n"
    "pop %rbx\n"
    "ret\n"


    "fl:\n"
    "xor %eax, %eax\n"
    "pop %rbx\n"
    "ret\n"
  );
}

bool SSE::test() {
  unsigned int eax, ebx, ecx, edx;
  __asm__ __volatile__("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"(1));
  return edx & (1 << 25);
}
