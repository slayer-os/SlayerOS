.intel_syntax noprefix
.section .data


.align 32
vec_x:
    .long 0x3F800000   # 1.0
    .long 0x40000000   # 2.0
    .long 0x40400000   # 3.0
    .long 0x40800000   # 4.0

vec_y:
    .long 0x40A00000   # 5.0
    .long 0x40C00000   # 6.0
    .long 0x40E00000   # 7.0
    .long 0x41000000   # 8.0
    .long 0x41100000   # 9.0
    .long 0x41200000   # 10.0
    .long 0x41300000   # 11.0
    .long 0x41400000   # 12.0

.section .text

.global __enable_avx
__enable_avx:
  # OSXSAVE => CPUID w EAX=1, check ECX bit 27.
  mov eax, 1
  cpuid
  bt ecx, 27
  jc osxsave_supported
  # OSXSAVE not supported
  xor al, al
  jmp done

osxsave_supported:
  # Enable FPU/SSE:
  mov rax, cr0
  and rax, ~(1 << 2)
  or  rax, (1 << 1)
  mov cr0, rax

  mov rax, cr4
  or  rax, (1 << 9)     # OSFXSR (bit 9)
  or  rax, (1 << 10)    # OSXMMEXCPT (bit 10)
  mov cr4, rax

  xor ecx, ecx
  mov eax, 7            # Bits: bit0 (x87), bit1 (SSE), bit2 (AVX) => 0b111 = 7.
  xor edx, edx
  xsetbv

testing_avx:
  vzeroall
  vmovups xmm0, XMMWORD PTR [rip+vec_x]
  vinsertf128 ymm0, ymm0, xmm0, 0
  vmovups ymm1, YMMWORD PTR [rip+vec_y]
  vaddps ymm0, ymm0, ymm1
  mov al, 1

done:
  ret

