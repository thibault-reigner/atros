/**
* \file include/types.h
* \brief Contains several architecture independant types definitions.
*
* Includes the types header of the target architecture.
*/

#ifndef TYPES_H
#define TYPES_H

#ifdef __ARCH_x86__
#include <x86/types.h>
#else
#error "Target architecture undefined for types.h"
#endif

#define NULL  ((void *)0)

#ifndef __ASM__


typedef __uchar_t     uchar_t;
typedef __uint8_t     uint8_t;
typedef __uint16_t    uint16_t;
typedef __uint32_t    uint32_t;
typedef __uint64_t    uint64_t;

typedef __int8_t      int8_t;
typedef __int16_t     int16_t;
typedef __int32_t     int32_t;

typedef __float32_t   float32_t;
typedef __float64_t   float64_t;

typedef __vaddr_t     vaddr_t;
typedef __paddr_t     paddr_t;

typedef __size_t      size_t;
typedef __uint8_t     byte_t;

typedef uint32_t      uint_t;
typedef int32_t       int_t;

/** \brief Architecture independant definition of a boolean.*/
typedef enum bool_t{ FALSE=0,TRUE=1} bool_t;

#endif //__ASM__


#endif
