#include "dev/ps2/ps2kb.h"
#include "proc/sched.h"
#include "cpu/idt/idt.h"
#include "lib/stdio.h"
#include "lib/io.h"
#include <stdint.h>
#include <stdbool.h>
#include "dev/ps2/ps2.h"

static char keybuffer[256];

static const char keymap[] = {
    '\0', '\e', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', '\t',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', '\0', 'a', 's',
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', '\0', '\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm', ',', '.', '/', '\0', '\0', '\0', ' '
};

void ps2kb_handler(){
    uint8_t scancode=ps2_read();
    bool release = scancode & 0x80;
    if (scancode < 0x57) {
        char c = keymap[scancode];
        char str[2] = {c,'\0'};
        printf("%s",str);
        if (c=='\n') printf("%s >  ",get_current_thread()->process->cwd->name);
    }
    lapic_eoi();
}

void ps2kb_init(){
    isr[33] = ps2kb_handler;
    ioapic_redirect_irq(0, 1, 33, 0);
    inb(0x60);
}
