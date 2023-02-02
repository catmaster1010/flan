#ifndef pit_h
#define pit_h

#include <stdint.h>
#define OSCILATOR_FREQ 1193182 
#define TIMER_FREQ 1000

void pit_set_reload_value(uint16_t count);
void pit_set_frequency();
void pit_init();

#endif
