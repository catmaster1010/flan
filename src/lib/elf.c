#include "lib/elf.h"
#include "lib/assert.h"
#include "lib/str.h"

bool elf_load(pagemap_t* pagemap, vfs_node_t* node){
    Elf64_Ehdr header;   
    if(vfs_read(node, &header, sizeof(header), 0)<= 0);
    assert(memcmp(header.e_ident,ELFMAG,4));
    printf(" HEADER %s\n",header);
}
