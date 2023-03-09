#include "cpu/smp.h"
#include "cpu/cpu.h"
#include <limine.h>
#include <stdint.h>
#include "memory/kheap.h"
#include "cpu/idt/idt.h"
#include "memory/gdt/gdt.h"
#include "memory/vmm.h"
#include "memory/pmm.h"
#include "lib/stdio.h"
#include "lib/lock.h"
#include "dev/apic/lapic.h"
#include "proc/sched.h"

spinlock_t smp_lock=LOCK_INIT;
static int  cpus_running;
vector_t cpus;

static volatile struct limine_smp_request smp_request = {
    .id = LIMINE_SMP_REQUEST,
    .revision = 0
};

void core_init(struct limine_smp_info *info) {
    cpu_local_t* local= (void *)info->extra_argument;
    int cpu_number = local->cpu_number;
    vector_t* queue = kheap_calloc(sizeof(vector_t));
    vector_create(queue,sizeof(thread_t));
    local->queue=queue;
    vector_replace(&cpus,local,cpu_number);
    gdt_load();
    
    idt_load();
    vmm_switch_pagemap(kernel_pagemap);
    set_gs_base((uint64_t)info);
    set_kgs_base((uint64_t)info);
 
    // enable SSE and SSE2 for SIMD
    uint64_t cr0 = read_cr0();
    cr0 &= ~(1 << 2);
    cr0 |= (1 << 1);
    write_cr0(cr0);

    uint64_t cr4 = read_cr4();
    cr4 |= (3 << 9);
    write_cr4(cr4); 

    lapic_init();
    printf("Processor #%d is running. \n",cpu_number);
    
    cpus_running++;  
    if (!local->bsp){
        sched_await();      
        for(;;){
        asm volatile("cli;hlt;");
        __builtin_unreachable(); }
    }

}

void smp_init(void) {
    struct limine_smp_response *smp_response = smp_request.response;
    vector_create(&cpus,sizeof(cpu_local_t));
    vector_resize(&cpus,smp_response->cpu_count);

    for (uint64_t i = 0; i < smp_response->cpu_count; i++) {
        struct limine_smp_info *cpu = smp_response->cpus[i];
        cpu_local_t* local = kheap_calloc(sizeof(cpu_local_t));
        cpu->extra_argument = (uint64_t)local;
        local->cpu_number = i;
        cpu->goto_address=core_init;
        if (cpu->lapic_id != smp_response->bsp_lapic_id) {
            smp_response->cpus[i]->goto_address = core_init;
        } 
        else{
            local->bsp=1;
            core_init(cpu);
        }
    }
    while (cpus_running != smp_response->cpu_count) {
        asm ("pause");
    }
}
cpu_local_t* this_cpu(){
    struct limine_smp_info* info = read_gs_base();
    cpu_local_t* this_cpu= (void *)info->extra_argument;
    return this_cpu;
}