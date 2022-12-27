#ifndef acpi_h
#define acpi_h
#include <stdint.h>

typedef struct{
 char signature[8];         // This 8-byte string (not null terminated!) must contain "RSD PTR "  
 uint8_t checksum;          // The value to add to calculate the Checksum of this table. If this value added isn't equal to 0, the table must be ignored.
 char oem_id[6];            // The specification says: "An OEM-supplied string that identifies the OEM".
 uint8_t revision;          // Version of ACPI.
 uint32_t rsdt_address;     // 32-bit *physical* address of the RSDT table.
 //v2
 uint32_t length;           // The size of the entire table since offset 0 to the end.
 uint64_t xsdt_address;      // 64-bit *physical* address of XSDT. If ACPI Version 2 use this table instead of RSDT.
 uint8_t extended_checksum;  // This field is used to calculate the checksum of the entire table, including both checksum fields.
 uint8_t reserved[3];       // 3 bytes to be ignored in reading and that must not be written.
}rsdp_descriptor_t;

struct acpi_sdt_header {
    char signature[4];      // Used to determine what table are you working with.
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    char oem_id[6];
    char oem_table_id[6];
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
};
typedef struct acpi_sdt_header sdt_header_t;

typedef struct {
    sdt_header_t header;
    char data[];
} rsdt_t;


void acpi_init();
void* acpi_find_sdt(char signature[4], uint64_t index);
int use_xsdt();
#endif

/*//Version 1.0
 struct rsdp_descriptor{
 char signature[8];         // This 8-byte string (not null terminated!) must contain "RSD PTR "  
 uint8_t checksum;          // The value to add to calculate the Checksum of this table. If this value added isn't equal to 0, the table must be ignored.
 char oem_id[6];            // The specification says: "An OEM-supplied string that identifies the OEM".
 uint8_t revision;          // Version of  ACPI.
 uint32_t rsdt_address;     // 32-bit *physical* address of the RSDT table.
} ;
*/