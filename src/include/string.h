#ifndef STRING_H
#define STRING_H

#include <types.h>

#ifndef __ASM__

void * memcpy(void *dest, const void *src, size_t num);
void * memmove(void *dest, const void *src, size_t num);
void * memset(void *ptr, int_t value, size_t num);
char * strncpy(char * dest, const char * src, size_t num );
int32_t strcmp(const char * str1, const char * str2 );

#endif // __ASM__


#endif
