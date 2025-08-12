#include <klib/types.h>
#include <cpu/idt.h>
#include <cpu/isr.h>
#include <dbg/log.h>
#include <err/handler.h>
#include <klib/string.h>
#include <klib/dwarf.h>
#include <cpu/lapic.h>

const char *messages[255] = {
  "Divide by zero",
  "Single step",
  "Non-maskable interrupt",
  "Breakpoint",
  "Overflow",
  "Bound range exceeded",
  "Invalid opcode",
  "Device not available",
  "Double fault",
  "Coprocessor segment overrun",
  "Invalid TSS",
  "Segment not present",
  "Stack-segment fault",
  "General protection fault",
  "Page fault",
  "Reserved",
  "x87 FPU error",
  "Alignment check",
  "Machine check",
  "SIMD floating-point exception",
  "Virtualization exception",
  "Control protection exception",
};

void dump_registers(struct isr_frame *info) {
  Log::print("\n\x1b[38;5;130m╔═══════════════════════════════════════════════════════════════════════════╗\x1b[0m\n");
  Log::print("\x1b[38;5;130m║\x1b[0m \x1b[1;91m SYSTEM EXCEPTION - CRITICAL ERROR DETECTED\x1b[0m                               \x1b[38;5;130m║\x1b[0m\n");
  Log::print("\x1b[38;5;130m╠═══════════════════════════════════════════════════════════════════════════╣\x1b[0m\n");

  
  Log::printf("\x1b[38;5;130m║\x1b[0m \x1b[1;96m Interrupt:\x1b[0m \x1b[1;93m0x%02x\x1b[0m", info->int_num);
  if (info->int_num < 22) {
    Log::printf(" \x1b[1;97m   %s\x1b[0m", messages[info->int_num]);
  }
  
  usize msg_len = strlen(messages[info->int_num]);
  for (usize i=0;i<54-msg_len;i++) {
    Log::print(" ");
  }
  Log::print("\x1b[38;5;130m║\x1b[0m\n");

  if (info->error_code != 0) {
    Log::printf("\x1b[38;5;130m║\x1b[0m \x1b[1;96m Error Code:\x1b[0m \x1b[1;93m0x%016llx\x1b[0m                                           \x1b[38;5;130m║\x1b[0m\n", info->error_code);
  }
  
  Log::print("\x1b[38;5;130m║\x1b[0m                                                                           \x1b[38;5;130m║\n");
  Log::print("\x1b[38;5;130m║\x1b[0m \x1b[1;94m GENERAL PURPOSE REGISTERS                                                \x1b[38;5;130m║\x1b[0m\n");
  Log::print("\x1b[38;5;130m╠═══════════════════════════════════════════════════════════════════════════╣\x1b[0m\n");
  Log::printf("\x1b[38;5;130m║\x1b[0m   \x1b[96mRAX:\x1b[0m \x1b[1;92m0x%016llx\x1b[0m  \x1b[96mRBX:\x1b[0m \x1b[1;92m0x%016llx\x1b[0m                        \x1b[38;5;130m║\x1b[0m\n", info->rax, info->rbx);
  Log::printf("\x1b[38;5;130m║\x1b[0m   \x1b[96mRCX:\x1b[0m \x1b[1;92m0x%016llx\x1b[0m  \x1b[96mRDX:\x1b[0m \x1b[1;92m0x%016llx\x1b[0m                        \x1b[38;5;130m║\x1b[0m\n", info->rcx, info->rdx);
  Log::printf("\x1b[38;5;130m║\x1b[0m   \x1b[96mRSI:\x1b[0m \x1b[1;92m0x%016llx\x1b[0m  \x1b[96mRDI:\x1b[0m \x1b[1;92m0x%016llx\x1b[0m                        \x1b[38;5;130m║\x1b[0m\n", info->rsi, info->rdi);
  Log::printf("\x1b[38;5;130m║\x1b[0m   \x1b[96mRSP:\x1b[0m \x1b[1;92m0x%016llx\x1b[0m  \x1b[96mRBP:\x1b[0m \x1b[1;92m0x%016llx\x1b[0m                        \x1b[38;5;130m║\x1b[0m\n", info->rsp, info->rbp);
  Log::printf("\x1b[38;5;130m║\x1b[0m   \x1b[96mR8: \x1b[0m \x1b[1;92m0x%016llx\x1b[0m  \x1b[96mR9: \x1b[0m \x1b[1;92m0x%016llx\x1b[0m                        \x1b[38;5;130m║\x1b[0m\n", info->r8, info->r9);
  Log::printf("\x1b[38;5;130m║\x1b[0m   \x1b[96mR10:\x1b[0m \x1b[1;92m0x%016llx\x1b[0m  \x1b[96mR11:\x1b[0m \x1b[1;92m0x%016llx\x1b[0m                        \x1b[38;5;130m║\x1b[0m\n", info->r10, info->r11);
  Log::printf("\x1b[38;5;130m║\x1b[0m   \x1b[96mR12:\x1b[0m \x1b[1;92m0x%016llx\x1b[0m  \x1b[96mR13:\x1b[0m \x1b[1;92m0x%016llx\x1b[0m                        \x1b[38;5;130m║\x1b[0m\n", info->r12, info->r13);
  Log::printf("\x1b[38;5;130m║\x1b[0m   \x1b[96mR14:\x1b[0m \x1b[1;92m0x%016llx\x1b[0m  \x1b[96mR15:\x1b[0m \x1b[1;92m0x%016llx\x1b[0m                        \x1b[38;5;130m║\x1b[0m\n", info->r14, info->r15);
  
  Log::print("\x1b[38;5;130m║\x1b[0m                                                                           \x1b[38;5;130m║\x1b[0m\n");
  Log::print("\x1b[38;5;130m║\x1b[0m \x1b[1;94m EXECUTION STATE\x1b[0m                                                          \x1b[38;5;130m║\x1b[0m\n");
  Log::print("\x1b[38;5;130m╠═══════════════════════════════════════════════════════════════════════════╣\x1b[0m\n");
  Log::printf("\x1b[38;5;130m║\x1b[0m   \x1b[96mRIP:\x1b[0m \x1b[1;93m0x%016llx\x1b[0m \x1b[95m←\x1b[0m \x1b[1;97mInstruction Pointer\x1b[0m                           \x1b[38;5;130m║\x1b[0m\n", info->rip);
  
  Log::print("\x1b[38;5;130m║\x1b[0m                                                                           \x1b[38;5;130m║\x1b[0m\n");
  Log::print("\x1b[38;5;130m║\x1b[0m \x1b[1;94m  CONTROL REGISTERS\x1b[0m                                                       \x1b[38;5;130m║\x1b[0m\n");
  Log::print("\x1b[38;5;130m╠═══════════════════════════════════════════════════════════════════════════╣\x1b[0m\n");
  Log::printf("\x1b[38;5;130m║\x1b[0m   \x1b[96mCR0:\x1b[0m \x1b[1;92m0x%016llx\x1b[0m  \x1b[96mCR2:\x1b[0m \x1b[1;92m0x%016llx\x1b[0m                        \x1b[38;5;130m║\x1b[0m\n", info->cr0, info->cr2);
  Log::printf("\x1b[38;5;130m║\x1b[0m   \x1b[96mCR3:\x1b[0m \x1b[1;92m0x%016llx\x1b[0m  \x1b[96mCR4:\x1b[0m \x1b[1;92m0x%016llx\x1b[0m                        \x1b[38;5;130m║\x1b[0m\n", info->cr3, info->cr4);
  Log::printf("\x1b[38;5;130m║\x1b[0m   \x1b[96mCR8:\x1b[0m \x1b[1;92m0x%016llx\x1b[0m                                                 \x1b[38;5;130m║\x1b[0m\n", info->cr8);
  
  Log::print("\x1b[38;5;130m║\x1b[0m                                                                           \x1b[38;5;130m║\x1b[0m\n");
  Log::print("\x1b[38;5;130m║\x1b[0m \x1b[1;94m SEGMENT REGISTERS\x1b[0m                                                        \x1b[38;5;130m║\x1b[0m\n");
  Log::print("\x1b[38;5;130m╠═══════════════════════════════════════════════════════════════════════════╣\x1b[0m\n");
  Log::printf("\x1b[38;5;130m║\x1b[0m \x1b[96mCS:\x1b[0m\x1b[1;92m0x%04llx\x1b[0m \x1b[96mDS:\x1b[0m\x1b[1;92m0x%04llx\x1b[0m \x1b[96mES:\x1b[0m\x1b[1;92m0x%04llx\x1b[0m \x1b[96mFS:\x1b[0m\x1b[1;92m0x%04llx\x1b[0m \x1b[96mGS:\x1b[0m\x1b[1;92m0x%04llx\x1b[0m \x1b[96mSS:\x1b[0m\x1b[1;92m0x%04llx\x1b[0m               \x1b[38;5;130m║\x1b[0m\n", 
             info->cs, info->ds, info->es, info->fs, info->gs, info->ss);
  Log::print("\x1b[38;5;130m╚═══════════════════════════════════════════════════════════════════════════╝\x1b[0m\n");
}

