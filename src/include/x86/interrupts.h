/**
 * \file include/x86/interrupts.h
 * \brief Contains definitions related to interrupts management
 *        on the x86 architecture.
 */
#ifndef x86_INTERRUPTS_H
#define x86_INTERRUPTS_H

#include <types.h>

#include <x86/cpu_context.h>
#include <x86/idt.h>

#define NBR_INTERRUPTS NBR_IDT_ENTRIES 

#ifndef __ASM__

void set_interrupt_handler(uint8_t n, void (*handler)(struct User_context*));

#endif //__ASM__

#endif   
