#ifndef kheap_h
#define kheap_h
#include <stdint.h>
#include "memory/pmm.h"

typedef struct dll {
     struct dll* next;
     struct dll* prev;
} dll_t;

typedef struct alloc_node {
     dll_t node;
     uint64_t size;
     void* cBlock;
} alloc_node_t;

void dll_add( dll_t* n,  dll_t* head);
void add_block(uint64_t *addr, uint64_t size);
void coalesce_dll();

void dll_list_add(dll_t* n, dll_t* prev, dll_t* next);

void* kheap_alloc(uint64_t size);
void* kheap_calloc(uint64_t size);
void* kheap_realloc(void *ptr, uint64_t new_size);
void kheap_free(void* ptr);
void kheap_init();
#endif
