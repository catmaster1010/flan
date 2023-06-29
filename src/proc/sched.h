#ifndef sched_h
#define sched_h
#include "lib/vector.h"
#include "memory/vmm.h"
#include "cpu/cpu.h"
#include "lib/lock.h"
#include "fs/vfs.h"

#define TIME_QUANTUM 3

typedef struct {
    uint64_t pid;
    pagemap_t* pagemap;
    vector_t* threads;
    vfs_node_t* cwd;
    vector_t* fildes;
    uint64_t thread_stack_top;
} process_t;

struct thread {
    void* kernel_stack;
    struct thread* prev;
    struct thread* next;
    uint64_t cr3;
    cpu_local_t* cpu;
    bool running; 
    process_t* process;
    spinlock_t lock; 
    interrupt_frame_t* state;
    uint8_t timeslice;
    bool enqueued;
    bool blocked;
    void* gs_base;
};
typedef struct thread thread_t;

typedef struct{
	thread_t* start;
	thread_t* end;
	spinlock_t lock;
} sched_queue_t;

extern thread_t idle_thread;
extern process_t* kernel_process;
thread_t* sched_thread();
bool dequeue_thread(thread_t* thread);
void dequeue_and_die();
process_t* sched_process(pagemap_t* pagemap);
__attribute__((__noreturn__)) void sched_await();
__attribute__((__noreturn__)) void sched_init(void* start);
thread_t* sched_kernel_thread(void* start, void* args);
thread_t* sched_user_thread(void *start, void *args, process_t* process);
bool sched_enqueue_thread(thread_t* thread);


#endif
