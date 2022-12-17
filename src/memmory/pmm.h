#ifndef pmm_h
#define pmm_h

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

void pmm_init();

void dll_add( dll_t* n,  dll_t* head);
void add_block(uint64_t *addr, uint64_t size);

uint64_t* malloc(uint64_t size);
void free(uint64_t ptr);
uint64_t* calloc(uint64_t size);

void coalesce_dll();

void dll_list_add(dll_t* n, dll_t* prev, dll_t* next);

void test_pmm();

#define ALIGN_UP(num,align) (((num) + align - 1) & ~(align - 1)) 
#define ALIGN_DOWN(num,align) ((num) & ~(align - 1))
#define container_of(ptr, type, member) ((type *)( (char *)ptr - offsetof(type,member) ))
#endif
