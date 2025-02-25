#include <mem/frames.h>
#include <libc/string.h>

static bit *frames;
static size_t nframes;
u32 latest_frame=0;
u32 available_frames;
void *base_addr;

void *allocate_frame() {
  while (latest_frame < available_frames && frames[latest_frame]) {
    latest_frame++;
  }
  if (latest_frame == available_frames) {
    return NULL;
  }
  frames[latest_frame] = 1;
  void *frame = base_addr + latest_frame * FRAME_SIZE;
  latest_frame++;
  memset(frame, 0, FRAME_SIZE);
  return frame;
}

void free_frame(void *frame) {
  u32 frame_num = (u32)((u64)frame - (u64)base_addr) / FRAME_SIZE;
  frames[frame_num] = 0;
  latest_frame = frame_num; // Set indexing start point to the freed frame
}

void init_frame_alloc() {
  struct limine_memmap_entry *bitmap_entry = boot_ctx.memmap_entries[1];
  nframes = bitmap_entry->length / FRAME_SIZE;
  frames = (bit *)PHYS2VIRT(bitmap_entry->base);
  memset(frames, 0, nframes);

  struct limine_memmap_entry *alloc_entry = boot_ctx.memmap_entries[0];
  base_addr = PHYS2VIRT(alloc_entry->base);
  available_frames = alloc_entry->length / FRAME_SIZE;
}
