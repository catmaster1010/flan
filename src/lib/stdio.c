#include "dev/console.h"
#include "dev/serial.h"
#include "flanterm/backends/fb.h"
#include "lib/lock.h"
#include "lib/str.h"
#include "limine.h"
#include <stdarg.h>

spinlock_t print_lock = LOCK_INIT;

void out(char *str) {
    serial_out(str);
    console_write(str);
}

void printf(char *fmt, ...) {
    spinlock_acquire(&print_lock);
    va_list args;
    va_start(args, fmt);
    for (char *c = fmt; *c != '\0'; c++) {
        if (*c == '%') {
            c++;
            switch (*c) {
            case 's':
                out(va_arg(args, char *));
                break;
            case 'd':
                out(itoa(va_arg(args, uint64_t), 10));
                break;
            case 'x':
                out("0x");
                out(itoa(va_arg(args, uint64_t), 16));
                break;
            case 'b':
                out("0b");
                out(itoa(va_arg(args, uint64_t), 2));
                break;
            }
        } else {
            char str[] = {*c, '\0'};
            out(str);
        }
    }
    va_end(args); // Not needed for GCC (or clang). Here for compatibility.
    spinlock_release(&print_lock);
}
