#include "acpi/acpi.h"
#include "cpu/cpu.h"
#include "cpu/idt/idt.h"
#include "cpu/smp.h"
#include "dev/fb.h"
#include "lib/stdio.h"
#include "memory/gdt/gdt.h"
#include "memory/pmm.h"
#include "memory/vmm.h"
#include <limine.h>
#include <stddef.h>
#include <stdint.h>
#include "dev/pit.h"
#include "proc/sched.h"
#include "fs/vfs.h"
#include "fs/initramfs.h"
#include "dev/ps2.h"
void kernel_thread();
void user_thread();

void _start(void) {

  printf("%sHello world! %s\n", cRED, cNONE);
  fb_info();
  gdt_init();
  idt_init();
  pmm_init();
  vmm_init();
  kheap_init();
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
 
  printf("%s >  ",get_current_thread()->process->cwd->name);
  pagemap_t* user_pagemap = vmm_new_pagemap();
  vfs_node_t* init_node = vfs_open(root,"/build/init");
  elf_load(user_pagemap, init_node);
  //sched_user_thread(user_thread, NULL, kernel_process);
  dequeue_and_die();
}

void user_thread(){
    asm ("mov rax, 69\n");
    asm ("syscall\n");
}

