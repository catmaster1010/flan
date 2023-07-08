#include <dev/ps2/ps2.h>
#include <dev/ps2/ps2kb.h>
#include <lib/assert.h>
#include <lib/io.h>
#include <sys/idt/idt.h>

uint8_t ps2_read() {
    while ((inb(0x64) & 1) == 0)
        ;
    return inb(0x60);
}

void ps2_write(uint16_t port, uint8_t value) {
    while ((inb(0x64) & 2) != 0)
        ;
    outb(port, value);
}

void ps2_init() {
    ps2_write(0x64, 0xad);
    ps2_write(0x64, 0xa7);

    inb(0x60);

    ps2_write(0x64, 0x20);
    // ps2_write(0x64,0xaa);
    // assert(ps2_read()==0x55);

    ps2_write(0x64, 0xae);

    ps2kb_init();
}
