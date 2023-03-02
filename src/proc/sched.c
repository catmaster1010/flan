#include "proc/sched.h"
#include "lib/assert.h"
#include "dev/apic/lapic.h"
#include "cpu/idt/idt.h"
#include "cpu/idt/idt.h"
#include "memory/kheap.h"
vector_t processes_vector;

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


static void sched_vector(uint8_t vector, interrupt_frame_t* state){
    lapic_stop();
    printf("ok");
    lapic_eoi();

    lapic_timer_oneshot(1000,33);
}

__attribute__((__noreturn__)) void sched_await(){
    lapic_timer_oneshot(1000,33);
    for (;;)
    {
        asm volatile("hlt;");
    }
   __builtin_unreachable(); 
}

__attribute__((__noreturn__)) void sched_init(void* (*start)(void* )){
    isr[33]=sched_vector;
    //idt_set_ist(33, 1);
    vector_create(&processes_vector,sizeof(process_t));
    process_t* kernel_proc=sched_process(kernel_pagemap);
  //  printf("%d",kernel_proc->pid);
    lapic_timer_oneshot(1000,33);

    for (;;)
    {
        asm volatile("sti;hlt");
    }
   __builtin_unreachable(); 
   assert(0);

}
