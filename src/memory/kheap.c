#include "memory/kheap.h"
#include "lib/lock.h"
#include "lib/stddef.h"
#include "lib/stdio.h"
#include "lib/str.h"
#include "memory/vmm.h"

#define HEADER_SIZE 24
static dll_t free_list = {.next = &free_list, .prev = &free_list};
spinlock_t kheap_lock = LOCK_INIT;

void dll_list_add(dll_t *n, dll_t *prev, dll_t *next) {
    next->prev = n;
    n->next = next;
    n->prev = prev;
    prev->next = n;
}

void add_block(void *addr, uint64_t size) {
    alloc_node_t *block = (alloc_node_t *)addr;
    block->size = size - HEADER_SIZE;
    dll_list_add((dll_t *)&block->node, free_list.prev, &free_list);
};

void coalesce_dll() {
    alloc_node_t *prevBlock = NULL;
    for (alloc_node_t *block = CONTAINER_OF(free_list.next, alloc_node_t, node);
         &block->node != &free_list;
         block = CONTAINER_OF(block->node.next, alloc_node_t, node)) {
        if (prevBlock != NULL && (alloc_node_t *)(prevBlock + prevBlock->size +
                                                  HEADER_SIZE) == block) {
            prevBlock->size += block->size + HEADER_SIZE;
            // removes block
            block->node.next->prev = block->node.prev;
            block->node.prev->next = block->node.next;
            continue;
        }
        prevBlock = block;
    }
}

void *kheap_alloc(uint64_t size) {
    spinlock_acquire(&kheap_lock);
    void *ptr = NULL;
    alloc_node_t *block;
    // Try to find a big enough block to alloc (First fit)
    for (block = CONTAINER_OF(free_list.next, alloc_node_t, node);
         &block->node != &free_list;
         block = CONTAINER_OF(block->node.next, alloc_node_t, node)) {
        if (block->size >= size) {
            ptr = &block->cBlock;
            // printf("\nFound block for requested size at: %x\n",ptr);
            break;
        }
    }
    if (!ptr) {
        uint64_t frames = ALIGN_UP(size + HEADER_SIZE, FRAME_SIZE) / FRAME_SIZE;
        void *new = pmm_malloc(frames) + HHDM_OFFSET;
        add_block(new, frames * FRAME_SIZE);
        coalesce_dll();
        spinlock_release(&kheap_lock);
        return kheap_alloc(size);
    }

    // Can block be split
    if ((block->size - size) >= HEADER_SIZE) {
        alloc_node_t *new_block =
            (alloc_node_t *)((uint64_t *)((char *)&block->cBlock + size));
        new_block->size = block->size - size - HEADER_SIZE;
        block->size = size;
        // add new block to list
        dll_list_add(&new_block->node, &block->node, block->node.next);
    }
    // remove block from list since its getting allocated
    block->node.next->prev = block->node.prev;
    block->node.prev->next = block->node.next;
    spinlock_release(&kheap_lock);
    // Finally, return pointer to newly allocated adress
    return ptr;
}

void *kheap_calloc(uint64_t size) {
    void *ptr = kheap_alloc(size);
    if (ptr != NULL) {
        memset(ptr, 0, size);
        return ptr;
    }
    return NULL;
}

void kheap_free(void *ptr) {
    alloc_node_t *block, *free_block;

    spinlock_acquire(&kheap_lock);

    block = (void *)CONTAINER_OF(ptr, alloc_node_t, cBlock);
    if ((block->size + sizeof(alloc_node_t)) >= FRAME_SIZE &&
        ((uint64_t)CONTAINER_OF(block, alloc_node_t, cBlock) % FRAME_SIZE) ==
            0) {
        pmm_free(CONTAINER_OF(block, alloc_node_t, cBlock) - HHDM_OFFSET,
                 (block->size + sizeof(alloc_node_t)) / FRAME_SIZE);
        return;
    }

    for (free_block = CONTAINER_OF(free_list.next, alloc_node_t, node);
         &free_block->node != &free_list;
         free_block = CONTAINER_OF(free_block->node.next, alloc_node_t, node)) {
        if (free_block > block) {
            dll_list_add(&block->node, free_block->node.prev,
                         &free_block->node);
            coalesce_dll(); // prevent fragmentation
            spinlock_release(&kheap_lock);
            return;
        }
    }
    dll_list_add(&block->node, &free_block->node, free_block->node.next);

    coalesce_dll(); // prevent fragmentation
    spinlock_release(&kheap_lock);
    return;
}
void *kheap_realloc(void *ptr, uint64_t new_size) {
    if (!ptr && !new_size) {
        return NULL;
    }
    if (!new_size) {
        kheap_free(ptr);
        return NULL;
    }
    if (!ptr) {
        return kheap_alloc(new_size);
    }
    alloc_node_t *node = CONTAINER_OF(ptr, alloc_node_t, cBlock);
    uint64_t old_size = node->size;
    void *ret = kheap_alloc(new_size);
    if (!ret) {
        return NULL;
    }
    memcpy(ret, ptr, old_size);
    return ret;
}

void kheap_init() {
    add_block(pmm_malloc(1) + HHDM_OFFSET, FRAME_SIZE);
    printf("Kernel heap initialized.\n");
}
