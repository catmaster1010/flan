#include "idt.h"
#include "lib/stdio.h"
#include <stdint.h>
#include "lib/lock.h"
spinlock_t idt_lock=LOCK_INIT;
static idt_gate_t idt[256];
void* isr[256];
extern void* isr_stubs[];
static idtptr_t idtr;

void idt_load(){
    idtr.limit = (uint16_t) (sizeof(idt_gate_t) *256 - 1); 
    idtr.base = (uint64_t)&idtr;
    __asm__ volatile("lidt %0"
                     :
                     : "m"(idtr));
    __asm__ volatile("sti");
}

static idt_gate_t encode_idt_entry(uint64_t vector, void*  handler,uint8_t type)
{
   idt[vector]=(idt_gate_t){
        .lowOffset16 = (uint64_t)handler & 0xFFFF,
        .selector16 = 0x28,         //Kernel code segmment . 
        .ist8 = 1,                  ///not using IST. (this is null sector + ist)
        .typeAttributes8 = type,    //gate type, dpl, p (must be set to 1).
        .midOffset16 = ((uint64_t)handler >> 16) & 0xFFFF,
        .highOffset32 = ((uint64_t)handler >> 32) & 0xFFFFFFFF,
        .reserved32 = 0};           //reserved for the upper bit of 64bit gdt. Set to 0.

}


static void exce_handler() {
    printf("EXCEPTION RECIVED!!");
    __asm__ volatile ("cli; hlt"); 
}

static void isr_generic() {
    printf("Something\n");
}

void idt_init(){
    idtr = (idtptr_t) {
        .limit = sizeof(idt) - 1,
        .base = (uint64_t)idt
    };

    for(uint64_t i = 0; i < 32; i++) { 
        encode_idt_entry(i, isr_stubs[i], 0x8e);
        isr[i] = exce_handler;
    }

    for(uint64_t i = 32; i < 256; i++) { 
        encode_idt_entry(i, isr_stubs[i], 0x8e);
        isr[i] = isr_generic;
    }
    idt_load();
    printf("IDT initialized.\n"); 
}
