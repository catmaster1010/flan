#ifndef pmm_h
#define pmm_h

void pmm_init();

struct dll {
     struct dll *next;
     struct dll *prev;
}__attribute__((packed));
typedef struct dll dll_t;

struct alloc_node {
     dll_t node;
     uint64_t size;
}__attribute__((packed));
typedef struct alloc_node alloc_node_t;

void dll_add( dll_t* n,  dll_t* head);
void add_block(uint64_t *addr, uint64_t size);
void malloc(uint64_t size);


#endif