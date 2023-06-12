#include "dev/serial.h"
#include "lib/io.h"
#include "lib/lock.h"

spinlock_t serial_lock = LOCK_INIT;

void serial_init(){
    outb(PORT + 1, 0x00);    
    outb(PORT + 3, 0x80);    // Enable DLAB 

    outb(PORT + 0, 0x01);    // 115200 baud
    outb(PORT + 1, 0x00);   

    outb(PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
    
    outb(PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    outb(PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
    serial_out("hi");
}

static inline bool is_transmitter_empty() {
    return (inb(PORT + 5) & 0b01000000) != 0;
}
static inline void transmit_data(uint8_t value) {
    while (!is_transmitter_empty()) asm volatile ("pause");
    outb(PORT, value);
}

void serial_out(char* str){
    spinlock_acquire(&serial_lock);
    while (*str) {
        if (*str == '\n') transmit_data('\r');

        transmit_data(*str++);
    }
    spinlock_release(&serial_lock);
}
