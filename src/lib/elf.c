#include "lib/elf.h"
#include "lib/assert.h"
#include "lib/str.h"

static bool check_headers(Elf64_Ehdr* header){
    if (memcmp(header->e_ident, ELFMAG, 4)) return false;
    if(header->e_ident[EI_CLASS]!=ELFCLASS64) return false;
    if(header->e_ident[EI_DATA]!=ELFDATA2LSB) return false;
    if(header->e_type!=ET_EXEC) return false;
    if(header->e_machine!=EM_AMD64) return false;
    return true;
}

bool elf_load(pagemap_t* pagemap, vfs_node_t* node){
    Elf64_Ehdr elf_header;   
    if(vfs_read(node, &elf_header, sizeof(Elf64_Ehdr), 0)<= 0) return false;
    
    if(!check_headers(&elf_header)) return false;
    
    for (uint64_t i = 0; i < elf_header.e_phnum; i++) {
        Elf64_Phdr program_header;
        if (vfs_read(node, &program_header, sizeof(Elf64_Phdr), elf_header.e_phoff + i * elf_header.e_phentsize) <= 0) return false;
    }
}
