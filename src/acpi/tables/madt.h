#ifndef madt_h
#define madt_h

#include  "acpi/acpi.h"
#include  <stdint.h>

typedef struct __attribute__((packed)){
    sdt_header_t header;
    uint32_t local_controller_address;
    uint32_t flags;
    char data[];
} madt_t;

typedef struct __attribute__((packed)) {
    uint8_t id;
    uint8_t length;
} madt_header_t;

typedef struct __attribute__((packed)) {
    madt_header_t header;
    uint8_t processor_id;
    uint8_t apic_id;
    uint32_t flags;
} madt_lapic_t;

typedef struct __attribute__((packed)) {
    madt_header_t header;
    uint8_t apic_id;
    uint8_t reserved;
    uint32_t address;
    uint32_t gsib;
} madt_ioapic_t;

typedef struct __attribute__((packed)) {
    madt_header_t header;
    uint8_t bus_source;
    uint8_t irq_source;
    uint32_t gsi;
    uint16_t flags;
} madt_iso_t;

typedef struct __attribute__((packed)) {
    madt_header_t header;
    uint8_t processor;
    uint16_t flags;
    uint8_t lint;
} madt_nmi_t;

void madt_init();
#endif