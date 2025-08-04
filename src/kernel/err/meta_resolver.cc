#include <err/handler.h>
#include <klib/memory.h>
#include <klib/elf.h>
#include <klib/string.h>
#include <bootloader/limine.h>

#include <dbg/log.h>

#define MAX_SYMBOLS 1024
symbol_entry_t *symbol_table[MAX_SYMBOLS];
struct elf_desc kernel_elf_desc;

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

void Err::Handler::init_symbols() {
  elf_parse(&kernel_elf_desc, boot_ctx.kernel_file->address, boot_ctx.kernel_file->size);

  u32 table_i=0;
  for (u32 i=0;i<(kernel_elf_desc.sects.symtab->sh_size / kernel_elf_desc.sects.symtab->sh_entsize);i++) {
    struct elf_sym *sym = &kernel_elf_desc.symtab[i];
    if (sym->st_name == 0 || sym->st_info == 0 || ELF64_ST_TYPE(sym->st_info) != STT_FUNC) {
      continue;
    }
    const char *name = kernel_elf_desc.strtab + sym->st_name;
    symbol_entry_t *entry = (symbol_entry_t*)kmalloc(sizeof(symbol_entry_t));
    entry->address = sym->st_value;
    entry->name = name;
    entry->size = sym->st_size;
    symbol_table[table_i++] = entry;
  }
  sort_symbol_table();
}


extern char *demangle_alloc_symbol(const char *name);


struct search_result Err::Handler::resolve_address(u64 address) {
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
    return {nullptr, false};
  }
  return {entry, exact};
}

const char *Err::Handler::search_repr(u64 address, struct search_result result) {
  if (result.entry == nullptr) return "FAILED TO RESOLVE";
  char buffer[128];
  if (!result.exact) {
    u64 offset = address - result.entry->address;
    sprintf(buffer, "%s + %p", result.entry->name, offset);
  } else {
    u32 name_len = strlen(result.entry->name);
    memcpy(buffer, result.entry->name, name_len);
    buffer[name_len] = '\0';
  }
  return demangle_alloc_symbol(buffer);
}

const char *Err::Handler::addrsym_repr(u64 address) {
  struct search_result result = Err::Handler::resolve_address(address);
  return Err::Handler::search_repr(address, result);
}

struct elf_desc *Err::Handler::kernel_desc() {
  return &kernel_elf_desc;
}
