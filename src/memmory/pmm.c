#include "std/stdio.h"
#include "memmory/pmm.h"
#include <limine.h>
#include "std/stddef.h"

#define ALIGN_UP(num,align) (((num) + align - 1) & ~(align - 1))
#define ALIGN_DOWN(num,align) ((num) & ~(align - 1))


#define container_of(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})

static dll_t* free_list;

volatile struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
    };

void pmm_init()
{
    printf("There are %d entries in the mmap\n", memmap_request.response->entry_count);
    for (int i = 0; i < memmap_request.response->entry_count; i++)
    {
        printf("entry %d    base: %x    length: %x    type: %d    tail: %x\n",
               i+1,
               memmap_request.response->entries[i]->base,
               memmap_request.response->entries[i]->length,
               memmap_request.response->entries[i]->type,
               memmap_request.response->entries[i]->base+memmap_request.response->entries[i]->length
               );
        if (memmap_request.response->entries[i]->type ==0)
        {
           add_block(memmap_request.response->entries[i]->base,memmap_request.response->entries[i]->length);
        }
    }   
}

void add_block(uint64_t *addr, uint64_t size){
    alloc_node_t *block = (uint64_t *) ALIGN_UP((uint64_t)addr, 8);
    //actual size - alignment-headerspace 
    block->size = addr + size - (uint64_t)block - 24;
    
    free_list->next->prev = &block->node;
	block->node.next = free_list->next;
	block->node.prev = free_list;
	free_list->next = &block->node.next;
};



void malloc(uint64_t size){
    dll_t* temp = &free_list;
    
 
}
