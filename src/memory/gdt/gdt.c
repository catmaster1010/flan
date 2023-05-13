#include "gdt.h"
#include "lib/stdio.h"
#include <stdint.h>
#include "lib/lock.h"

gdt_entry_t gdt[13];
gdt_register_t gdtr;

void gdt_load(){
    gdtr.limit = (sizeof(gdt) - 1);
    gdtr.base = (uint64_t)&gdt;
    extern void load_gdt(uint64_t gdtr);

    load_gdt((uint64_t)&gdtr);
}
void gdt_init() {

    gdt[0] = (gdt_entry_t) {
        .limit = 0,
        .baselow16 = 0,
        .basemid8 = 0,
        .access = 0,
        .granularity = 0,
        .basehigh8 = 0
    };

    // ring 0 gdt entries for the limine terminal (16-bit, 32-bit and 64-bit)

    gdt[1] = (gdt_entry_t) { // ring 0 16 code
        .limit = 0xffff,
        .baselow16 = 0,
        .basemid8 = 0,
        .access = 0x9a,
        .granularity = 0,
        .basehigh8 = 0
    };

    gdt[2] = (gdt_entry_t) { // ring 0 16 data
        .limit = 0xffff,
        .baselow16 = 0,
        .basemid8 = 0,
        .access = 0x92,
        .granularity = 0,
        .basehigh8 = 0
    };

    gdt[3] = (gdt_entry_t) { // ring 0 32 code
        .limit = 0xffff,
        .baselow16 = 0,
        .basemid8 = 0,
        .access = 0x9a,
        .granularity = 0xcf,
        .basehigh8 = 0
    };

    gdt[4] = (gdt_entry_t) { // ring 0 32 data
        .limit = 0xffff,
        .baselow16 = 0,
        .basemid8 = 0,
        .access = 0x92,
        .granularity = 0xcf,
        .basehigh8 = 0
    };

    // actual standard gdt entries follow (kernel, user)

    gdt[5] = (gdt_entry_t) { // kernel 64 code
        .limit = 0,
        .baselow16 = 0,
        .basemid8 = 0,
        .access = 0x9a,
        .granularity = 0x20,
        .basehigh8 = 0
    };

    gdt[6] = (gdt_entry_t) { // kernel 64 data
        .limit = 0,
        .baselow16 = 0,
        .basemid8 = 0,
        .access = 0x92,
        .granularity = 0,
        .basehigh8 = 0
    };
    
    //sysenter related dummy entries. 
    gdt[7] = (gdt_entry_t) {0};
    gdt[8] = (gdt_entry_t) {0};

    gdt[9] = (gdt_entry_t) { // user 64 data
        .limit = 0,
        .baselow16 = 0,
        .basemid8 = 0,
        .access = 0xf2,
        .granularity = 0,
        .basehigh8 = 0
    };

    gdt[10] = (gdt_entry_t) { // user 64 code
        .limit = 0,
        .baselow16 = 0,
        .basemid8 = 0,
        .access = 0xfa,
        .granularity = 0x20,
        .basehigh8 = 0
    };

    gdt[11] = (gdt_entry_t) {
      .limit = 104,
      .baselow16 = 0,
      .basemid8 = 0,
      .basehigh8 = 0,
      .access = 0x89,
      .granularity = 0x00
    };
    gdt[12] = (gdt_entry_t) {
      .limit = 0,
      .baselow16 = 0,
    };

  gdt_load();
  printf("GDT initialized.\n");  
}

void gdt_load_tss(tss_t* tss){
  static spinlock_t gdt_lock=LOCK_INIT;
  spinlock_acquire(&gdt_lock);
  gdt[11] = (gdt_entry_t) {
    .limit = 104,
    .baselow16 = (uint16_t)((uint64_t)tss),
    .basemid8 = (uint8_t)((uint64_t)tss >> 16),
    .basehigh8 = (uint8_t)((uint64_t)tss >> 24),
    .access = 0x89,
    .granularity = 0x00
  };
  gdt[12] = (gdt_entry_t) {
      .limit = (uint16_t)((uint64_t)tss >> 32),
      .baselow16 = (uint16_t)((uint64_t)tss >> 48)
  };
  asm volatile ("ltr %0" : : "rm" ((uint16_t)0x58) : "memory");
  spinlock_release(&gdt_lock);
}
