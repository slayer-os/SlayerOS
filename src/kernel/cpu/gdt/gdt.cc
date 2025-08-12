#include <cpu/gdt.h>
#include <cpu/tss.h>
#include <dbg/log.h>

extern "C" void load_gdt(void* gdt_addr);
extern "C" void reload_cs_segments();
extern "C" u64 get_tss_descriptor_low();
extern "C" u64 get_tss_descriptor_high();

constexpr usize ENTRIES_NUM = 7;
GDT::TablePtr gp;
u64 table[ENTRIES_NUM] = {
  0x00,
  0xaf9a000000ffff,
  0xaf92000000ffff,
  0xcffa000000ffff,
  0xcff2000000ffff,
  0x00,
  0x00
};

void GDT::init() {
  gp.limit = (sizeof(u64) * ENTRIES_NUM) - 1;
	gp.base = (u64)&table;

  load_gdt(&gp);
  Log::success("GDT Init... OK");
  reload_cs_segments();
  
  TSS::init();
  
  table[5] = get_tss_descriptor_low();
  table[6] = get_tss_descriptor_high();
  
  load_gdt(&gp);
  
  TSS::load_tss();
  
  Log::success("TSS loaded and configured");
}
