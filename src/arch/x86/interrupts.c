/**
 * \file arch/x86/interrupts.c
 * \brief Contains the interrupts dispatching function.
 */
#include <types.h>
#include <string.h>

#include <kernel/kernel.h>
#include <kernel/kprintf.h>
#include <kernel/panic.h>

#include <x86/interrupts.h>
#include <x86/idt.h>
#include <x86/x86.h>
#include <x86/8259a.h>


/************************************************************
                        Local variables
*************************************************************/

/** \brief Adresses of the actual interrupts handlers*/
static void (*interrupts_handlers[NBR_INTERRUPTS])(struct User_context *u_context) = {NULL};

/************************************************************
                        Local functions
*************************************************************/

void interrupt_dispatcher(struct User_context *u_context);

/**
 * \fn void interrupt_dispatcher(struct User_context *u_context)
 * \brief Called by each low level interrupt handler, calls the actual
 *        interrupt handler.
 * \param Pointer to the user's context saved on the kernel stack.
 */
/** \todo Obviously, a cleaner version of interrupt_dispatcher()*/
void interrupt_dispatcher(struct User_context *u_context)
{
  /* //Breakpoint */
  if (u_context->context.int_number == 3)
    {
      kprintf("Breakpoint!\n");
      int i;
      do
	{
	  do
	    {
	      i = inb(0x64);
	    } while ((i & 0x01) == 0);

	  i = inb(0x60);
	} while (i != 28); //We wait until the Enter key is pressed
      return;
    }

  if(interrupts_handlers[u_context->context.int_number] != NULL)
    {
      interrupts_handlers[u_context->context.int_number](u_context);
    }
  else if (u_context->context.int_number <= VECTOR_LAST_EXCEPTION)
    {
      if (u_context->context.int_number == 14) 
  	{
  	  vaddr_t fault_addr;
  	  asm volatile ("movl %%cr2, %0" : "=a" (fault_addr));
  	  kprintf("Fault's address: %p (instruction at %p)\n",fault_addr, u_context->context.eip);

  	  if (u_context->context.error_code & 1)
  	    {
 	      kprintf("Page-level protection violation\n");
  	    }
  	  else
  	    {
  	      kprintf("Non-present page\n");
  	    }
  	}
      panic("Unhandled exception %u!\n", u_context->context.int_number);
    }

  if(u_context->context.int_number == SYSCALL_VECTOR)
    {
       kprintf("%s", u_context->context.eax);
    }
}


/************************************************************
                        Global functions
*************************************************************/

/**
 * \fn void set_interrupt_handler(uint8_t n, void (*handler)(struct User_context*))
 * \brief Set a function as an interrupt handler.
 * \param n The vector of the interrupt
 * \param handler Pointer to the interrupt handler's function.
 */
void set_interrupt_handler(uint8_t n, void (*handler)(struct User_context*))
{
  interrupts_handlers[n] = handler;
}
