#ifndef assert_h
#define assert_h
#include "cpu/smp.h"
#define cRED    "\033[1;31m"
#define assert(check) ({ \
    if(!(check)) { \
        printf("\n%sassert() failed in %s() (%s:%d)\n", cRED,__func__, __FILE__, __LINE__); \
        for(;;) asm("hlt"); \
    } \
})
#endif
