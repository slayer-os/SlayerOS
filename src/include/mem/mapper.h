#ifndef MEM_MAPPER_H
#define MEM_MAPPER_H
#include <libc/types.h>
#include <bootloader/limine.h>
#include <mem/paging.h>

namespace Mem::Mapper {
  void full_map();
}

#endif
