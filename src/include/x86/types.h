/**
* \file include/x86/types.h
* \brief Contains several x86 architecture dependant types definitions.
*
*/

#ifndef x86_TYPES_H
#define x86_TYPES_H

#define KB (1024UL)
#define MB (KB * KB)
#define GB (KB * MB)

#define MAX_UINT32   (0xFFFFFFFFUL)

#ifndef __ASM__

typedef unsigned char		__uchar_t;
typedef unsigned char		__uint8_t;
typedef unsigned short		__uint16_t;
typedef unsigned long		__uint32_t;
typedef unsigned long long      __uint64_t;

typedef signed char		__int8_t;
typedef signed short		__int16_t;
typedef signed long		__int32_t;	 

typedef float			__float32_t;
typedef double			__float64_t;

typedef unsigned long           __vaddr_t;
typedef unsigned long           __paddr_t;

typedef unsigned long           __size_t;

#endif //__ASM__
		
#endif
