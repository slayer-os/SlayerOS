#include <err/handler.h>
#include <dbg/log.h>
#include <Zydis/Zydis.h>
#include <klib/dwarf.h>

extern "C" void __stack_chk_fail(void) {
  Log::debug("Stack check failed");
  while (1);
}
extern "C" void __assert_fail(const char*, const char*, unsigned, const char*) {
  Log::debug("Failed assertion");
  while (1);
}
extern "C" void *__memcpy_chk(void *dest, const void *src, usize len, usize destlen) {
  (void)destlen;
  uint8_t *d = (uint8_t *)dest;
  const uint8_t *s = (const uint8_t *)src;
  for (usize i = 0; i < len; ++i)
      d[i] = s[i];
  return dest;
}

void dump_address(u64 value) {
  Log::print("\x1b[33;1m0x");
  char buf[17];
  buf[16] = '\0';

  for (int i = 15; i >= 0; --i) {
    uint8_t nibble = value & 0xF;
    buf[i] = nibble < 10 ? ('0' + nibble) : ('A' + nibble - 10);
    value >>= 4;
  }
  Log::print(buf);
  Log::print("\x1b[0m");
}

void Err::Handler::backtrace(void **rbp) {
  for (int i = 0; i < 16 && rbp; ++i) {
    void* rip = *(rbp + 1);
    if (!rip) break;

    if (rip > (void*)0xffffffff80000000) {
      Addr2LineResult a2l_res = DWARF::addr2line_lookup(Err::Handler::kernel_desc(), (u64)rip-1);
      
      if (a2l_res.found) {
        struct search_result result = Err::Handler::resolve_address(a2l_res.address);
        const char *fn_repr = Err::Handler::search_repr(a2l_res.address, result);
        Log::printf("\x1b[90m│\x1b[0m   \x1b[90m[%d]\x1b[0m \x1b[31m%s\x1b[0m  @  \x1b[34m%s\x1b[0m:\x1b[36m%d\x1b[0m\n", i, fn_repr, a2l_res.file, a2l_res.line);
      } else {
        Log::printf("\x1b[90m│\x1b[0m   \x1b[90m[%d]\x1b[0m \x1b[31m%p\x1b[0m\n", i, rip);
      }
    }
    rbp = (void**)(*rbp);
  }
}

