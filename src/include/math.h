#ifndef MATH_H
#define MATH_H

#ifndef __ASM__

/** \brief Typesafe macro which returns the minimum between a and b*/
#define MIN(a,b) ({ \
  typeof(a) _a = (a); \
  typeof(b) _b = (b); \
  _a < _b ? _a : _b; })

/** \brief Typesafe macro which returns the maximum between a and b*/
#define MAX(a,b) ({ \
  typeof(a) _a = (a); \
  typeof(b) _b = (b); \
  _a > _b ? _a : _b; })

/** \brief Macro to round up x to align*/
#define ROUNDUP(x,align) ({ ((x/align) + (x % align ? 1UL : 0UL))*align;})
#define ROUNDDOWN(x, align) ({ (x/align)*align;})

#endif //_ASM__
#endif
