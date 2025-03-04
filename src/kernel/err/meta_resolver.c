#include <err/handler.h>
#include <libc/memory.h>
#include <libc/elf.h>
#include <libc/string.h>
#include <bootloader/limine.h>

#include <dbg/log.h>

#define MAX_SYMBOLS 1024
symbol_entry_t *symbol_table[MAX_SYMBOLS];

void sort_symbol_table() {
  for (u32 i=0;i<MAX_SYMBOLS;i++) {
    if (symbol_table[i] == nullptr) {
      break;
    }
    for (u32 j=i+1;j<MAX_SYMBOLS;j++) {
      if (symbol_table[j] == nullptr) {
        break;
      }
      if (symbol_table[j]->address < symbol_table[i]->address) {
        symbol_entry_t *tmp = symbol_table[i];
        symbol_table[i] = symbol_table[j];
        symbol_table[j] = tmp;
      }
    }
  }
}

void init_symbol_table() {
  struct elf_desc desc;
  elf_parse(&desc, boot_ctx.kernel_file->address, boot_ctx.kernel_file->size);

  u32 table_i=0;
  for (u32 i=0;i<(desc.sects.symtab->sh_size / desc.sects.symtab->sh_entsize);i++) {
    struct elf_sym *sym = &desc.symtab[i];
    if (sym->st_name == 0 || sym->st_info == 0 || ELF64_ST_TYPE(sym->st_info) != STT_FUNC) {
      continue;
    }
    const char *name = desc.strtab + sym->st_name;
    symbol_entry_t *entry = (symbol_entry_t*)kmalloc(sizeof(symbol_entry_t));
    entry->address = sym->st_value;
    entry->name = name;
    symbol_table[table_i++] = entry;
  }
  sort_symbol_table();
}


extern char *demangle_alloc_symbol(const char *name);
char *resolve_address(u64 address) {
  // find the symbol or the closest symbol lower bound (+offset)
  symbol_entry_t *entry = nullptr;
  bool exact = false;
  for (u32 i=0;i<MAX_SYMBOLS;i++) {
    if (symbol_table[i] == nullptr) {
      break;
    }
    if (symbol_table[i]->address == address) {
      entry = symbol_table[i];
      exact = true;
      break;
    }
    if (symbol_table[i]->address > address) {
      break;
    }
    entry = symbol_table[i];
  }
  if (entry == nullptr) {
    return nullptr;
  }
  char buffer[128];
  if (!exact) {
    u64 offset = address - entry->address;
    sprintf(buffer, "%s + %p", entry->name, offset);
  } else {
    memcpy(buffer, entry->name, strlen(entry->name));
  }
  return demangle_alloc_symbol(buffer);
}
