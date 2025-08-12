.section .text
.global load_tss_register

load_tss_register:
    ltr %di
    ret
