#include <mem/mapper.h>

extern void** _text_start_ld;
extern void** _text_end_ld;
extern void** _rodata_start_ld;
extern void** _rodata_end_ld;
extern void** _data_start_ld;
extern void** _data_end_ld;

void map_kernel_code() {
  u64 phys_base = (u64)boot_ctx.kern_address_phys;
  u64 virt_base = (u64)boot_ctx.kern_address_virt;

  u64 text_start = ALIGN_DOWN((u64)&_text_start_ld, FRAME_SIZE);
  u64 text_end = ALIGN_UP((u64)&_text_end_ld, FRAME_SIZE);
  u64 rodata_start = ALIGN_DOWN((u64)&_rodata_start_ld, FRAME_SIZE);
  u64 rodata_end = ALIGN_UP((u64)&_rodata_end_ld, FRAME_SIZE);
  u64 data_start = ALIGN_DOWN((u64)&_data_start_ld, FRAME_SIZE);
  u64 data_end = ALIGN_UP((u64)&_data_end_ld, FRAME_SIZE);
 
  for (u64 text = text_start; text < text_end; text += FRAME_SIZE) {
    Mem::Paging::map((void*)(text - virt_base + phys_base), (void*)text, Mem::Paging::PAGE_PRESENT | Mem::Paging::PAGE_USER);
  }
  for (u64 rodata = rodata_start; rodata < rodata_end; rodata += FRAME_SIZE) {
    Mem::Paging::map((void*)(rodata - virt_base + phys_base), (void*)rodata, Mem::Paging::PAGE_PRESENT | Mem::Paging::PAGE_NX | Mem::Paging::PAGE_USER);
  }
  for (u64 data = data_start; data < data_end; data += FRAME_SIZE) {
    Mem::Paging::map((void*)(data - virt_base + phys_base), (void*)data, Mem::Paging::PAGE_PRESENT | Mem::Paging::PAGE_WRITE | Mem::Paging::PAGE_NX | Mem::Paging::PAGE_USER);
  }
}

void map_found_memory() {
  for (size_t i =0; i < boot_ctx.memmap_entries_count; i++) {
    struct limine_memmap_entry *entry = boot_ctx.memmap_entries[i];
    u64 vaddr = (u64)PHYS2VIRT(entry->base);
    u64 paddr = entry->base;
    u64 pages = entry->length / FRAME_SIZE;
    if (entry->type != LIMINE_MEMMAP_USABLE) {
      continue;
    }
    for (u64 page = 0; page < pages; page++) {
      Mem::Paging::map((void*)(paddr + (page * FRAME_SIZE)), (void*)(vaddr + (page * FRAME_SIZE)), Mem::Paging::PAGE_PRESENT | Mem::Paging::PAGE_WRITE | Mem::Paging::PAGE_USER);
    }
  }
}

void map_hhdm_memory() {
  for (size_t i =0; i < boot_ctx.memmap_entries_count; i++) {
    struct limine_memmap_entry *entry = boot_ctx.memmap_entries[i];
    u64 vaddr = (u64)PHYS2VIRT(entry->base);
    u64 paddr = entry->base;
    u64 pages = entry->length / FRAME_SIZE;
    if (entry->type != LIMINE_MEMMAP_USABLE) {
      continue;
    }
    for (u64 page = 0; page < pages; page++) {
      Mem::Paging::map((void*)(paddr + (page * FRAME_SIZE)), (void*)(vaddr + (page * FRAME_SIZE)), Mem::Paging::PAGE_PRESENT | Mem::Paging::PAGE_WRITE | Mem::Paging::PAGE_USER);
    }
  }
}

void map_bootloader_memory() {
  for (u64 i=0; i<boot_ctx.memmap_entries_count; i++) {
    struct limine_memmap_entry *entry = boot_ctx.memmap_entries[i];
    if (entry->type == LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE) {
      u64 vaddr = (u64)PHYS2VIRT(entry->base);
      u64 paddr = entry->base;
      u64 pages = entry->length / FRAME_SIZE;
      for (u64 page = 0; page < pages; page++) {
        Mem::Paging::map((void*)(paddr + (page * FRAME_SIZE)), (void*)(vaddr + (page * FRAME_SIZE)), Mem::Paging::PAGE_PRESENT | Mem::Paging::PAGE_WRITE | Mem::Paging::PAGE_USER);
      }
    }
  }
}

void map_framebuffer_memory() {
  for (u64 i=0; i<boot_ctx.memmap_entries_count; i++) {
    struct limine_memmap_entry *entry = boot_ctx.memmap_entries[i];
    if (entry->type == LIMINE_MEMMAP_FRAMEBUFFER) {
      u64 vaddr = (u64)PHYS2VIRT(entry->base);
      u64 paddr = entry->base;
      u64 pages = entry->length / FRAME_SIZE;
      for (u64 page = 0; page < pages; page++) {
        Mem::Paging::map((void*)(paddr + (page * FRAME_SIZE)), (void*)(vaddr + (page * FRAME_SIZE)), Mem::Paging::PAGE_PRESENT | Mem::Paging::PAGE_WRITE | Mem::Paging::PAGE_USER);
      }
    }
  }
}



void Mem::Mapper::full_map() {
  map_bootloader_memory();
  map_framebuffer_memory();
  map_found_memory();
  map_hhdm_memory();
  map_kernel_code();
}
