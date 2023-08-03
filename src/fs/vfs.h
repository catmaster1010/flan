#ifndef vfs_h
#define vfs_h
#include <fcntl.h>
#include <lib/hashmap.h>
#include <lib/vector.h>
#include <sys/stat.h>

struct vfs_node;
typedef struct vfs_fs {
    struct vfs_node *(*mount)(struct vfs_node *mountpoint, struct vfs_node *dev,
                              char *name);
    struct vfs_node *(*create)(struct vfs_node *parent, char *name, int mode);
    int (*read)(struct vfs_node *node, void *buff, uint64_t count,
                uint64_t offset);
    int (*write)(struct vfs_node *node, void *buff, uint64_t count,
                 uint64_t offset);
    uint64_t inode_number;
} vfs_fs_t;

typedef struct vfs_node {
    char *name;
    struct vfs_node *parent;
    hashmap_t *children;
    vfs_fs_t *fs;
    int pid;
    uint64_t seek;
    uint64_t file_size;
    struct vfs_node *mountpoint;
    struct vfs_node *link;
    void *data;
    struct stat st;
    int offset;
    spinlock_t lock;
} vfs_node_t;

vfs_node_t *vfs_create_node(vfs_node_t *parent, char *name, vfs_fs_t *fs,
                            bool dir);
void vfs_init();
void add_filesystem(vfs_fs_t *fs, char *fs_name);
bool vfs_mount(vfs_node_t *parent, char *source, char *target, char *fs_name);
vfs_node_t *vfs_create(vfs_node_t *parent, char *path, mode_t mode);
int vfs_read(vfs_node_t *node, void *buff, uint64_t count, uint64_t offset);
int vfs_write(vfs_node_t *node, void *buff, uint64_t count, uint64_t offset);
vfs_node_t *vfs_open(vfs_node_t *parent, const char *path);
vfs_node_t *fd_to_node(int fd);
int node_to_fd(vfs_node_t *node);
vfs_node_t *path_to_parent(vfs_node_t *parent, const char *path);
vfs_node_t *get_node(vfs_node_t *parent, const char *path);
extern vfs_node_t *root;
#endif
