#pragma once
#include <klib/types.h>
#include <klib/elf.h>

typedef struct symbol_entry {
  u64 address;
  const char *name;
  size_t size;
} symbol_entry_t;

struct search_result {
  symbol_entry_t *entry;
  bool exact;
};

namespace Err::Handler {
  void init_symbols();
  struct search_result resolve_address(u64 address);
  const char *search_repr(u64 address, struct search_result result);
  const char *addrsym_repr(u64 address);

  int dump_at_addr(u64 address);

  struct elf_desc *kernel_desc();
}

