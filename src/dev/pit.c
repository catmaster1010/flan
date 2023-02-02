#include "dev/pit.h"
#include "lib/stdio.h"
#include "lib/io.h"
#include "dev/apic/lapic.h"
#include "dev/apic/ioapic.h"
#include "cpu/idt/idt.h"

void pit_set_reload_value(uint16_t count){
    outb(0x43,0x34);
    outb(0x40,(uint8_t) count);
    outb(0x40,(uint8_t) count>>8&0xff);
}

static void pit(){
    printf(".");
}

void pit_init(){
    uint16_t divisor = 1193182 / 1000;
    outb(0x46, 0x36);           
    outb(0x40, divisor & 0xFF);   
    outb(0x40, divisor >> 8);
    isr[32] =pit;
    ioapic_redirect_irq(lapic_id(), 0, 32, 1);
   // __asm__("int 32");
    printf("PIT initialized");
}


