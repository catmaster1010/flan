#include "lib/stdio.h"
#include <stdint.h>
#include <limine.h>
#include "memory/vmm.h"
#include "lib/stddef.h"
#include"memory/pmm.h"
pagemap_t *kernel_page_map = NULL;
volatile struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0
};
volatile struct limine_kernel_address_request kernel_address_request = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST,
    .revision = 0
};
void vmm_init(){
    printf("Our kernel's physical base: %x\n",kernel_address_request.response->physical_base);
    printf("Our kernel's virtual base: %x\n",kernel_address_request.response->virtual_base);
    printf("Our HHDM is %x\n",hhdm_request.response->offset);
    uint64_t top=pmm_malloc(1);
    memset(top+HIGHER_HALF,0,FRAME_SIZE);
    assert(top);
}