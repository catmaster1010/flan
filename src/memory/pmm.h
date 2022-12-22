#pragma once
#ifndef pmm_h
#define pmm_h
#include <stdint.h>
#include <limine.h>
void pmm_init();

void pmm_free(uint64_t ptr,uint64_t frames);

uint64_t* pmm_malloc(uint64_t frames);

void test_pmm();

#define FRAME_SIZE 0x1000 // 4096 bytes pages, 4kb 

#define ALIGN_UP(num,align) (((num) + align - 1) & ~(align - 1)) 
#define ALIGN_DOWN(num,align) ((num) & ~(align - 1))
#define container_of(ptr, type, member) ((type *)( (char *)ptr - offsetof(type,member) ))

#define BIT_SET(bit) (bitmap[(bit) / 8] |= (1<< ((bit) % 8))) //sets bit to  one
#define BIT_CLEAR(bit) (bitmap[(bit) / 8] &= ~(1 << ((bit) % 8))) //sets bit to  zero 
#define BIT_TEST(bit) ((bitmap[(bit) / 8] >> ((bit) % 8)) & 1) // returns the bit

#endif


/*struct dll {
     struct dll *next;
     struct dll *prev;
}__attribute__((packed));
typedef struct dll dll_t;

struct alloc_node {
     dll_t node;
     uint64_t size;
     char * cBlock;
}__attribute__((packed));
typedef struct alloc_node alloc_node_t;
void dll_add( dll_t* n,  dll_t* head);
void add_block(uint64_t *addr, uint64_t size);
void coalesce_dll();

void dll_list_add(dll_t* n, dll_t* prev, dll_t* next);*/