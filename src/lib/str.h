
#ifndef str_h
#define str_h

#include <stdint.h>
int strlen(char* f);

char* itoa(uint64_t num, int base);
void  memset(uint64_t*ptr,uint8_t n,uint64_t size);
int memcmp(void *s1, void *s2, uint64_t size);
#endif