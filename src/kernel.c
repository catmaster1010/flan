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

static void done(void) {
  printf("\nNothing to be done now...");
  for (;;) {
    asm("hlt");
  }
}

void kernel_thread(){
  printf("%sHello world from kernel thread!%s\n",cRED,cNONE);
  //printf("%s >  ",get_current_thread()->process->cwd->name);
  initramfs_init();
  done();
}

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
  vfs_init();
  sched_init(kernel_thread);
}
