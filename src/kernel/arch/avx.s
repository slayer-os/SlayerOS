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

.global __enable_sse
__enable_sse:
  mov eax, 0x1
  cpuid
  test edx, 1<<25
  jnz _sse_supported
  
  xor rax, rax
  jmp done

_sse_supported:
  mov rax, cr0
  and ax, 0xFFFB		# clear coprocessor emulation CR0.EM
  or ax, 0x2			  # set coprocessor monitoring  CR0.MP
  mov cr0, rax
  mov rax, cr4
  or ax, 3 << 9		  # set CR4.OSFXSR and CR4.OSXMMEXCPT at the same time
  mov cr4, rax
  ret

done:
  ret

