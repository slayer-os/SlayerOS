#include <cpu/pic.h>
#include <klib/types.h>
#include <dbg/log.h>
#include <drivers/io.h>

void PIC::disable() {
    Drivers::IO::outb(0x21, 0xFF);
    Drivers::IO::outb(0xA1, 0xFF);
    
    Log::success("Legacy PIC disabled");
}
