/**
* \file include/x86/tss.h
* \brief Contains definition related to TSS on the x86 architecture
*/
#ifndef x86_TSS_H
#define x86_TSS_H

#include <types.h>



#ifndef __ASM__

/** \struct Tss
 *  \brief Structure used by the processor on the x86 architecture 
 * for hardware task-switching.
 * Though we do not use this feature, we need at 
 * least one TSS to change the stack when an interrupt
 * occurs in user land*/
struct Tss{
  uint32_t previous_task; //the 16 most significant bits are reserved
  uint32_t esp0; 
  uint32_t ss0; //the 16 most significant bits are reserved
  uint32_t esp1;
  uint32_t ss1; //the 16 most significant bits are reserved
  uint32_t esp2;
  uint32_t ss2; //the 16 most significant bits are reserved
  uint32_t cr3;
  uint32_t eip;
  uint32_t eflags;
  uint32_t eax;
  uint32_t ecx;
  uint32_t edx;
  uint32_t ebx;
  uint32_t esp;
  uint32_t ebp;
  uint32_t esi;
  uint32_t edi;
  uint32_t es; //the 16 most significant bits are reserved
  uint32_t cs; //the 16 most significant bits are reserved
  uint32_t ss; //the 16 most significant bits are reserved
  uint32_t ds; //the 16 most significant bits are reserved
  uint32_t fs; //the 16 most significant bits are reserved
  uint32_t gs; //the 16 most significant bits are reserved
  uint32_t ldt_selector; //the 16 most significant bits are reserved
  uint16_t debug_trap_flag;
  uint16_t iomapbase;
  uint8_t iomap[8192];
}__attribute__ ((packed));


void tss_init(void);
void load_tss(uint32_t tss_selector);
#endif //__ASM__

#endif
