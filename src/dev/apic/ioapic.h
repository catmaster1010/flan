#ifndef ioapic_h
#define ioapic_h

#include <stdint.h>

#define IOAPIC_IOREGSEL 0x00
#define IOAPIC_IOWIN 0x10
#define IOAPIC_VER 0x01

uint32_t ioapic_read(uint64_t ioapic_address, uint64_t reg);
void ioapic_write(uint64_t ioapic_address, uint64_t reg, uint32_t data);
void ioapic_redirect_gsi(uint8_t lapic_id, uint32_t gsi, uint8_t vector, uint64_t flags);
void ioapic_redirect_irq(uint8_t lapic_id, uint8_t irq, uint8_t vector, uint64_t flags);

#endif