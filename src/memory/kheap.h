#ifndef kheap_h
#define kheap_h
#include <stdint.h>
struct dll {
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

void dll_list_add(dll_t* n, dll_t* prev, dll_t* next);

void* kheap_malloc(uint64_t size);
void* kheap_calloc(uint64_t size);
void* kheap_realloc(void *ptr, uint64_t newsize, uint64_t oldsize);
void kheap_free(uint64_t ptr);
void kheap_init();
#endif