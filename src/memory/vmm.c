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

static pagemap_t* kernel_pagemap;   

volatile struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0
};
volatile struct limine_kernel_address_request kernel_address_request = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST,
    .revision = 0
};
#define KERNEL_OFFSET  kernel_address_request.response->virtual_base

static uint64_t* get_next_level(uint64_t* current_level, uint64_t entry){

    if(!current_level[entry]&1){
        current_level[entry] = pmm_calloc(1);
        assert(current_level[entry]);
        current_level[entry] |= 0b11; // rw, present
    }
    return (uint64_t *) ((current_level[entry] & ~(0x1ff)) + HHDM_OFFSET);
}

void vmm_map_page(pagemap_t* pagemap, uint64_t physical_address, uint64_t virtual_address, uint64_t flags){
    spinlock_aquire(&vmm_lock);
    uint64_t pml4_index = (virtual_address & ((uint64_t) 0x1ff << 39)) >> 39;
    uint64_t pml3_index = (virtual_address & ((uint64_t) 0x1ff << 30)) >> 30;
    uint64_t pml2_index = (virtual_address & ((uint64_t) 0x1ff << 21)) >> 21;
    uint64_t pml1_index = (virtual_address & ((uint64_t) 0x1ff << 12)) >> 12;

    uint64_t* pml3 = get_next_level(pagemap->top, pml4_index);
    uint64_t* pml2 = get_next_level(pml3, pml3_index);
    uint64_t* pml1 = get_next_level(pml2, pml2_index);

    pml1[pml1_index] = physical_address|flags;
    spinlock_release(&vmm_lock);
}
void vmm_switch_pagemap(pagemap_t* pagemap)
{
    uint64_t *top = pagemap->top;
    __asm__ volatile (
        "mov cr3, %0"
        : : "r" (top)
        : "memory"
    );
}

void vmm_init(){
    printf("Our kernel's physical base: %x\n",kernel_address_request.response->physical_base);
    printf("Our kernel's virtual base: %x\n",KERNEL_OFFSET);
    printf("Our HHDM is %x\n",HHDM_OFFSET);

    kernel_pagemap->top=pmm_calloc(1);
    assert(kernel_pagemap->top);
    for (uintptr_t i = 0; i < 0x80000000; i += FRAME_SIZE) {
        vmm_map_page(kernel_pagemap, i, i + KERNEL_OFFSET, 0b111);
    }

    for (uintptr_t i = 0; i < 0x200000000; i += FRAME_SIZE) {
        vmm_map_page(kernel_pagemap, i, i, 0b11);
        vmm_map_page(kernel_pagemap, i, i + HHDM_OFFSET, 0b111);
    }
    vmm_switch_pagemap(kernel_pagemap);
//printf("ao;je");

}