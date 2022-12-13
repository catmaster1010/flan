#include "fb/fb.h"
#include <limine.h>
#include "std/stdio.h"

static volatile struct limine_framebuffer_request fb = {
  .id = LIMINE_FRAMEBUFFER_REQUEST,
  .revision = 0
};

struct limine_framebuffer* fb_adress(){
  return fb.response->framebuffers[0]->address;
 }

void fb_info(x,y,colour){
  struct limine_framebuffer* fb_adresss=fb_adress();
  printf("Our frame buffer is at %x.\n",fb_adresss);
  printf("Resolution: %dx%d\n",fb.response->framebuffers[0]->width,fb.response->framebuffers[0]->height);
  printf("Depth: %d\n",fb.response->framebuffers[0]->bpp);
}

void plot_pixel(uint8_t*screen, uint64_t x,uint64_t y, uint32_t colour) {
  uint64_t pitch   = fb.response->framebuffers[0]->pitch;
  uint64_t pix_wid = (fb.response->framebuffers[0]->bpp)/8;
  unsigned where = x*pix_wid+y*pitch;
    screen[where] = colour & 255;              // BLUE
    screen[where + 1] = (colour >> 8) & 255;   // GREEN
    screen[where + 2] = (colour >> 16) & 255;  // RED
}


void fb_colour_background(){
  uint8_t* fb_place = fb_adress();
  for (uint64_t x = 0; x < fb.response->framebuffers[0]->width; x++)
  {
    for (uint64_t y = 0; y < fb.response->framebuffers[0]->height; y++)
      {
        plot_pixel(fb_place, x,y, 0x8c4c4c);
    }
    
  }
  
}