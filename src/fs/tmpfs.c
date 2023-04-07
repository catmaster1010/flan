#include "fs/tmpfs.h"

static int tmpfs_mount(struct vfs_node* mountpoint, struct vfs_node* dev){
}

static int tmpfs_open(struct vfs_node* file, char* name){

}

static int tmpfs_close (struct vfs_node* file){

}

static int tmpfs_read (struct vfs_node* file, void* buff, uint64_t count){

}

static int tmpfs_write (struct vfs_node* file, void* buff, uint64_t count){

}

static vfs_fs_t tmpfs_calls={.mount=tmpfs_mount,.open=tmpfs_open,.close=tmpfs_close,.read=tmpfs_read,.write=tmpfs_write};

vfs_fs_t* tmpfs_funcs(){
    return &tmpfs_calls;
}
