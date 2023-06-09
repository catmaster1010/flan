#ifndef vmm_h
#define vmm_h
#include <stdint.h>

typedef struct{
    uint64_t* top;
} pagemap_t;

#define PTE_ADDR_MASK 0x000ffffffffff000
#define PTE_PRESENT (1 << 0)
#define PTE_WRITABLE (1 << 1)
#define PTE_USER (1 << 2)

void vmm_init();
void vmm_switch_pagemap(pagemap_t* pagemap);
void vmm_map_page(pagemap_t* pagemap, uint64_t physical_address, uint64_t virtual_address, uint64_t flags);
void vmm_unmap_page(pagemap_t* pagemap, uint64_t virtual_address);
pagemap_t*  new_pagemap();
pagemap_t* vmm_new_pagemap();
void vmm_map_pages(pagemap_t* pagemap, uint64_t physical_address, uint64_t virtual_address, uint64_t flags, uint64_t pages);
uint64_t* virt_to_pte(pagemap_t* pagemap, uint64_t virtual_address);
uint64_t virt_to_phys(pagemap_t* pagemap, uint64_t virtual_address);

extern volatile struct limine_hhdm_request hhdm_request;
#define HHDM_OFFSET (hhdm_request.response->offset)
extern pagemap_t* kernel_pagemap;
#endif
