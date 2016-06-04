#ifndef KERNEL_PANIC_H
#define KERNEL_PANIC_H


#ifndef __ASM__
void panic(const char* format, ...) __attribute__ ((__noreturn__));

#define KASSERT(exp)							\
  do{									\
    if(!(exp))								\
      {									\
	panic("Assertion " #exp " failed in function %s() at %s, line %d\n", \
	      __func__,							\
	      __FILE__,							\
	      __LINE__);						\
      }									\
  }while(0)

#endif //_ASM__

#endif
