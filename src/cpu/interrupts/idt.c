#include "idt.h"
#include "lib/stdio.h"
#include <stdint.h>

static idt_gate_t idt[256];

static idt_register_t idtr;

void idt_load(){
    idtr.limit = (uint16_t) (sizeof(idt_gate_t) *256 - 1); 
    idtr.base = (uint64_t)&idt;
    __asm__ volatile("lidt %0"
                     :
                     : "m"(idtr));
    __asm__ volatile("sti");
}

static idt_gate_t encode_idt_entry(uint64_t offset, uint8_t type)
{
    return (idt_gate_t){
        .lowOffset16 = offset & 0xFFFF,
        .selector16 = 0x28,         //Kernel code segmment . 
        .ist8 = 1,                  ///not using IST. (this is null sector + ist)
        .typeAttributes8 = type,    //gate type, dpl, p (must be set to 1).
        .midOffset16 = (offset >> 16) & 0xFFFF,
        .highOffset32 = (offset >> 32) & 0xFFFFFFFF,
        .reserved32 = 0};           //reserved for the upper bit of 64bit gdt. Set to 0.

}

void isr_init(){
    extern uintptr_t *isr_stub_table[];
    for (uint8_t vector = 0; vector < 32; vector++) {
        idt[vector] = encode_idt_entry(&isr_stub_table[vector],0x8e);//0b10001110 hardware interupts ignore this. (which is this since its the first 32 entries)
    }
}


void idt_init(){
    isr_init();
    idt_load();
    printf("IDT initialized.\n"); 
}

__attribute__((noreturn))
void exception_handler(void);
void exception_handler(){
    printf("EXCEPTION RECIVED!!");
    __asm__ volatile ("cli; hlt"); 
}