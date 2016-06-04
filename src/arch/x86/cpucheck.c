/**
 * \file arch/x86/cpucheck.c
 * \brief Contains functions to check if the CPU has the recquired features
 *        to run the kernel properly.
 */
#include <types.h>
#include <kernel/kprintf.h>

#include <x86/cpucheck.h>

/***************************************************
               Local variables
****************************************************/

/***************************************************
               Local functions
****************************************************/
static bool_t checkcpu_has_cpuid(void);

/**
 * \fn static bool_t checkcpu_has_cpuid(void)
 * \brief Check if the cpu supports the CPUID instruction
 * \return TRUE if CPUID is supported, FALSE otherwise.
 *
 * If this function has the ability to set and clear the ID flag of the
 * EFLAGS register, the CPUID instruction is available on the CPU.
 */
static bool_t checkcpu_has_cpuid(void)
{
  uint32_t has_cpuid = 0;

  asm volatile ("pushf\t\n"
	       "pop %%eax\t\n"
	       "mov %%eax, %%ebx\t\n"
	       "xor $0x200000, %%eax\t\n" //we flip the value of the 21st bit
	       "push %%eax\t\n"
	       "popf\t\n"
	       "pushf\t\n"
	       "pop %%eax\t\n"
	       "xorl %%ebx, %%eax\t\n"
	       "andl $0x200000, %%eax\t\n"
	       "movl %%eax, %0\t\n"
	       "push %%ebx\t\n"
	       "popf\t\n"                 //Restore EFLAGS
       	       : "=m"(has_cpuid)
	       :
	       : "eax", "ebx");
  return (has_cpuid ? TRUE : FALSE);
}


/*******************************************************
                 Global function
*******************************************************/

/**
 * \fn void do_cpuid_request(uint32_t request, struct Cpuid_info *cpuid_info)
 * \brief Performs an information request on the CPU with the CPUID instruction
 * \param request Code of the request to perform.
 * \param cpuid_info Pointers on a structure where to store the retrieved information.
 */
void do_cpuid_request(uint32_t request, struct Cpuid_info *cpuid_info)
{
  asm volatile ("mov %4, %%eax\t\n" //We load the request's code
	       "cpuid\t\n"
	       "mov %%eax, %0\t\n"
	       "mov %%ebx, %1\t\n"
	       "mov %%ecx, %2\t\n"
	       "mov %%edx, %3\t\n"
	       : "=a" (cpuid_info->eax), 
		 "=b" (cpuid_info->ebx), 
		 "=c" (cpuid_info->ecx),
		 "=d" (cpuid_info->edx)
	       : "a" (request)
	       :);
}

void checkcpu(void)
{
  if (checkcpu_has_cpuid() == TRUE)
    {
      kprintf("CPUID instruction support on the processor.\n");
    }
  else
    {
      kprintf("No CPUID instruction support on the processor.\n");
    }
}
