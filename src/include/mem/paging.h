#pragma once
#include <klib/types.h>
#include <mem/frames.h>

#define ROUND_DOWN(x)     ((x) & ~(0x1000 - 1))
#define ROUND_UP(x, y)    (x + (y - 1)) / y
#define ALIGN_UP(x, y)    (ROUND_UP(x, y) * y)
#define ALIGN_DOWN(x, y)  ((x / y) * y)


typedef u64 page_table_t;

namespace Mem::Paging {
  constexpr u64 PAGE_PRESENT = ( 0x1ull << 0 );
  constexpr u64 PAGE_WRITE   = ( 0x1ull << 1 );
  constexpr u64 PAGE_USER    = ( 0x1ull << 2 );
  constexpr u64 PAGE_UNCACHE = ( 0x1ull << 4 );
  constexpr u64 PAGE_NX      = ( 0x1ull << 63);

  void init();
  void map(void *phys, void *virt, u64 flags);
  u32 map_range(void *phys, void *virt, u64 size, u64 flags);
  void unmap(void *virt);
  void* map_mmio(u64 phys_addr, usize size);
}

