#include <mem/heap.h>
#include <dbg/log.h>

heap_page_t *heap = nullptr;

void page_segments_init(heap_page_t *page) {
  heap_segment_t *segment = (heap_segment_t *)page->addr;
  
  page->free_size -= sizeof(heap_segment_t);
  
  segment->addr = (void *)segment + sizeof(heap_segment_t);
  
  segment->size = page->free_size;
  segment->free = true;
  segment->next = nullptr;
  
  page->first = segment;
}

heap_page_t *create_heap_page(usize size) {
  usize alloc_size;
  // If the requested size (plus overhead) fits in one frame, use FRAME_SIZE.
  u32 num_pages = 1;
  if (size + sizeof(heap_page_t) + sizeof(heap_segment_t) <= FRAME_SIZE) {
    alloc_size = FRAME_SIZE;
  }
  else {
    alloc_size = sizeof(heap_page_t) + sizeof(heap_segment_t) + size;
    num_pages = ROUND_UP(alloc_size, FRAME_SIZE);
  }

  heap_page_t *page = (heap_page_t *)Mem::Frame::allocs(num_pages);
  if (page == nullptr) {
    Log::critical("Cannot allocate heap pages");
    __asm__ __volatile__ ("hlt");
  }
  
  page->addr = (void*)((char*)page + sizeof(heap_page_t));
  page->free_size = alloc_size - sizeof(heap_page_t);
  page_segments_init(page);
  page->next = nullptr;
  return page;
}

void *alloc_segment(heap_page_t *page, heap_segment_t *segment, usize size) {
  if (segment->size > size + sizeof(heap_segment_t)) {

    heap_segment_t *fragment = (heap_segment_t*)((char*)segment->addr + size);
    fragment->addr = (void*)((char*)fragment + sizeof(heap_segment_t));
    fragment->size = segment->size - size - sizeof(heap_segment_t);
    fragment->free = true;
    fragment->next = segment->next;
    segment->size = size;
    segment->next = fragment;

  }
  page->free_size -= segment->size;
  segment->free = false;
  return segment->addr;
}

void try_segment_merge(heap_page_t *page, heap_segment_t *segment) {
  heap_segment_t *next = segment->next;
  if (next && next->free) {
    segment->size += next->size + sizeof(heap_segment_t);
    segment->next = next->next;
    page->free_size += sizeof(heap_segment_t);
  }
}

void *kmalloc(usize size) {
  if (heap == nullptr) heap = create_heap_page(size);
  heap_page_t *page = heap;
  while (page) {
    heap_segment_t *segment = page->first;
    while (segment) {
      if (segment->free && segment->size >= size) {
        return alloc_segment(page, segment, size);
      }
      segment = segment->next;
    }
    if (page->next == nullptr) break;
    else page = page->next;
  }
  page->next = create_heap_page(size);
  return kmalloc(size);
}

bool kfree(void *addr) {
  heap_page_t *page = heap;
  heap_segment_t *segment = page->first;
  while (page) {
    while (segment) {
      if (segment->addr == addr) {
        segment->free = true;
        page->free_size += segment->size;
        try_segment_merge(page, segment);
        return true;
      }
      segment = segment->next;
    }
    page = page->next;
  }
  return false;
}
