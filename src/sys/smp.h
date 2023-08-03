#ifndef smp_h
#define smp_h
#include <lib/vector.h>
#include <limine.h>
#include <stdint.h>
#include <sys/cpu.h>

struct cpu_local;
extern vector_t cpus;
void smp_init();
void single_cpu_init(struct limine_smp_info *smp_info);

#endif
