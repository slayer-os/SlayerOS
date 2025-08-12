#include <cpu/tss.h>
#include <cpu/gdt.h>
#include <dbg/log.h>
#include <mem/frames.h>
#include <klib/memory.h>
#include <klib/string.h>

constexpr usize IST_STACK_SIZE = 16384;

static TSS::TaskStateSegment tss;
static u8* ist_stacks[8];

extern "C" void load_tss_register(u16 tss_segment);

static u64 create_tss_descriptor(u64 tss_addr) {
  TSS::Descriptor desc = {};
  
  desc.base_low = tss_addr & 0xFFFF;
  desc.base_mid = (tss_addr >> 16) & 0xFF;
  desc.base_high = (tss_addr >> 24) & 0xFF;
  desc.base_ext = (tss_addr >> 32) & 0xFFFFFFFF;
  
  u32 limit = sizeof(TSS::TaskStateSegment) - 1;
  desc.limit_low = limit & 0xFFFF;
  desc.granularity = (limit >> 16) & 0x0F;
  
  desc.access = 0x89;
  
  desc.reserved = 0;
  
  u64 result = 0;
  result |= (u64)desc.limit_low;
  result |= (u64)desc.base_low << 16;
  result |= (u64)desc.base_mid << 32;
  result |= (u64)desc.access << 40;
  result |= (u64)desc.granularity << 48;
  result |= (u64)desc.base_high << 56;
  
  return result;
}

static u64 create_tss_descriptor_high(u64 tss_addr) {
  u64 result = 0;
  result |= (tss_addr >> 32) & 0xFFFFFFFF;
  return result;
}

void TSS::init() {
  memset(&tss, 0, sizeof(TSS::TaskStateSegment));
  
  tss.iopb_offset = sizeof(TSS::TaskStateSegment);
  
  Log::success("TSS Init... OK");
}

void TSS::init_stacks() {
  for (int i = 1; i <= 7; i++) {
    u32 frames_per_stack = IST_STACK_SIZE / FRAME_SIZE;
    ist_stacks[i] = (u8*)Mem::Frame::allocs(frames_per_stack);
    if (!ist_stacks[i]) {
      Log::critical("Failed to allocate IST stack %d", i);
      return;
    }
    
    u64 stack_top = (u64)ist_stacks[i] + IST_STACK_SIZE;
    
    switch (i) {
      case 1: tss.ist1 = stack_top; break;
      case 2: tss.ist2 = stack_top; break;
      case 3: tss.ist3 = stack_top; break;
      case 4: tss.ist4 = stack_top; break;
      case 5: tss.ist5 = stack_top; break;
      case 6: tss.ist6 = stack_top; break;
      case 7: tss.ist7 = stack_top; break;
    }
  }
  
  Log::success("TSS IST stacks allocated and configured");
}

void TSS::set_kernel_stack(u64 stack_ptr) {
  tss.rsp0 = stack_ptr;
}

u64 TSS::get_ist_stack(ISTIndex index) {
  if (index < 1 || index > 7) {
    return 0;
  }
  
  switch (index) {
    case 1: return tss.ist1;
    case 2: return tss.ist2;
    default: return 0;
  }
}

void TSS::load_tss() {
  load_tss_register(0x28);
}

extern "C" {
  u64 get_tss_descriptor_low() {
    return create_tss_descriptor((u64)&tss);
  }
  
  u64 get_tss_descriptor_high() {
    return create_tss_descriptor_high((u64)&tss);
  }
}
