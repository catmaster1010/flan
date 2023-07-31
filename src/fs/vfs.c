#include <fs/ext2fs.h>
#include <fs/tmpfs.h>
#include <fs/vfs.h>
#include <lib/assert.h>
#include <lib/lock.h>
#include <lib/stddef.h>
#include <lib/str.h>
#include <lib/vector.h>
#include <memory/kheap.h>
#include <lib/errno.h>
#include <proc/sched.h>

spinlock_t vfs_lock = LOCK_INIT;
vfs_node_t *root;
hashmap_t filesystems;

static vfs_node_t *reduce_node(vfs_node_t *node) {
    if (node->mountpoint) {
        return reduce_node(node->mountpoint);
    }
    if (node->link) {
        return reduce_node(node->link);
    }
    return node;
}

static void create_dot(vfs_node_t *parent, vfs_node_t *node) {
    vfs_node_t *dot = vfs_create_node(node, ".", node->fs, false);
    vfs_node_t *dotdot = vfs_create_node(node, ".", node->fs, false);

    dot->link = node;
    dotdot->link = parent;

    hashmap_set(node->children, ".", dot);
    hashmap_set(node->children, "..", dotdot);
}

typedef struct {
    vfs_node_t *parent;
    vfs_node_t *result;
} path_to_node_t;

static path_to_node_t path_to_node(vfs_node_t *parent, const char *path) {
    if (path == NULL || strlen(path) == 0)
        return (path_to_node_t){NULL, NULL};
    uint64_t path_len = strlen(path);

    vfs_node_t *current_node;
    vfs_node_t *parent_node;
    if (!parent) {
        parent_node = root;
    } else {
        parent_node = parent;
    }
    current_node = parent_node = reduce_node(parent_node);
    uint64_t index = 0;

    if (path[index] == '/')
        current_node = reduce_node(root);

    for (; path[index] == '/'; index++) {
        if (index == path_len - 1) {
            return (path_to_node_t){parent_node, current_node};
        }
    }

    char *new_path;
    bool dir = path[path_len - 1] == '/';
    uint64_t amount = index + dir;
    if (amount) {
        new_path = kheap_alloc(path_len - (amount) + 1);
        memcpy(new_path, path + index, path_len - (amount));
        new_path[path_len - (amount)] = '\0';
    } else {
        new_path = strdup(path);
    }

    char *segment = strtok(new_path, "/");

    while (segment != NULL) {
        current_node = hashmap_get(current_node->children, segment);

        segment = strtok(NULL, "/");

        if (current_node) {
            if (current_node->children) {
                parent_node = current_node;
            }
            continue;
        }

        kheap_free(new_path);
        if (segment != NULL)
            return (path_to_node_t){NULL, NULL};
        return (path_to_node_t){parent_node, NULL};
    }

    return (path_to_node_t){parent_node, current_node};
}
vfs_node_t* get_node(vfs_node_t* parent, const char* path){
    path_to_node_t p2n_result = path_to_node(parent, path);

    return p2n_result.result;
}

vfs_node_t* path_to_parent(vfs_node_t* parent, const char* path){
    path_to_node_t p2n_result = path_to_node(parent, path);
    if (p2n_result.parent == NULL){
        return NULL;
    }
    return p2n_result.parent;
}

vfs_node_t* fd_to_node(int fd){
    process_t* process = get_current_thread()->process;
    vfs_node_t* result = NULL;

    if (fd == AT_FDCWD) {
        result = process->cwd;
    } 
    else {
        result = vector_get(process->fildes, (uint64_t) fd);
    }

    if (result == NULL){
        errno = EBADF;
    }

    return result;
}

int node_to_fd(vfs_node_t* node){
    process_t* process = get_current_thread()->process;
    int result;
    result = vector_get_index(process->fildes, (void *) node);
    if (result == -1) {
        errno = ENOENT;
    }
    return result;
}

vfs_node_t *vfs_create_node(vfs_node_t *parent, char *name, vfs_fs_t *fs,
                            bool dir) {
    vfs_node_t *node = kheap_calloc(sizeof(vfs_node_t));
    assert(node);
    node->name = kheap_alloc(strlen(name) + 1);
    assert(node->name);
    memcpy(node->name, name, strlen(name) + 1);
    node->fs = fs;
    if (dir) {
        node->children = kheap_alloc(sizeof(hashmap_t));
        hashmap_create(node->children, 256);
    }

    if (!parent)
        parent = root;

    if (parent) {
        node->parent = parent;
        assert(hashmap_set(parent->children, name, node));
    }

    return node;
}

vfs_node_t *vfs_open(vfs_node_t *parent, const char *path) {
    spinlock_acquire(&vfs_lock);
    process_t* process = get_current_thread()->process;
    path_to_node_t ret = path_to_node(parent, path);
    vfs_node_t *node = ret.result;

    if (node == NULL) {
        spinlock_release(&vfs_lock);
        errno = ENOENT; 
        return NULL;
    }
    node = vector_replace(process->fildes,node,process->current_filde);
    process->current_filde++;
    ret = path_to_node(parent, path);
    spinlock_release(&vfs_lock);
    return node;
}

bool vfs_mount(vfs_node_t *parent, char *source, char *target, char *fs_name) {
    spinlock_acquire(&vfs_lock);
    vfs_fs_t *fs = hashmap_get(&filesystems, fs_name);
    path_to_node_t p2n_result;

    vfs_node_t *dev = NULL;
    if (source != NULL) {
        p2n_result = path_to_node(parent, source);
        dev = p2n_result.result;
    }

    p2n_result = path_to_node(parent, target);
    vfs_node_t *target_node = p2n_result.result;

    vfs_node_t *mount_node = fs->mount(target_node, dev, basename(target));
    target_node->mountpoint = mount_node;
    create_dot(p2n_result.parent, mount_node);
    spinlock_release(&vfs_lock);
    return true;
}

vfs_node_t *vfs_create(vfs_node_t *parent, char *path, mode_t mode) {
    path_to_node_t p2n_result = path_to_node(parent, path);
    if (!p2n_result.parent)
        return NULL;
    if (p2n_result.result)
        return NULL;

    vfs_fs_t *target_fs = p2n_result.parent->fs;
    vfs_node_t *node =
        target_fs->create(p2n_result.parent, basename(path), mode);
    if (node == NULL)
        return NULL;

    if (S_ISDIR(mode))
        create_dot(p2n_result.parent, node);
    return node;
}

int vfs_write(vfs_node_t *node, void *buff, uint64_t count, uint64_t offset) {
    int write_count = node->fs->write(node, buff, count, offset);
    return write_count;
}

int vfs_read(vfs_node_t *node, void *buff, uint64_t count, uint64_t offset) {
    int read_count = node->fs->read(node, buff, count, offset);
    return read_count;
}

void add_filesystem(vfs_fs_t *fs, char *fs_name) {
    spinlock_acquire(&vfs_lock);
    assert(hashmap_set(&filesystems, fs_name, fs));
    spinlock_release(&vfs_lock);
}

void vfs_init() {
    root = vfs_create_node(NULL, "", NULL, true);
    hashmap_create(&filesystems, 64);
    assert(hashmap_set(&filesystems, "tmpfs", tmpfs_funcs()));
    assert(hashmap_set(&filesystems, "ext2fs", ext2fs_funcs()));
    vfs_mount(root, NULL, "/", "tmpfs");
}
