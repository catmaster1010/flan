#include "proc/sched.h"
#include "lib/assert.h"
#include "dev/apic/lapic.h"
#include "cpu/idt/idt.h"
#include "cpu/idt/idt.h"
#include "memory/kheap.h"
#include "memory/pmm.h"
#include "cpu/cpu.h"
#include "cpu/smp.h"

process_t* kernel_process;
vector_t processes_vector;

static inline thread_t* current_thread(){
    return vector_get(this_cpu()->queue,0);
}

static __attribute__((__noreturn__)) switch_to_thread(interrupt_frame_t* state){
    printf("okay");
    asm volatile (
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
   // "add rsp, 8\n"
    "swapgs\n"
    "iretq\n"
    :
    : "rm" (state)
    : "memory"
    );
    __builtin_unreachable();
}

process_t* sched_process(pagemap_t* pagemap){
    process_t* proc = kheap_malloc(sizeof(process_t));
    assert(proc);
    vector_push(&processes_vector,proc);
    vector_t* threads=kheap_malloc(sizeof(vector_t));
    vector_create(threads,sizeof(thread_t));
    proc->threads=threads;
    proc->pagemap=pagemap;
    return proc;
}

bool sched_enqueue_thread(thread_t* thread){
    if(thread->enqueued==true){
        return true;
    }
    vector_push(this_cpu()->queue,thread);
}

thread_t* volatile sched_kernel_thread(void* start, void* args){
    thread_t* thread=kheap_malloc(sizeof(thread_t));
    thread->lock=LOCK_INIT;
    interrupt_frame_t state = thread->state;
    state.rsp=kheap_calloc(STACK_SIZE)+HHDM_OFFSET+STACK_SIZE;
    state.cs=0x28;
    state.rflags=0x202;
    state.rip=start;
    state.rdi=args;
    thread->process=kernel_process;
    thread->timeslice=TIME_QUANTUM; 
    thread->running=0;
    sched_enqueue_thread(thread);
    return thread;
}

static bool work_steal(){
    uint64_t most_work = 0;
    cpu_local_t* most_work_cpu;

    for(uint64_t i=0;i<cpus.items;i++){
        cpu_local_t* cpu = vector_get(&cpus,i);
        //printf("%d  %d %d\n",cpu->cpu_number,cpu->queue->items,cpu->bsp);
        uint64_t work = cpu->queue->items;
        if(work>most_work){
            most_work=work;
            most_work_cpu=cpu;
        }
    }
    if(most_work<=1){
        return 0;
    }
    thread_t* last_job = vector_get(most_work_cpu->queue,most_work_cpu->queue_index);
    vector_remove(most_work_cpu->queue,most_work_cpu->queue_index);
    vector_push(this_cpu()->queue,last_job);
    return 1; 
}

static thread_t* get_next_thread(){
    uint64_t queue_index = this_cpu()->queue_index;
    if(this_cpu()->queue->items==queue_index+1){
        return vector_get(this_cpu()->queue,0);
    }
    return vector_get(this_cpu()->queue,queue_index+1);
}

static void sched_vector(uint8_t vector, interrupt_frame_t* state){
    lapic_stop();
    vector_t* queue=this_cpu()->queue;
    uint64_t queue_index = this_cpu()->queue_index;
    //printf("cpu:%d  jobs:%d\n",this_cpu()->cpu_number,this_cpu()->queue->items);
    if(queue->items==0){
        if(!work_steal()){ 
            lapic_eoi();
            lapic_timer_oneshot(1000,33);
            for(;;){asm("hlt");}
        }
        //printf("STOLEN WORK cpu:%d  jobs:%d\n",this_cpu()->cpu_number,this_cpu()->queue->items);
    }
    thread_t* current_thread = vector_get(queue,queue_index);
    current_thread->state=*state;
    thread_t* next_thread=get_next_thread();
    this_cpu()->queue_index=vector_get_index(this_cpu()->queue,next_thread);
    lapic_eoi();
    lapic_timer_oneshot(TIME_QUANTUM,33);
    switch_to_thread(&next_thread->state);
}

__attribute__((__noreturn__)) void sched_await(){
    lapic_timer_oneshot(1000,33);
    for (;;)
    {
        asm volatile("hlt;");
    }
   __builtin_unreachable(); 
}

__attribute__((__noreturn__)) void sched_init(void* start){
    asm("cli"); 
    isr[33]=sched_vector;
    //idt_set_ist(33, 1);
    vector_create(&processes_vector,sizeof(process_t));
    kernel_process=sched_process(kernel_pagemap);
    sched_kernel_thread(start,NULL);
    //printf("%d",kernel_proc->pid);
    asm("sti");
    sched_await();
}
