#ifndef pmm_h
#define pmm_h
#include <stdint.h>
#include <limine.h>
#include "lib/stddef.h"
void pmm_init();

void pmm_free(uint64_t ptr,uint64_t frames);

void* pmm_malloc(uint64_t frames);
void* pmm_calloc(uint64_t frames);
void* alloc(uint64_t size);

void test_pmm();

#define FRAME_SIZE 0x1000 // 4096 bytes pages, 4kb 

#define ALIGN_UP(num,align) (((num) + align - 1) & ~(align - 1)) 
#define ALIGN_DOWN(num,align) ((num) & ~(align - 1))
#define container_of(ptr, type, member) ((type* )( (char* )ptr - offset_of(type,member) ))

#define BIT_SET(bit) (bitmap[(bit) / 8] |= (1<< ((bit) % 8))) //sets bit to  one
#define BIT_CLEAR(bit) (bitmap[(bit) / 8] &= ~(1 << ((bit) % 8))) //sets bit to  zero 
#define BIT_TEST(bit) ((bitmap[(bit) / 8] >> ((bit) % 8)) & 1) // returns the bit
extern  volatile struct limine_memmap_request memmap_request;
#endif