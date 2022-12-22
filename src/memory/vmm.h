#pragma once
#ifndef vmm_h
#define vmm_h
#include <stdint.h>
#include"lib/assert.h"
#include"lib/lock.h"
#define PTE_PRESENT ((uint64_t)1 << (uint64_t)0)
#define PTE_WRITABLE ((uint64_t)1 << (uint64_t)1)
#define PTE_USER ((uint64_t)1 << (uint64_t)2)
#define PTE_NX ((uint64_t)1 << (uint64_t)3)
void vmm_init();
typedef struct{
    uint64_t *top;
} pagemap_t;

extern volatile struct limine_hhdm_request hhdm_request;
#define HIGHER_HALF (hhdm_request.response->offset)
#endif