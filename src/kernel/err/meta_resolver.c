#include <err/handler.h>
#include <libc/memory.h>
#include <libc/elf.h>
#include <libc/string.h>
#include <bootloader/limine.h>

#include <dbg/log.h>

symbol_entry_t *symbol_table = NULL;
symbol_entry_t *last_symbol = NULL;

void init_symbol_table() {
  struct elf_desc desc;
  elf_parse(&desc, boot_ctx.kernel_file->address, boot_ctx.kernel_file->size);

  for (u32 i=0;i<(desc.sects.symtab->sh_size / desc.sects.symtab->sh_entsize);i++) {
    struct elf_sym *sym = &desc.symtab[i];
    if (sym->st_name == 0 || sym->st_info == 0 || ELF64_ST_TYPE(sym->st_info) != STT_FUNC) {
      continue;
    }
    const char *name = desc.strtab + sym->st_name;
    symbol_entry_t *entry = kmalloc(sizeof(symbol_entry_t));
    entry->address = sym->st_value;
    entry->name = name;
    entry->next = NULL;
    if (symbol_table == NULL) {
      symbol_table = entry;
      last_symbol = entry;
    } else {
      last_symbol->next = entry;
      last_symbol = entry;
    }
  }
}

char *resolve_address(u64 address) {
  symbol_entry_t *entry = symbol_table;
  symbol_entry_t *nearest = symbol_table;
  while (entry != NULL) {
    if (entry->address == address) {
      return (char*)entry->name;
    }
    if (entry->address < address && entry->address > nearest->address) {
      nearest = entry;
    }
    entry = entry->next;
  }
  char *name = kmalloc(strlen(nearest->name) + 16);
  sprintf(name, "%s+%p", nearest->name, address - nearest->address);
  return name;
}
