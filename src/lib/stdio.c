#include "limine.h"
#include "lib/str.h"
#include <stdarg.h>
#include "lib/lock.h"

spinlock_t print_lock=LOCK_INIT;

 volatile struct limine_terminal_request terminal_request = {
    .id = LIMINE_TERMINAL_REQUEST,
    .revision = 0
};


void out(char* str){
  struct limine_terminal *terminal = terminal_request.response->terminals[0];
  terminal_request.response->write(terminal, str, strlen(str));
}

void printf(char* fmt, ... ){
  spinlock_acquire(&print_lock);
  va_list args; 
  va_start(args, fmt);
  for (char* c=fmt;*c!='\0';c++){
    if (*c=='%'){
      c++;
      switch(*c){
        case 's':
          out(va_arg(args,char*));break;
        case 'd':
          out(itoa(va_arg(args,uint64_t),10));break;
        case 'x':
          out("0x");
          out(itoa(va_arg(args,uint64_t),16));break;
        case 'b':
          out("0b");
          out(itoa(va_arg(args,uint64_t),2));break;
          }
    }
    else{
      char str[] = {*c,'\0'};
      out(str);
    }
  }         
  va_end(args); //Not needed for GCC (or clang). Here for compatibility. 
  spinlock_release(&print_lock);
}
