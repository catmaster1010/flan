#ifndef sched_h
#define sched_h
#include "lib/vector.h"
#include "memory/vmm.h"
#include "cpu/cpu.h"

typedef struct {
    uint64_t pid;
    pagemap_t* pagemap;
    vector_t* threads;
} process_t;

typedef struct {
    bool running; 
    uint64_t tid;
    process_t* process; 
    interrupt_frame_t state;
    uint8_t timeslice;
} thread_t;

__attribute__((__noreturn__)) void sched_await();

__attribute__((__noreturn__)) void sched_init(void* (*start)(void* ));
#endif
