#include "memmory/gdt/gdt.h"
#include "memmory/pmm.h"
#include "interrupts/idt/idt.h"
#include "std/stdio.h"
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
  fb_colour_background();
  printf("%s%sHello World! \n\n",cBLACK,cNONE); 
  gdt_init();
  idt_init();
  pmm_init();
  fb_info();
  done();
}