section .multiboot_header
align 8
multiboot2_header:
    dd 0xE85250D6
    dd 0
    dd multiboot2_end - multiboot2_header
    dd -(0xE85250D6 + 0 + (multiboot2_end - multiboot2_header))

    dw 0
    dw 0
    dd 8

multiboot2_end:

section .text
global _start
extern kernel_main

_start:
    cli
    xor rbp, rbp
    mov rsp, stack_top
    call kernel_main
    hlt

section .bss
align 16
resb 4096
stack_top:
