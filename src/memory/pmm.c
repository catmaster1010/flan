#include "lib/stdio.h"
#include "memory/pmm.h"
#include <limine.h>
#include "lib/stddef.h"
#include "lib/assert.h"
#include  "lib/str.h"
#include "lib/lock.h"
#include "memory/vmm.h"
volatile struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
    };

uint8_t* bitmap;
uint64_t limit;

spinlock_t pmm_lock;

void pmm_init()
{
    uint64_t usable=0;
    uint64_t available=0;
    uint64_t  highest;
    struct  limine_memmap_entry **mmaps = memmap_request.response->entries;
    uint64_t mmmap_count = memmap_request.response->entry_count;

    printf("There are %d entries in the mmap.\n", memmap_request.response->entry_count);
    for (int i = 0; i < mmmap_count; i++)
    {
        available+=mmaps[i]->length;

        printf("entry %d    base: %x    length: %x    type: %d    tail: %x\n",
                i+1,
                mmaps[i]->base,
                mmaps[i]->length,
                mmaps[i]->type,
                mmaps[i]->base+mmaps[i]->length 
            );
        if (!mmaps[i]->type ==LIMINE_MEMMAP_USABLE){continue;}
        usable+=mmaps[i]->length;
        uint64_t top = mmaps[i]->base+mmaps[i]->length;
        if (top > highest) highest = top;
    }   
    limit=highest/FRAME_SIZE;
    uint64_t bitmap_size=ALIGN_UP(highest/FRAME_SIZE/8, FRAME_SIZE);

    for (uint64_t i = 0; i < mmmap_count; i++)
    {
        if (!mmaps[i]->type == LIMINE_MEMMAP_USABLE) continue;;
        if (mmaps[i]->length >=bitmap_size)
        {
            bitmap  = (uint8_t*) mmaps[i]->base;
            memset(bitmap, 0xff, bitmap_size);
            mmaps[i]->length -= bitmap_size;
            mmaps[i]->base += bitmap_size;
            available-=bitmap_size;
            break;
        }
    }
    for (uint64_t i = 0; i < mmmap_count; i++)
    {
        if (!mmaps[i]->type == LIMINE_MEMMAP_USABLE) continue;

        for (uint64_t t = 0; t < mmaps[i]->length; t += FRAME_SIZE)
        {
            pmm_free(mmaps[i]->base+t,1);
        }
    }
    uint64_t a=pmm_malloc(6);
    assert(a);
    printf("Adress of malloc: %x\n",a);
    uint64_t b=pmm_malloc(6);
}

void pmm_free(uint64_t ptr,uint64_t frames){
    spinlock_aquire(&pmm_lock);
    for (uint64_t i = 0; i < frames; i++,ptr+=FRAME_SIZE)
    {
        BIT_CLEAR((uintptr_t) ptr / FRAME_SIZE);
    }
    spinlock_release(&pmm_lock);
}

void* pmm_malloc(uint64_t wanted_frames){
    spinlock_aquire(&pmm_lock);
    uint64_t* ptr;
    
    uint64_t available_frames;
    for (uint64_t frame = 1; frame < limit; frame++)
    {
        if (!BIT_TEST(frame)) {
            available_frames++;
        } else if (available_frames != wanted_frames)
            {available_frames = 0;
            continue;
            }
        if (available_frames == wanted_frames) {
            uint64_t i;
            for  (i = 0; i < wanted_frames; i++)
            {
                BIT_SET(frame-i);
            }
            frame -= i - 1;
            spinlock_release(&pmm_lock);

            ptr=FRAME_SIZE * frame;
            return ptr;
            }
        }
        spinlock_release(&pmm_lock);
        return NULL;
    }
void* pmm_calloc(uint64_t frames){
    void* ptr;
    ptr  = pmm_malloc(1);
    memset(ptr+HHDM_OFFSET,0,frames*FRAME_SIZE);
    return ptr;
}

void *malloc(uint64_t size)
{
    uint64_t frames = (size + (FRAME_SIZE - 1)) / FRAME_SIZE;
    void*  ptr = pmm_calloc(frames+1);
    return ptr;
}
/*
void dll_list_add(dll_t* n, dll_t* prev, dll_t* next){
	next->prev = n;
	n->next = next;
	n->prev = prev;
	prev->next = n;
}
void pmm_init()
{
    uint64_t usable =NULL;
    uint64_t available=NULL;
    printf("There are %d entries in the mmap.\n", memmap_request.response->entry_count);
    for (int i = 0; i < memmap_request.response->entry_count; i++)
    {
        available+=memmap_request.response->entries[i]->length;

        printf("entry %d    base: %x    length: %x    type: %d    tail: %x\n",
                i+1,
                memmap_request.response->entries[i]->base,
                memmap_request.response->entries[i]->length,
                memmap_request.response->entries[i]->type,
                memmap_request.response->entries[i]->base+memmap_request.response->entries[i]->length 
            );
        if (!memmap_request.response->entries[i]->type ==0){continue;}
        usable+=memmap_request.response->entries[i]->length;
        alloc_node_t *block = (uint64_t *) ALIGN_UP((uint64_t)memmap_request.response->entries[i]->base, 8); 
        block->size = memmap_request.response->entries[i]->base + memmap_request.response->entries[i]->length - (uint64_t)block - HEADER_SIZE;
        dll_list_add(&block->node,&free_list,free_list.next);
        
    }   
    printf("%dMiB/%dMiB of usable memmory\n",usable/1024 / 1024,available/1024 / 1024);
    printf("PMM initialized.\n"); 
    //Testing malloc() and free()
    test_pmm();
}

void add_block(uint64_t *addr, uint64_t size){
    alloc_node_t *block = (uint64_t *) ALIGN_UP((uint64_t)addr, 8);
    //actual size - alignment-headerspace 
    block->size = addr + size - (uint64_t)block - HEADER_SIZE;
    
    dll_list_add(&block->node,&free_list,&free_list.next);
};


uint64_t* pmm_malloc(uint64_t size){
    uint64_t* ptr;
    alloc_node_t* block;
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

    if (!ptr){printf("Could not find block for requested size.\n"); return NULL;}
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

uint64_t* pmm_calloc(uint64_t size){
    uint64_t* ptr = pmm_malloc(size);
    if (ptr!=NULL)
    {
        memset(ptr,0,size);
	    return ptr;
    }
	return NULL;
}
void pmm_free(uint64_t ptr){
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

uint64_t* pmm_alloc(uint64_t num_frames){
    uint64_t ptr;
    ptr=pmm_malloc(num_frames*PAGE_SIZE);
    assert(ptr);
    if(ptr != NULL) memset(ptr + HIGHER_HALF, 0, num_frames * 0x1000); // clear memory page
    return ptr;
}

void test_pmm(){
    printf("Testing PMM...\n");
    printf("Allocating 6 bytes 2 times...\n");
    uint64_t a=pmm_malloc(6);
    assert(a);
    printf("Adress of malloc: %x\n",a);
    uint64_t b=pmm_malloc(6);
    assert(b);
    printf("Adress of malloc: %x\n",b);
    printf("Freeing the last 2 pmm_malloc()...\n");
    pmm_free(a);
    pmm_free(b);
    printf("Allocating 6 bytes 1 times...\n");
    a=pmm_malloc(6);
    assert(a);
    printf("Adress of pmm_malloc: %x\n",a);
    pmm_free(a);
    
    printf("Done PMM test.\n");
}
#define HEADER_SIZE 24
static dll_t free_list={.next=&free_list,.prev=&free_list};
*/