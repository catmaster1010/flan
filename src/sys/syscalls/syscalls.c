#include <lib/stdio.h>
#include <proc/sched.h>
#include <stdint.h>
#include <bits/off_t.h>
#include <lib/errno.h>
#include <memory/vmm.h>
#include <memory/pmm.h>
#include <fcntl.h>

int syscall_openat(int dirfd, const char *pathname, int flags, mode_t mode);

void *syscall_mmap(void* hint, uint64_t length, int prot, int flags, int fd, off_t offset){
    (void)offset;
    (void)fd;

    process_t* proc =get_current_thread()->process;

    if ((flags & MAP_ANON) == 0) {
        printf("Not implemented.\n");
        return MAP_FAILED;
    }
    
    uintptr_t virt;
    uint64_t pages = ALIGN_UP(length, FRAME_SIZE) / FRAME_SIZE;

    if ((flags & MAP_FIXED) != 0) {

        if (hint > USER_SPACE_END || length == 0 || hint == NULL) {
            errno = EINVAL;
            return MAP_FAILED;
        }
        virt = (uintptr_t) hint;
    } else {
        virt = proc->anon_base;
        proc->anon_base += (pages * FRAME_SIZE);
    }
    
    void* phys = pmm_calloc(pages);
    if (phys == NULL) return MAP_FAILED;

    vmm_map_pages(proc->pagemap, (uintptr_t) phys, virt, prot, pages);

    return (void*) virt;
}

void syscall_log(char *str) { printf(str); }

__attribute__((__noreturn__)) void syscall_exit_thread() { dequeue_and_die(); }
