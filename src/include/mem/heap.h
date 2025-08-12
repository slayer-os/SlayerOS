#pragma once
#include <klib/types.h>
#include <mem/frames.h>
#include <mem/paging.h>


typedef struct heap_segment {
  void *addr;
  usize size;
  bool free;
  struct heap_segment *next;
} heap_segment_t;

typedef struct heap_page {
  void *addr;
  heap_segment_t *first;
  u32 free_size;
  struct heap_page *next;
} heap_page_t;

void *kmalloc(usize size);
bool kfree(void *addr);
//void *krealloc(void *addr, usize size);


