#include "lib/lock.h"

int spinlock_test_and_acq(spinlock_t*  lock){
        return CAS(lock, 0, 1);
}

void spinlock_acquire(spinlock_t* lock){
    for (;;) {
        if (spinlock_test_and_acq(lock)) {
            break;
        }

        __asm__ volatile ("pause");

    }
}

void spinlock_release(spinlock_t* lock){
   CAS(lock, 1, 0);
}