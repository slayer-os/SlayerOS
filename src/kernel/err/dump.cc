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

void dump_address(u64 value) {
  Log::print("\x1b[33m");
  char buf[17];
  buf[16] = '\0';

  for (int i = 15; i >= 0; --i) {
    uint8_t nibble = value & 0xF;
    buf[i] = nibble < 10 ? ('0' + nibble) : ('A' + nibble - 10);
    value >>= 4;
  }
  Log::print(buf);
}

int Err::Handler::dump_at_addr(u64 address) {
  struct search_result result = Err::Handler::resolve_address(address);
  if (result.entry == nullptr) return -1;
  const char *fn_repr = Err::Handler::search_repr(address, result);
  Addr2LineResult a2l_res = DWARF::addr2line_lookup(Err::Handler::kernel_desc(), address);

  Log::print("\n▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰\n\n");
  Log::printf(" ≻ \x1b[32m%s:\n\x1b[0m", fn_repr);
  if (a2l_res.found) {
    Log::printf("  ⤷  \x1b[34m%s\x1b[0m:\x1b[36m%d\x1b[0m\n", a2l_res.file, a2l_res.line);
  }
  Log::print("\n");

  const u64 SYMBOL_START_THRESHOLD = 96;
  u64 offset_from_symbol = address - result.entry->address;
  
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
      
      if (runtime_address >= address) {
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
    if (runtime_address == address) {
      Log::print("\x1b[4m• ");
    }
    dump_address(runtime_address);
    Log::printf("\x1b[34m  %s", instruction.text);
    
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
                Log::printf("  \x1b[36m; %s\x1b[0m", symbol_repr);
              }
            }
          }
          break;
        }
      }
    }
    
    Log::print("\n\x1b[0m");
    offset += instruction.info.length;
    runtime_address += instruction.info.length;
    if (i >= instructions_to_show - 1) break;
    i++;
  }
  Log::print("\n▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰▰\n\n");
  return 0;
}
