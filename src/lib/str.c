#include "lib/str.h"
#include "lib/stddef.h"
#include "lib/assert.h"
#include "memory/kheap.h"

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
    assert(ptr);
    for (uint64_t i = 0; i < size; i++)
    {
        ((uint8_t*)ptr)[i]=(uint8_t)n;			
    }
}

//returns 1 if  not equal
bool memcmp(void *str1, void *str2, uint64_t size) {
    for(uint64_t i = 0; i < size; i++)
        if(((uint8_t *)str1)[i] != ((uint8_t *)str2)[i]) return ((uint8_t *)str1)[i] < ((uint8_t *)str2)[i] ? -1 : 1; // different (with comparison)

    return 0; // equal
}

void memcpy(uint8_t* dest, uint8_t* src, uint64_t n) {
    assert(src);
    assert(dest);
    for (uint64_t i = 0; i < n; i++)
    {
        ((uint8_t*)dest)[i]=(uint8_t)src[i];			
    }
    
}

bool strcmp(char* str1,char*  str2){
    if(strlen(str1)==strlen(str2) && !(memcmp(str1,str2,strlen(str1)))){
        return 0; //same
    }
    return 1;
}

char* strchr(char* str, char c){

    while (*str!=c) {
        if (!*str++) {
            return NULL;
        }
    }
    return str;
}

char *strtok(const char *str, char *delim){
    static char* s = NULL;
    char* token;
    if (str!=NULL) {    
        s=str;
    }
    if (s==NULL) {
        return NULL;
    }
    token=s;
    while (*s!='\0') {
        if (strchr(delim,*s)!=NULL) {
            *s= '\0';
            s++;
            printf("%d %s\n",token=='\0',token);
            return token;
        }
        s++;
    }
    s=NULL; 
    return token;
}

char *basename(char *s)
{
	uint64_t i;
    // This empty string behavior is specified by POSIX.
	if (!s || !*s) return ".";
	i = strlen(s)-1;
	for (; i&&s[i]=='/'; i--) {
        s[i] = 0;
    }
	for (; i&&s[i-1]!='/'; i--);
	return s+i;
}

char* strdup(const char *str){
	uint64_t l = strlen(str);
	char *d = kheap_alloc(l+1);
	if (!d) return NULL;
    memcpy(d, str, l+1);
	return d;
}
