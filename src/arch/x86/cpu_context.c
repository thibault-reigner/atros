/**
 * \file arch/x86/cpu_context.c
 * \brief Contains functions to manipulate CPU's contexts.
 */
#include <types.h>
#include <string.h>

#include <x86/cpu_context.h>
#include <x86/gdt.h>

/**********************************************************
                      Local variables
**********************************************************/


/**********************************************************
                      Local functions
**********************************************************/


/**********************************************************
                      Globals functions
***********************************************************/

/**
 * \fn void user_context_init(struct User_context *a_context,
 *                            vaddr_t program_counter,
 *                            vaddr_t stack_top,
 *                            uint32_t param)
 * \brief Initialise the context of a user task.
 * \param a_context The context to initialise
 * \param program_counter The initial value of the program counter.
 * \param stack_top The initial value of the pointer register.
 */
void user_context_init(struct User_context *a_context,
		       vaddr_t program_counter,
		       vaddr_t stack_top,
		       uint32_t param)
{
  if (a_context != NULL)
    {
      memset(a_context, 0, sizeof(struct User_context));

      a_context->context.eax = param;

      a_context->context.ds  = USER_DS;
      a_context->context.es  = USER_DS;
      a_context->context.fs  = USER_DS;
      a_context->context.gs  = USER_DS;
      
      a_context->context.eip = program_counter;
      a_context->context.cs  = USER_CS;
      
      /* The process must not have the right to perform 
       * critical operations such as reading or writing 
       * on the I/O bus.
       * The process must be interruptible (especially by the 
       * system timer).
       */
      a_context->context.eflags = EFLAGS_IOPL0 | EFLAGS_INTERRUPT_ENABLE;
      a_context->ss = USER_DS;
      a_context->esp = stack_top;
    }
}
