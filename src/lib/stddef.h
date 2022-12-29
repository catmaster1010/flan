#ifndef stddef_h
#define stddef_h

#define offset_of(TYPE, MEMBER) ((uint64_t) &((TYPE *)0)->MEMBER)

#define MAX(A, B) ({ \
    __auto_type a = A; \
    __auto_type b = B; \
    a > b ? a : b; \
})
#define NULL 0

#endif