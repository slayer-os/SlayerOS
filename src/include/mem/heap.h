#ifndef HEAP_ALLOCATOR_H
#define HEAP_ALLOCATOR_H
#include <libc/types.h>
#include <mem/frames.h>
#include <mem/paging.h>


typedef struct heap_segment {
  void *addr;
  size_t size;
  bool free;
  struct heap_segment *next;
} heap_segment_t;

typedef struct heap_page {
  void *addr;
  heap_segment_t *first;
  u32 free_size;
  struct heap_page *next;
} heap_page_t;

void *kmalloc(size_t size);
bool kfree(void *addr);
//void *krealloc(void *addr, size_t size);


#endif
