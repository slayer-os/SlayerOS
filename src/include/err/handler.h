#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H
#include <libc/types.h>

typedef struct symbol_entry {
  u64 address;
  const char *name;
} symbol_entry_t;

namespace Err::Handler {
  void init_symbols();
  char *resolve_address(u64 address);
}

#endif