void dump_info(struct isr_frame *info) {
  dump_registers(info);
}

void handle_double_fault(struct isr_frame *info) {
  Log::print("\n\x1b[95m╔═══════════════════════════════════════════════════════════════════════════╗\x1b[0m\n");
  Log::print("\x1b[95m║\x1b[0m \x1b[1;91m DOUBLE FAULT - CRITICAL SYSTEM ERROR\x1b[0m                          \x1b[95m║\x1b[0m\n");
  Log::print("\x1b[95m╠═══════════════════════════════════════════════════════════════════════════╣\x1b[0m\n");
  
  u8 ext = info->error_code & 0x01;
  u8 tbl = (info->error_code & 0x07) >> 1;
  u16 index = (info->error_code & 0xFFFF) >> 3;
  
  if (ext) {
    Log::print("\x1b[95m║\x1b[0m \x1b[96m Source:\x1b[0m \x1b[1;93mExternal interrupt\x1b[0m                               \x1b[95m║\x1b[0m\n");
  } else {
    Log::print("\x1b[95m║\x1b[0m \x1b[96m Source:\x1b[0m \x1b[1;93mInternal exception\x1b[0m                               \x1b[95m║\x1b[0m\n");
  }
  
  if (tbl == 0b00) {
    Log::print("\x1b[95m║\x1b[0m \x1b[96m Table:\x1b[0m \x1b[1;93mGDT (Global Descriptor Table)\x1b[0m                   \x1b[95m║\x1b[0m\n");
  } else if (tbl == 0b01 || tbl == 0b11) {
    Log::print("\x1b[95m║\x1b[0m \x1b[96m Table:\x1b[0m \x1b[1;93mIDT (Interrupt Descriptor Table)\x1b[0m               \x1b[95m║\x1b[0m\n");
  } else {
    Log::print("\x1b[95m║\x1b[0m \x1b[96m Table:\x1b[0m \x1b[1;93mLDT (Local Descriptor Table)\x1b[0m                   \x1b[95m║\x1b[0m\n");
  }
  
  Log::printf("\x1b[95m║\x1b[0m \x1b[96m Index:\x1b[0m \x1b[1;93m%d\x1b[0m                                                   \x1b[95m║\x1b[0m\n", index);
  Log::print("\x1b[95m╚═══════════════════════════════════════════════════════════════════════════╝\x1b[0m\n");
}

