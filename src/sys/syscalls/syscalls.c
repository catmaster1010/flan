#include <bits/off_t.h>
#include <fcntl.h>
#include <lib/errno.h>
#include <lib/stdio.h>
#include <lib/str.h>
#include <memory/pmm.h>
#include <memory/vmm.h>
#include <proc/sched.h>
#include <stdint.h>

int syscall_set_fs_base(void *pointer) {
    set_fs_base(pointer);
    return 0;
}

off_t syscall_seek(int fd, off_t offset, int whence) {
    off_t ret = -1;
    vfs_node_t *node = fd_to_node(fd);

    off_t new_off = 0;
    switch (whence) {
    case SEEK_SET:
        new_off = offset;
        break;
    case SEEK_CUR:
        new_off = node->offset + offset;
        break;
    case SEEK_END:
        new_off = node->st.st_size + offset;
        break;
    default:
        errno = EINVAL;
        return -1;
    }

    if (new_off < 0) {
        errno = EINVAL;
        return -1;
    }

    node->offset = new_off;
    ret = new_off;
    return ret;
}

int syscall_read(int fd, void *buf, uint64_t count) {
    vfs_node_t *node = fd_to_node(fd);
    int ret = -1;

    if (node == NULL) {
        errno = ENOENT;
        return ret;
    }

    return vfs_read(node, buf, count, node->offset);
}

int syscall_openat(int dirfd, const char *pathname, int flags, mode_t mode) {
    vfs_node_t *dir = fd_to_node(dirfd);
    int ret = -1;
    vfs_node_t *node = vfs_open(dir, pathname);
    if (node == NULL) {

        if ((flags & O_CREAT) == NULL) {
            errno = ENOENT;
            return ret;
        }

        vfs_node_t *parent = path_to_parent(dir, pathname);
        if (parent == NULL) {
            errno = ENOENT;
            return ret;
        }
        char *new_pathname = strdup(pathname);
        if (new_pathname == NULL) {
            errno = ENOMEM;
            return ret;
        }

        node = vfs_create(parent, basename, S_IFREG);
    }
    ret = node_to_fd(node);
    return ret;
}

void *syscall_mmap(void *hint, uint64_t length, int prot, int flags, int fd,
                   off_t offset) {
    (void)offset;
    (void)fd;

    process_t *proc = get_current_thread()->process;

    if ((flags & MAP_ANON) == 0) {
        printf("Not implemented.\n");
        return MAP_FAILED;
    }

    uintptr_t virt;
    uint64_t pages = ALIGN_UP(length, FRAME_SIZE) / FRAME_SIZE;

    if (flags & MAP_FIXED) {

        if (hint > USER_SPACE_END || length == 0 || hint == NULL) {
            errno = EINVAL;
            return MAP_FAILED;
        }
        virt = (uintptr_t)hint;
    } else {
        virt = proc->anon_base;
        proc->anon_base += (pages * FRAME_SIZE);
    }

    void *phys = pmm_calloc(pages);
    if (phys == NULL)
        return MAP_FAILED;

    vmm_map_pages(proc->pagemap, (uintptr_t)phys, virt,
                  prot | PTE_PRESENT | PTE_WRITABLE | PTE_USER, pages);

    return (void *)virt;
}

void syscall_log(char *str) { printf(str); }

__attribute__((__noreturn__)) void syscall_exit_thread() { dequeue_and_die(); }
