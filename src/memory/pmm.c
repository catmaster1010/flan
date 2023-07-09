#include <lib/assert.h>
#include <lib/lock.h>
#include <lib/stddef.h>
#include <lib/stdio.h>
#include <lib/str.h>
#include <limine.h>
#include <memory/pmm.h>
#include <memory/vmm.h>
#include <lib/stddef.h>

volatile struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST, .revision = 0};

uint8_t *bitmap;
uint64_t limit;

spinlock_t pmm_lock = LOCK_INIT;

void pmm_init() {
    uint64_t usable = 0;
    uint64_t available = 0;
    uint64_t highest = 0;
    struct limine_memmap_entry **mmaps = memmap_request.response->entries;
    uint64_t mmmap_count = memmap_request.response->entry_count;

    printf("There are %d entries in the mmap.\n",
           memmap_request.response->entry_count);
    for (uint64_t i = 0; i < mmmap_count; i++) {
        available += mmaps[i]->length;

        printf("entry %d    base: %x    length: %x    type: %d    tail: %x\n",
               i + 1, mmaps[i]->base, mmaps[i]->length, mmaps[i]->type,
               mmaps[i]->base + mmaps[i]->length);
        if (!mmaps[i]->type == LIMINE_MEMMAP_USABLE) {
            continue;
        }
        usable += mmaps[i]->length;
        uint64_t top = mmaps[i]->base + mmaps[i]->length;
        if (top > highest)
            highest = top;
    }
    limit = highest / FRAME_SIZE;
    uint64_t bitmap_size = ALIGN_UP(highest / FRAME_SIZE / 8, FRAME_SIZE);

    for (uint64_t i = 0; i < mmmap_count; i++) {
        if (!mmaps[i]->type == LIMINE_MEMMAP_USABLE)
            continue;
        ;
        if (mmaps[i]->length >= bitmap_size) {
            bitmap = (uint8_t *)mmaps[i]->base;
            memset(bitmap, 0xff, bitmap_size);
            mmaps[i]->length -= bitmap_size;
            mmaps[i]->base += bitmap_size;
            available -= bitmap_size;
            break;
        }
    }
    for (uint64_t i = 0; i < mmmap_count; i++) {
        if (mmaps[i]->type != LIMINE_MEMMAP_USABLE)
            continue;

        for (uint64_t t = 0; t < mmaps[i]->length; t += FRAME_SIZE) {
            pmm_free((void *)mmaps[i]->base + t, 1);
        }
    }
    printf("%dMiB/%dMiB of usable memmory\n", usable / 1000 / 1000,
           available / 1000 / 1000);
    printf("PMM initialized.\n");
}

void pmm_free(void *ptr, uint64_t frames) {
    spinlock_acquire(&pmm_lock);
    uint64_t frame = (uint64_t)ptr / FRAME_SIZE;
    for (uint64_t i = frame; i < frames + frame; i++) {
        BIT_CLEAR(i);
    }
    spinlock_release(&pmm_lock);
}

void *pmm_malloc(uint64_t wanted_frames) {
    spinlock_acquire(&pmm_lock);
    void *ptr;

    uint64_t available_frames = 0;
    for (uint64_t frame = 1; frame < limit; frame++) {
        if (!BIT_TEST(frame)) {
            available_frames++;
        } else if (available_frames != wanted_frames) {
            available_frames = 0;
            continue;
        }
        if (available_frames == wanted_frames) {
            uint64_t i;
            for (i = 0; i < wanted_frames; i++) {
                BIT_SET(frame - i);
            }
            frame -= i - 1;
            spinlock_release(&pmm_lock);

            ptr = (void *)(FRAME_SIZE * frame);
            return ptr;
        }
    }
    spinlock_release(&pmm_lock);
    return NULL;
}

void *pmm_calloc(uint64_t frames) {
    void *ptr = pmm_malloc(frames);
    memset(ptr + HHDM_OFFSET, 0, frames * FRAME_SIZE);
    return ptr;
}

void *pmm_alloc(uint64_t size) {
    uint64_t frames = (size + (FRAME_SIZE - 1)) / FRAME_SIZE;
    void *ptr = pmm_calloc(frames);
    assert(ptr);
    return ptr;
}
