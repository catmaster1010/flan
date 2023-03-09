#ifndef cpu_h
#define cpu_h
#include <stdint.h>
#include  <stdbool.h>
#include "lib/vector.h"
#include "lib/lock.h"

#define STACK_SIZE 0x10000

#define cpuid(in,a,b,c,d) asm volatile ("cpuid" : "=a"(a),"=b"(b),"=c"(c),"=d"(d) : "a"(in)); 

typedef struct __attribute__((packed)) {
    uint32_t rsvd0;
    uint64_t rsp0;
    uint64_t rsp1;
    uint64_t rsp2;
    uint64_t rsvd1;
    uint64_t ist1;
    uint64_t ist2;
    uint64_t ist3;
    uint64_t ist4;
    uint64_t ist5;
    uint64_t ist6;
    uint64_t ist7;
    uint64_t rsvd2;
    uint32_t iopb;
} tss_t;

struct interrupt_frame {
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rbp;
    uint64_t rdi;
    uint64_t rsi;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rbx;
    uint64_t rax;
   // uint64_t intno;
    //uint64_t err;
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
} __attribute__((__packed__));
typedef struct interrupt_frame interrupt_frame_t;

typedef struct {
    int cpu_number;
    bool bsp;
    bool active;
    int last_run_queue_index;
    uint32_t lapic_freq;
    vector_t* queue;
} cpu_local_t;


static inline uint64_t wrmsr(uint32_t msr, uint64_t val) {
    uint32_t eax = (uint32_t)val, edx = (uint32_t)(val >> 32);
    asm volatile (
        "wrmsr"
        : : "a" (eax), "d" (edx), "c" (msr)
        : "memory"
    );
    return ((uint64_t)edx << 32) | eax;
}

static inline uint64_t rdmsr(uint32_t msr) {
    uint32_t edx = 0, eax = 0;
    asm volatile (
        "rdmsr"
        : "=a" (eax), "=d" (edx)
        : "c" (msr)
        : "memory"
    );
    return ((uint64_t)edx << 32) | eax;
}

static inline uint64_t read_gs_base() {
    return rdmsr(0xc0000101);
}

static inline void set_gs_base(void *addr) {
    wrmsr(0xc0000101, (uint64_t)addr);
}

static inline void set_fs_base(void *addr) {
    wrmsr(0xc0000100, (uint64_t)addr);
}

static inline void set_kgs_base(void *addr) {
    wrmsr(0xc0000102, (uint64_t)addr);
}

static inline uint64_t read_cr0(void) {
    uint64_t ret = 0;
    asm volatile (
        "mov %0, cr0"
        : "=r" (ret)
        : : "memory"
    );
    return ret;
}
static inline void write_cr0(uint64_t val) {
    asm volatile (
        "mov cr0, %0"
        : : "r" (val)
        : "memory"
    );
}

static inline uint64_t read_cr4(void) {
    uint64_t ret = 0;
    asm volatile (
        "mov %0, cr4"
        : "=r" (ret)
        : : "memory"
    );
    return ret;
}
static inline void write_cr4(uint64_t val) {
    asm volatile (
        "mov cr4, %0"
        : : "r" (val)
        : "memory"
    );
}

#endif
