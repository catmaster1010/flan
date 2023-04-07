#include "fs/vfs.h"
#include "memory/kheap.h"
#include "lib/str.h"
#include "lib/vector.h"
#include "lib/stddef.h"
#include "lib/lock.h"
#include "lib/str.h"
#include "lib/lock.h"
#include "fs/tmpfs.h"

spinlock_t vfs_lock=LOCK_INIT;
vfs_node_t* root;
hashmap_t filesystems;

static vfs_node_t* path_to_node(vfs_node_t* parent, const char* path){
    vfs_node_t* current_node;
    if(!parent){
        current_node = root;
    }
    else{
        current_node=parent;
    }
    char* segment = strtok(strdup(path),"/");
    while(segment!=NULL){
        current_node=hashmap_get(current_node->children,segment);
        if(current_node==NULL){
            return NULL;
        }
        char* segment = strtok(NULL,"/");
    }
    return current_node;
}

vfs_node_t* vfs_create_node(vfs_node_t* parent,vfs_fs_t* fs, const char* name, bool dir){
    vfs_node_t* node=kheap_calloc(sizeof(vfs_node_t));
    node->name=kheap_malloc(strlen(name)+1);
    memcpy(node->name,name,strlen(name)+1);
    node->fs=fs;
    if(dir){
        node->children = kheap_malloc(sizeof(hashmap_t));
        hashmap_create(node->children,256);
    }
    if(!parent){
        parent=root;
    }
    if(parent){
        node->parent=parent;
        hashmap_set(parent->children,name,node);
    }
    return node;
}

bool vfs_mount(vfs_node_t* parent, char* source, char* target, const char* fs_name){
    spinlock_acquire(&vfs_lock);
    vfs_fs_t* fs = hashmap_get(&filesystems,fs_name);
    vfs_node_t* node = path_to_node(parent,target);
    vfs_node_t* dev = path_to_node(parent,source);
    vfs_node_t* mount_node = fs->mount(node,dev);
    node->mountpoint=mount_node;
    spinlock_release(&vfs_lock);
}

void add_filesystem(vfs_fs_t* fs, const char* fs_name){
    spinlock_acquire(&vfs_lock);
    hashmap_set(&filesystems,fs_name,fs);
    spinlock_release(&vfs_lock);
}

void vfs_init(){
    root=vfs_create_node(NULL,NULL,"/",true);
    hashmap_create(&filesystems,64);
    hashmap_set(&filesystems,"tmpfs",tmpfs_funcs());
    hashmap_set(&filesystems,"ext2fs",ext2fs_funcs());
}