void handle_invalid_tss(struct isr_frame *info) {
  Log::print("\n\x1b[93m╔═══════════════════════════════════════════════════════════════════════════╗\x1b[0m\n");
  Log::print("\x1b[93m║\x1b[0m \x1b[1;91m INVALID TSS - Task State Segment Error\x1b[0m                       \x1b[93m║\x1b[0m\n");
  Log::print("\x1b[93m╠═══════════════════════════════════════════════════════════════════════════╣\x1b[0m\n");
  Log::printf("\x1b[93m║\x1b[0m \x1b[96m TSS Index:\x1b[0m \x1b[1;93m%lld\x1b[0m                                             \x1b[93m║\x1b[0m\n", info->error_code);
  Log::print("\x1b[93m╚═══════════════════════════════════════════════════════════════════════════╝\x1b[0m\n");
}

void handle_invalid_segment(struct isr_frame *info) {
  Log::print("\n\x1b[94m╔═══════════════════════════════════════════════════════════════════════════╗\x1b[0m\n");
  Log::print("\x1b[94m║\x1b[0m \x1b[1;91m INVALID SEGMENT - Segment Fault\x1b[0m                              \x1b[94m║\x1b[0m\n");
  Log::print("\x1b[94m╠═══════════════════════════════════════════════════════════════════════════╣\x1b[0m\n");
  Log::printf("\x1b[94m║\x1b[0m \x1b[96m Segment Index:\x1b[0m \x1b[1;93m%lld\x1b[0m                                        \x1b[94m║\x1b[0m\n", info->error_code);
  Log::print("\x1b[94m╚═══════════════════════════════════════════════════════════════════════════╝\x1b[0m\n");
}

void handle_gpf(struct isr_frame *info) {
  Log::print("\n\x1b[38;5;130m╔═══════════════════════════════════════════════════════════════════════════╗\x1b[0m\n");
  Log::print("\x1b[38;5;130m║\x1b[0m \x1b[1;93m GENERAL PROTECTION FAULT - Access Violation\x1b[0m                              \x1b[38;5;130m║\x1b[0m\n");
  Log::print("\x1b[38;5;130m╠═══════════════════════════════════════════════════════════════════════════╣\x1b[0m\n");
  Log::printf("\x1b[38;5;130m║\x1b[0m \x1b[96m Segment Index:\x1b[0m \x1b[1;93m0x%04llx\x1b[0m                                                    \x1b[38;5;130m║\x1b[0m\n", info->error_code);
  Log::print("\x1b[38;5;130m╚═══════════════════════════════════════════════════════════════════════════╝\x1b[0m\n");
}

