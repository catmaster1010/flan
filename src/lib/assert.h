#ifndef assert_h
#define assert_h
#include "cpu/smp.h"
#define cRED    "\033[1;31m"
#define assert(check) ({ \
    if(!(check)) { \
        printf("%sPANIC in %s() (%s:%d) triggered by core #%d\n", cRED,__func__, __FILE__, __LINE__,vector_get_index(&cpus,this_cpu())); \
        for(;;) asm("hlt"); \
    } \
})
#endif
