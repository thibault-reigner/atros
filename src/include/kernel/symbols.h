/**
* \file symbols.h
* \brief Contains the symbols defined in the linker script.
*
* These symbols are used by the kernel in order to determinate its location 
* and its size in both the physical and virtual address space.
*
*/

#ifndef KERNEL_SYMBOLS_H
#define KERNEL_SYMBOLS_H

#include <types.h>

#ifndef __ASM__

/*Physical addresses*/

/** \brief The physical address of the beginning of the .boot section*/
extern void* _boot_pa_start; 

/** \brief The top of the kernel stack*/
extern void* _kernel_stack_pa;

/** \brief The physical address of the end of the .boot section*/
extern void* _boot_pa_end; 

/** \brief The physical address of the beginning of the kernel*/
extern void* _kernel_pa_start; 

/** \brief the physical address of the begining of the .text section*/
extern void* _text_pa_start; 
/** \brief the physical address of the end of the .text section*/
extern void* _text_pa_end; 

/** \brief the physical address of the begining of the .data section*/
extern void* _data_pa_start; 
/** \brief the physical address of the end of the .data section*/
extern void* _data_pa_end; 

/** \brief the physical address of the begining of the .rodata section*/
extern void* _rodata_pa_start; 
/** \brief the physical address of the end of the .rodata section*/
extern void* _rodata_pa_end; 

/** \brief the physical address of the begining of the .bss section*/
extern void* _bss_pa_start; 
/** \brief the physical address of the end of the .bss section*/
extern void* _bss_pa_end; 

/** \brief the physical address of the end of the kernel*/
extern void* _kernel_pa_end; 

#define boot_pa_start        ((paddr_t)&_boot_pa_start)
#define kernel_stack_pa      ((paddr_t)&_kernel_stack_pa)
#define boot_pa_end          ((paddr_t)&_boot_pa_end)

#define kernel_pa_start      ((paddr_t)&_kernel_pa_start)

#define text_pa_start        ((paddr_t)&_text_pa_start)
#define text_pa_end          ((paddr_t)&_text_pa_end)

#define data_pa_start        ((paddr_t)&_data_pa_start)
#define data_pa_end          ((paddr_t)&_data_pa_end)

#define rodata_pa_start      ((paddr_t)&_rodata_pa_start)
#define rodata_pa_end        ((paddr_t)&_rodata_pa_end)

#define bss_pa_start         ((paddr_t)&_bss_pa_start)
#define bss_pa_end           ((paddr_t)&_bss_pa_end))
  
#define kernel_pa_end        ((paddr_t)&_kernel_pa_end)

#endif //__ASM__

#endif
