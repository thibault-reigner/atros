#ifndef x86_x86_H
#define x86_x86_H

#include <types.h>

#ifndef __ASM__

/**
* \fn inline uint8_t inb(uint16_t port)
* \brief Reads a byte on the port given as parameter.
*
* \param port The port where the input byte is read.
* \return The read byte.
*/
static inline uint8_t inb(uint16_t port)
{
  uint8_t toreturn;
  asm volatile("inb %1, %0" : "=a" (toreturn) : "Nd" (port));
  return toreturn;
}

/**
* \fn inline void outb(uint8_t value, uint16_t port)
* \brief Writes a byte on the port given as parameter.
* 
* \param value The byte to write on the ouput port.
* \param port The port where to write the byte.
*/
static inline void outb(uint8_t value, uint16_t port)
{
  asm volatile("outb %0, %1": : "a" (value), "Nd" (port));
}

/**
 * \fn inline void sti(void)
 * \brief Enable interrupts external from the CPU
 */
static inline void sti(void)
{
  asm volatile("sti");
}

/**
 * \fn inline void cli(void)
 * \brief Disable interrupts external from the CPU
 */
static inline void cli(void)
{
  asm volatile("cli");
}
#endif //__ASM__


#endif
