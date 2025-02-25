#ifndef PAGING_H
#define PAGING_H
#include <libc/types.h>
#include <mem/frames.h>

#define ROUND_DOWN(x)     ((x) & ~(0x1000 - 1))
#define ROUND_UP(x, y)    (x + (y - 1)) / y
#define ALIGN_UP(x, y)    (ROUND_UP(x, y) * y)
#define ALIGN_DOWN(x, y)  ((x / y) * y)


#define PTE_MASK     0x0000FFFFFFFFF000

#define PAGE_PRESENT ( 0x1ull << 0 )
#define PAGE_WRITE   ( 0x1ull << 1 )
#define PAGE_USER    ( 0x1ull << 2 )
#define PAGE_UNCACHE ( 0x1ull << 4 )
#define PAGE_NX      ( 0x1ull << 63)

typedef u64 page_table_t;

void init_paging();
void map_page(void *phys, void *virt, u64 flags);
u32 map_range(void *phys, void *virt, u64 size, u64 flags);
void unmap_page(void *virt);

#endif
