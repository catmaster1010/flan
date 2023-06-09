#include "lib/elf.h"
#include "lib/assert.h"
#include "lib/str.h"
#include "memory/vmm.h"
#include "memory/pmm.h"

static inline bool check_headers(Elf64_Ehdr* header){
    if (memcmp(header->e_ident, ELFMAG, 4)) return false;
    if(header->e_ident[EI_CLASS]!=ELFCLASS64) return false;
    if(header->e_ident[EI_DATA]!=ELFDATA2LSB) return false;
    if(header->e_type!=ET_EXEC) return false;
    if(header->e_machine!=EM_AMD64) return false;
    return true;
}

bool elf_load(pagemap_t* pagemap, vfs_node_t* node,struct auxval* aux){
    Elf64_Ehdr elf_header;   
    if(vfs_read(node, &elf_header, sizeof(Elf64_Ehdr), 0)<= 0) return false;
    
    if(!check_headers(&elf_header)) return false;
    
    for (uint64_t i = 0; i < elf_header.e_phnum; i++) {
        Elf64_Phdr program_header;
        if (vfs_read(node, &program_header, sizeof(Elf64_Phdr), elf_header.e_phoff + i * elf_header.e_phentsize) <= 0) return false;
        if (program_header.p_type == PT_LOAD) {
            
            uint64_t prot = PTE_PRESENT;
            if (program_header.p_flags & PF_W) prot |= PTE_WRITABLE;
            if (program_header.p_flags & PF_X) prot |= PTE_USER;
            
            uint64_t unaligned = program_header.p_vaddr & (FRAME_SIZE-1);
            uint64_t pages = ALIGN_UP(program_header.p_memsz + unaligned,FRAME_SIZE) / FRAME_SIZE;
            void* phys = pmm_calloc(pages);
            assert(phys);

            vmm_map_pages(pagemap, phys, program_header.p_vaddr, 0b111, pages);
            
            assert(vfs_read(node, phys + unaligned + HHDM_OFFSET, program_header.p_filesz, program_header.p_offset));
            
        }

    }
    aux->at_entry = elf_header.e_entry;
    
}
