#define assert(check) ({ \
    if(!(check)) { \
        printf("ERROR! assert() in function %s (%s:%d)\n", __func__, __FILE__, __LINE__); \
        for(;;) __asm__("hlt"); \
    } \
})