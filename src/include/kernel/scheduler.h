/**
 * \file include/kernel/scheduler.h
 * \brief Contains definitions relating to threads scheduling.
 */
#ifndef KERNEL_SCHEDULER_H
#define KERNEL_SCHEDULER_H

#include <kernel/thread.h>

#ifdef __ARCH_x86__
#include <x86/cpu_context.h>
#else
#error "No target architecture defined in scheduler.h!"
#endif

#ifndef __ASM__
void scheduler_launch(void);
void scheduler(User_context *u_context);
void scheduler_set_ready(Thread *a_thread);
#endif

#endif
