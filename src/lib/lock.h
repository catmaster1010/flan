#ifndef  lock_h
#define  lock_h
#include  <stdbool.h>
#define CAS __sync_bool_compare_and_swap

typedef struct {
    int lock;
    const char* file;
} spinlock_t;

#define LOCK_INIT (spinlock_t) {0,__FILE__}

int spinlock_test_and_acq(spinlock_t* lock);
void spinlock_acquire(spinlock_t* lock);
void spinlock_release(spinlock_t* lock);
#endif
