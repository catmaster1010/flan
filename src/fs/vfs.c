#include "fs/vfs.h"
#include "memory/kheap.h"
#include "lib/str.h"
#include "lib/vector.h"
#include "lib/stddef.h"

vfs_node_t* root;

vfs_node_t* vfs_create_node(vfs_node_t* parent,vfs_fs_t* fs, const char* name,bool dir){
    vfs_node_t* node=kheap_calloc(sizeof(vfs_node_t));
    node->name=kheap_malloc(strlen(name)+1);
    memcpy(node->name,name,strlen(name)+1);
    node->fs=fs;
    if(dir){
        node->children = kheap_malloc(sizeof(vector_t));
        vector_create(node->children,sizeof(vfs_node_t));
    }
    if(!parent){
        parent=root;
    }
    if(parent){
        node->parent=parent;
        vector_push(node->parent->children,node);
    }
    return node;
}

void vfs_init(){
    root=vfs_create_node(NULL,NULL,"/",true);
}
