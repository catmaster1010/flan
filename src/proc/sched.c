#include "proc/sched.h"
#include "lib/assert.h"
#include "dev/apic/lapic.h"
#include "cpu/idt/idt.h"
#include "cpu/idt/idt.h"
#include "memory/kheap.h"
#include "memory/pmm.h"
#include "cpu/cpu.h"
#include "cpu/smp.h"

#define SCHED_VECTOR 34

process_t *kernel_process;
vector_t processes_vector;

thread_t* get_current_thread(){
    vector_t *queue = this_cpu()->queue;
    uint64_t last_queue_index = this_cpu()->last_run_queue_index;
    thread_t* current_thread = vector_get(queue, last_queue_index);
    return current_thread;
}

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
    process_t *proc = kheap_alloc(sizeof(process_t));
    assert(proc);
    vector_push(&processes_vector, proc);
    vector_t *threads = kheap_alloc(sizeof(vector_t));
    vector_create(threads, sizeof(thread_t));
    proc->threads = threads;
    proc->pagemap = pagemap;
    proc->fildes = kheap_alloc(sizeof(vector_t));
    vector_create(proc->fildes,sizeof(vfs_node_t));
    proc->cwd=root;
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
    thread->running = false;
    sched_enqueue_thread(thread);
    return thread;
}

static bool work_steal()
{
    uint64_t most_work = 0;
    cpu_local_t *most_work_cpu;

    for (uint64_t i = 0; i < cpus.items; i++){
        cpu_local_t *cpu = vector_get(&cpus, i);
        uint64_t work = cpu->queue->items;
        if (work<=1){continue;}

        for (uint64_t k;k<work; k++) {
            thread_t* thread = vector_get(cpu,k);
            if (thread->blocked) {continue;} 
            vector_remove(cpu,k);
            vector_push(this_cpu()->queue,thread);
            return 1;
        }
    }
    return 0;
}

static thread_t *get_next_thread(uint64_t index)
{
    uint64_t last_queue_index = this_cpu()->last_run_queue_index;
    thread_t* thread;
    if (this_cpu()->queue->items == index + 1){
        thread= vector_get(this_cpu()->queue, 0);
    }
    else {
    thread= vector_get(this_cpu()->queue, index + 1);
    }
    if (thread->blocked) {
        thread=get_next_thread(index+1);
    }
    return thread;
}

static void sched_vector(uint8_t vector, interrupt_frame_t *state)
{
    lapic_stop();

    vector_t *queue = this_cpu()->queue;
    if (queue->items == 0)
    {
        if (!work_steal())
        {
            lapic_eoi();
            lapic_timer_oneshot(1000, SCHED_VECTOR);
            for (;;)
            {
                asm("hlt");
            }
        }
    }
    thread_t* current_thread = get_current_thread();
    if (current_thread->running){
        current_thread->state = state;
    }
    current_thread->running=0;
    thread_t* next_thread = get_next_thread(this_cpu()->last_run_queue_index);
    this_cpu()->last_run_queue_index = vector_get_index(this_cpu()->queue, next_thread);
    lapic_eoi();
    lapic_timer_oneshot(TIME_QUANTUM, SCHED_VECTOR);
    switch_to_thread(next_thread);
}

bool dequeue_thread(thread_t* thread){
    if (!thread->enqueued) return true;
    vector_t* queue=this_cpu()->queue;
    vector_remove(queue,vector_get_index(queue,thread));
    thread->enqueued=false;
    thread->running=false;
    if (!thread==get_current_thread()) return true;

    if (this_cpu()->last_run_queue_index==0){
        this_cpu()->last_run_queue_index=this_cpu()->queue->items;
    }
    else {
       this_cpu()->last_run_queue_index--; 
    }  

    return true;
}

void dequeue_and_die(){
    asm volatile ("cli");
    lapic_stop();
    thread_t* current_thread = get_current_thread();
    dequeue_thread(current_thread);
    kheap_free(current_thread);
    lapic_ipi(this_cpu()->cpu_number,SCHED_VECTOR); 
    asm volatile ("sti");
    for (;;) {
        asm volatile("hlt");
    }
    __builtin_unreachable();
}

__attribute__((__noreturn__)) void sched_await()
{
    lapic_timer_oneshot(1000, SCHED_VECTOR);
    for (;;)
    {
        asm volatile("hlt;");
    }
    __builtin_unreachable();
}

__attribute__((__noreturn__)) void sched_init(void *start)
{
    asm("cli");
    isr[SCHED_VECTOR] = sched_vector;
    // idt_set_ist(33, 1);
    vector_create(&processes_vector, sizeof(process_t));
    kernel_process = sched_process(kernel_pagemap);
    sched_kernel_thread(start, NULL);
    asm("sti");
    sched_await();
}
