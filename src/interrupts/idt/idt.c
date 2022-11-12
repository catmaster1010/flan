#include "idt.h"
#include "std/stdio.h"
#include <stdint.h>


static idt_gate_t idt[256];

static void encode_idt_entry(uint64_t offset, uint8_t type)
{
    return (idt_gate_t){
        .lowOffset16 = offset & 0xFFFF,
        .selector16 = 0x28,         //Kernel code offset. 
        .ist8 = 0,                  ///not using Interrupt Stack Table. (this is null sector + ist)
        .typeAttributes8 = type,    //gate type, dpl, p (must be set to 1).
        .midOffset16 = (offset >> 16) & 0xFFFF,
        .highOffset32 = (offset >> 32) & 0xFFFFFFFF,
        .reserved32 = 0};           //reserved for the upper bit of 64bit gdt. Should always be set to 0.

}

void idt_load() {


   
   static idt_register_t idtr;
    idtr.limit = (sizeof(idt) - 1);
    idtr.base = (uint64_t)&idt;

    extern void load_idt(uint16_t limit, uint64_t base);
    load_idt(idtr.limit, idtr.base);
    printf("IDT Loaded.\n");
}



void isr_init(){
    extern uintptr_t isr_stub_table[];
    for (uint8_t vector = 0; vector < 32; vector++) {
        isr_stub_table[vector] =0;//0b10001110 hardware interupts ignore this. (which is this since its the first 32 entries)
        printf("Loading IRS...\n");
    }
   // printf("All ISRs have been loaded.");
}


void idt_init(){
    isr_init();
    idt_load();
    printf("idt_init routine finished...");
}
/////////////////////////

void exception_handler(){
    printf("EXCEPTION RECIVED!!");
    __asm__ volatile ("cli; hlt"); //Change this latter. 
} 


