#ifndef smp_h
#define smp_h
#include <stdint.h>
#include  "cpu/cpu.h"

typedef struct {
    int cpu_number;
    int bsp;
    int active;
    int last_run_queue_index;
    uint32_t lapic_id;
    uint32_t lapic_freq;
    tss_t tss;
} cpu_local_t;


void smp_init();

#endif