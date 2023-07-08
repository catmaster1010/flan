#include <dev/apic/ioapic.h>
#include <dev/apic/lapic.h>
#include <dev/pit.h>
#include <dev/ps2/ps2.h>
#include <dev/ps2/ps2kb.h>
#include <lib/io.h>
#include <lib/stdio.h>
#include <proc/sched.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/idt/idt.h>

static const char keymap[] = {
    '\0', '\e', '1',  '2',  '3',  '4',  '5',  '6',  '7',  '8', '9', '0',
    '-',  '=',  '\b', '\t', 'q',  'w',  'e',  'r',  't',  'y', 'u', 'i',
    'o',  'p',  '[',  ']',  '\n', '\0', 'a',  's',  'd',  'f', 'g', 'h',
    'j',  'k',  'l',  ';',  '\'', '`',  '\0', '\\', 'z',  'x', 'c', 'v',
    'b',  'n',  'm',  ',',  '.',  '/',  '\0', '\0', '\0', ' '};

void ps2kb_handler() {
    uint8_t scancode = ps2_read();
    if (scancode < 0x57) {
        char c = keymap[scancode];
        char str[2] = {c, '\0'};
        printf("%s", str);
    }
    lapic_eoi();
}

void ps2kb_init() {
    isr[33] = ps2kb_handler;
    ioapic_redirect_irq(0, 1, 33, 0);
    inb(0x60);
}
