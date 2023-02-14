#include "memory/kheap.h"
#include "lib/vector.h"
#include "lib/assert.h"

void vector_create(vector_t* vector, uint64_t item_size,spinlock_t lock){
    vector->items = 0;
    vector->item_size = item_size;
    vector->lock=lock;
    vector->data = 0;
}

uint64_t vector_size(vector_t* vector){
    return vector->items*vector->item_size;
}

uint64_t vector_get_items(vector_t* vector) {
    spinlock_acquire(&vector->lock);
    uint64_t items = vector->items;
    spinlock_release(&vector->lock);
    return items;
}

void *vector_get(vector_t* vector, uint64_t index) {
    spinlock_acquire(&vector->lock);
    if (index > vector->items) {return 0;};
    uint64_t data = vector->data + (index * vector->item_size);
    spinlock_release(&vector->lock);
    return data;
}

void vector_push(vector_t* vector, void* data) {
    spinlock_acquire(&vector->lock);
    uint64_t vector_size = vector->items * vector->item_size;
    void *ptr = kheap_realloc(vector->data, vector_size + vector->item_size, vector_size);
    assert(ptr);
    memcpy(ptr + vector_size, data, vector->item_size);
    vector->data = ptr;
    vector->items++;
    spinlock_release(&vector->lock);
    return 1;
}

void vector_remove(vector_t* vector, uint64_t index) {
    spinlock_acquire(&vector->lock);
    if (index < vector->items) {
        memcpy(vector->data + (index * vector->item_size), vector->data + ((index + 1) * vector->item_size), vector->item_size * vector->items - (index * vector->item_size));
    }
    spinlock_release(&vector->lock);
}

void vector_delete(vector_t* vector) {
    kheap_free(vector->data);
}
