#include "fs/initramfs.h"
#include <limine.h>
#include "lib/assert.h"
#include "lib/str.h"
#include "lib/stddef.h"
#include "fs/vfs.h"
#include "memory/pmm.h"
#include "memory/vmm.h"

#define TAR_FILE_TYPE_NORMAL '0'
#define TAR_FILE_TYPE_HARD_LINK '1'
#define TAR_FILE_TYPE_SYMLINK '2'
#define TAR_FILE_TYPE_CHAR_DEV '3'
#define TAR_FILE_TYPE_BLOCK_DEV '4'
#define TAR_FILE_TYPE_DIRECTORY '5'
#define TAR_FILE_TYPE_FIFO '6'
#define TAR_FILE_TYPE_GNU_LONG_PATH 'L'

struct tar_header
{
    char filename[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char chksum[8];
    char typeflag;
    char link_name[100];
    char magic[6];
    char version[2];
    char uname[32];
    char gname[32];
    char dev_major[8];
    char dev_minor[8];
    char prefix[155];
};
typedef struct tar_header tar_header_t;

static volatile struct limine_module_request module_request = {
    .id = LIMINE_MODULE_REQUEST,
    .revision = 0
};

static inline uint64_t oct2int(const char* str, int size) {
    uint64_t n = 0;
    const char* c = str;
    while (size-- > 0) {
        n *= 8;
        n += *c - '0';
        c++;
    }
    return n;
}

void initramfs_init(){
    struct limine_module_response *modules = module_request.response;
    if (modules == 0 || modules->module_count == 0) {
        assert(false);
    }
    struct limine_file* module = modules->modules[0];
    tar_header_t* current_file= (tar_header_t*) module->address;
    while(memcmp(current_file->magic ,"ustar" ,5)==0){
        uint64_t size=oct2int(current_file->size,sizeof(current_file->size));
        uint64_t mode=oct2int(current_file->mode,sizeof(current_file->mode));
        char* name = current_file->filename;

        switch (current_file->typeflag) {
            case TAR_FILE_TYPE_NORMAL: {
                vfs_node_t* node = vfs_create(root,name,false);
                assert(node);
                assert(node->fs->write(node,(void*)current_file+512,size,0) == (int)size);
                printf("New file: %s Size: %x Mode: %x\n",name,size,mode);
                break;
            }
            case TAR_FILE_TYPE_DIRECTORY : {
                vfs_node_t* node = vfs_create(root,name,true);
                assert(node);
                printf("New dir: %s\n",name);
                break;
            }
        }
        pmm_free((void*) current_file - HHDM_OFFSET, (512+ALIGN_UP(size,512))/FRAME_SIZE);
        current_file= (void*) current_file + 512 + ALIGN_UP(size,512);
    }

}
