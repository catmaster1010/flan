#ifndef elf_h
#define elf_h
#include <fs/vfs.h>
#include <memory/vmm.h>
#include <stdint.h>
#include <elf.h>

struct auxval {
    uint64_t at_entry;
    uint64_t at_phdr;
    uint64_t at_phent;
    uint64_t at_phnum;
};
bool elf_load(pagemap_t *pagemap, vfs_node_t *node, struct auxval *aux,
              const char **ld_path);
#endif
