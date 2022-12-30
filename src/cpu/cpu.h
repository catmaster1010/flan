#ifndef cpu_h
#define cpu_h

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

static inline uint64_t wrmsr(uint32_t msr, uint64_t val) {
    uint32_t eax = (uint32_t)val, edx = (uint32_t)(val >> 32);
    __asm__ volatile (
        "wrmsr"
        : : "a" (eax), "d" (edx), "c" (msr)
        : "memory"
    );
    return ((uint64_t)edx << 32) | eax;
}


static inline void cpu_set_gs_base(void *addr) {
    wrmsr(0xc0000101, (uint64_t)addr);
}

static inline void cpu_set_fs_base(void *addr) {
    wrmsr(0xc0000100, (uint64_t)addr);
}

#endif