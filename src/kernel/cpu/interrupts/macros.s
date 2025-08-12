.macro pusha
pushq %r15
pushq %r14
pushq %r13
pushq %r12
pushq %r11
pushq %r10
pushq %r9
pushq %r8
pushq %rbp
pushq %rsp
pushq %rdi
pushq %rsi
pushq %rdx
pushq %rcx
pushq %rbx
pushq %rax
lea (%rip), %rax
pushq %rax
mov %cr8, %rax
pushq %rax
mov %cr4, %rax
pushq %rax
mov %cr3, %rax
pushq %rax
mov %cr2, %rax
pushq %rax
mov %cr0, %rax
pushq %rax
mov %ss, %rax
pushq %rax
mov %cs, %rax
pushq %rax
mov %ds, %rax
pushq %rax
mov %es, %rax
pushq %rax
mov %fs, %rax
pushq %rax
mov %gs, %rax
pushq %rax
.endm

.macro popa
popq %rax # Ret address
popq %rax
popq %rax
popq %rax
popq %rax
popq %rax
popq %rax
popq %rax
popq %rax
popq %rax
popq %rax
popq %rax
popq %rax
popq %rax
popq %rbx
popq %rcx
popq %rdx
popq %rsi
popq %rdi
popq %rsp
popq %rbp
popq %r8
popq %r9
popq %r10
popq %r11
popq %r12
popq %r13
popq %r14
popq %r15
popq %r15 # int num
popq %r15 # err code
.endm

.macro save_ra
mov 240(%rsp), %rax
pushq %rax
.endm

.macro pushasysc
pushq %r9
pushq %r8
pushq %rcx
pushq %rdx
pushq %rsi
pushq %rdi
pushq %rax
.endm

.macro popasysc
popq %rdi
popq %rdi
popq %rsi
popq %rdx
popq %rcx
popq %r8
popq %r9
.endm

.macro spushq reg
  movq \reg, %rax
  pushq %rax
.endm

.macro spopq reg
  popq %rax
  movq %rax, \reg
.endm

.macro pushairq num irq 
pushq \irq
pushq %rax
pushq %rbx
pushq %rcx
pushq %rdx
pushq %rdi
pushq %rsi
pushq %rbp
pushq %r8
pushq %r9
pushq %r10
pushq %r11
pushq %r12
pushq %r13
pushq %r14
pushq %r15
.endm

.macro popairq
popq %r15
popq %r14
popq %r13
popq %r12
popq %r11
popq %r10
popq %r9
popq %r8
popq %rbp
popq %rsi
popq %rdi
popq %rdx
popq %rcx
popq %rbx
popq %rax
add $8, %rsp  # Skip the IRQ number
.endm


.macro ISR_NOERR num
.global isr\num
isr\num:
    pushq $0
    pushq $\num
    pusha
    save_ra
    mov %rsp, %rdi
    jmp isr_common
.endm

.macro ISR_ERR num
.global isr\num
isr\num:
    pushq $\num
    pusha
    save_ra
    mov %rsp, %rdi
    jmp isr_common
.endm

.macro IRQ num irq
.global isr\num
isr\num:
    cli
    pushairq $\num $\irq
    mov %rsp, %rdi
    jmp irq_common
.endm

.macro SWSYSCALL num
.global isr\num
isr\num:
    cli
    pushasysc
    mov %rsp, %rdi
    jmp sysc_common
.endm
