#include <acpi/acpi.h>
#include <dev/console.h>
#include <dev/fb.h>
#include <dev/pit.h>
#include <dev/ps2/ps2.h>
#include <dev/serial.h>
#include <fs/initramfs.h>
#include <fs/vfs.h>
#include <lib/assert.h>
#include <lib/elf.h>
#include <lib/stdio.h>
#include <limine.h>
#include <memory/gdt/gdt.h>
#include <memory/kheap.h>
#include <memory/pmm.h>
#include <memory/vmm.h>
#include <proc/sched.h>
#include <stdint.h>
#include <sys/cpu.h>
#include <sys/idt/idt.h>
#include <sys/smp.h>

void kernel_thread();

void _start(void) {
    serial_init();
    printf("%sHello world! %s\n", cRED, cNONE);
    fb_info();
    gdt_init();
    idt_init();
    pmm_init();
    vmm_init();
    kheap_init();
    console_init();
    acpi_init();
    smp_init();
    pit_init();
    ps2_init();
    vfs_init();

    sched_init(kernel_thread);
}

void kernel_thread() {
    printf("%sHello world from kernel thread!%s\n", cRED, cNONE);
    initramfs_init();

    printf("\nNothing to be done now...\n");

    vfs_node_t *init_node = vfs_open(root, "/usr/bin/init");
    assert(init_node);

    pagemap_t *user_pagemap = vmm_new_pagemap();
    struct auxval init_aux, ld_aux;
    const char *ld_path;

    assert(elf_load(user_pagemap, init_node, &init_aux, &ld_path));

    vfs_node_t *ld_node = vfs_open(root, ld_path);
    assert(ld_node);

    assert(elf_load(user_pagemap, ld_node, &ld_aux, NULL));
    process_t *user_proc = sched_process(user_pagemap);

    const char *argv[] = {"/usr/bin/init", NULL};
    const char *envp[] = {NULL};

    sched_user_thread((void *)ld_aux.at_entry, NULL, user_proc, argv, envp,
                      &init_aux);
    dequeue_and_die();
}
