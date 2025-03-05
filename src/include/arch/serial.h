#ifndef SERIAL_H
#define SERIAL_H
#include <libc/types.h>

#define COM1 0x3F8

void outb(u16 port, u8 data);
u8 inb(u16 port);
void io_wait();
namespace UART {
  void init();
  bool active();
  bool transmit_empty();
  char read();
  void write(char data);
}


#endif
