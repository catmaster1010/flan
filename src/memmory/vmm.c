#include "lib/stdio.h"
#include <stdint.h>
#include <limine.h>
#include "memmory/vmm.h"
#include "lib/stddef.h"
#include"memmory/pmm.h"
pagemap_t *kernel_page_map = NULL;
volatile struct limine_kernel_address_request kernel_address_request = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST,
    .revision = 0
};
void vmm_init(){
    printf("Our kernel's physical base: %x\n",kernel_address_request.response->physical_base);
    printf("Our kernel's virtual base: %x\n",kernel_address_request.response->virtual_base);
    uint64_t top=pmm_alloc(1);
    printf("%x",top);
    assert(top);
}