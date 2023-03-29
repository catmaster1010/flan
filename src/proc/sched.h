#ifndef sched_h
#define sched_h
#include "lib/vector.h"
#include "memory/vmm.h"
#include "cpu/cpu.h"
#include "lib/lock.h"
#include "fs/vfs.h"

#define TIME_QUANTUM 3
#define MAX_THREADS 40

typedef struct {
    uint64_t pid;
    pagemap_t* pagemap;
    vector_t* threads;
    vfs_node_t* cwd;
    vector_t* fildes;
} process_t;

typedef struct {
    bool running; 
    process_t* process;
    spinlock_t lock; 
    interrupt_frame_t* state;
    uint8_t timeslice;
    bool enqueued;
    bool blocked;
} thread_t;

thread_t* get_current_thread();
thread_t* sched_thread();
process_t* sched_process(pagemap_t* pagemap);
__attribute__((__noreturn__)) void sched_await();
__attribute__((__noreturn__)) void sched_init(void* start);
thread_t* sched_kernel_thread(void* start, void* args);
bool sched_enqueue_thread(thread_t* thread);
#endif
