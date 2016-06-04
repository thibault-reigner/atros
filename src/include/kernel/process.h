/**
 * \file include/kernel/process.h
 * \brief Contains definitions related to 
 *        processes management.
 */
#ifndef KERNEL_PROCESS_H
#define KERNEL_PROCESS_H

#include <types.h>

#include <kernel/thread.h>

#ifdef __ARCH_x86__
#include <x86/mmu.h>
#else
#error "No target architecture defined in process.h!"
#endif

#define LENGTH_PROCESS_NAME 30

#ifndef __ASM__


/**
 * \struct Process
 * \brief  Describes a process for the kernel scheduler.
 */
typedef struct Process{
  uint32_t pid;
  char name[LENGTH_PROCESS_NAME + 1];

  /*Architecture-dependant information*/

  /*The MMU's context related to the process: defines the process
    virtual address space.*/
  Mmu_context *mmu_context;

  //The list of threads which belong to this process
  struct Thread *threads;

} Process;

void processes_mngr_init(void);
Process * process_create(const char *name,
			 vaddr_t program_counter,
			 vaddr_t stack_top,
			 vaddr_t stack_bot,
			 paddr_t program_image_start,
			 size_t program_image_end);
#endif //__ASM__

#endif
