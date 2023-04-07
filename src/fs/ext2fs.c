#include "fs/ext2fs.h"

static int ext2fs_mount(struct vfs_node* mountpoint, struct vfs_node* dev){
}

static int ext2fs_open(struct vfs_node* file, char* name){

}

static int ext2fs_close (struct vfs_node* file){

}

static int ext2fs_read (struct vfs_node* file, void* buff, uint64_t count){

}

static int ext2fs_write (struct vfs_node* file, void* buff, uint64_t count){

}

static vfs_fs_t ext2fs_calls={.mount=ext2fs_mount,.open=ext2fs_open,.close=ext2fs_close,.read=ext2fs_read,.write=ext2fs_write};

vfs_fs_t* ext2fs_funcs(){
    return &ext2fs_calls;
}
