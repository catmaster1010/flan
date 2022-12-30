#include "cpu/smp.h"
#include <limine.h>
#include <stdint.h>
#include "memory/kheap.h"
#include "cpu/interrupts/idt.h"
#include "memory/gdt/gdt.h"
#include "memory/vmm.h"
static volatile struct limine_smp_request smp_request = {
    .id = LIMINE_SMP_REQUEST,
    .revision = 0
};

static void  core_init(struct limine_smp_info* info){
    cpu_local_t* local= (void *)info->extra_argument;
    int cpu_number = local->cpu_number;
    local->lapic_id = info->lapic_id;


    gdt_load();
    idt_load();
    gdt_load_tss(&local->tss);
    vmm_switch_pagemap(kernel_pagemap);
}


void  smp_init(){
     for (uint64_t i = 0; i < smp_request.response->cpu_count; i++) {
            struct limine_smp_info* cpu = smp_request.response->cpus[i];

            if  (cpu->lapic_id==smp_request.response->bsp_lapic_id) {core_init(cpu);}

            cpu_local_t* local = kheap_malloc( sizeof(cpu_local_t));
            cpu->extra_argument = (uint64_t)local;
            local->cpu_number = i;
            cpu->goto_address=core_init;
    }
    
}