/*
 This file contains the multiboot header and the kernel entry point which
 initialises a proper runtime environment before calling C-code.
*/

#define __ASM__
	
#include <kernel/kernel.h>
//#include <x86/boot/multiboot.h>
#include <x86/paging.h>

.globl _start	
.code32		
	.section ".boot", "w"	


#include "multiboot_header.S"
#include "multiboot2_header.S"

	
/**************************DATA******************************/
//The value to load in the GDTR register
boot_gdt_ptr:
	.word (boot_gdt_end - boot_gdt - 1)
	.long boot_gdt
	
//The GDT used during the boot process
.align 8
boot_gdt:
	//null gdt entry
	.long 0, 0
	//code segment, DPL 0, base 0, limit 4Go
	.byte 0xFF, 0xFF, 0, 0, 0, 0b10011010, 0b11001111, 0
	//data segment, DPL 0, base 0, limit 4Go
	.byte 0xFF, 0xFF, 0, 0, 0, 0b10010010, 0b11001111, 0
boot_gdt_end:
	
//The page directory used during the boot process
.extern boot_pgd
//A page directory used during the boot process (to map 4Mb)
.extern boot_pgt
	
/************************CODE*******************************/	

_start:
//We load our own gdt since we don't anything about the one
//set up by the bootloader.
	cli
	lgdt (boot_gdt_ptr)
	mov $0x10, %cx
	mov %cx, %ds
	mov %cx, %es
	mov %cx, %fs
	mov %cx, %gs
	mov %cx, %ss

	ljmp $0x08,$boot_gdt_loaded

boot_gdt_loaded:
	//We set a new stack
	mov $_kernel_stack_pa, %esp
	//We have to clear the bss section
	movl $_bss_pa_start, %edi
	movl $_bss_pa_end, %ecx
	subl $(_bss_pa_start - 1), %ecx
	call mem_clear
	
	//We initialise the page directory
	movl $boot_pgd, %edi
	movl $(1024*4), %ecx
	call mem_clear
	
	//we set boot_pgd[0] = boot_pgt | PAGE_SUPERVISOR | PAGE_READ_WRITE | PAGE_PRESENT
	movl $boot_pgd, %edi
	movl $boot_pgt, %edx
	orl $(PAGE_SUPERVISOR | PAGE_READ_WRITE | PAGE_PRESENT), %edx
	movl %edx, (%edi)
	//we set boot_pgd[KERNEL_SPACE >> 22] = boot_pgd[0]
	addl $((KERNEL_SPACE >> 22)*4), %edi
	movl %edx, (%edi)
	
//We initialise the page table
	//Virtual pages at 0x0 and 0xC0000000 are marked as non-present
	//Useful to define NULL pointers
	movl $0, boot_pgt
	movl $(boot_pgt + 4), %edi
	movl $(4096 + PAGE_SUPERVISOR | PAGE_READ_WRITE | PAGE_PRESENT), %edx
set_boot_pgt:
	cmp $(boot_pgt + 4*1024), %edi
	je end_set_boot_pgt
	movl %edx, (%edi)
	addl $4096, %edx
	addl $4, %edi
	jmp set_boot_pgt
end_set_boot_pgt:	

//We load the boot page directory's address in the CR3 register
	movl $boot_pgd, %ecx
	movl %ecx, %cr3
//We enable paging
	movl %cr0, %ecx
	orl $0x80000000, %ecx
	movl %ecx, %cr0
/*
  At this stage, we deal with virtual addresses.
  The first 4Mb of the virtual address space and the
  first 4Mb after KERNEL_SPACE in the virtual address space
  are both mapped to the first 4Mb of the physical memory
*/
//Intel documentation recommands a far jump after enabling paging
	jmp $0x08,$(paging_enabled + KERNEL_SPACE)
	
paging_enabled:

	//Update kernel stack location (add virtual adresses offset)
	addl $KERNEL_SPACE, %esp
	//We push on the kernel stack the arguments given by the multiboot loader
	push %ebx	//The address of the multiboot information structure
	push %eax	//EAX contains 0x2BADBOO2
	movl $main, %eax
	call *%eax

	/*In case the main function returns*/
	cli
endless_loop:
	hlt
	jmp endless_loop


/*
mem_clear(), set to 0 each byte of a memory zone.
parameters:	%edi: pointer to the memory zone to clear
		%ecx: number of bytes to clear
*/
mem_clear:
	cmpl $0, %ecx
	jbe mem_clear_end
mem_clear_loop:	
	movb $0, (%edi)
	inc %edi
	loop mem_clear_loop
mem_clear_end:
	ret
