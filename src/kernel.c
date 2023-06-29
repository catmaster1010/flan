#include <limine.h>
#include "acpi/acpi.h"
#include "cpu/cpu.h"
#include "cpu/idt/idt.h"
#include "cpu/smp.h"
#include "dev/fb.h"
#include "lib/stdio.h"
#include "memory/gdt/gdt.h"
#include "memory/pmm.h"
#include "memory/vmm.h"
#include "memory/kheap.h"
#include "dev/console.h"
#include <stdint.h>
#include "dev/pit.h"
#include "proc/sched.h"
#include "fs/vfs.h"
#include "fs/initramfs.h"
#include "dev/ps2/ps2.h"
#include "lib/elf.h"
#include "dev/serial.h"
#include "dev/console.h"

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

void kernel_thread(){
    printf("%sHello world from kernel thread!%s\n",cRED,cNONE);
    initramfs_init();

    printf("\nNothing to be done now...\n");
 
    vfs_node_t* init_node = vfs_open(root,"/build/init");

    pagemap_t* user_pagemap = vmm_new_pagemap();
    struct auxval aux;
    elf_load(user_pagemap, init_node, &aux);
    process_t* user_proc = sched_process(user_pagemap);
    thread_t* user_thread = sched_user_thread((void*)aux.at_entry, NULL, user_proc);

    dequeue_and_die();
}

