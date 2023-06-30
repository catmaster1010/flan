#include "proc/sched.h"
#include "lib/stdio.h"
#include "dev/fb.h"
#include "cpu/cpu.h"

void syscall_handler(){
    printf("suscall");
    //dequeue_and_die();
    return;
}
