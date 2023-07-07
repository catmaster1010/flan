#include "cpu/cpu.h"
#include "dev/fb.h"
#include "lib/stdio.h"
#include "proc/sched.h"

void syscall_handler() {
    printf("suscall");
    // dequeue_and_die();
    return;
}
