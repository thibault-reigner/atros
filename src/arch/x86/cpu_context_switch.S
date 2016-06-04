#define __ASM__
#include <x86/gdt.h>

	
.code32	
.globl cpu_context_load
.globl __cpu_context_restore

//void cpu_context_load(struct Cpu_context)
cpu_context_load:
	addl $4, %esp //we remove the EIP value saved on the stack
	jmp __cpu_context_restore

	
/* cpu_context_restore restore a CPU context (User_context or Kernel_context structure)
 * which is on the stack.
 * NB: this function is not supposed to be called with the CALL instruction, use
 * JMP instead.
 * If you need to call it from C code call cpu_context_switch instead which is a
 * wrapper.	
 */
__cpu_context_restore:
	popl %ds
	popl %es
	popl %fs
	popl %gs
	popa
	
	addl $8, %esp //We delete the error code and the interrupt number
	iret