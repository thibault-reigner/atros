/**
* \file stdarg.h
* \brief Header in the C standard library that allows functions to accept an indefinite number of arguments.
*
*/

#ifndef STDARG_H
#define STDARG_H

#include <types.h>

#ifndef __ASM__

#define va_start(v,l) __builtin_va_start(v,l)
#define va_arg(v,l)   __builtin_va_arg(v,l)
#define va_end(v)     __builtin_va_end(v)
#define va_copy(d,s)  __builtin_va_copy(d,s)
typedef __builtin_va_list va_list;


uint32_t vsnprintf(char* s, size_t n, const char* format, va_list args);
size_t i_to_nstr(int32_t value, size_t n, char* str, uint16_t base);
size_t u_to_nstr(uint32_t value, size_t n, char* str,  uint16_t base);

#endif

#endif
