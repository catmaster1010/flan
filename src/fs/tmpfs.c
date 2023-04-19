#include "fs/tmpfs.h"
#include "memory/kheap.h"
#include "lib/str.h"

static int tmpfs_create(const char* path, int mode){
    
}

static int tmpfs_open(struct vfs_node* file, char* name){

}

static int tmpfs_close (struct vfs_node* file){

}

static int tmpfs_read (struct vfs_node* file, void* buff, uint64_t count){

}

static int tmpfs_write (struct vfs_node* file, void* buff, uint64_t count){

}

static int tmpfs_mount (vfs_node_t* node, vfs_node_t* dev){

}

static vfs_fs_t tmpfs={.mount=tmpfs_mount,.create=tmpfs_create,.open=tmpfs_open,.close=tmpfs_close,.read=tmpfs_read,.write=tmpfs_write};

vfs_fs_t* tmpfs_funcs(){
    return &tmpfs;
}
