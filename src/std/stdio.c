#include "limine.h"
#include "std/str.h"
#include "memmory/gdt/gdt.h"
#include <stdarg.h>

static volatile struct limine_terminal_request terminal_request = {
    .id = LIMINE_TERMINAL_REQUEST,
    .revision = 0
};


void out(char* str){
  struct limine_terminal *terminal = terminal_request.response->terminals[0];
  terminal_request.response->write(terminal, str, strlen(str));
}

void printf(char* str,...){
  out(str);            
        
}


        


