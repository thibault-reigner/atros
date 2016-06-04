/***************************************************
 Linker script for AttrOS.
 Target architecture: x86
 NB: this script will be preprocessed by GCC to
 generate linker.lds which is the actual linker
 script. 
 Doing so is usefull since we can use constants
 or symbols defined in C headers.
***************************************************/
#define __ASM__

#include <kernel/kernel.h>

OUTPUT_FORMAT("elf32-i386")
/*OUTPUT_ARCH(i386)*/
ENTRY(_start)

/* base virtual address of the kernel */
VIRT_BASE = KERNEL_SPACE;

SECTIONS
{
    . = 0x100000;
   
   /*The following section contains code and data used during the
     initialisation process of the kernel.*/
     _boot_pa_start = .;
    .boot :
    {
        *(.boot)
        . = ALIGN(0x1000);
	boot_pgd = .;
	. +=  0x1000;
	boot_pgt = .;
	. +=  0x4000;
	/*The kernel stack*/
	. += KERNEL_STACK_SIZE;
	_kernel_stack_pa = .;
    }
    _boot_pa_end = .;

    /*
     * From now, code and data will use at virtual addresses
     */
	
    . += VIRT_BASE;

    _kernel_pa_start = . - VIRT_BASE;

    .text ALIGN(0x1000) : AT(ADDR(.text) - VIRT_BASE)
    {
        _text_pa_start = . - VIRT_BASE;
        *(.text)
        *(.gnu.linkonce.t*)
	_text_pa_end = . - VIRT_BASE;
    }

    .data ALIGN(0x1000) : AT(ADDR(.data) - VIRT_BASE)
    {
	_data_pa_start = . - VIRT_BASE;
        *(.data)
        *(.gnu.linkonce.d*)
	_data_pa_start = . - VIRT_BASE;
    }
    
    .rodata ALIGN(0x1000) : AT(ADDR(.rodata) - VIRT_BASE)
    {
	_rodata_pa_start = . - VIRT_BASE;
        *(.rodata*)
        *(.gnu.linkonce.r*)
	_rodata_pa_end = . - VIRT_BASE;
    }
    
    .bss ALIGN(0x1000) : AT(ADDR(.bss) - VIRT_BASE)
    {
	_bss_pa_start = . - VIRT_BASE;
        *(COMMON)
        *(.bss)
        *(.gnu.linkonce.b*)

	. = ALIGN(0x1000);
	/*The kernel page directory*/
	_kpgd = .;
	. += 0x1000;
	/*The kernel needs at least one page table*/
	_first_kpgt = .;
	. += 0x1000;

       _bss_pa_end = . - VIRT_BASE;
    }

    _kernel_pa_end = . - VIRT_BASE;
    
    /DISCARD/ :
    {
        *(.comment)
        *(.eh_frame)
    }
    
    /*Since the boot code only map 4Mb, we have too check the kernel size.
      We define 1Mb as the maximum size for the kernel.*/
    . = ASSERT(_kernel_pa_end  < 0x200000, "Kernel is too big! (More than 1Mb)");
}