int Err::Handler::dump_at_addr(u64 address, void **rbp, u64 disas_address) {
  if (rbp == nullptr)
    rbp = (void**)__builtin_frame_address(0);
  if (disas_address == 0)
    disas_address = address;
  struct search_result result = Err::Handler::resolve_address(address);
  if (result.entry == nullptr) return -1;
  const char *fn_repr = Err::Handler::search_repr(address, result);
  Addr2LineResult a2l_res = DWARF::addr2line_lookup(Err::Handler::kernel_desc(), address);

  Log::print("\n\x1b[90m┌────────────────────────────────────────────────────────────────────────────────────────┐\x1b[0m\n");
  Log::printf("\x1b[90m│\x1b[0m \x1b[1;32m► %s\x1b[0m\n", fn_repr);
  if (a2l_res.found) {
    Log::printf("\x1b[90m│\x1b[0m   \x1b[34m%s\x1b[0m:\x1b[36m%d\x1b[0m\n", a2l_res.file, a2l_res.line);
  }
  Log::print("\x1b[90m│\x1b[0m\n");


  Log::printf("\x1b[90m│\x1b[0m \x1b[1;32mBacktrace:\x1b[0m\n");
  Err::Handler::backtrace(rbp);
  Log::print("\x1b[90m│\x1b[0m\n");

  const u64 SYMBOL_START_THRESHOLD = 96;
  u64 offset_from_symbol = disas_address - result.entry->address;
  
  ZyanU8 *data = (ZyanU8 *)result.entry->address;
  ZyanU64 runtime_address;
  ZyanUSize offset = 0;
  ZydisDisassembledInstruction instruction;
  int instructions_to_show = 7;
  
  if (offset_from_symbol <= SYMBOL_START_THRESHOLD) {
    runtime_address = result.entry->address;
    offset = 0;
  } else {
    runtime_address = result.entry->address;
    offset = 0;
    
    u64 instruction_addresses[32];
    int instruction_count = 0;
    
    while (ZYAN_SUCCESS(ZydisDisassembleIntel(
          ZYDIS_MACHINE_MODE_LONG_64,
          runtime_address,
          data + offset,
          result.entry->size - offset,
          &instruction
      ))) {
      instruction_addresses[instruction_count % 32] = runtime_address;
      instruction_count++;
      
      if (runtime_address >= disas_address) {
        break;
      }
      
      offset += instruction.info.length;
      runtime_address += instruction.info.length;
    }
    
    int start_index = instruction_count >= 4 ? instruction_count - 4 : 0;
    u64 start_address = instruction_addresses[start_index % 32];
    
    runtime_address = result.entry->address;
    offset = 0;
    while (runtime_address < start_address && 
           ZYAN_SUCCESS(ZydisDisassembleIntel(
             ZYDIS_MACHINE_MODE_LONG_64,
             runtime_address,
             data + offset,
             result.entry->size - offset,
             &instruction
           ))) {
      offset += instruction.info.length;
      runtime_address += instruction.info.length;
    }
  }

  int i = 0;
  while (ZYAN_SUCCESS(ZydisDisassembleIntel(
        /* machine_mode:    */ ZYDIS_MACHINE_MODE_LONG_64,
        /* runtime_address: */ runtime_address,
        /* buffer:          */ data + offset,
        /* length:          */ result.entry->size - offset,
        /* instruction:     */ &instruction
    ))) {
    if (disas_address >= runtime_address && disas_address < runtime_address + instruction.info.length) {
      Log::print("\x1b[90m│\x1b[0m \x1b[41;1;37m▶ ");
      dump_address(runtime_address);
      Log::printf("  %s\x1b[0m", instruction.text);
    } else {
      Log::print("\x1b[90m│\x1b[0m   ");
      dump_address(runtime_address);
      Log::printf("\x1b[90m  %s\x1b[0m", instruction.text);
    }
    
    bool is_call = (instruction.info.mnemonic == ZYDIS_MNEMONIC_CALL);
    bool is_lea = (instruction.info.mnemonic == ZYDIS_MNEMONIC_LEA);
    bool is_jmp = (instruction.info.mnemonic == ZYDIS_MNEMONIC_JMP);
    bool is_mov = (instruction.info.mnemonic == ZYDIS_MNEMONIC_MOV);
    
    if (is_call || is_lea || is_jmp || is_mov) {
      for (int op_idx = 0; op_idx < instruction.info.operand_count; op_idx++) {
        const ZydisDecodedOperand* operand = &instruction.operands[op_idx];
        u64 target_address = 0;
        bool has_target = false;
        
        if (operand->type == ZYDIS_OPERAND_TYPE_IMMEDIATE) {
          if (operand->imm.is_relative) {
            target_address = runtime_address + instruction.info.length + operand->imm.value.s;
            has_target = true;
          } else {
            target_address = operand->imm.value.u;
            has_target = true;
          }
        } else if (operand->type == ZYDIS_OPERAND_TYPE_MEMORY) {
          if (operand->mem.base == ZYDIS_REGISTER_RIP && operand->mem.disp.size > 0) {
            target_address = runtime_address + instruction.info.length + operand->mem.disp.value;
            has_target = true;
          }
        }
        
        if (has_target && target_address >= 0xffffffff80000000ULL) {
          struct search_result symbol_result = Err::Handler::resolve_address(target_address);
          if (symbol_result.entry != nullptr && symbol_result.exact) {
            const char* symbol_repr = Err::Handler::search_repr(target_address, symbol_result);
            if (symbol_repr && symbol_repr[0] != '\0') {
              if (symbol_repr[0] != '0' || symbol_repr[1] != 'x') {
                Log::printf("\x1b[96m  → %s\x1b[0m", symbol_repr);
              }
            }
          }
          break;
        }
      }
    }
    
    Log::print("\n");
    offset += instruction.info.length;
    runtime_address += instruction.info.length;
    if (i >= instructions_to_show - 1) break;
    i++;
  }
  Log::print("\x1b[90m└────────────────────────────────────────────────────────────────────────────────────────┘\x1b[0m\n\n");
  return 0;
}
