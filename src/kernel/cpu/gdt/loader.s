reload_cs:
   movw $0x10, %ax
   movw %ax, %ds
   movw %ax, %es
   movw %ax, %fs
   movw %ax, %gs
   movw %ax, %ss
   ret

.global load_gdt
.global reload_cs_segments
load_gdt:
   lgdt (%rdi)
   ret
reload_cs_segments:
   push $0x08
   lea reload_cs(%rip), %rax
   push %rax
   retfq


