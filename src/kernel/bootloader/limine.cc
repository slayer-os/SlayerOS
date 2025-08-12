#include <bootloader/limine.h>

BootloaderCtx boot_ctx;

struct limine_bootloader_info_request limine_info = {
  .id = LIMINE_BOOTLOADER_INFO_REQUEST,
};

struct limine_paging_mode_request limine_paging = {
  .id = LIMINE_PAGING_MODE_REQUEST,
  .mode = LIMINE_PAGING_MODE_X86_64_4LVL,
};

struct limine_memmap_request limine_memmap = {
  .id = LIMINE_MEMMAP_REQUEST,
};

struct limine_hhdm_request limine_hhdm = {
  .id = LIMINE_HHDM_REQUEST,
};

struct limine_kernel_address_request limine_kernaddr = {
  .id = LIMINE_KERNEL_ADDRESS_REQUEST,
};

struct limine_framebuffer_request limine_fb = {
  .id = LIMINE_FRAMEBUFFER_REQUEST,
};

struct limine_kernel_file_request limine_kernel_file = {
  .id = LIMINE_KERNEL_FILE_REQUEST,
};

struct limine_rsdp_request limine_rsdp = {
  .id = LIMINE_RSDP_REQUEST,
};

void sort_memmap() {
  // run bubble sort to reverse sort the memory map by usable memory size
  for (uint64_t i = 0; i < boot_ctx.memmap_entries_count; i++) {
    for (uint64_t j = i + 1; j < boot_ctx.memmap_entries_count; j++) {
      if (boot_ctx.memmap_entries[i]->type != LIMINE_MEMMAP_USABLE &&
          boot_ctx.memmap_entries[i]->length < boot_ctx.memmap_entries[j]->length) {
        struct limine_memmap_entry *temp = boot_ctx.memmap_entries[i];
        boot_ctx.memmap_entries[i] = boot_ctx.memmap_entries[j];
        boot_ctx.memmap_entries[j] = temp;
      }
    }
  }
}

void BootloaderCtx::gather() {
  boot_ctx.name = limine_info.response->name;
  boot_ctx.version = limine_info.response->version;
  boot_ctx.memmap_entries_count = limine_memmap.response->entry_count;
  boot_ctx.memmap_entries = limine_memmap.response->entries;
  boot_ctx.hhdm_addr = limine_hhdm.response->offset;
  boot_ctx.kern_address_virt = (void*)limine_kernaddr.response->virtual_base;
  boot_ctx.kern_address_phys = (void*)limine_kernaddr.response->physical_base;
  boot_ctx.fb_info = limine_fb.response;
  boot_ctx.kernel_file = limine_kernel_file.response->kernel_file;
  boot_ctx.rsdp_addr = limine_rsdp.response->address - boot_ctx.hhdm_addr;
  sort_memmap();
}
