#ifndef pmm_h
#define pmm_h
#include "lib/stddef.h"
#include <limine.h>
#include <stdint.h>
void pmm_init();

void pmm_free(void *ptr, uint64_t frames);

void *pmm_malloc(uint64_t frames);
void *pmm_calloc(uint64_t frames);
void *pmm_alloc(uint64_t size);

#define FRAME_SIZE 0x1000 // 4096 bytes pages, 4kb

extern volatile struct limine_memmap_request memmap_request;
#endif
