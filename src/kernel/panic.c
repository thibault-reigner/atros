/**
 * \file /kernel/panic.c
 * \brief Contains functions to display information and halt the CPU
 *        when the kernel encounters a critical error.
 */
#include <types.h>
#include <stdarg.h>

#include <kernel/panic.h>
#include <kernel/kprintf.h>

#include <x86/x86.h>

static char buffer[1024];

/**
 * \fn void panic(const char* format, ...)
 * \brief Print a formatted string and halt the CPU
 * \param format The string to format.
 * \param ... Optional parameters. Types and numbers are defined by format
 *            specifiers in format (such as: %c, %d, %s, etc...)
 * This function is used when the kernel encounters a critical error.
 */
void panic(const char* format, ...)
{
  va_list args;

  cli();

  va_start(args,format);
  vsnprintf(buffer,1024,format,args);
  va_end(args);

  kprintf("\nPANIC: ");
  kprintf(buffer);

  asm("hlt");
  for(;;);
}
