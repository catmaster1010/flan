#include "gdt.h"
#include "std/stdio.h"
#include <stdint.h>

uint64_t gdt[7] =
      {
        0x0000000000000000, // null

        0x00009a000000ffff, // 16-bit code
        0x000093000000ffff, // 16-bit data

        0x00cf9a000000ffff, // 32-bit code
        0x00cf93000000ffff, // 32-bit data

        0x00af9b000000ffff, // 64-bit code
        0x00af93000000ffff, // 64-bit data
          };
gdt_register_t gdtr;


void gdt_init() {
  gdtr.limit = (sizeof(gdt) - 1);
  gdtr.base = (uint64_t)&gdt;
  extern void load_gdt(uint64_t gdtr);

  load_gdt((uint64_t)&gdtr);
  printf("%sGDT initialized.%s\n",cBLUE,cNONE);  
}

