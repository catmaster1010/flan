#include "lib/elf.h"
#include "lib/assert.h"
#include "lib/str.h"

static bool check_headers(Elf64_Ehdr* header){
    if (memcmp(header->e_ident, ELFMAG, 4)) return false;
    if(header->e_ident[EI_CLASS]!=ELFCLASS64) return false;
    return true;
}

bool elf_load(pagemap_t* pagemap, vfs_node_t* node){
    Elf64_Ehdr elf_header;   
    if(vfs_read(node, &elf_header, sizeof(Elf64_Ehdr), 0)<= 0) return false;
    
    if(!check_headers(&elf_header)) return false;
}
