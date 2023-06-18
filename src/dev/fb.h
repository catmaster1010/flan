#ifndef fb_h
#define fb_h
#include <limine.h>

extern volatile struct limine_framebuffer_request fb_request;
void fb_info();
struct limine_framebuffer* fb_address(void);
void plot_pixel(uint8_t*screen, uint64_t x,uint64_t y, uint32_t colour);
void fb_colour_background();
#endif
