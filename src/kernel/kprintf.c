/**
 * \file /kernel/kprintf.c
 * \brief Contains kprintf() definition.
 */
#include <types.h>
#include <string.h>
#include <stdarg.h>
#include <kernel/kernel.h>
#include <kernel/kprintf.h>

static char buffer[1024];

/**
 * \fn uint32_t kprintf(const char* format, ...)
 * \brief Format and print a string to the default output.
 * \param param The string to format and print
 * \param ...   The list of optionnal arguments
 * \return The number of caracters printed.
 */
/** \todo A cleaner and reentrant kprintf could be useful.
 *       Especially by removing these ugly lines:
 *       static char buffer[1024];  
 *       *(char*)(KERNEL_SPACE + 0xb8000 + 160*line + 2*column) = buffer[i]; */
uint32_t kprintf(const char* format, ...)
{
  static uint32_t line = 0;
  static uint32_t column = 0;

  uint32_t printed_characters = 0;
  uint32_t i;
  va_list args;

  va_start(args, format);
  vsnprintf(buffer,1024, format, args);
  va_end(args);

  for (i = 0; buffer[i] != '\0'; i++)
    {
      /*Is the character to print an escape sequence ?*/
      switch (buffer[i])
	{
	  //Carriage return
	case '\r':
	  column = 0;
	  break;
	  //Newline
	case '\n':
	  line++;
	  column = 0;
	  break;
	  //Horizontal tab
	case '\t':
	  column = column + 4;
	  break;
	  //Backspace
	case '\b':
	  if(column > 0)
	    column--;
	  break;
	  //If not an escape sequence
	default:
	  *(char*)(KERNEL_SPACE + 0xb8000 + 160*line + 2*column) = buffer[i];
	  *(char*)(KERNEL_SPACE + 0xb8000 + 160*line + 2*column +1 ) = 0x7;
	  printed_characters++;
	  column++;
	}

      if (column > 79)
	{
	  line++;
	  column = column % 80;
	}
      if (line > 24)
	{
	  line = 24;

	  //We scroll the screen by copying the last 24 lines to the top of the screen
	  memmove((void*)(KERNEL_SPACE + 0xB8000),
		  (void*)(KERNEL_SPACE + 0xB8000 + 160),
		  2*80*24);
	  //We clear the end of the line
	  memset((void*)(KERNEL_SPACE + 0xB8000 + 2 * 80 * 24), 0, 160);
	}
    }

  return printed_characters;
}


