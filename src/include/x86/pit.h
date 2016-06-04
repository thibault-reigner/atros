/**
 * \file include/x86/pit.h
 * \brief Contains definitions related to PIT on the x86 architecture.
 */
#ifndef x86_PIT_H
#define x86_PIT_H

#ifndef __ASM__
void pit_init(void);
void pit_set_frequency(uint32_t hz);
#endif

#endif
