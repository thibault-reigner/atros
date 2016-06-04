/**
 * \file include/kernel/thread.h
 * \brief Contains definitions related to threads management.
 */
#ifndef KERNEL_THREAD_H
#define KERNEL_THREAD_H

#include <types.h>

#include <kernel/process.h>

#ifdef __ARCH_x86__
#include <x86/cpu_context.h>
#else
#error "No target architecture defined in process.h!"
#endif

#ifndef __ASM__

/**
 * \struct Thread
 * \brief Describes a thread.
 */
typedef struct Thread{
  /** \brief Identifier of the thread*/
  uint32_t tid; 

  /** \brief Status of the thread.*/
  enum {
    READY, 
    RUNNING, 
    WAITING, 
    ZOMBIE
  } status; 
  
  /** \brief Process to which belongs the thread*/
  struct Process *owner_process; 

  /** \brief The CPU context of the process.
    NB: each time the process is interrupted or calls the kernel
    its context is saved on the stack, then copied in this structure.*/
  User_context context; 

  struct Thread *prev_thread, *next_thread;

  struct Thread *prev_scheduled, *next_scheduled;

} Thread;

void threads_mngr_init(void);
bool_t thread_create(struct Process *owner_process,
		     vaddr_t program_counter,
		     vaddr_t stack_top);
#endif //__ASM__

#endif
