#include "memory/kheap.h"
#include "lib/vector.h"
#include "lib/assert.h"

void vector_create(vector_t* vector, uint64_t item_size){
    vector->items = 0;
    vector->item_size = item_size;
    vector->lock=kheap_calloc(sizeof(spinlock_t));
    vector->data = 0;
}

uint64_t vector_size(vector_t* vector){
    return vector->items*vector->item_size;
}

uint64_t vector_get_items(vector_t* vector) {
    spinlock_acquire(vector->lock);
    uint64_t items = vector->items;
    spinlock_release(vector->lock);
    return items;
}

void* vector_get(vector_t* vector, uint64_t index) {
    spinlock_acquire(vector->lock);
    if (index > vector->items) {return 0;};
    uint64_t data = vector->data + (index * vector->item_size);
    spinlock_release(vector->lock);
    return data;
}

void vector_push(vector_t* vector, void* data) {
    spinlock_acquire(vector->lock);
    uint64_t vector_size = vector->items * vector->item_size;
    void* ptr = kheap_realloc(vector->data, vector_size + vector->item_size, vector_size);
    assert(ptr);
    memcpy(ptr + vector_size, data, vector->item_size);
    vector->data = ptr;
    vector->items++;
    spinlock_release(vector->lock);
    return 1;
}

void vector_remove(vector_t* vector, uint64_t index) {
    spinlock_acquire(vector->lock);
    if (index < vector->items) {
        memcpy(vector->data + (index * vector->item_size), vector->data + ((index + 1) * vector->item_size), vector->item_size * vector->items - (index * vector->item_size));
    }
    vector->items--;
    spinlock_release(vector->lock);
}

void vector_delete(vector_t* vector) {
    kheap_free(vector->data);
}

void vector_pop(vector_t* vector){ 
    spinlock_acquire(vector->lock);
    vector->items--;
    spinlock_release(vector->lock);
}

void vector_insert(vector_t* vector, void* data, uint64_t index){
    void* ptr=vector_get(vector,index);
    spinlock_acquire(vector->lock);
    memcpy(ptr,data,vector->item_size);
    spinlock_release(vector->lock);
}

void vector_resize(vector_t* vector, uint64_t items){
    spinlock_acquire(vector->lock);
    if (vector->items<items)
    {
        uint64_t new_vector_size = items * vector->item_size;
        uint64_t vector_size = vector->items * vector->item_size;
        void* ptr=kheap_realloc(vector->data,new_vector_size,vector_size);
        assert(ptr);
        vector->data = ptr;
    }
    vector->items=items;
    spinlock_release(vector->lock);
}

uint64_t vector_get_index(vector_t* vector, void* data){
    spinlock_acquire(vector->lock);
    if (vector->data+vector->item_size*vector->items<data || data<vector->data)
    {
        assert(0);
    }
    uint64_t index=(data-vector->data)/vector->item_size; 
    spinlock_release(vector->lock);
    return index;
}