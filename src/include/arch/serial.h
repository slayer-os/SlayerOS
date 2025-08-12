#pragma once
#include <klib/types.h>

#define COM1 0x3F8


namespace UART {
  void init();
  bool active();
  bool transmit_empty();
  char read();
  void write(char data);
}


