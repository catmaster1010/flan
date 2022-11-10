#include "idt.h"
#include "std/stdio.h"
#include <stdint.h>

static idt_gate_t encodeIdtEntry(uint64_t offset, uint8_t type)
{
    return (idt_gate_t){
        .lowOffset16 = offset & 0xFFFF,
        .selector16 = 0x28,
        .ist8 = 0,
        .typeAttributes8 = type,
        .midOffset16 = (offset >> 16) & 0xFFFF,
        .highOffset32 = (offset >> 32) & 0xFFFFFFFF,
        .reserved = 0};
}

void idt_load() {
    static idt_gate_t idt[256];

    static idt_register_t idtr;
    idtr.limit = (sizeof(idt) - 1);
    idtr.base = (uint64_t)&idt;

    extern void load_gdt(uint16_t limit, uint64_t base);
    load_gdt(idtr.limit, idtr.base);
    printf("IDT Loaded.");
}
void isr_init(){}


void idt_init(){
    idt_load();
    isr_init();}


