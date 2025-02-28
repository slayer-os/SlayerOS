#include <libc/elf.h>
#include <libc/assert.h>
#include <libc/string.h>
#include <libc/dwarf.h>

#include <dbg/log.h>


void elf_parse(struct elf_desc *desc, void *data, size_t size) {
  assert(size >= sizeof(struct elf_header), "Invalid ELF file");
  desc->header = (struct elf_header *)data;
  assert(*((u32*)desc->header->e_ident.magic) == 0x464c457f, "Invalid ELF magic");
  desc->phdrs = (struct elf_phdr *)(data + desc->header->e_phoff);
  desc->shdrs = (struct elf_shdr *)(data + desc->header->e_shoff);
  desc->symtab = NULL;
  desc->strtab = NULL;
  desc->shstrtab = NULL;
  desc->shstrtab_shdr = NULL;

  struct elf_shdr *possible_dbg[16];
  int dbg_count = 0;
  for (size_t i = 0; i < desc->header->e_shnum; i++) {
    struct elf_shdr *shdr = &desc->shdrs[i];
    if (shdr->sh_type == SHT_SYMTAB) {
      desc->symtab = (struct elf_sym *)(data + shdr->sh_offset);
      desc->strtab = (const char *)(data + desc->shdrs[shdr->sh_link].sh_offset);
      desc->sects.symtab = shdr;
    } else if (shdr->sh_type == SHT_STRTAB && i == desc->header->e_shstrndx) {
      desc->shstrtab = (char *)(data + shdr->sh_offset);
      desc->shstrtab_shdr = shdr;
    }
    if (shdr->sh_type == SHT_PROGBITS
    && (shdr->sh_flags == 0 || shdr->sh_flags == (SHF_MERGE | SHF_STRINGS))
    && shdr->sh_addralign == 1) {

      possible_dbg[dbg_count++] = shdr;
    }
  }

  for (int i = 0; i < dbg_count; i++) {
    struct elf_shdr *shdr = possible_dbg[i];
    if (!shdr) break;
    char *name = desc->shstrtab + shdr->sh_name;
    if (strcmp(name, ".debug_info") == 0) {
      desc->debug.debug_info = shdr;
    } else if (strcmp(name, ".debug_abbrev") == 0) {
      desc->debug.debug_abbrev = shdr;
    } else if (strcmp(name, ".debug_loclist") == 0) {
      desc->debug.debug_loclist = shdr;
    } else if (strcmp(name, ".debug_line") == 0) {
      desc->debug.debug_line = shdr;
    } else if (strcmp(name, ".debug_str") == 0) {
      desc->debug.debug_str = shdr;
    } else if (strcmp(name, ".debug_line_str") == 0) {
      desc->debug.debug_line_str = shdr;
    }
  }

  assert(desc->symtab != NULL, "No symbol table found");
  assert(desc->strtab != NULL, "No string table found");
  assert(desc->shstrtab != NULL, "No section header string table found");
  assert(desc->shstrtab_shdr != NULL, "No section header string table section found");

  log_debug("ELF file parsed");
  //elf_symbol_dump(desc);
}
