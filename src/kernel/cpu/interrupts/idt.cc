#include <cpu/idt.h>
#include <cpu/isr.h>
#include <cpu/tss.h>
#include <dbg/log.h>

constexpr usize IDT_MAX_DESCRIPTORS = 256;

[[gnu::aligned(0x10)]]
static IDT::Gate idt[IDT_MAX_DESCRIPTORS];
static IDT::Table idtr;

typedef void (*ISR_Handler)(void);
ISR_Handler isr_handlers[IDT_MAX_DESCRIPTORS] = {
  [0] = isr0,
  [1] = isr1,
  [2] = isr2,
  [3] = isr3,
  [4] = isr4,
  [5] = isr5,
  [6] = isr6,
  [7] = isr7,
  [8] = isr8,
  [9] = isr9,
  [10] = isr10,
  [11] = isr11,
  [12] = isr12,
  [13] = isr13,
  [14] = isr14,
  [15] = isr15,
  [16] = isr16,
  [17] = isr17,
  [18] = isr18,
  [19] = isr19,
  [20] = isr20,
  [21] = isr21,
  [22] = isr22,
  [23] = isr23,
  [24] = isr24,
  [25] = isr25,
  [26] = isr26,
  [27] = isr27,
  [28] = isr28,
  [29] = isr29,
  [30] = isr30,
  [31] = isr31,
  [32] = isr32,
  [33] = isr33,
  [34] = isr34,
  [35] = isr35,
  [36] = isr36,
  [37] = isr37,
  [38] = isr38,
  [39] = isr39,
  [40] = isr40,
  [41] = isr41,
  [42] = isr42,
  [43] = isr43,
  [44] = isr44,
  [45] = isr45,
  [46] = isr46,
  [47] = isr47,
};

void IDT::register_entry(u8 index, u64 offset, u16 selector, u8 type) {
  idt[index].offset_low = offset & 0xFFFF;
  idt[index].selector = selector;
  idt[index].ist = 0;
  idt[index].type_attr = type | 0x80;
  idt[index].offset_mid = (offset >> 16) & 0xFFFF;
  idt[index].offset_high = (offset >> 32) & 0xFFFFFFFF;
  idt[index].zero = 0;
}

void IDT::register_entry_with_ist(u8 index, u64 offset, u16 selector, u8 type, u8 ist) {
  idt[index].offset_low = offset & 0xFFFF;
  idt[index].selector = selector;
  idt[index].ist = ist;
  idt[index].type_attr = type | 0x80;
  idt[index].offset_mid = (offset >> 16) & 0xFFFF;
  idt[index].offset_high = (offset >> 32) & 0xFFFFFFFF;
  idt[index].zero = 0;
}

void IDT::init() {
  idtr.offset = (u64)&idt[0];
  idtr.size = (u16)(sizeof(struct IDT::Gate) * IDT_MAX_DESCRIPTORS) - 1;

  isr_handlers[0xF0] = isr0xF0;
  isr_handlers[0xFF] = isr0xFF;

  for (u8 i = 0; i < 32; i++) {
    if (isr_handlers[i] == NULL)
      continue;
    IDT::register_entry_with_ist(i, (u64)isr_handlers[i], 0x08, INT_GATE, TSS::ISTIndex::IST_EXCEPTION);
  }
  
  if (isr_handlers[0xF0] != NULL)
    IDT::register_entry_with_ist(0xF0, (u64)isr_handlers[0xF0], 0x08, INT_GATE, TSS::ISTIndex::IST_EXCEPTION);
  if (isr_handlers[0xFF] != NULL)
    IDT::register_entry_with_ist(0xFF, (u64)isr_handlers[0xFF], 0x08, INT_GATE, TSS::ISTIndex::IST_EXCEPTION);
  
  __asm__ volatile ("lidt %0" : : "m"(idtr));
  __asm__ volatile ("sti");
  Log::success("IDT Init... OK");
}
