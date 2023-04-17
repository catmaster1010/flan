#ifndef stddef_h
#define stddef_h

#define offset_of(TYPE, MEMBER) ((uint64_t) &((TYPE *)0)->MEMBER)

#define MAX(A, B) ({ \
    __auto_type a = A; \
    __auto_type b = B; \
    a > b ? a : b; \
})
#define NULL 0

#define ALIGN_UP(num,align) (((num) + align - 1) & ~(align - 1)) 
#define ALIGN_DOWN(num,align) ((num) & ~(align - 1))
#define container_of(ptr, type, member) ((type* )( (char* )ptr - offset_of(type,member) ))

#define BIT_SET(bit) (bitmap[(bit) / 8] |= (1<< ((bit) % 8))) //sets bit to  one
#define BIT_CLEAR(bit) (bitmap[(bit) / 8] &= ~(1 << ((bit) % 8))) //sets bit to  zero 
#define BIT_TEST(bit) ((bitmap[(bit) / 8] >> ((bit) % 8)) & 1) // returns the bit
#endif
