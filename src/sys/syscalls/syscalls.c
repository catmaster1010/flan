#include <lib/stdio.h>
#include <proc/sched.h>

void syscall_log(char* str){
    printf(str);    
}

__attribute__((__noreturn__)) void syscall_exit_thread(){
    dequeue_and_die();
}
