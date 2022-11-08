#include <stdint.h>

#ifndef gdt_h
#define gdt_h


struct gdt_entry {
    uint16_t lowLimit16;           //Limit 0:15 [IGNORED since 64]
    uint16_t lowBase16;           //Base 16:31 [IGNORED since 64]
    uint8_t  midBase8;           // Base 32:39 [IGNORED since 64]
    uint8_t  access8;            //Access Byte 40:47 
    uint8_t  highLimit8;         // LIMIT4 and FLAGS4 48:55
    uint8_t  baseHigh8;           //base 56:63 [IGNORED since 64]
} __attribute__((packed));
typedef struct gdt_entry gdt_entry_t;
//LIMIT 48:51 | FLAGS 52:55


struct gdt_register {
    uint16_t limit;  // length-1
    uint64_t base;      //GDT BASE
} __attribute__((packed));
typedef struct gdt_register gdt_register_t;

void encodeGdtEntry();
void gdt_init();
void gdt_load();

#endif
