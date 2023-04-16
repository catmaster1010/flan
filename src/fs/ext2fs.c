#include "fs/ext2fs.h"

typedef struct ext2fs_superblock{
    uint32_t inode_num;
    uint32_t block_num;
    uint32_t rsvd_num;
    uint32_t unallocb;
    uint32_t unalloci;
    uint32_t sb;
    uint32_t log2_blksize;
    uint32_t log2_fragsize;
    uint32_t blocks_per_group;
    uint32_t frags_per_group;
    uint32_t inodes_per_group;
    uint32_t last_mnt; // unix epoch for last mount
    uint32_t last_written; // unix epoch for last write
    uint16_t last_check_mounts;
    uint16_t mounts_allowed_before_check; // are we allowed to mount this filesystem?
    uint16_t ext2_sig; //ext signature
    uint16_t fss_tate;
    uint16_t errorr_action;
    uint16_t minor_version;
    uint32_t last_fsck; // last time we cleaned the filesystem
    uint32_t forced_fsck_time;
    uint32_t osid;
    uint32_t major_version;
    uint16_t uid;
    uint16_t gid;

    //extend superblock fields
    uint32_t first;
    uint16_t inode_size;
    uint16_t bg_sb;
    uint32_t optional_features;
    uint32_t required_features;
    uint32_t readonly_features;
    char uuid[16]; // filesystem uuid
    char name[16];
    char lastmountedpath[64]; // last path we had when mounted
    uint32_t comp_algo;
	uint8_t  blocks_prealloc_files;
	uint8_t  blocks_prealloc_dirs;
    uint16_t unused;
	char journal_id[16];
	uint32_t journal_inode;
	uint32_t journal_dev;
	uint32_t head_orphan_inode_list;

} __attribute__((packed));
typedef struct ext2fs_superblock ext2fs_superblock_t;

struct ext2fs_block_group_desc{
    uint32_t usage_bitmap;
    uint32_t inode_bitmap;
    uint32_t inode_table;
    uint16_t unalloc_blocks;
    uint16_t unalloc_inodes;
    uint16_t dirs;
    uint8_t unused[14];
}__attribute__((packed));
typedef struct ext2fs_block_group_desc ext2fs_block_group_desc_t;

struct ext2fs_inode{
    uint16_t perms;
    uint16_t uid;
    uint32_t size_low;
    uint32_t last_acc;
    uint32_t create_time;
    uint32_t last_mod;
    uint32_t del_time;
    uint16_t gid;
    uint16_t hardlinks;
    uint32_t sectors;
    uint32_t flags;
    uint32_t ossv1;
    uint32_t blocks[15];
    uint32_t generation_num;
    uint32_t eab;
    uint32_t size_high;
    uint32_t frag_addr;
    uint32_t ossv2[3];

}__attribute__((packed));
typedef struct ext2fs_inode ext2fs_inode_t;

struct ext2fs_dir_entry{
    uint32_t inode;
    uint16_t entry_size;
    uint8_t name_length;
    uint8_t type;
    char name[];
}__attribute__((packed));
typedef struct ext2fs_dir_entry ext2fs_dir_entry_t;

typedef struct {
    ext2fs_inode_t root;
    ext2fs_superblock_t sb;

    uint32_t block_size;
    uint32_t frag_size;
    

} ext2fs_t;

static int ext2fs_create(const char* path, int mode){

}

static int ext2fs_open(struct vfs_node* file, char* name){

}

static int ext2fs_close (struct vfs_node* file){

}

static int ext2fs_read (struct vfs_node* file, void* buff, uint64_t count){

}

static int ext2fs_write (struct vfs_node* file, void* buff, uint64_t count){

}

static vfs_fs_t ext2fs_calls={.create=ext2fs_create,.open=ext2fs_open,.close=ext2fs_close,.read=ext2fs_read,.write=ext2fs_write};

vfs_fs_t* ext2fs_funcs(){
    return &ext2fs_calls;
}

void ext2fs_init(){

}
