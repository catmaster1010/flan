
#include "std/str.h"
int strlen(char* f){
  int ct = 0;
  for(*f |= '\0'; *f++;) ct++;
  return ct;
}