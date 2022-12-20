
#ifndef str_h
#define str_h

#include <stdint.h>
int strlen(char* f);

char* itoa(uint64_t num, int base);
void  memset(uint64_t*ptr,int n,uint64_t size);
#endif