#ifndef ps2_h
#define ps2_h
#include <stdint.h>

void ps2_init();

uint8_t ps2_read();
void ps2_write(uint16_t port, uint8_t value);
#endif
