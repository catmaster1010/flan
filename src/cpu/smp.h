#ifndef smp_h
#define smp_h
#include <stdint.h>
#include  "cpu/cpu.h"
#include <limine.h>
#include  <stdbool.h>
typedef struct {
    int cpu_number;
    bool bsp;
    bool active;
    int last_run_queue_index;
    uint32_t lapic_id;
    uint32_t lapic_freq;
} cpu_local_t;


void smp_init();
void single_cpu_init(struct limine_smp_info* smp_info) ;
#endif