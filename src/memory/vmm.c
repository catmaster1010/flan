#include "lib/stdio.h"
#include <stdint.h>
#include <limine.h>
#include "memory/vmm.h"
#include "lib/stddef.h"
#include"memory/pmm.h"
#include  "lib/assert.h"
#include "lib/lock.h"
#include <stddef.h>
#include  "fb/fb.h"

spinlock_t vmm_lock;
extern char kernel_end_addr[];

volatile struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0
};
volatile struct limine_kernel_address_request kernel_address_request = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST,
    .revision = 0
};

int five_level_enabled = NULL;

#define KERNEL_OFFSET  kernel_address_request.response->virtual_base

static uint64_t *get_next_level(uint64_t *current_level, uint64_t index){
    if(current_level[index]&1){
        return (current_level[index] & ~0xfff)+ HHDM_OFFSET;
    }
    uint64_t next = pmm_calloc(1);
    assert(next);
    current_level[index] = next|0b11; 
    return (uint64_t *) (next + HHDM_OFFSET);
}


void vmm_map_page(pagemap_t* pagemap, uintptr_t physical_address, uintptr_t virtual_address, uint64_t flags){
    spinlock_aquire(&vmm_lock);
    size_t pml4_index = (virtual_address & ((size_t) 0x1ff << 39)) >> 39;
    size_t pml3_index = (virtual_address & ((size_t) 0x1ff << 30)) >> 30;
    size_t pml2_index = (virtual_address & ((size_t) 0x1ff << 21)) >> 21;
    size_t pml1_index = (virtual_address & ((size_t) 0x1ff << 12)) >> 12;

    uint64_t* pml3 = get_next_level(pagemap->top, pml4_index);
    uint64_t* pml2 = get_next_level(pml3, pml3_index);
    uint64_t* pml1 = get_next_level(pml2, pml2_index);

    pml1[pml1_index] = physical_address|flags;
    uint64_t* a =  pml1[pml1_index] ;
    spinlock_release(&vmm_lock);
}

void vmm_switch_pagemap(pagemap_t* pagemap)
{
    uint64_t *top = pagemap->top;
    __asm__ volatile (
        "mov cr3, %0\n"
        : : "r" (top)
        : "memory"
    );
}

void vmm_init(){
   /* printf("Our kernel's physical base: %x\n",kernel_address_request.response->physical_base);
    printf("Our kernel's virtual base: %x\n",KERNEL_OFFSET);
    printf("Our HHDM is %x\n",HHDM_OFFSET);*/
    pagemap_t* kernel_pagemap=alloc(sizeof(kernel_pagemap));
      
    kernel_pagemap->top=pmm_calloc(1);
    assert(kernel_pagemap->top);

    uintptr_t virtual = kernel_address_request.response->virtual_base;
    uintptr_t physical = kernel_address_request.response->physical_base;
    uint64_t length = ALIGN_UP((uintptr_t)kernel_end_addr, FRAME_SIZE) - virtual;

    for(uint64_t i = 0; i < length; i += FRAME_SIZE) {
        vmm_map_page(kernel_pagemap,  physical + i,virtual + i, 0b11);
    }

    for ( uintptr_t i = 0; i < 0x100000000; i += FRAME_SIZE) {
        vmm_map_page(kernel_pagemap, i, i, 0b11);
        vmm_map_page(kernel_pagemap, i, i + HHDM_OFFSET, 0b111);
    }
    vmm_switch_pagemap(kernel_pagemap);
    printf("VMM intilasized.\n");
}