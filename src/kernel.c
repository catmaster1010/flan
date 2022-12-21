#include "memmory/gdt/gdt.h"
#include "memmory/pmm.h"
#include "interrupts/idt/idt.h"
#include "lib/stdio.h"
#include "memmory/pmm.h"
#include "memmory/vmm.h"
#include "fb/fb.h"
#include <limine.h>
#include <stddef.h>
#include <stdint.h>
// The Limine requests can be placed anywhere, but it is important that
// the compiler does not optimise them away, so, usually, they should
// be made volatile or equivalent.

static void done(void) {
  for (;;) {
    __asm__("hlt");
  }
}

// The following will be our kernel's entry point.
void _start(void) {
  printf("%sHello world! %s \n\n","\033[1;96m",cNONE); 
  fb_info();
  gdt_init();
  idt_init();
  pmm_init();
  //vmm_init();
  done();
}