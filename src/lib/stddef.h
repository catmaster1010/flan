#ifndef stddef_h
#define stddef_h

#define OFFSET_OF(TYPE, MEMBER) ((uint64_t) &((TYPE *)0)->MEMBER)

#define MAX(A, B) ({ \
    __auto_type a = A; \
    __auto_type b = B; \
    a > b ? a : b; \
})
#define NULL 0

#define ALIGN_UP(NUM,ALIGN) (((NUM) + ALIGN - 1) & ~(ALIGN - 1)) 
#define ALIGN_DOWN(NUM,ALIGN) ((NUM) & ~(ALIGN - 1))
#define CONTAINER_OF(PTR, TYPE, MEMBER) ((TYPE* )((void*) PTR - OFFSET_OF(TYPE,MEMBER)))

#define BIT_SET(BIT) (bitmap[(BIT) / 8] |= (1<< ((BIT) % 8))) //sets BIT to  one
#define BIT_CLEAR(BIT) (bitmap[(BIT) / 8] &= ~(1 << ((BIT) % 8))) //sets BIT to  zero 
#define BIT_TEST(BIT) ((bitmap[(BIT) / 8] >> ((BIT) % 8)) & 1) // returns the BIT
#endif
