#include <sys/ioapic.h>
#include <mem/paging.h>
#include <dbg/log.h>

struct acpi_madt_ioapic apics[24];
struct acpi_madt_interrupt_source_override iso[24];
u8 apic_ent, iso_ent;

void Sys::IOAPIC::write(u64 base, u32 reg, u32 value) {
  u64 virt = (u64)PHYS2VIRT(base);
  *(volatile u32*)virt = reg;
  *(volatile u32*)(virt + 0x10) = value;
}

u32 Sys::IOAPIC::read(u64 base, u32 reg) {
  u64 virt = (u64)PHYS2VIRT(base);
  *(volatile u32*)virt = reg;
  return *(volatile u32*)(virt + 0x10);
}

void Sys::IOAPIC::init() {
  apic_ent = 0, iso_ent = 0;
  struct uacpi_table apic_table;
  uacpi_status ret = uacpi_table_find_by_signature(ACPI_MADT_SIGNATURE,&apic_table);
  if(ret != UACPI_STATUS_OK) {
    Log::critical("Missing MADT table");
    asm("hlt");
  }
  struct acpi_madt* apic = (struct acpi_madt*)apic_table.virt_addr;
  struct acpi_entry_hdr* current = (struct acpi_entry_hdr*)apic->entries;
  while (1) {
    if ((u64)current >= (u64)apic->entries + apic->hdr.length - sizeof(acpi_madt))
      break;

    switch (current->type) {
      case ACPI_MADT_ENTRY_TYPE_IOAPIC: {
        struct acpi_madt_ioapic* cur_ioapic = (acpi_madt_ioapic*)current;
        u64 phys_addr = cur_ioapic->address;
        Mem::Paging::map((void*)phys_addr, PHYS2VIRT(phys_addr), Mem::Paging::PAGE_PRESENT | Mem::Paging::PAGE_WRITE);
        apics[apic_ent++] = *cur_ioapic;
        break;
      }
      case ACPI_MADT_ENTRY_TYPE_INTERRUPT_SOURCE_OVERRIDE: {
        struct acpi_madt_interrupt_source_override* cur_iso = (struct acpi_madt_interrupt_source_override*)current;
        iso[iso_ent++] = *cur_iso;
        break;
      }
    }
    current = (acpi_entry_hdr*)((u64)current + current->length);
  }
  
  Log::success("IOAPIC Init - Found %d IOAPICs, %d ISOs", apic_ent, iso_ent);
  
  for (u8 i = 0; i < apic_ent; i++) {
    Sys::IOAPIC::read(apics[i].address, 1);
    Sys::IOAPIC::read(apics[i].address, 0);
  }
}

void Sys::IOAPIC::set(u8 vec, u8 irq, u64 flags, u64 lapic) {
  struct acpi_madt_ioapic* apic = nullptr;
  struct acpi_madt_interrupt_source_override* ciso;
  bool found_iso = false;
  u8 actual_irq = irq;
  
  for (u8 i=0; i<iso_ent; i++) {
    ciso = &iso[i];
    if (ciso->source == irq) {
      found_iso = true;
      actual_irq = ciso->gsi;
      break;
    }
  }
  
  u64 c_flags = (lapic << 56) | flags | (vec & 0xFF);
  if (found_iso) {
    u8 pol = (ciso->flags & 0b11) == 0b11 ? 1 : 0;
    u8 mode  = ((ciso->flags >> 2) & 0b11) == 0b11 ? 1 : 0;
    c_flags = (lapic << 56) | (mode << 15) | (pol << 13) | (vec & 0xff) | flags;
  }

  for (u8 i=0; i<apic_ent; i++) {
    apic = &apics[i];
    u32 ver = Sys::IOAPIC::read(apic->address,1);
    u32 max = ver >> 16;
    if(apic->gsi_base <= actual_irq && apic->gsi_base + max > actual_irq)
      break;
  }

  if (apic == nullptr) {
    Log::critical("No IOAPIC found for IRQ %d (GSI %d)", irq, actual_irq);
    return;
  }

  u32 irqreg = ((actual_irq - apic->gsi_base) * 2) + 0x10;
  
  Sys::IOAPIC::write(apic->address, irqreg, (u32)c_flags);
  Sys::IOAPIC::write(apic->address, irqreg+1, (u32)((u64)c_flags >> 32));
}

void Sys::IOAPIC::dump_entries() {
  Log::info("=== IOAPIC Redirection Table Dump ===");
  
  for (u8 i = 0; i < apic_ent; i++) {
    struct acpi_madt_ioapic* apic = &apics[i];
    u32 ver = Sys::IOAPIC::read(apic->address, 1);
    u32 max = (ver >> 16) & 0xFF;
    
    Log::info("IOAPIC %d (0x%llx): Entries 0-%d", i, apic->address, max);
    
    for (u32 entry = 0; entry <= max; entry++) {
      u32 reg = (entry * 2) + 0x10;
      u32 low = Sys::IOAPIC::read(apic->address, reg);
      u32 high = Sys::IOAPIC::read(apic->address, reg + 1);
      
      u8 vector = low & 0xFF;
      u8 delivery_mode = (low >> 8) & 0x7;
      u8 dest_mode = (low >> 11) & 0x1;
      u8 polarity = (low >> 13) & 0x1;
      u8 trigger = (low >> 15) & 0x1;
      u8 mask = (low >> 16) & 0x1;
      u8 dest = (high >> 24) & 0xFF;
      
      if (vector != 0 || !mask) {
        Log::info("  Entry %02d: Vec=0x%02x DM=%d DestM=%d Pol=%d Trig=%d Mask=%d Dest=%d", 
                  entry, vector, delivery_mode, dest_mode, polarity, trigger, mask, dest);
      }
    }
  }
}
