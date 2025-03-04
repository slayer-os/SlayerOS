.intel_syntax noprefix

.global __enable_sse_ext
__enable_sse_ext:
  # --- Check for kernel mode ---
  mov sil, 0             # SSE version (0 = not supported)
  mov ax, cs
  and ax, 3             # extract CPL from CS (lower 2 bits)
  cmp ax, 0
  jne not_kernel        # if not zero, not in kernel mode

  # --- Use CPUID function 1 to check SSE, SSE2, SSE3, SSE4.1, and SSE4.2 ---
  mov eax, 1
  cpuid
  # Check SSE (EDX bit 25)
  bt edx, 25
  jc sse_ok             # if carry set, SSE is present
  jmp not_supported

sse_ok:
  # (ECX bits from CPUID function 1:)
  # Bit 0: SSE3, Bit 19: SSE4.1, Bit 20: SSE4.2
  test ecx, 1           # SSE3
  jz not_supported
  mov ebx, ecx          # preserve a copy for further tests
  test ebx, (1 << 19)   # SSE4.1
  jz not_supported
  test ecx, (1 << 20)   # SSE4.2
  jz not_supported
  add sil, 1            # SSE, SSE3, SSE4.1, and SSE4.2 supported

  # --- Enable SSE in hardware ---
  # Clear CR0.EM (bit2) and set CR0.MP (bit1)
  mov rax, cr0
  and rax, ~(1 << 2)    # clear EM
  or  rax, (1 << 1)     # set MP
  mov cr0, rax

  # Set CR4.OSFXSR (bit 9) and CR4.OSXMMEXCPT (bit 10)
  mov rax, cr4
  or  rax, ((1 << 9) | (1 << 10))
  mov cr4, rax
  add sil, 1             # SSE supported

  # --- Check SSE2 support (EDX bit 26) ---
  bt edx, 26
  jc sse2_ok
  jmp not_supported
  add sil, 1           # SSE2 supported

sse2_ok:
  # --- Optionally, check for additional extensions ---
  # Check for "SSE5":
  mov eax, 0x80000001
  cpuid
  # XOP is typically indicated in ECX bit 11 (may vary by CPU)
  bt ecx, 11
  jc sse5_ok
  jmp not_supported
  add sil, 1           # XOP supported

sse5_ok:
  # SSE, SSE2, SSE3, SSE4.1, SSE4.2, and (optionally) XOP ("SSE5").
  jmp done

not_kernel:
not_supported:

done:
  xor eax, eax
  mov al, sil
  ret

.global __test_sse
__test_sse:
  mov eax, 1
  movd xmm0, eax
  addss xmm0, xmm0
  movd eax, xmm0
  cmp eax, 2
  sete al
  ret
