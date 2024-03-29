#include <lib/assert.h>
#include <lib/hashmap.h>
#include <lib/stddef.h>
#include <lib/str.h>
#include <memory/kheap.h>

static inline uint64_t hash(char *key, uint64_t size) {
    uint64_t hash = 5381;
    for (uint64_t i = 0; i < size; i++) {
        uint8_t c = key[i];
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash;
}

#define INDEX(KEY, ENTRY_COUNT)                                                \
    { hash(KEY, strlen(KEY)) % ENTRY_COUNT }

static inline hashmap_entry_t *hashmap_entry_get(hashmap_t *hashmap,
                                                 char *key) {
    uint64_t index = INDEX(key, hashmap->entry_count);
    hashmap_entry_t *entry = &hashmap->entries[index];
    uint64_t key_length = strlen(key);
    while (entry) {
        if (key_length == entry->key_length) {
            if (!memcmp(entry->key, key, key_length))
                break;
        }
        entry = entry->next;
    }
    return entry;
}

bool hashmap_create(hashmap_t *hashmap, uint64_t entry_count) {
    void *data = kheap_calloc(entry_count * sizeof(hashmap_entry_t));
    if (!data)
        return false;
    hashmap->entries = data;
    hashmap->entry_count = entry_count;
    hashmap->items = 0;
    return true;
}

bool hashmap_set(hashmap_t *hashmap, char *key, void *val) {
    hashmap_entry_t *entry = hashmap_entry_get(hashmap, key);
    if (entry) {
        entry->val = val;
        return true;
    }
    // key is not in hashmap
    uint64_t index = INDEX(key, hashmap->entry_count);
    entry = &hashmap->entries[index];
    while (true) {
        if (entry->next == NULL)
            break;
        entry = entry->next;
    }

    hashmap_entry_t *new_entry = kheap_calloc(sizeof(hashmap_entry_t));
    if (!new_entry)
        return false;

    new_entry->key = key;
    new_entry->key_length = strlen(key);
    new_entry->val = val;
    new_entry->next = NULL;
    entry->next = new_entry;
    hashmap->items++;
    return true;
}

void *hashmap_get(hashmap_t *hashmap, char *key) {
    hashmap_entry_t *entry = hashmap_entry_get(hashmap, key);
    if (entry == NULL) {
        return 0;
    }
    return entry->val;
}

bool hashmap_remove(hashmap_t *hashmap, char *key) {
    uint64_t index = INDEX(key, hashmap->entry_count);
    hashmap_entry_t *entry = &hashmap->entries[index];
    hashmap_entry_t *prev = NULL;
    uint64_t key_length = strlen(key);
    while (entry) {
        if (key_length == entry->key_length) {
            if (memcmp(entry->key, key, key_length))
                break;
        }
        prev = entry;
        entry = entry->next;
    }

    if (prev) {
        prev->next = entry->next;
        kheap_free(entry);
        kheap_free(entry->key);
    } else if (entry->next) {
        memcpy(entry, entry->next, sizeof(hashmap_entry_t));
        kheap_free(entry->next);
    }
    hashmap->items--;
    return true;
}
