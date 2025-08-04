#include <mem/frames.h>
#include <klib/string.h>

static bool *frames;
static size_t nframes;
u32 latest_frame=0;
u32 available_frames;
void *base_addr;

void *Mem::Frame::alloc() {
  while (latest_frame < available_frames && frames[latest_frame]) {
    latest_frame++;
  }
  if (latest_frame == available_frames) {
    return nullptr;
  }
  frames[latest_frame] = 1;
  void *frame = base_addr + latest_frame * FRAME_SIZE;
  latest_frame++;
  memset(frame, 0, FRAME_SIZE);
  return frame;
}


void *Mem::Frame::allocs(size_t count) {
  // find adjacent free frames
  u32 start_frame = latest_frame;
  u32 frame_count = 0;
  for (u32 i =start_frame; i < available_frames; i++) {
    if (frames[i] == 1) {
      start_frame = i;
      frame_count = 1;
    } else {
      frame_count++;
    }
    if (frame_count == count) {
      break;
    }
  }
  if (frame_count < count) {
    return NULL;
  }
  for (u32 i = start_frame; i < start_frame + count; i++) {
    frames[i] = 1;
  }
  latest_frame++; // at least one frame is allocated, preventing the next allocation from starting at the same frame
  void *frame = base_addr + start_frame * FRAME_SIZE;
  memset(frame, 0, count * FRAME_SIZE);
  return frame;
}

void Mem::Frame::free(void *frame) {
  u32 frame_num = (u32)((u64)frame - (u64)base_addr) / FRAME_SIZE;
  frames[frame_num] = 0;
  latest_frame = frame_num; // Set indexing start point to the freed frame
}

void Mem::Frame::init() {
  struct limine_memmap_entry *bitmap_entry = boot_ctx.memmap_entries[1];
  nframes = bitmap_entry->length / FRAME_SIZE;
  frames = (bool *)PHYS2VIRT(bitmap_entry->base);
  memset(frames, 0, nframes);

  struct limine_memmap_entry *alloc_entry = boot_ctx.memmap_entries[0];
  base_addr = PHYS2VIRT(alloc_entry->base);
  available_frames = alloc_entry->length / FRAME_SIZE;
}
