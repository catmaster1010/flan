#include "lib/stdio.h"
#include  <limine.h>
#include  <stddef.h>
#include  "memory/vmm.h"
#include  "acpi/acpi.h"
static volatile struct limine_rsdp_request rsdp_request = {
    .id = LIMINE_RSDP_REQUEST,
    .revision = 0
};

static rsdp_descriptor_t* rsdp = NULL;
static rsdt_t* rsdt = NULL;

void acpi_init(){
    
    assert(rsdp_request.response);
    assert(rsdp_request.response->address);
    rsdp = rsdp_request.response->address;

    if (rsdp->revision >= 2 && rsdp->xsdt_address) {
        rsdt = (rsdt_t*)(rsdp->xsdt_address + HHDM_OFFSET);
    } else {
        rsdt = (rsdt_t*)((uint64_t)rsdp->rsdt_address + HHDM_OFFSET);
    }

    printf("Revision: %d\n", rsdp->revision);
    printf("RSDT at %x\n", rsdt);
    

    acpi_sdt_header_t *fadt = acpi_find_sdt("FACP", 0);

}
void* acpi_find_sdt(char signature[4], uint64_t index){
    

    //printf("%d",entry);
}