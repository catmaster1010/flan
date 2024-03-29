#ifndef vector_h
#define vector_h
#include <lib/lock.h>
#include <stdint.h>

typedef struct {
    uint64_t items;
    uint64_t item_size;
    uint64_t capacity;
    spinlock_t lock;
    void *data;

} vector_t;

void vector_create(vector_t *vector, uint64_t item_size);
uint64_t vector_get_items(vector_t *vector);
void *vector_get(vector_t *vector, uint64_t index);
void vector_push(vector_t *vector, void *data);
void vector_remove(vector_t *vector, uint64_t index);
void vector_delete(vector_t *vector);
uint64_t vector_size(vector_t *vector);
void vector_resize(vector_t *vector, uint64_t items);
void vector_pop(vector_t *vector);
void *vector_replace(vector_t *vector, void *data, uint64_t index);
int vector_get_index(vector_t *vector, void *data);
void vector_insert(vector_t *vector, void *data, uint64_t index);
void vector_reserve(vector_t *vector, uint64_t new_cap);
#endif
