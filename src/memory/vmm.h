#pragma once
#ifndef vmm_h
#define vmm_h
#include <stdint.h>
#include"lib/assert.h"
#include"lib/lock.h"
#define PTE_PRESENT ((uint64_t)1 << (uint64_t)0)
#define PTE_WRITABLE ((uint64_t)1 << (uint64_t)1)
#define PTE_USER ((uint64_t)1 << (uint64_t)2)
#define PTE_NX ((uint64_t)1 << (uint64_t)3)

typedef struct{
    uint64_t top;
} pagemap_t;

void vmm_init();
void vmm_switch_pagemap(pagemap_t* pagemap);
void vmm_map_page(pagemap_t* pagemap, uintptr_t physical_address, uintptr_t virtual_address, uint64_t flags);
pagemap_t*  new_pagemap();
pagemap_t vmm_new_pagemap();


extern volatile struct limine_hhdm_request hhdm_request;
#define HHDM_OFFSET (hhdm_request.response->offset)
#endif