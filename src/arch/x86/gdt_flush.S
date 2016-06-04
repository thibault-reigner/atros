#define __ASM__

#include <x86/gdt.h>


.globl gdt_flush

/*
  Load a new value in the GDT register, then update
  the segment selectors.
*/
gdt_flush:
	pushl %eax
	movl 8(%esp), %eax
	
	lgdt (%eax)
	
	mov $KERNEL_DS, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov %ax, %ss

	ljmp $KERNEL_CS, $gdt_flush_done
	
gdt_flush_done:
	pop %eax
	ret
