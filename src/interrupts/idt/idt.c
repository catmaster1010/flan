#include "idt.h"
#include "std/stdio.h"
#include <stdint.h>


static idt_gate_t idt[256];

void idt_load() {


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
/////////////////////////

void exception_handler(){
    printf("EXCEPTION RECIVED!!");
    __asm__ volatile ("cli; hlt");
}

static void encode_idt_entry(uint8_t vector, void* isr, uint8_t flags) {
    idt_gate_t* descriptor = &idt[vector];
 
    descriptor->lowOffset16       = (uint64_t)isr & 0xFFFF;
    descriptor->selector16        = 0x28; //Kernel code offset. 
    descriptor->ist8              = 0; //not using Interrupt Stack Table. 
    descriptor->typeAttributes8   = flags; //gate type, dpl, p (must be set to 1).
    descriptor->midOffset16       = ((uint64_t)isr >> 16) & 0xFFFF;
    descriptor->highOffset32      = ((uint64_t)isr >> 32) & 0xFFFFFFFF;
    descriptor->reserved32           = 0;
}


