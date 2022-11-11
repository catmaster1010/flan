#include "idt.h"
#include "std/stdio.h"
#include <stdint.h>


static idt_gate_t idt[256];

static void encode_idt_entry(uint8_t vector, void* isr, uint8_t flags) {
    idt_gate_t* descriptor = idt[vector];
 
    descriptor->lowOffset16       = (uint64_t)isr & 0xFFFF;
    descriptor->selector16        = 0x28; //Kernel code offset. 
    descriptor->ist8              = 0; //not using Interrupt Stack Table. (this is null sector + ist)
    descriptor->typeAttributes8   = flags; //gate type, dpl, p (must be set to 1).
    descriptor->midOffset16       = ((uint64_t)isr >> 16) & 0xFFFF;
    descriptor->highOffset32      = ((uint64_t)isr >> 32) & 0xFFFFFFFF;
    descriptor->reserved32        = 0; //reserved for the upper bit of 64bit gdt. Should always be set to 0.
}



void idt_load() {


    static idt_register_t idtr;
    idtr.limit = (sizeof(idt) - 1);
    idtr.base = (uint64_t)&idt;

    extern void load_gdt(uint16_t limit, uint64_t base);
    load_gdt(idtr.limit, idtr.base);



    printf("IDT Loaded.");
}



void isr_init(){
    /*extern void isr_stub_table;
    for (uint8_t vector = 0; vector < 32; vector++) {
        encode_idt_entry(vector, &isr_stub_table[vector], 0x8E);//0b10001110 hardware interupts ignore this. (which is this since its the first 32 entries)

        //0b1110 or 0xE: 64-bit Interrupt Gate
        //0b1111 or 0xF: 64-bit Trap Gate
    
    }*/

}


void idt_init(){
    isr_init();
    idt_load();
}
/////////////////////////

void exception_handler(){
    printf("EXCEPTION RECIVED!!");
    __asm__ volatile ("cli; hlt"); //Change this latter. 
} 


