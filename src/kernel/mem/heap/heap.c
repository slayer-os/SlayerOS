#include <mem/heap.h>

heap_page_t *heap = NULL;
heap_page_t *farthest_page = NULL;

void page_segments_init(heap_page_t *page) {
  heap_segment_t *segment = (heap_segment_t *)page->addr;
  
  page->free_size -= sizeof(heap_segment_t);
  
  segment->addr = (void *)segment + sizeof(heap_segment_t);
  
  segment->size = page->free_size;
  segment->free = true;
  segment->next = NULL;
  
  page->first = segment;
}

heap_page_t *create_heap_page(size_t size) {
  size_t alloc_size;
  // If the requested size (plus overhead) fits in one frame, use FRAME_SIZE.
  u32 num_pages = 1;
  if (size + sizeof(heap_page_t) + sizeof(heap_segment_t) <= FRAME_SIZE) {
    alloc_size = FRAME_SIZE;
  }
  else {
    alloc_size = sizeof(heap_page_t) + sizeof(heap_segment_t) + size, FRAME_SIZE;
    num_pages = ROUND_UP(alloc_size, FRAME_SIZE);
  }

  heap_page_t *page = (heap_page_t *)allocate_frames(num_pages);
  
  page->addr = (void*)page + sizeof(heap_page_t);
  page->free_size = alloc_size - sizeof(heap_page_t);
  page_segments_init(page);
  page->next = NULL;
  return page;
}

const size_t OPT_SIZE_TRESHOLD = 0x40; // Actual free size required to fragment segment
void *alloc_segment(heap_page_t *page, heap_segment_t *segment, size_t size) {
  if (segment->size > size + sizeof(heap_segment_t)
      && segment->size - size - sizeof(heap_segment_t) > OPT_SIZE_TRESHOLD) {

    heap_segment_t *fragment = segment->addr + size;
    fragment->addr = (void*)fragment + sizeof(heap_segment_t);
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

void *kmalloc(size_t size) {
  if (heap == NULL) farthest_page = heap = create_heap_page(size);
  heap_page_t *page = farthest_page;
  while (page) {
    heap_segment_t *segment = page->first;
    while (segment) {
      if (segment->free && segment->size >= size) {
        return alloc_segment(page, segment, size);
      }
      segment = segment->next;
    }
    if (page->next == NULL) break;
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
