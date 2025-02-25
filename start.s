.section .multiboot_header
.align 8
multiboot2_header:
    .long 0xE85250D6
    .long 0
    .long multiboot2_end - multiboot2_header
    .long -(0xE85250D6 + 0 + (multiboot2_end - multiboot2_header))
    .word 0
    .word 0
    .long 8
multiboot2_end:

.section .text
.global _start
.extern kernel_main

_start:
    cli
    xor %rbp, %rbp
    mov $stack_top, %rsp
    call kernel_main
    hlt

.section .bss
.align 16
.lcomm stack_bottom, 4096
stack_top: