#include "fs/tmpfs.h"
#include "memory/kheap.h"
#include "lib/str.h"
#include "lib/assert.h"

static int tmpfs_create(vfs_node_t* parent, const char* name, int mode){
    vfs_fs_t* fs=tmpfs_funcs();
    vfs_node_t* node = vfs_create_node(parent,fs,name,mode);
    void* new_data = kheap_alloc(FRAME_SIZE);
    assert(new_data);
    node->data=new_data;
    stat* st = &node->st;
	st->st_blksize = FRAME_SIZE;
	st->st_blocks = 1;
	st->st_mode = mode;
	st->st_nlink = 1;
	st->st_ino = node->fs->inode_number++;
	
}

static int tmpfs_read(struct vfs_node* node, void* buff, uint64_t count, uint64_t offset){
    uint64_t max = node->st.st_blksize-offset;
    if (count>max) {
        count=max;
    }
    memcpy(buff,node->data,count);
    return count;
}


static int tmpfs_write(struct vfs_node* node, void* buff, uint64_t count, uint64_t offset){
    uint64_t page = (offset + count) / FRAME_SIZE;
    if (node->st.st_blocks<=page){
        void* new = kheap_realloc(node->data,(page+1)*FRAME_SIZE);
        assert(new);
        node->data=new;
        node->st.st_blocks=page+1;
        node->st.st_size=offset+count;
    }
    memcpy(node->data + offset, buff, count);
    return count;
}

static vfs_node_t* tmpfs_mount(vfs_node_t* node, vfs_node_t* dev){
    (void)dev;
    vfs_fs_t* fs=tmpfs_funcs();
    vfs_node_t* ret = vfs_create_node(node, fs, node->name, true);
    return ret;

}

static vfs_fs_t tmpfs={.mount=tmpfs_mount,.create=tmpfs_create,.read=tmpfs_read,.write=tmpfs_write};

vfs_fs_t* tmpfs_funcs(){
    return &tmpfs;
}
