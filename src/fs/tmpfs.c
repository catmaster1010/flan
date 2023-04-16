#include "fs/tmpfs.h"
#include "memory/kheap.h"
#include "lib/str.h"

static vfs_fs_t tmpfs;

static int tmpfs_create(const char* path, int mode){
    vfs_node_t* parent;
    vfs_node_t* node = vfs_create_node(parent, basename(path),&tmpfs,true);
    
}

static int tmpfs_open(struct vfs_node* file, char* name){

}

static int tmpfs_close (struct vfs_node* file){

}

static int tmpfs_read (struct vfs_node* file, void* buff, uint64_t count){

}

static int tmpfs_write (struct vfs_node* file, void* buff, uint64_t count){

}

static vfs_fs_t tmpfs={.create=tmpfs_create,.open=tmpfs_open,.close=tmpfs_close,.read=tmpfs_read,.write=tmpfs_write};

vfs_fs_t* tmpfs_funcs(){
    return &tmpfs;
}
