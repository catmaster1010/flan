#include "memmory/gdt/gdt.h"
#include "memmory/pmm.h"

#include "interrupts/idt/idt.h"
#include "std/stdio.h"
#include "memmory/pmm.h"
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


  printf("%sHello World!%s \n\n",cRED,cNONE); 
  gdt_init();
  idt_init();
  pmm_init();
  done();
}
