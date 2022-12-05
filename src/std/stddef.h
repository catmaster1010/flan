#ifndef stddef_h
#define stddef_h
    #define offsetof(TYPE, MEMBER) ((uint64_t) &((TYPE *)0)->MEMBER)
    #define NULL 0
#endif