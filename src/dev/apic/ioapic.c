#ifndef ioapic_h
#define ioapic_h
#include <stdint.h>
#include "dev/apic/ioapic.h"
#include "acpi/tables/madt.h"
#include "lib/vector.h"
#include "memory/vmm.h"
#include <limine.h>

uint32_t ioapic_read(uint64_t ioapic_address, uint64_t reg){
    *((volatile uint32_t* ) (ioapic_address+HHDM_OFFSET)) = reg;
    return *((volatile uint32_t* ) (ioapic_address+HHDM_OFFSET+0x10));
}

void ioapic_write(uint64_t ioapic_address, uint64_t reg, uint32_t data){
    *((volatile uint32_t* ) (ioapic_address+HHDM_OFFSET)) = reg;
    *((volatile uint32_t* ) (ioapic_address+HHDM_OFFSET+0x10)) = data;
}


static uint64_t ioapic_gsi_count(madt_ioapic_t* ioapic) {
    return (ioapic_read((uint64_t)ioapic, 1) & 0xff0000) >> 16;
}

static madt_ioapic_t* ioapic_from_gsi(uint32_t gsi){
    for (uint64_t i = 0; i < madt_ioapics.items; i++) {
        madt_ioapic_t* io_apic = vector_get(&madt_ioapics, i);
        if (gsi >= io_apic->gsib && gsi < io_apic->gsib + ioapic_gsi_count(io_apic)) {
            return io_apic;
        }
    }
    printf("Cannot find an IOAPIC from the given GSI\n");
    assert(0);
}

void ioapic_redirect_gsi(uint8_t lapic_id, uint32_t gsi, uint8_t vector, uint64_t flags){
    uint64_t ioredtbl_data = ((uint64_t) lapic_id << 56) | ((uint64_t) vector) | flags;
    madt_ioapic_t* ioapic = ioapic_from_gsi(gsi);
    uint64_t ioredtbl = (gsi - ioapic->gsib) * 2;
    ioapic_write(ioapic->address + HHDM_OFFSET, ioredtbl, (uint32_t) ioredtbl_data);
    ioapic_write(ioapic->address + HHDM_OFFSET, ioredtbl + 1, (uint32_t) (ioredtbl_data >> 32));   
}

void ioapic_redirect_irq(uint8_t lapic_id, uint8_t irq, uint8_t vector, uint64_t flags){
    for (uint64_t i = 0; i < madt_isos.items; i++) {
        madt_iso_t* iso = vector_get(&madt_isos, i);
        if (iso->irq_source == irq) {
            ioapic_redirect_gsi(lapic_id, iso->gsi, vector, (uint64_t) iso->flags | flags);
            return;
        }
    }
    ioapic_redirect_gsi(lapic_id, (uint32_t) irq, vector, flags);
}

#endif
