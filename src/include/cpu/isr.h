#pragma once
#include <klib/types.h>

#define ISR(x) void isr##x();

extern "C" {
ISR(0);
ISR(1);
ISR(2);
ISR(3);
ISR(4);
ISR(5);
ISR(6);
ISR(7);
ISR(8);
ISR(9);
ISR(10);
ISR(11);
ISR(12);
ISR(13);
ISR(14);
ISR(15);
ISR(16);
ISR(17);
ISR(18);
ISR(19);
ISR(20);
ISR(21);
ISR(22);
ISR(23);
ISR(24);
ISR(25);
ISR(26);
ISR(27);
ISR(28);
ISR(29);
ISR(30);
ISR(31);
ISR(32);
ISR(33);
ISR(34);
ISR(35);
ISR(36);
ISR(37);
ISR(38);
ISR(39);
ISR(40);
ISR(41);
ISR(42);
ISR(43);
ISR(44);
ISR(45);
ISR(46);
ISR(47);
ISR(0xF0);
ISR(0xFF);
}

struct isr_frame {
    u64 return_address;
    u64 gs, fs, es, ds, cs, ss;
    u64 cr0, cr2, cr3, cr4, cr8;
    u64 rip;
    u64 rax, rbx, rcx, rdx, rsi, rdi, rsp, rbp, r8, r9, r10, r11, r12, r13, r14, r15;
    u64 int_num;
    u64 error_code;
} __attribute__((packed));

