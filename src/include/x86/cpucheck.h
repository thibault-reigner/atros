#ifndef x86_CPUCHECK_H
#define x86_CPUCHECK_H

#include <types.h>


#ifndef __ASM__

struct Cpuid_info{
  uint32_t eax;
  uint32_t ebx;
  uint32_t ecx;
  uint32_t edx;
};

void checkcpu(void);
void do_cpuid_request(uint32_t request, struct Cpuid_info *cpuid_info);

#endif

#endif
