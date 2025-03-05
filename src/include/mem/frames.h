#ifndef FRAME_ALLOCATOR_H
#define FRAME_ALLOCATOR_H
#include <libc/types.h>
#include <bootloader/limine.h>

#define FRAME_SIZE       0x1000
#define PHYS2VIRT(addr)  ((void *)boot_ctx.hhdm_addr + (u64)(addr))
#define VIRT2PHYS(addr)  ((u64)(addr) - (u64)boot_ctx.hhdm_addr)

namespace Mem::Frame {
  void *alloc();
  void free(void *frame);
  void *allocs(size_t count);
  void init();
}

#endif
