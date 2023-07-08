#include <dev/fb.h>
#include <lib/stdio.h>
#include <proc/sched.h>
#include <sys/cpu.h>

void syscall_handler() {
    printf("suscall");
    // dequeue_and_die();
    return;
}
