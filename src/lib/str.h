#ifndef str_h
#define str_h
#include <stdbool.h>
#include <stdint.h>

char* strchr(const char* str, char c);
char* strtok(char* str, const char* delim);
int strlen(const char* f);
char* itoa(uint64_t num, int base);
void  memset(void* ptr,uint8_t n,uint64_t size);
int memcmp(const void *s1, const void *s2, uint64_t size);
void memcpy(void* dest, const void* src, uint64_t n);
bool strcmp(char* str1,char*  str2);
char* basename(char* path);
char* strdup( const char *str);
#endif
