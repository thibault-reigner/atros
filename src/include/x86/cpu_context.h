/**
 * \file include/x86/cpu_context.h
 * \brief Contains definitions related to the CPU contexts (the state of the CPU
 *        saved on the stack during an interrupt) management on the x86 architecture.
 */
#ifndef x86_CPU_CONTEXT_H
#define x86_CPU_CONTEXT_H

/* Definitions used to set/clear bits of the EFLAGS register. 
 * The EFLAGS register is the x86 Machine Status Word, its
 * value determinates the behavior of the machine.
 *
 * Each bit has a special meaning and belongs to one of three
 * following categories:
 * status flag: indicate the state of the processor, such flag
 *              is modified by several common instructions such
 *              as ADD, SUB, etc... and indicates the sign, the
 *              carry, etc... of the result.
 * control flag: indicate how the processor should perform some
 *               instructions.
 * system flag : indicates privileges of the running task or
 *               other critical parameters such as the ability
 *               of the running to be whether or not interrupted.
 *
 */
#define EFLAGS_IOPL0 0
#define EFLAGS_IOPL3 (3 << 12)

#define EFLAGS_INTERRUPT_ENABLE (1 << 9)



#ifndef __ASM__

/**
 * \struct Cpu_context
 * \brief The CPU context saved on the stack during an interruption by low level
 *        interrupts handlers.
 */
struct Cpu_context 
{
  uint32_t ds;         /**< DS: data segment selector register*/
  uint32_t es;         /**< ES: extra segment selector register*/
  uint32_t fs;         /**< FS: general purpose segment selector register*/
  uint32_t gs;         /**< GS: general purpose segment selector register*/
  uint32_t edi;        /**< EDI: destination index register*/
  uint32_t esi;        /**< ESI: source index register*/
  uint32_t ebp;        /**< EBP: stack base pointer register*/
  uint32_t old_esp;    /**< ESP: the old value of the stack pointer register*/
  uint32_t ebx;        /**< EBX: base register*/
  uint32_t edx;        /**< EDX: data register*/
  uint32_t ecx;        /**< ECX: counter register*/
  uint32_t eax;        /**< EAX: accumulator register*/
  uint32_t int_number; /**< The interrupt number*/
  uint32_t error_code; /**< Error code pushed by the processor (used for some exception)*/

 /* The following registers are pushed by the processor*/

  uint32_t eip;               /**< EIP: instruction pointer register*/
  uint32_t cs;                /**< CS: code segment selector register*/
  uint32_t eflags;            /**< EFLAGS: flags register*/
}__attribute__ ((packed));

/**
 * \struct User_context
 * \brief User's context saved on the stack during an interruption.
 */
typedef struct User_context{
  struct Cpu_context context; /**< The cpu context saved by the interrupt's handler*/

  /* The following registers are pushed by the processor only during
     a prilivege level switch*/
  uint32_t esp;               /**< ESP: the stack pointer of the interrupted process*/
  uint32_t ss;                /**< SS: the stack segment selector of the interrupted process*/
} User_context __attribute__ ((packed));

void user_context_init(struct User_context *a_context,
		       vaddr_t program_counter,
		       vaddr_t stack_top,
		       uint32_t param);
void cpu_context_load(struct User_context a_context);

#endif //__ASM__

#endif
