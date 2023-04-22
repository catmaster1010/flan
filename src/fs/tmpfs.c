#include "fs/tmpfs.h"
#include "memory/kheap.h"
#include "lib/str.h"

static int tmpfs_create(vfs_node_t* parent, const char* name, int mode){
    vfs_fs_t* fs=tmpfs_funcs();
    vfs_node_t* node = vfs_create_node(parent,fs,name,mode);
    stat* st = &node->st;
	st->st_blksize = 512;
	st->st_blocks = 1;
	st->st_mode = mode;
	st->st_nlink = 1;
	st->st_ino = node->fs->inode_number++;
	
}

static int tmpfs_open(struct vfs_node* file, char* name){

}

static int tmpfs_close (struct vfs_node* file){

}

static int tmpfs_read (struct vfs_node* file, void* buff, uint64_t count){

}

static int tmpfs_write (struct vfs_node* file, void* buff, uint64_t count){

}

static vfs_node_t* tmpfs_mount (vfs_node_t* node, vfs_node_t* dev){
    (void)dev;
    vfs_fs_t* fs=tmpfs_funcs();
    vfs_node_t* ret = vfs_create_node(node, fs, node->name, true);
    return ret;

}

static vfs_fs_t tmpfs={.mount=tmpfs_mount,.create=tmpfs_create,.open=tmpfs_open,.close=tmpfs_close,.read=tmpfs_read,.write=tmpfs_write};

vfs_fs_t* tmpfs_funcs(){
    return &tmpfs;
}
