#ifndef smp_h
#define smp_h
#include "cpu/cpu.h"
#include "cpu/cpu.h"
#include "lib/vector.h"
#include <limine.h>
#include <stdint.h>

extern vector_t cpus;
void smp_init();
void single_cpu_init(struct limine_smp_info *smp_info);

cpu_local_t *this_cpu();

#endif
