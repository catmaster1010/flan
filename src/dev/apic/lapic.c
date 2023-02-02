#include "dev/apic/lapic.h"
#include "cpu/cpu.h"
#include "lib/stdio.h"
#include "memory/vmm.h"

static inline uint64_t lapic_base() {
    return (rdmsr(0x1b) & 0xfffff000) + HHDM_OFFSET;
}

uint32_t lapic_read(uint32_t reg){
    return *((volatile uint32_t *) (lapic_base() + reg));
}

void lapic_write(uint32_t reg, uint32_t data){
    *((volatile uint32_t *) (lapic_base() + reg)) = data;
}

void lapic_init(){
   // printf("%x\n",lapic_base());
    lapic_write(LAPIC_SPURIOUS_IVR, lapic_read(LAPIC_SPURIOUS_IVR) | (1 << 8) | 0xff);
}

void lapic_eoi() {
    lapic_write(LAPIC_EOI, 0x00);
}

void lapic_ipi(uint32_t lapic_id, uint8_t vector) {
    lapic_write(LAPIC_ICR1, (uint32_t) lapic_id << 24);
    lapic_write(LAPIC_ICR0, vector);
}

uint32_t lapic_id() {
    return lapic_read(LAPIC_APIC_ID);
}
