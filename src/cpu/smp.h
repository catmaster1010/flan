#ifndef smp_h
#define smp_h
#include <stdint.h>
#include  "cpu/cpu.h"
#include <limine.h>
#include "lib/vector.h"
#include "cpu/cpu.h"

extern vector_t cpus;
void smp_init();
void single_cpu_init(struct limine_smp_info* smp_info) ;

cpu_local_t* this_cpu();

#endif
