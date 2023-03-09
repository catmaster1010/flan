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

static void done(void) {
  printf("\nNothing to be done now...");
  for (;;) {
    asm("hlt");
  }
}

void kernel_thread(){
  done();
}

void _start(void) {
  printf("%sHello world! %s \n", cRED, cNONE);
  fb_info();
  gdt_init();
  idt_init();
  pmm_init();
  vmm_init();
  kheap_init();
  acpi_init();
  smp_init();
  pit_init();
  sched_init(kernel_thread);
 //done();
}
