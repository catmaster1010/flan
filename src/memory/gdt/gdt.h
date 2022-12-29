#include <stdint.h>

#ifndef gdt_h
#define gdt_h

#include "cpu/cpu.h" 

typedef struct __attribute__((packed)) {
    uint16_t limit;
    uint16_t baselow16;
    uint8_t basemid8;
    uint8_t access;
    uint8_t granularity;
    uint8_t basehigh8;
} gdt_entry_t;


typedef struct __attribute__((packed)) {
    uint16_t limit;  // length-1
    uint64_t base;      //GDT BASE
} gdt_register_t;

void encodeGdtEntry();
void gdt_init();
void gdt_load_tss(tss_t* tss);

#endif
