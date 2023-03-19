#include "lib/hashmap.h"
#include "memory/kheap.h"
#include "lib/str.h"
#include "lib/assert.h"
#include "lib/stddef.h"

static inline uint64_t hash(char* key, uint64_t size){
    uint64_t hash=5381;
    for(uint64_t i=0;i < size;i++){
        uint8_t c  = key[i];
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash;
}

#define INDEX(KEY,ENTRY_COUNT) { hash(KEY,strlen(KEY)) % ENTRY_COUNT}

static inline hashmap_entry_t* hashmap_entry_get(hashmap_t* hashmap, char* key){
    uint64_t index = INDEX(key,hashmap->entry_count); 
    hashmap_entry_t* entry = &hashmap->entries[index];
    while(strcmp(entry->key,key)){
        entry=entry->next;
        if(entry==NULL){break;}
    }
    return entry; 
}

void hashmap_create(hashmap_t* hashmap, uint64_t entry_count){
    void* data=kheap_calloc(entry_count*sizeof(hashmap_entry_t));
    assert(data);
    hashmap->entries=data;
    hashmap->entry_count=entry_count;
}

void hashmap_set(hashmap_t* hashmap, char* key, void* val){

    hashmap_entry_t* entry=hashmap_entry_get(hashmap,key);
    if (entry){
        entry->val=val;
        return;
    }
    //key is not in hashmap
    hashmap_entry_t* new_entry=kheap_malloc(sizeof(hashmap_entry_t));
    assert(new_entry);
    new_entry->key=key;
    new_entry->val=val;
    uint64_t index = INDEX(key,hashmap->entry_count);  
    new_entry->next=&hashmap->entries[index];
    hashmap->entries[index]=*new_entry;
    hashmap->items++;
}

void* hashmap_get(hashmap_t* hashmap, char* key){
    hashmap_entry_t* entry=hashmap_entry_get(hashmap,key);
    if(entry==NULL){return 0;}
    return entry->val;
}