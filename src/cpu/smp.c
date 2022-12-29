#include "cpu/smp.h"
#include <limine.h>
#include <stdint.h>
#include ""
static volatile struct limine_smp_request smp_request = {
    .id = LIMINE_SMP_REQUEST,
    .revision = 0
};

static void  core_init(){}


void  smp_init(){
     for (uint64_t i = 0; i < smp_request.response->cpu_count; i++) {
            struct limine_smp_info *cpu = smp_request.response->cpus[i];
            cpu_local_t
    }
}