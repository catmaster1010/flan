#include "lib/lock.h"

int spinlock_test_aquire(spinlock_t*  lock){
    int ret;
    __asm__ volatile (
        "lock bts %0, 0;"
        : "+m" (lock->lock), "=@ccc" (ret) //carry flag into ret
        : 
        : "memory"
    );
    return ret;
}

void spinlock_aquire(spinlock_t* lock){
        for (;;) {
            if (spinlock_test_aquire(lock)) {break;}
            __asm__ volatile("pause");
        }
}

void spinlock_release(spinlock_t* lock){
    __asm__ volatile (
        "lock bts %0, 0;"
        : "+m" (lock->lock)
        :
        : "memory", "cc"
    );
}