#ifndef LIBC_ELF_H
#define LIBC_ELF_H
#include <libc/types.h>

struct elf_ident {
  char magic[4];
  u8 class;
  u8 endianness;
  u8 version;
  u8 os_abi;
  u8 abi_version;
  u8 pad[7];
};

struct elf_header {
  struct elf_ident e_ident;
  u16 e_type;
  u16 e_machine;
  u32 e_version;
  u64 e_entry;
  u64 e_phoff;
  u64 e_shoff;
  u32 e_flags;
  u16 e_ehsize;
  u16 e_phentsize;
  u16 e_phnum;
  u16 e_shentsize;
  u16 e_shnum;
  u16 e_shstrndx;
};

struct elf_phdr {
  u32 p_type;
  u32 p_flags;
  u64 p_offset;
  u64 p_vaddr;
  u64 p_paddr;
  u64 p_filesz;
  u64 p_memsz;
  u64 p_align;
};

struct elf_shdr {
  u32 sh_name;
  u32 sh_type;
  u64 sh_flags;
  u64 sh_addr;
  u64 sh_offset;
  u64 sh_size;
  u32 sh_link;
  u32 sh_info;
  u64 sh_addralign;
  u64 sh_entsize;
};

struct elf_sym {
  u32 st_name;
  u8 st_info;
  u8 st_other;
  u16 st_shndx;
  u64 st_value;
  u64 st_size;
};

struct elf_dyn {
  u64 d_tag;
  union {
    u64 d_val;
    u64 d_ptr;
  } d_un;
};

enum elf_type {
  ET_NONE = 0,
  ET_REL = 1,
  ET_EXEC = 2,
  ET_DYN = 3,
  ET_CORE = 4,
  ET_LOPROC = 0xff00,
  ET_HIPROC = 0xffff
};

enum elf_ptype {
  PT_NULL = 0,
  PT_LOAD = 1,
  PT_DYNAMIC = 2,
  PT_INTERP = 3,
  PT_NOTE = 4,
  PT_SHLIB = 5,
  PT_PHDR = 6,
  PT_LOPROC = 0x70000000,
  PT_HIPROC = 0x7fffffff
};

enum elf_stype {
  SHT_NULL = 0,
  SHT_PROGBITS = 1,
  SHT_SYMTAB = 2,
  SHT_STRTAB = 3,
  SHT_RELA = 4,
  SHT_HASH = 5,
  SHT_DYNAMIC = 6,
  SHT_NOTE = 7,
  SHT_NOBITS = 8,
  SHT_REL = 9,
  SHT_SHLIB = 10,
  SHT_DYNSYM = 11,
  SHT_LOPROC = 0x70000000,
  SHT_HIPROC = 0x7fffffff,
  SHT_LOUSER = 0x80000000,
  SHT_HIUSER = 0xffffffff
};

enum elf_shtype {
  STB_LOCAL = 0,
  STB_GLOBAL = 1,
  STB_WEAK = 2,
  STB_LOPROC = 13,
  STB_HIPROC = 15
};

enum elf_shflags {
  SHF_WRITE = 0x1,
  SHF_ALLOC = 0x2,
  SHF_EXECINSTR = 0x4,
  SHF_MERGE = 0x10,
  SHF_STRINGS = 0x20,
  SHF_INFO_LINK = 0x40,
  SHF_LINK_ORDER = 0x80,
  SHF_OS_NONCONFORMING = 0x100,
  SHF_GROUP = 0x200,
  SHF_TLS = 0x400,
  SHF_MASKOS = 0x0ff00000,
  SHF_MASKPROC = 0xf0000000,
  SHF_ORDERED = 0x4000000,
  SHF_EXCLUDE = 0x8000000
};

struct elf_desc {
  struct elf_header *header;
  struct elf_phdr *phdrs;
  struct elf_shdr *shdrs;
  struct elf_sym *symtab;
  
  struct common_sects {
    struct elf_shdr *symtab;
  } sects;

  struct debug_sects {
    struct elf_shdr *debug_info;
    struct elf_shdr *debug_abbrev;
    struct elf_shdr *debug_loclist;
    struct elf_shdr *debug_line;
    struct elf_shdr *debug_str;
    struct elf_shdr *debug_line_str;
  } debug;

  const char *strtab;
  char *shstrtab;
  struct elf_shdr *shstrtab_shdr;
};

#define ELF64_ST_BIND(i) ((i) >> 4)

void elf_parse(struct elf_desc *desc, void *data, size_t size);

#endif
