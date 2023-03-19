#ifndef str_h
#define str_h
#include <stdbool.h>
#include <stdint.h>
int strlen(char* f);

char* itoa(uint64_t num, int base);
void  memset(uint64_t*ptr,uint8_t n,uint64_t size);
bool memcmp(void *str1, void *str2, uint64_t size);
void memcpy(uint8_t* dest, uint8_t* src, uint64_t n);
bool strcmp(char* str1,char*  str2);
#endif
