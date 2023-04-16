#include "fs/initramfs.h"
#include <limine.h>
#include "lib/assert.h"

static volatile struct limine_module_request module_request = {
    .id = LIMINE_MODULE_REQUEST,
    .revision = 0
};

void initramfs_init(){
    struct limine_module_response *modules = module_request.response;
    if (modules == 0 || modules->module_count == 0) {
        assert(true);
    }
    struct limine_file *module = modules->modules[0];

}
