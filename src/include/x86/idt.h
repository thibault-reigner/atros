/**
 * \file include/x86/idt.h
 * \brief Contains definition related to the IDT on
 *        the x86 architecture.
 */
#ifndef x86_IDT_H
#define x86_IDT_H

#include <types.h>

#define NBR_IDT_ENTRIES 256

#define VECTOR_FIRST_EXCEPTION 0
#define VECTOR_LAST_EXCEPTION  31

#define VECTOR_FIRST_IRQ 32
#define VECTOR_LAST_IRQ 46

#define VECTOR_IRQ0 32                /**<  IRQ #0 interrupt vector number*/
#define VECTOR_IRQ8 (VECTOR_IRQ0 + 8) /**<  IRQ #8 interrupt vector number*/

#define VECTOR_SYSCALL 0x80

/*Used for flags2*/
#define IDT_TASK 5                /**< \brief Defines a task gate in the IDT*/
#define IDT_INTERRUPT 6           /**< \brief Defines an interrupt gate in the IDT*/
#define IDT_TRAP 7                /**< \brief Defines a trap gate in the IDT*/

#define IDT_DPL0 0
#define IDT_DPL1 (1 << 5)
#define IDT_DPL2 (2 << 5)
#define IDT_DPL3 (3 << 5)

#define IDT_PRESENT (1 << 7)

#define IDT_16B 0
#define IDT_32B (1 << 3)


#ifndef __ASM__

/**
* \struct Idt_reg
* \brief Structure which represents the content of the IDT register
*
* This structure is packed: no padding will be added by gcc.
*/
struct Idt_reg
{
  uint16_t limit; 		/**< The size of the IDT*/
  vaddr_t linear_base_addr; 	/**< The linear address of the IDT*/
}__attribute__ ((packed));

/**
* \struct Idt_entry
* \brief Structure which represents an entry of the IDT
*
* This structure is packed: no padding will be added by the C compiler.
*/
struct Idt_entry
{
  uint16_t offset_0_15; 	/**< The bits 0 to 15 of the handler's address*/
  uint16_t seg_selector; 	/**< The selector of the segment which contains the interrupt handler*/
  uint8_t flags1; 		/**< Unused*/
  uint8_t flags2; 		/**< Flag which defines the type of IDT gate and its DPL*/
  uint16_t offset_16_31; 	/**< The bits 16 to 31 of the handler's address*/
}__attribute__ ((packed));


void idt_init(void);
void set_intr_gate(uint32_t n, vaddr_t addr);
void set_trap_gate(uint32_t n, vaddr_t addr);
void set_system_gate(uint32_t n, vaddr_t addr);

#endif //__ASM__

#endif
