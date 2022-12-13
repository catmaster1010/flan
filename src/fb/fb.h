#ifndef fb_h
#define fb_h
#include <limine.h>

void fb_info();
struct limine_framebuffer* fb_adress(x,y,colour);
void plot_pixel(uint8_t*screen, uint64_t x,uint64_t y, uint32_t colour);
void fillrect(uint8_t* screen, unsigned char w, unsigned char h, unsigned char colour);
void fb_colour_background();
#endif