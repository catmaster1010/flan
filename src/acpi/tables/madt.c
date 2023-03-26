#include "lib/stdio.h"
#include  "lib/stddef.h"
#include  "acpi/tables/madt.h"
#include "lib/assert.h"
#include  "lib/vector.h"
#include "lib/lock.h"


vector_t madt_lapics;
vector_t madt_ioapics;
vector_t madt_isos;

void madt_init(){
    madt_t* madt =acpi_find_sdt("APIC", 0);
    assert(madt);

    vector_create(&madt_lapics, sizeof(madt_lapic_t));
    vector_create(&madt_ioapics,sizeof(madt_ioapic_t));
    vector_create(&madt_isos, sizeof(madt_iso_t));

    uint64_t offset;

    while(1){
         if(madt->header.length - sizeof(madt_t) - offset < 2) break;

        madt_header_t* header = (madt_header_t*)(madt->data + offset);

        switch(header->id) {
            case 0:
                madt_lapic_t* lapic=(madt_lapic_t*)header;
                //printf("Found local LAPIC id: %d (%d)\n",lapic->apic_id,lapic->processor_id);
                vector_push(&madt_lapics, header);
                break;
            case 1:
                madt_ioapic_t* ioapic=(madt_ioapic_t*)header;
                printf("Found IO APIC id: %d\n",ioapic->header.id);
                vector_push(&madt_ioapics, header);
                break;
            case 2:
                madt_iso_t* isos=(madt_iso_t*)header;
                printf("Found ISO IRQ %d redirected to GSI %d\n",isos->irq_source,isos->gsi);
                vector_push(&madt_isos, header);
                break;
            default:
                break;
        }

        offset += MAX(header->length, 2);
    }
}
