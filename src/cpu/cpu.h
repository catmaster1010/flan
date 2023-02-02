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

static inline uint64_t rdmsr(uint32_t msr) {
    uint32_t edx = 0, eax = 0;
    __asm__ volatile (
        "rdmsr"
        : "=a" (eax), "=d" (edx)
        : "c" (msr)
        : "memory"
    );
    return ((uint64_t)edx << 32) | eax;
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
    __asm__ volatile (
        "mov %0, cr0"
        : "=r" (ret)
        : : "memory"
    );
    return ret;
}
static inline void write_cr0(uint64_t val) {
    __asm__ volatile (
        "mov cr0, %0"
        : : "r" (val)
        : "memory"
    );
}

static inline uint64_t read_cr4(void) {
    uint64_t ret = 0;
    __asm__ volatile (
        "mov %0, cr4"
        : "=r" (ret)
        : : "memory"
    );
    return ret;
}
static inline void write_cr4(uint64_t val) {
    __asm__ volatile (
        "mov cr4, %0"
        : : "r" (val)
        : "memory"
    );
}
#endif
