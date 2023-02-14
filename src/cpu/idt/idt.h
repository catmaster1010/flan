#ifndef idt_h
#define idt_h
#include <stdint.h>

typedef struct __attribute__((packed)) {
    uint16_t limit;
    uint64_t base;
} idtptr_t;

typedef struct __attribute__((packed)) {
    uint16_t offset_low16;
    uint16_t selector;
    uint8_t ist;
    uint8_t flags;
    uint16_t offset_mid16;
    uint32_t offset_high32;
    uint32_t reserved;
} idt_gate_t;

extern void* isr[];
void idt_init();
void isr_init();
void idt_load();

#endif
