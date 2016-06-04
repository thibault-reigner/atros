/**
 * \file arch/x86/tss.c
 * \brief Data and functions used for TSS (Task-State Segment) management
 *	 on the x86 architecture.
 *
 *  TSS: Task-State Segment \n
 *  Structure used by the processor on x86-architecture for
 *  the hardware multitasking.\n
 *  Since we use software multitasking, we only need to define
 *  one TSS structure, which contains two useful fields which
 *  are: \n
 *  -the kernel stack (ring0) used when an interrupt occurs
 *  in user land \n
 *  -the I/O bitmap which allows user code to access to some
 *  I/O ports.
 */

#include <types.h>
#include <string.h>

#include <kernel/kernel.h>
#include <kernel/symbols.h>
#include <kernel/kprintf.h>

#include <x86/tss.h>
#include <x86/gdt.h>

/** 
 * \brief The only TSS used by both the kernel and user processes.
 *
 * Aligned on a page as required in Intel's documentation.
 */
static struct Tss ktss __attribute__ ((aligned (4096))); 


/**
 * \fn void tss_init(void)
 * \brief Initialise the TSS which will be used for interrupts in user land.
 */
void tss_init(void)
{
  set_gdt_entry(GDT_TSS_INDEX,
		(uint32_t)&ktss,
		(uint32_t)&ktss + sizeof(struct Tss),
		GDT_TSS_SEGMENT | GDT_PRESENT | GDT_DPL3,
		0);

  //We set the unused fields of the TSS to zero
  memset(&ktss, 0, sizeof(struct Tss));

  //We set the used fields of the TSS structure to proper values
  ktss.ss0 = KERNEL_DS;
  ktss.esp0 = (uint32_t)&_kernel_stack_pa + KERNEL_SPACE; 
  ktss.debug_trap_flag = 0;

  ktss.iomapbase = 104;
 
  //Forbid access to all I/O ports for user's code
  for (int i=0; i < 8192; i++)
    {
      ktss.iomap[i] = 0xFF;
    }

  //We load our new TSS into the task register with a RPL of 0
  load_tss((GDT_TSS_INDEX << 3) | 0);

}

/**
 * \fn void load_tss(uint32_t tss_selector)
 * \brief Load a tss descriptor selector in the task register. (LTR instruction)
 * \param tss_selector The value to load in the task register
 *
 * The task register behaves like any segment selector, its contains the RPL (Requestor's Privilege Level)
 * and the index in the GDT of the new TSS.
 */
void load_tss(uint32_t tss_selector)
{
  if ((tss_selector >> 3) <= NBR_GDT_ENTRIES)
    {
      asm ("movw %0, %%ax\n"\
	  "ltr %%ax\n"\
	  :: "m" (tss_selector)
	  : "ax");
    }
}


