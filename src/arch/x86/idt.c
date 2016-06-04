/**
 * \file arch/x86/idt.c
 * \brief This file contains functions used to modify the IDT (Interrupt Descriptor Table).
 *
 * IDT: Interrupt Descriptor Table
 * This table associates to each interrupt a procedure or a task to handle it.
 */
#include <types.h>

#include <kernel/kernel.h>

#include <x86/idt.h>
#include <x86/gdt.h>
#include <x86/interrupts.h>

extern vaddr_t isr_entries[];

/*************************************************************************
                          Local variables
 ************************************************************************/
/** \brief The IDT used by the kernel*/
static struct Idt_entry idt[NBR_IDT_ENTRIES] __attribute__ ((__aligned__ (8)));

/** \brief The value to load in the IDT register.
 *
 *    Contains the physical address and the size of the IDT.
 */
static struct Idt_reg idtr = {
  .limit = NBR_IDT_ENTRIES * sizeof(struct Idt_entry) - 1,
  .linear_base_addr = (uint32_t)&idt
};

/**
 * \fn static void load_idtr(struct Idt_reg *idt_reg)
 * \brief Load the value given as parameter in the IDT register.
 * \param idt_reg pointer to the value to load.
 */
static void load_idtr(struct Idt_reg *idt_reg)
{
  asm volatile ("lidt %0\n":: "m" (*idt_reg));
}


/***************************************************************************
                            Local functions
 **************************************************************************/

/**
 * \fn static void set_idt_entry(uint32_t n, uint32_t seg_selector, uint32_t offset, uint8_t flags1, uint8_t flags2)
 * \brief Specifies a new entry in the IDT.
 *
 * \param n The index of the entry in the IDT. (Must be lower than NBR_IDT_ENTRIES)
 * \param seg_selector The selector of the segment which contains the interrupt handler.
 * \param offset The address of the interrupt handler in the segment.
 * \param flags1 Flag unused, set to 0.
 * \param flags2 Flag related to the type of the IDT gate and the DPL.
 */
static void set_idt_entry(uint32_t n, 
		   uint32_t seg_sel, 
		   uint32_t offset, 
		   uint8_t flags1, 
		   uint8_t flags2)
{
  if (n < NBR_IDT_ENTRIES)
    {
      idt[n].offset_0_15  = (uint16_t)(offset & 0xFFFF);
      idt[n].seg_selector = (uint16_t)seg_sel;
      idt[n].flags1       = flags1;
      idt[n].flags2       = flags2;
      idt[n].offset_16_31 = (uint16_t)((offset >> 16) & 0xFFFF);
    }
}

/***********************************************************************************
                                 Global functions
 **********************************************************************************/

/**
 * \fn void set_intr_gate(uint32_t n, vaddr_t addr)
 * \brief Set an interrupt gate handler for the vector # n
 *
 * An interrupt gate masks interrupts when called
 *
 * \param n The vector number
 * \param addr The handler's address
 */
void set_intr_gate(uint32_t n, vaddr_t addr)
{
  set_idt_entry(n, 
		KERNEL_CS, 
		addr, 
		0, 
		IDT_INTERRUPT | IDT_DPL0 | IDT_PRESENT | IDT_32B);
}

/**
 * \fn void set_trap_gate(uint32_t n, vaddr_t addr)
 * \brief Set a trap gate handler for the vector # n
 *
 * A trap gate does not mask interrupts when called
 *
 * \param n The vector number
 * \param addr The handler's address
 */
void set_trap_gate(uint32_t n, vaddr_t addr)
{
  set_idt_entry(n, 
		KERNEL_CS, 
		addr, 
		0, 
		IDT_TRAP | IDT_DPL0 | IDT_PRESENT | IDT_32B);
}

/**
 * \fn void set_system_gate(uint32_t n, vaddr_t addr)
 * \brief Set a system gate handler for the vector # n
 *
 *  A system gate is a trap gate which can be called from CPL3 code
 *
 * \param n The vector number
 * \param addr The  handler's address
 */
void set_system_gate(uint32_t n, vaddr_t addr)
{
  set_idt_entry(n, 
		KERNEL_CS, 
		addr, 
		0, 
		IDT_INTERRUPT | IDT_DPL3 | IDT_PRESENT | IDT_32B);
}
 
/**
 * \fn void idt_init(void)
 * \brief Initialise the IDT.
 *
 *  Associates handlers to each interrupts.
 */
void idt_init(void)
{
  uint32_t i;
  
  /*Set all interrupts as interrupt gates by default*/
  for (i=0; i < NBR_IDT_ENTRIES; i++)
    {
      set_intr_gate(i, isr_entries[i]);
    }

  /*Set system gates (can be called from userland)*/
  set_system_gate(3, isr_entries[3]); /*int 3*/
  set_system_gate(4, isr_entries[4]); /*overflow*/
  set_system_gate(5, isr_entries[5]); /*bounds*/
  set_system_gate(SYSCALL_VECTOR, isr_entries[SYSCALL_VECTOR]);

  load_idtr(&idtr);
}

