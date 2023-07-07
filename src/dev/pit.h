#ifndef pit_h
#define pit_h
#include <stdint.h>

#define OSCILATOR_FREQ ((uint64_t)1193182)
#define TIMER_FREQ 1000

void pit_set_reload_value(uint16_t count);
void pit_set_frequency();
void pit_init();
void sleep(uint64_t ms);
uint16_t pit_get_current_count();
#endif
