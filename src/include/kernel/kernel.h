/**
* \file include/kernel/kernel.h
* \brief Contains usefull symbols and defines for the kernel
*/
#ifndef KERNEL_KERNEL_H
#define KERNEL_KERNEL_H


/** \brief The size of the kernel stack, used during the initialisation process*/
#define KERNEL_STACK_SIZE 0x4000 //16ko

/** \brief The virtual address of the beginning of the kernel space*/
#define KERNEL_SPACE 0xC0000000
#define KERNEL_SPACE_SIZE ((1<<32) - KERNEL_SPACE)
#define KERNEL_SPACE_SIZE_SHIFT 30

/* Kernel virtual address space map:
 *
 *   +-----------------------+ 0x0
 *   |     User space        |
 *   .                       .
 *   .                       .
 *   +-----------------------+ KERNEL_SPACE (3Go)
 *   |                       |
 *   +-----------------------+ KERNEL_SPACE + 1MB
 *   |  Kernel image         | 
 *   |  & modules            |
 *   +-----------------------+ 
 *   . Physical pages'       .
 *   . descriptors array     .
 *   .                       .
 *   +---------------------- +
 *   . First slab for        .
 *   . Objs_cache structure  .
 *   . cache                 .
 *   +-----------------------+ 
 *   . First slab  for       . 
 *   . Slab structures cache .
 *   +-----------------------+ 
 *   . First slab for        .
 *   . Vregion structures    .
 *   . cache                 .
 *   +-----------------------+ 
 *   . Kernel virtual space  .
 *   . used to allocate      .
 *   . new objects or virtual.
 *   . pages                 .
 *   +-----------------------+ 4Go - 4Mo
 *   | Recursive paging area |
 *   +-----------------------+ 4Go
 */


#define SYSCALL_VECTOR 0x80

#ifndef __ASM__


#endif //__ASM__

#endif
