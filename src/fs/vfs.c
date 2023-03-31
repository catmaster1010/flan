#include "fs/vfs.h"
#include "memory/kheap.h"
#include "lib/str.h"
#include "lib/vector.h"
#include "lib/stddef.h"
#include "lib/lock.h"
#include "lib/str.h"

spinlock_t vfs_lock=LOCK_INIT;
vfs_node_t* root;

static vfs_node_t* path_to_node(vfs_node_t* parent, const char* path){
    vfs_node_t* parent_node;
    if(!parent){
        parent_node = root;
    }
    else{
        parent_node=parent;
    }
    uint64_t path_length=strlen(path);


}

vfs_node_t* vfs_create_node(vfs_node_t* parent,vfs_fs_t* fs, const char* name,bool dir){
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

void vfs_init(){
    root=vfs_create_node(NULL,NULL,"/",true);
}

