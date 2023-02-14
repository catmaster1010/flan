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

spinlock_t cpu_lock=LOCK_INIT;
static int  cpus_running;

static volatile struct limine_smp_request smp_request = {
    .id = LIMINE_SMP_REQUEST,
    .revision = 0
};

void core_init(struct limine_smp_info *info) {
    cpu_local_t* local= (void *)info->extra_argument;
    int cpu_number = local->cpu_number;
    local->lapic_id = info->lapic_id;
    gdt_load();
    
    idt_load();
    vmm_switch_pagemap(kernel_pagemap);
    set_gs_base((uint64_t)local->cpu_number);
    set_kgs_base((uint64_t)local->cpu_number);
 
    // enable SSE and SSE2 for SIMD
    uint64_t cr0 = read_cr0();
    cr0 &= ~(1 << 2);
    cr0 |= (1 << 1);
    write_cr0(cr0);

    uint64_t cr4 = read_cr4();
    cr4 |= (3 << 9);
    write_cr4(cr4); 

    lapic_init();
    printf("Processor #%d is running.(%d)\n",cpu_number,lapic_id());
    
    cpus_running++;  
    if (!local->bsp){
        for (;;) {
        __asm__("hlt");
        }
    }

}

void smp_init(void) {
    struct limine_smp_response *smp_response = smp_request.response;

    for (uint64_t i = 0; i < smp_response->cpu_count; i++) {
        struct limine_smp_info *cpu = smp_response->cpus[i];
        cpu_local_t* local = kheap_malloc(sizeof(cpu_local_t));
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
        __asm__ ("pause");
    }
}
