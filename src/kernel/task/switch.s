.intel_syntax noprefix
.global task_switch

# rdi = current context
# rsi = next context

task_switch:
    mov [rdi + 0x78], rsp
    lea rax, [rip + 1f]
    mov [rdi + 0x50], rax
    
    mov rsp, [rsi + 0x88]
    mov rax, [rsi + 0x78]
    
    jmp rax

1:
    mov rsp, [rdi + 0x78]
    ret
