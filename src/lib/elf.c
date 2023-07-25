#include <lib/assert.h>
#include <lib/elf.h>
#include <lib/stddef.h>
#include <lib/str.h>
#include <memory/pmm.h>
#include <memory/vmm.h>
#include <memory/kheap.h>

static inline bool check_headers(Elf64_Ehdr *header) {
    if (memcmp(header->e_ident, ELFMAG, 4))
        return false;
    if (header->e_ident[EI_CLASS] != ELFCLASS64)
        return false;
    if (header->e_ident[EI_DATA] != ELFDATA2LSB)
        return false;
    if (header->e_type != ET_EXEC)
        return false;
    if (header->e_machine != EM_AMD64)
        return false;
    return true;
}

bool elf_load(pagemap_t *pagemap, vfs_node_t *node, struct auxval *aux, const char** ld_path) {
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
        if (program_header.p_type == PT_LOAD) {

            uint64_t prot = PTE_PRESENT;
            if (program_header.p_flags & PF_W)
                prot |= PTE_WRITABLE;
            if (program_header.p_flags & PF_X)
                prot |= PTE_USER;

            uint64_t unaligned = program_header.p_vaddr & (FRAME_SIZE - 1);
            uint64_t pages =
                ALIGN_UP(program_header.p_memsz + unaligned, FRAME_SIZE) /
                FRAME_SIZE;
            void *phys = pmm_calloc(pages);
            if (!phys)
                return false;

            vmm_map_pages(pagemap, (uintptr_t)phys, program_header.p_vaddr,
                          prot, pages);

            assert(vfs_read(node, phys + unaligned + HHDM_OFFSET,
                            program_header.p_filesz, program_header.p_offset));
        }
        else if(program_header.p_type == PT_INTERP){
            void* path = kheap_calloc(program_header.p_filesz + 1);
            assert(path); 
            assert(vfs_read(node, path, program_header.p_filesz, program_header.p_offset));
            if (ld_path) *ld_path = path;


        }
    }
    aux->at_entry = elf_header.e_entry;
    return true;
}
