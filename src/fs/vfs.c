#include "fs/vfs.h"
#include "memory/kheap.h"
#include "lib/str.h"
#include "lib/vector.h"
#include "lib/stddef.h"
#include "lib/lock.h"
#include "lib/str.h"

spinlock_t vfs_lock=LOCK_INIT;
vfs_node_t* root;
hashmap_t mountpoints;

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

bool vfs_mount(vfs_node_t* where, const char* fs_name, char* name, char*  dev){
    vfs_node_t* mountpoint=hashmap_get(&mountpoints,fs_name);
}

void vfs_init(){
    root=vfs_create_node(NULL,NULL,"/",true);
    hashmap_create(&mountpoints,64);
}

