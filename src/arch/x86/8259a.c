/**
* \file arch/x86/8259a.c
* \brief Contains functions used to 
*        configure the PIC 8259A on the 
*        x86-architecture.
*/

#include <types.h>
#include <kernel/kernel.h>

#include <x86/x86.h>
#include <x86/8259a.h>
#include <x86/interrupts.h>

/**
* \fn void pic_8259a_init(void)
* \brief Initialises the master and slave 8259A chipsets.
*
*/
void pic_8259a_init(void)
{
  outb(ICW1_INIT + ICW1_ICW4_NEEDED, PIC1_COMMAND); //ICW1: Master PIC initialisation
  outb(ICW1_INIT + ICW1_ICW4_NEEDED, PIC2_COMMAND); //ICW1: Slave PIC initialisation

  outb(VECTOR_IRQ0, PIC1_DATA); //ICW2: IRQO-7 mapped to IDT interrupts
  outb(VECTOR_IRQ8, PIC2_DATA); //ICW2: IRQ8-15 mapped to IDT interrupts

  outb(4, PIC1_DATA); //ICW3: we use the IRQ2 to connect the master PIC to the slave PIC
  outb(2, PIC2_DATA); //ICW3: tells to the PIC it is a slave

  outb(1, PIC1_DATA); //ICW4: x86 mode and no automatic EOI (End Of Interrupt)
  outb(1, PIC2_DATA); //ICW4: x86 mode and no automatic EOI (End Of Interrupt)

  outb(0xFF, PIC1_DATA); //We mask all IRQ
  outb(0xFF, PIC2_DATA); //We mask all IRQ
}

/**
* \fn void irq_ack(uint32_t irq)
* \brief Sends an EOI (End Of Interrupt) to the PIC who sent the irq given as parameter
* \param irq The irq which has been handled
*/
void irq_ack(uint32_t irq)
{
  if (irq >= 8)
    {
      outb(PIC_EOI,PIC2_COMMAND);
    }
  else
    outb(PIC_EOI,PIC1_COMMAND);
}

/**
 * \fn void irq_disable(uint32_t irq)
 * \brief Disables the irq given as parameter
 * \param irq The irq to disable
 */
void irq_disable(uint32_t irq)
{
  uint8_t mask;
  if (irq >= 8)
    {
      mask = inb(PIC2_DATA);
      mask = mask | (uint8_t)(1U << (irq - 8));
      outb(mask, PIC2_DATA);
    }
  else
    {
      mask = inb(PIC1_DATA);
      mask = mask | (uint8_t)(1U << irq);
      outb(mask, PIC1_DATA);
    }
}

/**
 * \fn void irq_enable(uint32_t irq)
 * \brief Enables the irq given as paramter
 * \param irq The irq to enable
 */
void irq_enable(uint32_t irq)
{
  uint8_t mask;

  if (irq >= 8)
    {
      mask = inb(PIC2_DATA);
      mask = mask & (uint8_t)~(1U << (irq - 8));
      outb(mask, PIC2_DATA);
    }
  else
    {
      mask = inb(PIC1_DATA);
      mask = mask & (uint8_t)~(1U << irq);
      outb(mask, PIC1_DATA);
    }
}

