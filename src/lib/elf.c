#include <lib/assert.h>
#include <lib/elf.h>
#include <lib/stddef.h>
#include <lib/str.h>
#include <memory/kheap.h>
#include <memory/pmm.h>
#include <memory/vmm.h>

static inline bool check_headers(Elf64_Ehdr *header) {
    if (memcmp(header->e_ident, ELFMAG, 4))
        return false;
    if (header->e_ident[EI_CLASS] != ELFCLASS64)
        return false;
    if (header->e_ident[EI_DATA] != ELFDATA2LSB)
        return false;
    if (header->e_machine != EM_X86_64)
        return false;
    return true;
}

bool elf_load(pagemap_t *pagemap, vfs_node_t *node, struct auxval *aux,
              const char **ld_path) {
    Elf64_Ehdr elf_header;

    if (vfs_read(node, &elf_header, sizeof(Elf64_Ehdr), 0) <= 0)
        return false;

    if (!check_headers(&elf_header))
        return false;

    for (uint64_t i = 0; i < elf_header.e_phnum; i++) {
        Elf64_Phdr program_header;
        if (vfs_read(node, &program_header, sizeof(Elf64_Phdr),
                     elf_header.e_phoff + i * elf_header.e_phentsize) <= 0)
            return false;

        switch (program_header.p_type) {
        case PT_LOAD: {

            /*
            uint64_t prot = PROT_READ;
            if (program_header.p_flags & PF_W)
                prot |= PROT_WRITE;
            if (program_header.p_flags & PF_X)
                prot |= PROT_EXEC;
            */

            uint64_t unaligned = program_header.p_vaddr & (FRAME_SIZE - 1);
            uint64_t pages =
                ALIGN_UP(program_header.p_memsz + unaligned, FRAME_SIZE) /
                FRAME_SIZE;
            void *phys = pmm_calloc(pages);
            if (!phys)
                return false;

            vmm_map_pages(pagemap, (uintptr_t)phys, program_header.p_vaddr,
                          PTE_PRESENT | PTE_WRITABLE | PTE_USER, pages);

            assert(vfs_read(node, phys + unaligned + HHDM_OFFSET,
                            program_header.p_filesz, program_header.p_offset));
            break;
        }
        case PT_INTERP: {
            void *path = kheap_calloc(program_header.p_filesz + 1);
            assert(path);
            assert(vfs_read(node, path, program_header.p_filesz,
                            program_header.p_offset));
            if (ld_path)
                *ld_path = path;
            break;
        }
        case PT_PHDR: {
            aux->at_phdr = program_header.p_vaddr;
            break;
        }
        }
    }
    aux->at_entry = elf_header.e_entry;
    aux->at_phent = elf_header.e_phentsize;
    aux->at_phnum = elf_header.e_phnum;
    return true;
}
