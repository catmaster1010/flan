#ifndef hashmap_h
#define hashmap_h
#include <stdint.h>
#include <stdbool.h>

typedef struct hashmap_item{
    struct hashmap_item* next;
    char* key;
    void* val;
}hashmap_entry_t;

typedef struct{
    struct hashmap_item* entries;
    uint64_t entry_count;
    uint64_t items;
} hashmap_t;

bool hashmap_create(hashmap_t* hashmap, uint64_t item_count);
bool hashmap_set(hashmap_t* hashmap, char* key, void* val);
void* hashmap_get(hashmap_t* hashmap, char* key);
bool hashmap_remove(hashmap_t* hashmap,char* key);
bool hashmap_delete(hashmap_t* hashmap);
#endif