#ifndef LIMINE_INTERFACE_H
#define LIMINE_INTERFACE_H
#include <libc/types.h>
#include <limine.h>

class BootloaderCtx {
public:
  const char *name;
  const char *version;
  uint64_t memmap_entries_count;
  struct limine_memmap_entry **memmap_entries;
  u64 hhdm_addr;
  void *kern_address_virt;
  void *kern_address_phys;
  struct limine_framebuffer_response *fb_info;
  struct limine_file *kernel_file;
  /*
   * LMFAO, self-aware kernel
   * this is meta-introspecting stuff
  */

  void gather();
};

extern BootloaderCtx boot_ctx;

#endif
