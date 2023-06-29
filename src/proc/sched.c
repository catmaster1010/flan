#include "proc/sched.h"
#include "lib/assert.h"
#include "dev/apic/lapic.h"
#include "cpu/idt/idt.h"
#include "cpu/idt/idt.h"
#include "memory/kheap.h"
#include "memory/pmm.h"
#include "cpu/cpu.h"
#include "cpu/smp.h"
#include "lib/lock.h"

#define SCHED_VECTOR 34
process_t* kernel_process;
vector_t processes_vector;

thread_t idle_thread= {.running=false,.enqueued=true,.blocked=true,.next=&idle_thread,.prev=&idle_thread};
sched_queue_t queue = {.lock=LOCK_INIT,.start=&idle_thread,.end=&idle_thread};

static inline thread_t* get_current_thread(){
    thread_t* current_thread =  (thread_t*) read_fs_base();
    return current_thread;
}

static __attribute__((__noreturn__)) void switch_to_thread(thread_t* thread)
{
    thread->running=1;
    interrupt_frame_t* state = thread->state;

    asm volatile(
    "mov rsp, %0\n"
        "pop rax\n"
        "mov es, eax\n"
        "pop rax\n" 
        "mov ds, eax\n"
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
        "add rsp, 8\n"
        "swapgs\n"
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
    proc->thread_stack_top = 0x70000000000;
    return proc;
}

bool sched_enqueue_thread(thread_t *thread)
{
    if (thread->enqueued == true) return true;

    spinlock_acquire(&queue.lock);

    thread->prev=queue.end;
    queue.end->next=thread;
    queue.end=thread;
    thread->next=queue.start;
    queue.start->prev=thread;

    spinlock_release(&queue.lock);
    thread->enqueued = true;
    return true;
}

thread_t* sched_user_thread(void *start, void *args, process_t* process){
    thread_t* thread = kheap_calloc(sizeof(thread_t));
    thread->lock = LOCK_INIT;
    thread->state = kheap_calloc(sizeof(interrupt_frame_t));
    interrupt_frame_t* state = thread->state;
    void* stack_phys = pmm_alloc(STACK_SIZE);
    assert(stack_phys);
    vmm_map_pages(process->pagemap, (uintptr_t) stack_phys, process->thread_stack_top - STACK_SIZE, PTE_PRESENT | PTE_WRITABLE | PTE_USER, STACK_SIZE / FRAME_SIZE);
    state->rsp = (uint64_t) process->thread_stack_top;+ STACK_SIZE;
    state->cs = 0x38 | 3;
    state->ds = state->es = state->ss = 0x40 | 3;
    state->rflags = 0x202;
    state->rip = (uint64_t)start;
    state->rdi = (uint64_t)args;
    thread->process = process;
    vector_push(process->threads, thread);
    thread->timeslice = TIME_QUANTUM;
    thread->cr3 = (uint64_t) process->pagemap->top - HHDM_OFFSET;
    sched_enqueue_thread(thread);
    return thread;
}

thread_t* sched_kernel_thread(void *start, void *args)
{
    thread_t* thread = kheap_calloc(sizeof(thread_t));
    thread->lock = LOCK_INIT;
    thread->state = kheap_calloc(sizeof(interrupt_frame_t));
    interrupt_frame_t* state = thread->state;
    void* stack = pmm_alloc(STACK_SIZE);
    assert(stack);
    state->rsp = (uint64_t) stack + STACK_SIZE + HHDM_OFFSET;
    state->cs = 0x28;
    state->ds = state->es = state->ss = 0x30;
    state->rflags = 0x202;
    state->rip = (uint64_t)start;
    state->rdi = (uint64_t)args;
    thread->process = kernel_process;
    vector_push(kernel_process->threads, thread);
    thread->timeslice = TIME_QUANTUM;
    thread->cr3 = (uint64_t) kernel_process->pagemap->top -HHDM_OFFSET;
    sched_enqueue_thread(thread);
    return thread;
}

static thread_t* get_next_thread(){
    thread_t* current_thread = get_current_thread();

    for (thread_t* next_thread = current_thread->next; next_thread != current_thread; next_thread=next_thread->next) {
        if (next_thread->blocked || next_thread->running) continue;
        if (spinlock_test_and_acq(&next_thread->lock) == true) return next_thread;
    }
    return NULL;
}

static void sched_vector(uint8_t vector, interrupt_frame_t *state)
{
    thread_t* current_thread = get_current_thread();
    if (current_thread->running){
        current_thread->state = state;
        current_thread->gs_base = read_gs_base();
    }

    thread_t* next_thread = get_next_thread();
    if (next_thread == NULL) {
        lapic_eoi();
        lapic_timer_oneshot(current_thread->timeslice, SCHED_VECTOR);
        return;
    }
    current_thread->running=0;
    spinlock_release(&current_thread->lock);

    next_thread->cpu= this_cpu();

    set_fs_base(next_thread);
    if (next_thread->state->cs == 0x3b) { // if user
        set_kgs_base(next_thread->gs_base);
    }

    if (read_cr3() != next_thread->cr3) {
        write_cr3(next_thread->cr3);
    }

    lapic_eoi();
    lapic_timer_oneshot(current_thread->timeslice, SCHED_VECTOR);

    switch_to_thread(next_thread);
}

bool dequeue_thread(thread_t* thread){
    if (!thread->enqueued) return true;
    //remove thread frome queue
	thread->next->prev =thread->prev;
	thread->prev->next=thread->next;
    spinlock_release(&thread->lock);
    thread->enqueued=false;
    thread->running=false;
    return true;
}

void dequeue_and_die(){
    asm volatile ("cli");
    lapic_stop();
    thread_t* current_thread = get_current_thread();
    dequeue_thread(current_thread);
    kheap_free(current_thread);
    set_fs_base(&idle_thread);
    asm volatile ("sti");
    lapic_ipi(this_cpu()->lapic_id,SCHED_VECTOR); 
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
    idt_set_ist(SCHED_VECTOR, 1);
    vector_create(&processes_vector, sizeof(process_t));

    kernel_process = sched_process(kernel_pagemap);
    idle_thread.process=kernel_process;
    thread_t* kernel_thread = sched_kernel_thread(start, NULL);
    asm("sti");
    sched_await();
}
