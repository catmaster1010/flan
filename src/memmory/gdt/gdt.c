#include "gdt.h"
#include "std/stdio.h"
#include <stdint.h>

void gdt_load() {

<<<<<<< HEAD
  uint64_t descriptor_table[9] = // GDT STRUCT IS IN GDT.H CALLED "gdt_entry",
    
=======
  uint64_t gdt[9] = // GDT STRUCT IS IN GDT.H CALLED "gdt_entry",
                                 // ILL FIX THIS UP LATER, although paging will
                                 // be used
>>>>>>> 3c4ffc6 (loaded idt)
      {
          0x0000000000000000, // null

          0x00009a000000ffff, // 16-bit code
          0x000093000000ffff, // 16-bit data

          0x00cf9a000000ffff, // 32-bit code
          0x00cf93000000ffff, // 32-bit data

          0x00af9b000000ffff, // 64-bit code
          0x00af93000000ffff, // 64-bit data

          0x00affb000000ffff, // usermode 64-bit code
          0x00aff3000000ffff, // usermode 64-bit data
      };


  static gdt_register_t gdtr;

  gdtr.limit = (sizeof(gdt) - 1);
  gdtr.base = (uint64_t)&gdt;

  extern void load_gdt(uint16_t limit, uint64_t base);
  extern void reloadSegments();

  load_gdt(gdtr.limit, gdtr.base);
  reloadSegments();

  printf("GDT INITALIZED");
}

gdt_init() { gdt_load(); }
