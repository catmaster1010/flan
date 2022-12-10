#include "std/stdio.h"
#include "memmory/pmm.h"
#include <limine.h>
#include "std/stddef.h"

#define HEADER_SIZE 24
static dll_t free_list={.next=&free_list,.prev=&free_list};

volatile struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
    };

void dll_list_add(dll_t* n, dll_t* prev, dll_t* next){
	next->prev = n;
	n->next = next;
	n->prev = prev;
	prev->next = n;
}

void pmm_init()
{
    printf("There are %d entries in the mmap.\n", memmap_request.response->entry_count);
    for (int i = 0; i < memmap_request.response->entry_count; i++)
    {
        if (memmap_request.response->entries[i]->type ==0)
        {
            alloc_node_t *block = (uint64_t *) ALIGN_UP((uint64_t)memmap_request.response->entries[i]->base, 8); 
            block->size = memmap_request.response->entries[i]->base + memmap_request.response->entries[i]->length - (uint64_t)block - HEADER_SIZE;
            dll_list_add(&block->node,&free_list,free_list.next);
        }

        printf("entry %d    base: %d    length: %x    type: %d    tail: %x\n",
                i+1,
                memmap_request.response->entries[i]->base,
                memmap_request.response->entries[i]->length,
                memmap_request.response->entries[i]->type,
                memmap_request.response->entries[i]->base+memmap_request.response->entries[i]->length 
            );
    }   
    printf("%sFrame alocator initialized.%s\n",cBLUE,cNONE); 
    //Testing malloc() and free()
    test_pmm();
}


void add_block(uint64_t *addr, uint64_t size){
    alloc_node_t *block = (uint64_t *) ALIGN_UP((uint64_t)addr, 8);
    //actual size - alignment-headerspace 
    block->size = addr + size - (uint64_t)block - HEADER_SIZE;
    
    dll_list_add(&block->node,&free_list,&free_list.next);
};


uint64_t* malloc(uint64_t size){
    void *ptr;
    alloc_node_t *block;
    // Try to find a big enough block to alloc (First fit)
  for (block = container_of(free_list.next,alloc_node_t,node); &block->node != &free_list; block=container_of(block->node.next,alloc_node_t,node))
    {      
        if (block->size>=size)
        {   
            ptr = &block->cBlock;
            //printf("\nFound block for requested size.\n");
            break;
        }        
    }

    if (!ptr){printf("Could not find block for requested size."); return NULL;}
    //Can block be split

    if((block->size - size) >= HEADER_SIZE)
        {
            alloc_node_t *new_block = (alloc_node_t *)((uint64_t*)((char*)&block->cBlock + size));
            new_block->size = block->size - size - HEADER_SIZE;
            block->size =  size;
            //add new block to list
            dll_list_add(&new_block->node,&block->node,block->node.next);
        }
    //remove block from list since its getting allocated 
    block->node.next->prev =block->node.prev;
    block->node.prev->next=block->node.next;
    //Finally, return pointer to newly allocated adress
    return ptr;  
}


void free(uint64_t ptr){
    alloc_node_t *block, *free_block;
    block = container_of(ptr, alloc_node_t,cBlock);
    for (free_block = container_of(free_list.next,alloc_node_t,node); &free_block->node!= &free_list; free_block=container_of(free_block->node.next,alloc_node_t,node))
    {
       if (free_block>block)
       {
        dll_list_add(&block->node,free_block->node.prev,&free_block->node);
        coalesce_dll(); //prevent fragmentation 
        return;
       }
    }
    dll_list_add(&block->node,&free_block->node,free_block->node.next);
    coalesce_dll();//prevent fragmentation 
}

void coalesce_dll(){
    alloc_node_t *block;
    alloc_node_t *prevBlock=container_of(&free_list,alloc_node_t,node);
    for (block=container_of(free_list.next,alloc_node_t,node); &block->node!= &free_list; block=container_of(block->node.next,alloc_node_t,node)){

        if ((uint64_t*)((char*)prevBlock+prevBlock->size+HEADER_SIZE)==block)
        {
            prevBlock->size+=block->size+HEADER_SIZE;
            //removes block
            block->node.next->prev =block->node.prev;
            block->node.prev->next=block->node.next;
            continue;
        }
        prevBlock=block;
    }
}

void test_pmm(){
    printf("Testing pmm...\n\n");
    printf("Allocating 6 bytes 2 times...\n\n");
    int *a=malloc(6);printf("Adress of malloc: %d\n",a);
    int *b=malloc(6);printf("Adress of malloc: %d\n",b);
    printf("Freeing the last 2 malloc()...\n\n");
    free(a);
    free(b);
    printf("Allocating 6 bytes 1 times...\n\n");a=malloc(6);printf("Adress of malloc: %d\n",a);
    /*
    for (alloc_node_t* block=container_of(free_list.next,alloc_node_t,node); &block->node!= &free_list; block=container_of(block->node.next,alloc_node_t,node)){
        printf("%d\n",block);
     }*/
    printf("Done pmm test.\n");
}