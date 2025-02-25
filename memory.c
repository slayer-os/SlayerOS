#include "memory.h"
#include "serial.h"

void memory_init(void) {
    serial_write("Memory initialized.\n");
}
