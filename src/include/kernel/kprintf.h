#ifndef KERNEL_KPRINTF_H
#define KERNEL_KPRINTF_H

#include <types.h>

#ifndef __ASM__

uint32_t kprintf(const char* format, ...);

#endif //__ASM__

#endif 
