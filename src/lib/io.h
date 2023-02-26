#ifndef io_h
#define io_h
#include <stdint.h>

static inline void outb(uint16_t port, uint8_t data) {
    __asm__ volatile (
        "outb %1, %0\n"
        :
        : "a" (data), "Nd" (port)
        : "memory"
    );
}

static inline void outw(uint16_t port, uint16_t data) {
    __asm__ volatile (
        "outw %1, %0\n"
        :
        : "a" (data), "Nd" (port)
        : "memory"
    );
}

static inline void outd(uint16_t port, uint32_t data) {
    __asm__ volatile (
        "outd %1, %0\n"
        :
        : "a" (data), "Nd" (port)
        : "memory"
    );
}

static inline uint8_t inb(uint16_t port) {
    uint8_t data;
    __asm__ volatile (
        "inb %0, %1\n"
        : "=a" (data)
        : "Nd" (port)
        : "memory"
    );
    return data;
}

static inline uint16_t inw(uint16_t port) {
    uint16_t data;
    __asm__ volatile (
        "inw %1, %0\n"
        : "=a" (data)
        : "Nd" (port)
        : "memory"
    );
    return data;
}

static inline uint32_t ind(uint16_t port) {
    uint32_t data;
    __asm__ volatile (
        "ind %1, %0\n"
        : "=a" (data)
        : "Nd" (port)
        : "memory"
    );
    return data;
}


#endif
