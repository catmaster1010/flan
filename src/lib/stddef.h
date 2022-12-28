#ifndef stddef_h
#define stddef_h
    #define offsetof(TYPE, MEMBER) ((uint64_t) &((TYPE *)0)->MEMBER)
    #define MAX(A, B) ({ \
    __auto_type a = A; \
    __auto_type b = B; \
    a > b ? a : b; \
})
#endif