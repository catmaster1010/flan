#ifndef pmm_h
#define pmm_h
#include <stdint.h>
#include <limine.h>
#include "lib/stddef.h"
void pmm_init();

void pmm_free(uint64_t ptr,uint64_t frames);

void* pmm_malloc(uint64_t frames);
void* pmm_calloc(uint64_t frames);
void* pmm_alloc(uint64_t size);

#define FRAME_SIZE 0x1000 // 4096 bytes pages, 4kb 

extern  volatile struct limine_memmap_request memmap_request;
#endif
