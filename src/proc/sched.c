#include "proc/sched.h"
#include "lib/assert.h"



__attribute__((__noreturn__)) void sched_await(){
    for (;;)
    {
        __asm__ volatile("hlt;");
    }
   __builtin_unreachable(); 
}

__attribute__((__noreturn__)) void sched_init(void* (*start)(void* )){

    sched_await();
   __builtin_unreachable(); 
   assert(0);

}
