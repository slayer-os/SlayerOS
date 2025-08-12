#include <arch/serial.h>
#include <drivers/io.h>


void UART::init() {
  Drivers::IO::outb(COM1 + 1, 0x00);
  Drivers::IO::outb(COM1 + 3, 0x80);
  Drivers::IO::outb(COM1 + 0, 0x03);
  Drivers::IO::outb(COM1 + 1, 0x00);
  Drivers::IO::outb(COM1 + 3, 0x03);
  Drivers::IO::outb(COM1 + 2, 0xC7);
  Drivers::IO::outb(COM1 + 4, 0x0B);
  Drivers::IO::outb(COM1 + 4, 0x1E);
  Drivers::IO::outb(COM1 + 0, 0xAE);
  if(Drivers::IO::inb(COM1 + 0) != 0xAE) {
      for(;;);
 }
 Drivers::IO::outb(COM1 + 4, 0x0F);
}

bool UART::active() {
  return Drivers::IO::inb(COM1 + 5) & 1;
}

bool UART::transmit_empty() {
  return Drivers::IO::inb(COM1 + 5) & 0x20;
}

char UART::read() {
  while(!active());
  return Drivers::IO::inb(COM1);
}

void UART::write(char data) {
  while (!transmit_empty());
  Drivers::IO::outb(COM1,data);
}
