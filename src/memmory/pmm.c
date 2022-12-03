#include "std/stdio.h"
#include "memmory/pmm.h"
#include <limine.h>

volatile struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
    };

void pmm_init()
{
    printf("There are %d entries in the mmap\n", memmap_request.response->entry_count);
    for (int i = 0; i < memmap_request.response->entry_count; i++)
    {
        printf("entry %d    base: %x    length: %x    type: %d    tail: %x\n",
               i+1,
               memmap_request.response->entries[i]->base,
               memmap_request.response->entries[i]->length,
               memmap_request.response->entries[i]->type,
               memmap_request.response->entries[i]->base+memmap_request.response->entries[i]->length
               );
    }
}
