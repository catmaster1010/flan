#include "proc/sched.h"
#include "lib/assert.h"
#include "dev/apic/lapic.h"
#include "cpu/idt/idt.h"
#include "cpu/idt/idt.h"
#include "memory/kheap.h"
#include "memory/pmm.h"
#include "cpu/cpu.h"
#include "cpu/smp.h"

process_t *kernel_process;
vector_t processes_vector;

static __attribute__((__noreturn__)) void switch_to_thread(thread_t* thread)
{
    thread->running=1;
    interrupt_frame_t* state=thread->state;
    
    asm volatile(
        "mov rsp, %0\n"
        "pop r15\n"
        "pop r14\n"
        "pop r13\n"
        "pop r12\n"
        "pop r11\n"
        "pop r10\n"
        "pop r9\n"
        "pop r8\n"
        "pop rbp\n"
        "pop rdi\n"
        "pop rsi\n"
        "pop rdx\n"
        "pop rcx\n"
        "pop rbx\n"
        "pop rax\n"
        "iretq\n"
        :
        : "rm"(state)
        : "memory");
    __builtin_unreachable();
}

process_t *sched_process(pagemap_t *pagemap)
{
    process_t *proc = kheap_malloc(sizeof(process_t));
    assert(proc);
    vector_push(&processes_vector, proc);
    vector_t *threads = kheap_malloc(sizeof(vector_t));
    vector_create(threads, sizeof(thread_t));
    proc->threads = threads;
    proc->pagemap = pagemap;
    return proc;
}

bool sched_enqueue_thread(thread_t *thread)
{
    if (thread->enqueued == true)
    {
        return true;
    }
    if (this_cpu()->last_run_queue_index == 0)
    {
        vector_push(this_cpu()->queue, thread);
        return true;
    }
    vector_insert(this_cpu()->queue, thread, this_cpu()->last_run_queue_index);
}

thread_t* sched_kernel_thread(void *start, void *args)
{
    thread_t* thread = kheap_calloc(sizeof(thread_t));
    thread->lock = LOCK_INIT;
    thread->state = kheap_calloc(sizeof(interrupt_frame_t));
    interrupt_frame_t* state = thread->state;
    state->rsp = kheap_calloc(STACK_SIZE) + HHDM_OFFSET + STACK_SIZE;
    state->cs = 0x28;
    state->rflags = 0x202;
    state->rip = start;
    state->rdi = args;
    thread->process = kernel_process;
    thread->timeslice = TIME_QUANTUM;
    thread->running = 0;
    sched_enqueue_thread(thread);
    return thread;
}

static bool work_steal()
{
    uint64_t most_work = 0;
    cpu_local_t *most_work_cpu;

    for (uint64_t i = 0; i < cpus.items; i++)
    {
        cpu_local_t *cpu = vector_get(&cpus, i);
        uint64_t work = cpu->queue->items;
        if (work > most_work)
        {
            most_work = work;
            most_work_cpu = cpu;
        }
    }
    if (most_work <= 1)
    {
        return 0;
    }
    thread_t *last_job = vector_get(most_work_cpu->queue, most_work_cpu->last_run_queue_index);
    vector_remove(most_work_cpu->queue, most_work_cpu->last_run_queue_index);
    vector_push(this_cpu()->queue, last_job);
    return 1;
}

static thread_t *get_next_thread()
{
    uint64_t last_queue_index = this_cpu()->last_run_queue_index;
    if (this_cpu()->queue->items == last_queue_index + 1)
    {
        return vector_get(this_cpu()->queue, 0);
    }
    return vector_get(this_cpu()->queue, last_queue_index + 1);
}

static void sched_vector(uint8_t vector, interrupt_frame_t *state)
{
    lapic_stop();

    vector_t *queue = this_cpu()->queue;
    uint64_t last_queue_index = this_cpu()->last_run_queue_index;
    if (queue->items == 0)
    {
        if (!work_steal())
        {
            lapic_eoi();
            lapic_timer_oneshot(1000, 33);
            for (;;)
            {
                asm("hlt");
            }
        }
    }
    thread_t* current_thread = vector_get(queue, last_queue_index);
    if (current_thread->running){
        current_thread->state = state;
    }
    current_thread->running=0;
    thread_t* next_thread = get_next_thread();
    this_cpu()->last_run_queue_index = vector_get_index(this_cpu()->queue, next_thread);
    lapic_eoi();
    lapic_timer_oneshot(TIME_QUANTUM, 33);
    switch_to_thread(next_thread);
}

__attribute__((__noreturn__)) void sched_await()
{
    lapic_timer_oneshot(1000, 33);
    for (;;)
    {
        asm volatile("hlt;");
    }
    __builtin_unreachable();
}

__attribute__((__noreturn__)) void sched_init(void *start)
{
    asm("cli");
    isr[33] = sched_vector;
    // idt_set_ist(33, 1);
    vector_create(&processes_vector, sizeof(process_t));
    kernel_process = sched_process(kernel_pagemap);
    sched_kernel_thread(start, NULL);
    asm("sti");
    sched_await();
}
