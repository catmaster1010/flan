#ifndef cpu_h
#define cpu_h
#include <stdint.h>
#include  <stdbool.h>
#include "lib/vector.h"
#include "lib/lock.h"

#define STACK_SIZE 0x10000

#define MSR_EFER 0xC0000080
#define MSR_STAR 0xC0000081
#define MSR_LSTAR 0xC0000082
#define MSR_CSTAR 0xC0000083
#define MSR_FMASK 0xC0000084
#define MSR_FSBASE 0xC0000100
#define MSR_GSBASE 0xC0000101
#define MSR_KGSBASE 0xC0000102

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
    uint64_t es; 
    uint64_t ds;
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
    uint64_t err;
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
} __attribute__((__packed__));
typedef struct interrupt_frame interrupt_frame_t;

typedef struct cpu_local {
    void* kernel_stack;
    uint64_t cpu_number;
    uint64_t lapic_id;
    bool bsp;
    bool active;
    uint32_t lapic_freq;
    tss_t tss;
    uint64_t last_run_index;
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

static inline void* read_gs_base() {
    return (void*) rdmsr(MSR_GSBASE);
}

static inline void* read_kgs_base() {
    return (void*) rdmsr(MSR_KGSBASE);
}

static inline void* read_fs_base() {
    return (void*) rdmsr(MSR_FSBASE);
}

static inline void set_gs_base(void *addr) {
    wrmsr(MSR_GSBASE, (uint64_t)addr);
}

static inline void set_fs_base(void *addr) {
    wrmsr(MSR_FSBASE, (uint64_t)addr);
}

static inline void set_kgs_base(void *addr) {
    wrmsr(MSR_KGSBASE, (uint64_t)addr);
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

static inline uint64_t read_cr3(void) {
    uint64_t ret = 0;
    asm volatile (
        "mov %0, cr3"
        : "=r" (ret)
        : : "memory"
    );
    return ret;
}
static inline void write_cr3(uint64_t val) {
    asm volatile (
        "mov cr3, %0"
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
