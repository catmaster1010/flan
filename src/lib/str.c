#include "lib/str.h"
#include "lib/stddef.h"
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

void memset(uint64_t*ptr,uint8_t n,uint64_t size){
	if (ptr!=NULL)
    {
        for (uint64_t i = 0; i < size; i++)
        {
            ((uint8_t*)ptr)[i]=(uint8_t)n;
        }
	    return ptr;
    }
}