void handle_page_fault(struct isr_frame *info) {
  Log::print("\n\x1b[96m╔═══════════════════════════════════════════════════════════════════════════╗\x1b[0m\n");
  Log::print("\x1b[96m║\x1b[0m \x1b[1;91m PAGE FAULT - Memory Access Violation\x1b[0m                                     \x1b[96m║\x1b[0m\n");
  Log::print("\x1b[96m╠═══════════════════════════════════════════════════════════════════════════╣\x1b[0m\n");
  
  u8 attrs = info->error_code & 0x7F;
  
  Log::printf("\x1b[96m║\x1b[0m \x1b[97m Faulting Address:\x1b[0m \x1b[1;93m0x%016llx\x1b[0m                                     \x1b[96m║\x1b[0m\n", info->cr2);
  Log::print("\x1b[96m║\x1b[0m                                                                           \x1b[96m║\x1b[0m\n");
  
  if ((attrs >> 0) & 0x1) {
    Log::print("\x1b[96m║\x1b[0m \x1b[97m Cause:\x1b[0m \x1b[1;91mPage protection violation\x1b[0m                                         \x1b[96m║\x1b[0m\n");
  } else {
    Log::print("\x1b[96m║\x1b[0m \x1b[97m Cause:\x1b[0m \x1b[1;93mPage not present\x1b[0m                                                  \x1b[96m║\x1b[0m\n");
  }
  
  if ((attrs >> 1) & 0x1) {
    Log::print("\x1b[96m║\x1b[0m \x1b[97m  Access:\x1b[0m \x1b[1;93mWrite operation\x1b[0m                                                 \x1b[96m║\x1b[0m\n");
  } else {
    Log::print("\x1b[96m║\x1b[0m \x1b[97m  Access:\x1b[0m \x1b[1;93mRead operation\x1b[0m                                                   \x1b[96m║\x1b[0m\n");
  }
  
  if ((attrs >> 2) & 0x1) {
    Log::print("\x1b[96m║\x1b[0m \x1b[97m Mode:\x1b[0m \x1b[1;93mUser mode access\x1b[0m                                                       \x1b[96m║\x1b[0m\n");
  } else {
    Log::print("\x1b[96m║\x1b[0m \x1b[97m Mode:\x1b[0m \x1b[1;93mSupervisor mode access\x1b[0m                                             \x1b[96m║\x1b[0m\n");
  }
  
  if ((attrs >> 3) & 0x1) {
    Log::print("\x1b[96m║\x1b[0m \x1b[97m Flags:\x1b[0m \x1b[1;91mReserved bit violation\x1b[0m                            \x1b[96m║\x1b[0m\n");
  }
  if ((attrs >> 4) & 0x1) {
    Log::print("\x1b[96m║\x1b[0m \x1b[97m Type:\x1b[0m \x1b[1;93mInstruction fetch\x1b[0m                                   \x1b[96m║\x1b[0m\n");
  }
  if ((attrs >> 5) & 0x1) {
    Log::print("\x1b[96m║\x1b[0m \x1b[97m Flags:\x1b[0m \x1b[1;91mProtection key violation\x1b[0m                          \x1b[96m║\x1b[0m\n");
  }
  if ((attrs >> 6) & 0x1) {
    Log::print("\x1b[96m║\x1b[0m \x1b[97m  Flags:\x1b[0m \x1b[1;91mShadow Stack access\x1b[0m                               \x1b[96m║\x1b[0m\n");
  }
  
  Log::print("\x1b[96m╚═══════════════════════════════════════════════════════════════════════════╝\x1b[0m\n");
}


void handle_err_code(struct isr_frame *info) {
  if (info->int_num == 0x08) {
    handle_double_fault(info);
  }
  else if (info->int_num == 0x0A) {
    handle_invalid_tss(info);
  }
  else if (info->int_num == 0x0B) {
    handle_invalid_segment(info);
  }
  else if (info->int_num == 0x0D) {
    handle_gpf(info);
  }
  else if (info->int_num == 0x0E) {
    handle_page_fault(info);
  }
}

u64 find_rip(u64 return_address) {
  Addr2LineResult res = DWARF::addr2line_lookup(Err::Handler::kernel_desc(), return_address-1);
  if (!res.found) return return_address;
  return res.address;
}

extern "C" void isr_handler(struct isr_frame *info) {
  if (info->int_num == 0xFF) {
    spurious_handler(info);
    return;
  }

  info->rip = find_rip(info->return_address);
  dump_info(info);

  if (info->int_num == 0x08 || info->int_num == 0x0A || info->int_num == 0x0B || info->int_num == 0x0D || info->int_num == 0x0E) {
      handle_err_code(info);
  }

  u64 disas_addr = info->return_address - info->rip < 0x2 ? info->return_address : info->rip;

  Err::Handler::dump_at_addr(info->rip, (void**)info->rbp, disas_addr);

  __asm__ volatile ("cli; hlt"); // hangs
}

