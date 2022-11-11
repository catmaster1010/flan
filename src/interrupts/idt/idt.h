#include <stdint.h>


#ifndef idt_h
#define idt_h


struct idt_register {
    uint16_t limit;  // length-1
    uint64_t base;      //GDT BASE
} __attribute__((packed));
typedef struct idt_register idt_register_t;

struct idt_gate {
   uint16_t lowOffset16;        // offset bits 0..15
   uint16_t selector16;        // a code segment selector in GDT or LDT
   uint8_t  ist8;             // bits 0..2 holds Interrupt Stack Table offset, rest of bits zero (RESERVED).
   uint8_t  typeAttributes8; // gate type, dpl, and p fields
   uint16_t midOffset16;        // offset bits 16..31
   uint32_t highOffset32;        // offset bits 32..63
   uint32_t reserved32;            // reserved (aka 0)
}__attribute__((packed));
typedef struct idt_gate idt_gate_t; 

void idt_init();
void isr_init();
void idt_load();

#endif
