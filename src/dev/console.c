#include "dev/console.h"
#include "dev/fb.h"
#include "memory/kheap.h"
#include "lib/lock.h"

spinlock_t console_lock = LOCK_INIT;
static struct flanterm_context *term_context = NULL;

void console_init(){
    struct limine_framebuffer* framebuffer = fb.response->framebuffers[0];
    term_context =  flanterm_fb_init(
        kheap_alloc,
        kheap_free,
        framebuffer->address, framebuffer->width, framebuffer->height, framebuffer->pitch,
        NULL,
        NULL, NULL,
        NULL, NULL,
        NULL, NULL,
        NULL, 0, 0, 1,
        1, 1,
        0
    );
}

void console_write(char* str){
    if (term_context != NULL) {
        spinlock_acquire(&console_lock);
        flanterm_write(term_context, str, strlen(str));
        spinlock_release(&console_lock);
    }
}


