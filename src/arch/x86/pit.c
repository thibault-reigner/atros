/**
 * \file arch/x86/pit.c
 * \brief Contains functions to manage the PIT
 *        (Programmable Interval Timer)
 */
#include <types.h>

#include <kernel/kprintf.h>


#include <x86/pit.h>
#include <x86/x86.h>
#include <x86/8259a.h>
#include <x86/idt.h>
#include <x86/interrupts.h>
#include <x86/cpu_context.h>

#define GEN0_COUNTER (0x40)
#define CLOCK_TICK_RATE (1193180)


static uint32_t pit_tick = 0;


static void pit_irq_handler(User_context *u_context);

/**
 * \fn static void pit_irq_handler(User_context *u_context)
 * \brief Handler of the timer interrupt.
 */
static void pit_irq_handler(User_context *u_context)
{
  pit_tick++;

  //scheduler(u_context);

  irq_ack(0);
}


/**
 * \fn void pit_init(void)
 * \brief Initialise the PIT's management.
 */
void pit_init(void)
{
  set_interrupt_handler(VECTOR_IRQ0, pit_irq_handler);
}

/**
 * \fn void pit_set_frequency(uint32_t hz)
 * \brief Set the frequency of the PIT.
 */
void pit_set_frequency(uint32_t hz)
{
  if (hz > 0)
    {
      uint16_t divisor = CLOCK_TICK_RATE / hz;
      outb(0x36,0x43);
      outb(divisor & 0xFF, GEN0_COUNTER);
      outb((divisor >> 8) & 0xFF, GEN0_COUNTER);
    }
}


