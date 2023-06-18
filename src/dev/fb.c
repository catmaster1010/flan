#include "dev/fb.h"
#include <limine.h>
#include "lib/stdio.h"

volatile struct limine_framebuffer_request fb_request = {
  .id = LIMINE_FRAMEBUFFER_REQUEST,
  .revision = 0
};

struct limine_framebuffer* fb_address(){
  return fb_request.response->framebuffers[0]->address;
 }

void fb_info(){
  struct limine_framebuffer* fb_addresss=fb_address();
  printf("Our frame buffer is at virtual address: %x\n",fb_addresss);
  printf("Resolution: %dx%d\n",fb_request.response->framebuffers[0]->width,fb_request.response->framebuffers[0]->height);
  printf("Depth: %d\n",fb_request.response->framebuffers[0]->bpp);
  printf("\n");
}

void plot_pixel(uint8_t* screen, uint64_t x,uint64_t y, uint32_t colour) {
  uint64_t pitch   = fb_request.response->framebuffers[0]->pitch;
  uint64_t pix_wid = (fb_request.response->framebuffers[0]->bpp)/8;
  unsigned where = x*pix_wid+y*pitch;
    screen[where] = colour & 255;              // BLUE
    screen[where + 1] = (colour >> 8) & 255;   // GREEN
    screen[where + 2] = (colour >> 16) & 255;  // RED
}


void fb_colour_background(){
  uint8_t* fb_place = (uint8_t*) fb_address();
  for (uint64_t x = 0; x < fb_request.response->framebuffers[0]->width; x++)
  {
    for (uint64_t y = 0; y < fb_request.response->framebuffers[0]->height; y++)
      {
        plot_pixel(fb_place, x, y, 0x8c4c4c);
    } 
  }
}
