#ifndef ERROR_HANDLER_H
#define ERORR_HANDLER_H
#include <libc/types.h>

typedef struct symbol_entry {
  u64 address;
  const char *name;
  struct symbol_entry *next;
} symbol_entry_t;

void init_symbol_table();
char *resolve_address(u64 address);

#endif
