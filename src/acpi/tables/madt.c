#include "lib/stdio.h"
#include  "lib/stddef.h"
#include  "acpi/tables/madt.h"
#include "lib/assert.h"
#include  "lib/vector.h"
void madt_init(){

    madt_t* madt =acpi_find_sdt("APIC", 0);
    assert(madt);

    uint64_t=offset;
}
