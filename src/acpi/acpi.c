#include "lib/stdio.h"
#include  <limine.h>
#include  <stddef.h>
#include  "memory/vmm.h"
#include  "acpi/acpi.h"
#include  "lib/str.h"
static volatile struct limine_rsdp_request rsdp_request = {
    .id = LIMINE_RSDP_REQUEST,
    .revision = 0
};

static rsdp_descriptor_t* rsdp = NULL;
static rsdt_t* rsdt = NULL;

int use_xsdt(){
    return  rsdp->revision >= 2 && rsdp->xsdt_address!=0;
}

void acpi_init(){
    
    assert(rsdp_request.response);
    assert(rsdp_request.response->address);
    rsdp = rsdp_request.response->address;

    if (use_xsdt()) {
        rsdt = (rsdt_t*)(rsdp->xsdt_address + HHDM_OFFSET);
    } else {
        rsdt = (rsdt_t*)((uint64_t)rsdp->rsdt_address + HHDM_OFFSET);
    }

    printf("ACPI version: %d\n", rsdp->revision);
    printf("RSDT at %x\n", rsdt);
    if(use_xsdt()) printf("Using XSDT.\n");
    

    sdt_header_t *fadt = acpi_find_sdt("FACP", 0);
}
void* acpi_find_sdt(char signature[4], uint64_t index){
    uint64_t entry_count = (rsdt->header.length - sizeof(rsdt_t))  /  (use_xsdt() ? 8 : 4);


    for (uint64_t i = 0; i < entry_count; i++) {
        sdt_header_t *sdt = NULL;
        if (use_xsdt()) {
            sdt = (sdt_header_t*) (*((uint64_t*)rsdt->data + i) + HHDM_OFFSET);
        } else {
            sdt = (sdt_header_t*) (*((uint32_t*)rsdt->data + i) + HHDM_OFFSET);
        }

        if (memcmp(sdt->signature, signature, 4)!=0) {
            continue;
        }

        if (index > 0) {
            index--;
            continue;
        }

        printf("Found '%s' at %x\n", signature,sdt);
        return sdt;
    }
}
