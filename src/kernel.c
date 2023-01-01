#include "memory/gdt/gdt.h"
#include "memory/pmm.h"
#include "cpu/interrupts/idt.h"
#include "lib/stdio.h"
#include "memory/pmm.h"
#include "memory/vmm.h"
#include "fb/fb.h"
#include <limine.h>
#include <stddef.h>
#include <stdint.h>
#include "acpi/acpi.h"
#include "cpu/smp.h"
#include "cpu/cpu.h"

static void done(void) {
  printf("\nNothing to be done now...");
  for (;;) {
    __asm__("hlt");
  }
}

void _start(void) {
  printf("%sHello world! %s \n",cRED,cNONE); 
  fb_info();
  gdt_init();
  idt_init();
  pmm_init();
  vmm_init();
  kheap_init();
  acpi_init();
  smp_init();
  done();
}