#include <dev/apic/lapic.h>
#include <lib/assert.h>
#include <lib/lock.h>
#include <lib/stdio.h>
#include <limine.h>
#include <memory/gdt/gdt.h>
#include <memory/kheap.h>
#include <memory/pmm.h>
#include <memory/vmm.h>
#include <proc/sched.h>
#include <stdint.h>
#include <sys/cpu.h>
#include <sys/idt/idt.h>
#include <sys/smp.h>

spinlock_t smp_lock = LOCK_INIT;
static uint64_t cpus_running;
vector_t cpus;

static volatile struct limine_smp_request smp_request = {
    .id = LIMINE_SMP_REQUEST, .revision = 0};

extern void syscall_entry_asm();

void core_init(struct limine_smp_info *info) {
    uint32_t eax, ebx, ecx, edx;

    cpuid(0x80000001, eax, ebx, ecx, edx);

    if (edx && (uint32_t)1 << 11) {

        uint64_t efer = rdmsr(MSR_EFER);
        efer |= 1;
        wrmsr(MSR_EFER, efer);

        uint64_t star = 0;
        star |= (uint64_t)(0x30 | 3) << 48;
        star |= (uint64_t)(0x28) << 32;

        wrmsr(MSR_STAR, star);
        wrmsr(MSR_LSTAR, (uint64_t)syscall_entry_asm);
        wrmsr(MSR_CSTAR, 0);
        wrmsr(MSR_FMASK, 0x200);
    } else {
        printf("Syscall is not supported.");
        assert(0);
    }

    cpu_local_t *local = (void *)info->extra_argument;
    int cpu_number = local->cpu_number;
    vector_replace(&cpus, local, cpu_number);

    gdt_load();
    idt_load();

    void *int_stack = pmm_alloc(STACK_SIZE);
    assert(int_stack);
    void *stack = (int_stack + HHDM_OFFSET + STACK_SIZE);
    local->tss.rsp0 = (uint64_t)stack;
    local->kernel_stack = stack;

    void *sched_stack = pmm_alloc(STACK_SIZE);
    assert(sched_stack);
    local->tss.ist1 = (uint64_t)(sched_stack + HHDM_OFFSET + STACK_SIZE);

    gdt_load_tss(&local->tss);

    vmm_switch_pagemap(kernel_pagemap);
    thread_t *idle_thread = kheap_calloc(sizeof(thread_t));
    idle_thread->blocked = true;
    idle_thread->cpu = local;
    idle_thread->process = kernel_process;

    local->idle_thread = idle_thread;
    local->current_thread = idle_thread;
    sched_enqueue_thread(idle_thread);

    set_gs_base(local);

    // enable SSE and SSE2 for SIMD
    uint64_t cr0 = read_cr0();
    cr0 &= ~(1 << 2);
    cr0 |= (1 << 1);
    write_cr0(cr0);

    uint64_t cr4 = read_cr4();
    cr4 |= (3 << 9);
    write_cr4(cr4);

    lapic_init();
    printf("Processor #%d is running. \n", cpu_number);

    cpus_running++;
    if (!local->bsp)
        sched_await();
}

void smp_init(void) {

    struct limine_smp_response *smp_response = smp_request.response;
    vector_create(&cpus, sizeof(cpu_local_t));
    vector_resize(&cpus, smp_response->cpu_count);

    for (uint64_t i = 0; i < smp_response->cpu_count; i++) {
        struct limine_smp_info *cpu = smp_response->cpus[i];
        cpu_local_t *local = kheap_calloc(sizeof(cpu_local_t));
        cpu->extra_argument = (uint64_t)local;
        local->cpu_number = i;
        local->lapic_id = cpu->lapic_id;
        if (cpu->lapic_id != smp_response->bsp_lapic_id) {
            smp_response->cpus[i]->goto_address = core_init;
            // while(cpus_running != i + 1);
        } else {
            local->bsp = 1;
            core_init(cpu);
        }
    }
    while (cpus_running != smp_response->cpu_count) {
        asm("pause");
    }
}
