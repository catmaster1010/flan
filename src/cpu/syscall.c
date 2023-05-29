#include "proc/sched.h"
#include "lib/stdio.h"

void syscall_handler(){
    printf("syscall ");
    dequeue_and_die();
    return;
}
