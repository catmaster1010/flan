#ifndef assert_h
#define assert_h
#define cRED    "\033[1;31m"
#define assert(check) ({ \
    if(!(check)) { \
        printf("%sPANIC in %s() (%s:%d)\n", cRED,__func__, __FILE__, __LINE__); \
        for(;;) __asm__("hlt"); \
    } \
})
#endif
