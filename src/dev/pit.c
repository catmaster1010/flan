#include "dev/pit.h"
#include "lib/stdio.h"
#include "lib/io.h"
#include "dev/apic/lapic.h"
#include "dev/apic/ioapic.h"
#include "cpu/idt/idt.h"

static volatile uint64_t ticks=0;
uint16_t pit_get_current_count() {
    outb(0x43, 0x00);
    uint8_t lo = inb(0x40);
    uint8_t hi = inb(0x40);
    return ((uint16_t)hi << 8) | lo;
}

void pit_set_reload_value(uint16_t count){
    asm("cli");
    outb(0x43,0x34);
    outb(0x40,(uint8_t) count);
    outb(0x40,(uint8_t) (count>>8));
    asm("sti");
}

static void pit(){
    //printf(".");
    ticks++;
    
    lapic_eoi();
}
void sleep(uint64_t ms){
    uint64_t etick=ticks+ms;
    while (ticks<etick){}
}


void pit_init(){
    isr[32] =pit;
    ioapic_redirect_irq(0, 0, 32, 0);
    uint16_t divisor=OSCILATOR_FREQ/TIMER_FREQ;
    pit_set_reload_value(divisor);
    
    printf("PIT initialized.\n");
}


