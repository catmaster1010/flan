#ifndef sched_h
#define sched_h
#include <fs/vfs.h>
#include <lib/elf.h>
#include <lib/lock.h>
#include <lib/stddef.h>
#include <lib/vector.h>
#include <memory/vmm.h>
#include <sys/cpu.h>

struct interrupt_frame;
#define TIME_QUANTUM 3
#define MAX_FILDES 256

typedef struct {
    uint64_t pid;
    pagemap_t *pagemap;
    vector_t *threads;
    vfs_node_t *cwd;
    vector_t *fildes;
    int current_filde;
    uintptr_t thread_stack_top;
    uintptr_t anon_base;
} process_t;

struct thread {
    int errno;
    struct thread *prev;
    struct thread *next;
    uint64_t cr3;
    struct cpu_local *cpu;
    bool running;
    process_t *process;
    spinlock_t lock;
    struct interrupt_frame *state;
    uint8_t timeslice;
    bool enqueued;
    bool blocked;
    void *gs_base;
    void *fs_base;
};
typedef struct thread thread_t;

typedef struct {
    thread_t *start;
    thread_t *end;
    spinlock_t lock;
} sched_queue_t;

extern process_t *kernel_process;
thread_t *sched_thread();
bool dequeue_thread(thread_t *thread);
void dequeue_and_die();
__attribute__((__noreturn__)) void dequeue_and_die();
process_t *sched_process(pagemap_t *pagemap);
__attribute__((__noreturn__)) void sched_await();
__attribute__((__noreturn__)) void sched_init(void *start);
thread_t *sched_kernel_thread(void *start, void *args);
thread_t *sched_user_thread(void *start, void *arg, process_t *process,
                            const char **argv, const char **envp,
                            struct auxval *aux);
bool sched_enqueue_thread(thread_t *thread);
thread_t *get_current_thread();
#endif
