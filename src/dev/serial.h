#ifndef serial_h
#define serial_h
#define PORT 0x3f8          // COM1
void serial_init();
void serial_out(char* str);
#endif
