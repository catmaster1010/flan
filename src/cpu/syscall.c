#include "proc/sched.h"
#include "lib/stdio.h"
#include "dev/fb.h"

void syscall_handler(){
    fb_colour_background();
    return;
}
