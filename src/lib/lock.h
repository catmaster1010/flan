#ifndef  lock_h
#define  lock_h
#include  <stdbool.h>
#define CAS __sync_bool_compare_and_swap
#define LOCK_INIT 0
typedef int spinlock_t;

int spinlock_test_acq(spinlock_t* lock);
void spinlock_acquire(spinlock_t* lock);
void spinlock_release(spinlock_t* lock);
#endif
