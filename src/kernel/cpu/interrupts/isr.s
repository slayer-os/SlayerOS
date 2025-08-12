.section .data
rax_saver:
  .quad 0x00

.section .text

.include "src/kernel/cpu/interrupts/macros.s"

.section .text

.extern isr_handler
.extern irq_handler
.extern syscall_handler

ISR_NOERR 0
ISR_NOERR 1
ISR_NOERR 2
ISR_NOERR 3
ISR_NOERR 4
ISR_NOERR 5
ISR_NOERR 6
ISR_NOERR 7
ISR_ERR 8
ISR_NOERR 9
ISR_ERR 10
ISR_ERR 11
ISR_ERR 12
ISR_ERR 13
ISR_ERR 14
ISR_NOERR 15
ISR_NOERR 16
ISR_NOERR 17
ISR_NOERR 18
ISR_NOERR 19
ISR_NOERR 20
ISR_NOERR 21
ISR_NOERR 22
ISR_NOERR 23
ISR_NOERR 24
ISR_NOERR 25
ISR_NOERR 26
ISR_NOERR 27
ISR_NOERR 28
ISR_NOERR 29
ISR_NOERR 30
ISR_NOERR 31
IRQ 32 0
IRQ 33 1
IRQ 34 2
IRQ 35 3
IRQ 36 4
IRQ 37 5
IRQ 38 6
IRQ 39 7
IRQ 40 8
IRQ 41 9
IRQ 42 10
IRQ 43 11
IRQ 44 12
IRQ 45 13
IRQ 46 14
IRQ 47 15
ISR_NOERR 0xF0
ISR_NOERR 0xFF

isr_common:
    call isr_handler
    popa
    iretq

irq_common:
    call irq_handler
    popairq
    iretq
