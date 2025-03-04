#include <mem/paging.h>
#include <mem/mapper.h>
#include <libc/string.h>

static page_table_t *pml4;

void flush_tlb() {
  asm volatile("mov %cr3, %rax; mov %rax, %cr3");
}

void load_cr3(void *cr3) {
  asm volatile("mov %0, %%cr3" :: "r"((u64)VIRT2PHYS(cr3)) : "memory");
}

void enable_paging() {
  // PAE paging
  u64 cr4;
  asm volatile("mov %%cr4, %0" : "=r"(cr4));
  cr4 |= 0x20;
  asm volatile("mov %0, %%cr4" :: "r"(cr4));


  // Enable paging
  u64 cr0;
  asm volatile("mov %%cr0, %0" : "=r"(cr0));
  cr0 |= 0x80000000;
  asm volatile("mov %0, %%cr0" :: "r"(cr0));
}

void map_page(void *physptr, void *virtptr, u64 flags) {
  u64 phys = (u64)physptr;
  u64 virt = (u64)virtptr;
  u16 pml4_index = (virt >> 39) & 0x1ff;
  u16 pml3_index = (virt >> 30) & 0x1ff;
  u16 pml2_index = (virt >> 21) & 0x1ff;
  u16 pml1_index = (virt >> 12) & 0x1ff;
  u64 *pml3 = nullptr;
  if (pml4[pml4_index] & PAGE_PRESENT) {
      pml3 = (u64*)PHYS2VIRT((pml4[pml4_index] & PTE_MASK));
  } else {
      pml3 = (u64*)allocate_frame();
      pml4[pml4_index] = (u64)VIRT2PHYS(pml3) | PAGE_PRESENT | PAGE_WRITE;
  }
  u64 *pml2 = nullptr;
  if (pml3[pml3_index] & PAGE_PRESENT) {
      pml2 = (u64*)PHYS2VIRT((pml3[pml3_index] & PTE_MASK));
  } else {
      pml2 = (u64*)allocate_frame();
      pml3[pml3_index] = (u64)VIRT2PHYS(pml2) | PAGE_PRESENT | PAGE_WRITE;
  }

  u64 *pml1 = nullptr;
  if (pml2[pml2_index] & PAGE_PRESENT) {
      pml1 = (u64*)PHYS2VIRT((pml2[pml2_index] & PTE_MASK));
  } else {
      pml1 = (u64*)allocate_frame();
      pml2[pml2_index] = (u64)VIRT2PHYS(pml1) | PAGE_PRESENT | PAGE_WRITE;
  }
  bool perm_mod = pml1[pml1_index] & PAGE_PRESENT;
  pml1[pml1_index] = phys | flags;
  if (perm_mod) { flush_tlb(); }
}

u32 map_range(void *phys, void *virt, u64 size, u64 flags) {
  for (u64 i = 0; i < size; i += FRAME_SIZE) {
    map_page(phys + i, virt + i, flags);
  }
  return size / FRAME_SIZE;
}

void unmap_page(void *virtptr) {
  u64 virt = (u64)virtptr;
  u16 pml4_index = (virt >> 39) & 0x1ff;
  u16 pml3_index = (virt >> 30) & 0x1ff;
  u16 pml2_index = (virt >> 21) & 0x1ff;
  u16 pml1_index = (virt >> 12) & 0x1ff;
  u64 *pml3 = nullptr;
  if (pml4[pml4_index] & PAGE_PRESENT) {
      pml3 = (u64*)PHYS2VIRT((pml4[pml4_index] & PTE_MASK));
  } else {
      return;
  }
  u64 *pml2 = nullptr;
  if (pml3[pml3_index] & PAGE_PRESENT) {
      pml2 = (u64*)PHYS2VIRT((pml3[pml3_index] & PTE_MASK));
  } else {
      return;
  }

  u64 *pml1 = nullptr;
  if (pml2[pml2_index] & PAGE_PRESENT) {
      pml1 = (u64*)PHYS2VIRT((pml2[pml2_index] & PTE_MASK));
  } else {
      return;
  }
  pml1[pml1_index] = 0;
  flush_tlb();
}

void init_paging() {
  pml4 = (page_table_t*)allocate_frame();
  memset(pml4, 0, FRAME_SIZE);
  full_memory_map();
  load_cr3(pml4);
  enable_paging();
}
