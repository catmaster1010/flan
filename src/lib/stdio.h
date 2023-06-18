#ifndef stdio_h
#define stdio_h

#include <limine.h>

#define cRED    "\033[1;31m"
#define cBLUE   "\033[1;34m"
#define cNONE   "\033[0m"
#define cBLACK "\033[0;30m"

void printf(char* str,...);
void put(char* str);

#endif
