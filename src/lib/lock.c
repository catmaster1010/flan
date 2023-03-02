#include "lib/lock.h"
#include <stdint.h>
#include "lib/assert.h"

int spinlock_test_and_acq(spinlock_t*  lock){
        return CAS(lock, 0, 1);
}

void spinlock_acquire(spinlock_t* lock){
    uint64_t spinlock_count;

    for (;;) {
        if (spinlock_test_and_acq(lock)) {
            break;
        }
        spinlock_count++;
        if (spinlock_count>10000000)
        {
             assert(0);
        }
        
        asm volatile ("pause");
    }
}

void spinlock_release(spinlock_t* lock){
   CAS(lock, 1, 0);
}