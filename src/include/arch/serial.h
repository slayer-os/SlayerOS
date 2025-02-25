#ifndef SERIAL_H
#define SERIAL_H
#include <libc/types.h>

#define COM1 0x3F8

void outb(u16 port, u8 data);
u8 inb(u16 port);
void serial_init();
bit b_serial_active();
bit b_transmit_empty();
char read_serial();
void write_serial(char data);
void io_wait();

#endif
