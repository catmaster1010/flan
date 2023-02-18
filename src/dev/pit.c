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
    //printf(".");
    lapic_eoi();
}

void pit_init(){
    isr[32] =pit;
    ioapic_redirect_irq(0, 0, 32, 0);
    uint16_t divisor = OSCILATOR_FREQ/TIMER_FREQ;
    pit_set_reload_value(divisor);

    printf("PIT initialized.\n");
}


