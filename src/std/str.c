#include "std/str.h"
int strlen(char* f){
  int ct = 0;
  for(*f |= '\0'; *f++;) ct++;
  return ct;
}

char* itoa(uint64_t num, int base){ 
	static char repr[]= "0123456789abcdef";
	static char buffer[50]; 
	char *ptr; 

	ptr = &buffer[50]; 
	*ptr = '\0'; 

	do {
		*--ptr = repr[num%base]; 
		num /= base; 
	} while(num != 0); 

	return(ptr); 
}
