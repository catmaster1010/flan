#include "memory/vmm.h"
#include "lib/assert.h"
#include "lib/lock.h"
#include "lib/stddef.h"
#include "lib/stdio.h"
#include "memory/pmm.h"
#include <limine.h>
#include <stdint.h>

spinlock_t vmm_lock;
extern char kernel_end_addr[];

volatile struct limine_hhdm_request hhdm_request = {.id = LIMINE_HHDM_REQUEST,
                                                    .revision = 0};

volatile struct limine_kernel_address_request kernel_address_request = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST, .revision = 0};

pagemap_t *kernel_pagemap;

#define KERNEL_OFFSET kernel_address_request.response->virtual_base

static uint64_t *get_next_level(uint64_t *current_level, uint64_t index) {
    if ((current_level[index] & 1) != 0) {
        return (uint64_t *)((current_level[index] & PTE_ADDR_MASK) +
                            HHDM_OFFSET);
    }
    void *next = pmm_calloc(1);
    assert(next);
    current_level[index] =
        (uint64_t)next | PTE_PRESENT | PTE_WRITABLE | PTE_USER;
    return (uint64_t *)(next + HHDM_OFFSET);
}

void vmm_unmap_page(pagemap_t *pagemap, uintptr_t virtual_address) {

    spinlock_acquire(&vmm_lock);

    uint64_t pml4_index = (virtual_address & ((uint64_t)0x1ff << 39)) >> 39;
    uint64_t pml3_index = (virtual_address & ((uint64_t)0x1ff << 30)) >> 30;
    uint64_t pml2_index = (virtual_address & ((uint64_t)0x1ff << 21)) >> 21;
    uint64_t pml1_index = (virtual_address & ((uint64_t)0x1ff << 12)) >> 12;

    uint64_t *pml3 = get_next_level(pagemap->top, pml4_index);
    assert(pml3);
    uint64_t *pml2 = get_next_level(pml3, pml3_index);
    assert(pml2);
    uint64_t *pml1 = get_next_level(pml2, pml2_index);
    assert(pml1);

    pml1[pml1_index] = 0;

    asm volatile("invlpg [%0]" : : "r"(virtual_address) : "memory");

    spinlock_release(&vmm_lock);
}
void vmm_map_page(pagemap_t *pagemap, uintptr_t physical_address,
                  uintptr_t virtual_address, uint64_t flags) {

    spinlock_acquire(&vmm_lock);
    uint64_t pml4_index = (virtual_address & ((uint64_t)0x1ff << 39)) >> 39;
    uint64_t pml3_index = (virtual_address & ((uint64_t)0x1ff << 30)) >> 30;
    uint64_t pml2_index = (virtual_address & ((uint64_t)0x1ff << 21)) >> 21;
    uint64_t pml1_index = (virtual_address & ((uint64_t)0x1ff << 12)) >> 12;
    uint64_t *pml3 = get_next_level(pagemap->top, pml4_index);
    assert(pml3);
    uint64_t *pml2 = get_next_level(pml3, pml3_index);
    assert(pml2);
    uint64_t *pml1 = get_next_level(pml2, pml2_index);
    assert(pml1);

    pml1[pml1_index] = physical_address | flags;

    spinlock_release(&vmm_lock);
}

void vmm_map_pages(pagemap_t *pagemap, uintptr_t physical_address,
                   uintptr_t virtual_address, uint64_t flags, uint64_t pages) {
    for (uint64_t i = 0; i < pages; i++) {
        vmm_map_page(pagemap, physical_address + (i * FRAME_SIZE),
                     virtual_address + (i * FRAME_SIZE), flags);
    }
}

void vmm_switch_pagemap(pagemap_t *pagemap) {
    uint64_t *top = (void *)pagemap->top - HHDM_OFFSET;
    asm volatile("mov cr3, %0\n" : : "r"(top) : "memory");
}

pagemap_t *vmm_new_pagemap() {
    pagemap_t *pagemap = pmm_alloc(sizeof(pagemap_t));

    assert(pagemap);
    pagemap->top = pmm_calloc(1);

    assert(pagemap->top);

    pagemap->top = (void *)pagemap->top + HHDM_OFFSET;
    for (uint64_t i = 256; i < 512; i++) {
        pagemap->top[i] = kernel_pagemap->top[i];
    }

    return pagemap;
}

void vmm_init() {
    printf("Our kernel's physical base: %x\n",
           kernel_address_request.response->physical_base);
    printf("Our kernel's virtual base: %x\n", KERNEL_OFFSET);
    printf("Our HHDM offset is %x\n", HHDM_OFFSET);

    kernel_pagemap = pmm_alloc(sizeof(kernel_pagemap));
    kernel_pagemap->top = pmm_calloc(1);
    assert(kernel_pagemap->top);

    kernel_pagemap->top = (void *)kernel_pagemap->top + HHDM_OFFSET;

    uintptr_t virtual = kernel_address_request.response->virtual_base;
    uintptr_t physical = kernel_address_request.response->physical_base;
    uint64_t length = ALIGN_UP((uintptr_t)kernel_end_addr, FRAME_SIZE) -
                      virtual;

    for (uint64_t i = 0; i < length; i += FRAME_SIZE) {
        vmm_map_page(kernel_pagemap, physical + i, virtual + i,
                     PTE_PRESENT | PTE_WRITABLE);
    }

    // 4gb
    for (uintptr_t i = 0x1000; i < 0x100000000; i += FRAME_SIZE) {
        vmm_map_page(kernel_pagemap, i, i, PTE_PRESENT | PTE_WRITABLE);
        vmm_map_page(kernel_pagemap, i, i + HHDM_OFFSET,
                     PTE_PRESENT | PTE_WRITABLE);
    }

    /*
    struct  limine_memmap_entry **mmaps = memmap_request.response->entries;
    uint64_t mmmap_count = memmap_request.response->entry_count;

   for (int i = 0; i < mmmap_count; i++){
        uint64_t base = ALIGN_DOWN(mmaps[i]->base,FRAME_SIZE);
        uint64_t tail = ALIGN_UP(mmaps[i]->base+mmaps[i]->length,FRAME_SIZE);

        if (base<0x100000000){continue;}

        for (uint64_t k = base; k < tail; k++){
            vmm_map_page(kernel_pagemap, k, k, 0b11);
            vmm_map_page(kernel_pagemap, k, k + HHDM_OFFSET, 0b111);
        }

    }
    */
    vmm_switch_pagemap(kernel_pagemap);
    printf("VMM intilasized.\n");
}

uint64_t *virt_to_pte(pagemap_t *pagemap, uint64_t virtual_address) {
    uint64_t pml4_index = (virtual_address & ((uint64_t)0x1ff << 39)) >> 39;
    uint64_t pml3_index = (virtual_address & ((uint64_t)0x1ff << 30)) >> 30;
    uint64_t pml2_index = (virtual_address & ((uint64_t)0x1ff << 21)) >> 21;
    uint64_t pml1_index = (virtual_address & ((uint64_t)0x1ff << 12)) >> 12;

    uint64_t *pml3 = get_next_level(pagemap->top, pml4_index);
    assert(pml3);
    uint64_t *pml2 = get_next_level(pml3, pml3_index);
    assert(pml2);
    uint64_t *pml1 = get_next_level(pml2, pml2_index);
    assert(pml1);

    return &pml1[pml1_index];
}

uint64_t virt_to_phys(pagemap_t *pagemap, uint64_t virtual_address) {
    uint64_t *pte = virt_to_pte(pagemap, virtual_address);
    return *pte & PTE_ADDR_MASK;
}
