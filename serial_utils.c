#include "serial_utils.h"
#include "io.h"

#define COM1 0x3F8

void serial_write(const char* data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        while ((inb(COM1 + 5) & 0x20) == 0);
        outb(COM1, data[i]);
    }
}