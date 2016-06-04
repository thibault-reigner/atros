/**
 * \file include/x86/8259a.h
 * \brief Contains definitions related to the PIC
 *        Intel 8259A on the x86 architecture.
 */
#ifndef x86_8259A_H
#define x86_8259A_H

#include <types.h>
#include <x86/idt.h>

#define PIC1              0x20 /**< \brief I/O base address of the master PIC*/
#define PIC2              0xA0 /**< \brief I/O base address of the slave PIC*/


#define PIC1_COMMAND      PIC1 /**< \brief The command port of the master PIC*/
#define PIC1_DATA         (PIC1+1) /**< \brief The data port of the master PIC*/

#define PIC2_COMMAND      PIC2 /**< \brief The command port of the slave PIC*/
#define PIC2_DATA         (PIC2+1) /**< \brief The data port of the slave PIC*/

#define ICW1_ICW4_NEEDED  1 /**< \brief Indicates if ICW4 needed*/
#define ICW1_INIT         16 /**< \brief Indicates if the PIC is to be initialised*/

#define PIC_EOI           0x20 /**< \brief End of Interrupt command*/

#ifndef __ASM__

void pic_8259a_init(void);
void irq_ack(uint32_t irq);
void irq_disable(uint32_t irq);
void irq_enable(uint32_t irq);

#endif //__ASM__

#endif
