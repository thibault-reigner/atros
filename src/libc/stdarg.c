
#include <stdarg.h>
#include <types.h>

uint32_t vsnprintf(char* str, size_t n, const char* format, va_list args)
{
  uint32_t i = 0; //Index for format
  uint32_t j = 0; //Index for s
  uint16_t base = 0; //base used for number representation
  uint32_t arg_width;
  bool_t specifier_expected = FALSE;

  uint32_t k;
  char* arg_str = NULL;

    for (i = 0; format[i] != '\0' && i < (n-1); i++)
      {
		
      /*If the previous character was '%', a format specifier is expected*/
      if (specifier_expected)
	{
	  switch (format[i])
	    {
	      /*Integer argument*/
	    case 'd':
	    case 'i':
	      base = 10;
	      goto convert_int_to_str;
	    case 'o':
	      base = 8;
	      goto convert_int_to_str;
	    case 'x':
	    case 'X':
	      base = 16;
	    convert_int_to_str:
	      arg_width = i_to_nstr(va_arg(args,int32_t), (n-j), &str[j], base);
	      j += arg_width;
	      break;
	      /*Unsigned integer argument*/
	    case 'b':
	      base = 2;
	      goto convert_uint_to_str;
	    case 'u':
	      base = 10;
	      goto convert_uint_to_str;
	    case 'p':
	      base = 16;
	    convert_uint_to_str:
	      arg_width = u_to_nstr(va_arg(args, uint32_t), (n-j), &str[j], base);
	      j += arg_width;
	      break;
	      /*Character argument*/
	    case 'c':
	      /*NB: A 'char' argument is promoted to 'int' when passed through '...'*/
	      str[j] = (char)va_arg(args, int32_t);
	      j++;
	      break;
	      /*String argument*/
	    case 's':
	      arg_str = (char*)va_arg(args, char*);
	      for (k = 0; arg_str[k] != '\0' && (n-j-1); k++)
		{
		  str[j] = arg_str[k];
		  j++;
		}
	      break;
	      /*The '%' character*/
	    case '%':
	      str[j] = '%';
	      j++;
	      break;
	    }

	  specifier_expected = FALSE;
	}
      else if ( format[i] == '%')
	{
	  specifier_expected = TRUE;
	}
      else
	{
	str[j] = format[i];
	j++;
      }

    }

  str[j] = '\0';

  return j;
}

/*Converts an integer value to a null-terminated string
  using the specified base and stores the result in the array 
  given by str parameter.
  If the resulting string would be longer than n-1 characters
  then the resulting string only contains the character '\0'.

  Returns the length of the resulting string.
*/
size_t i_to_nstr(int32_t value, size_t n, char* str, uint16_t base)
{
  uint32_t abs_value;

  if (value < 0)
    {
      uint32_t width;
      abs_value = (uint32_t)(-value);
      width = u_to_nstr(abs_value,n - 1,str + 1,base);

      if (width > 0)
	{
	  /*We add the '-' character*/
	  str[0] = '-';
	  return width + 1;
	}
      else
	{
	  return 0;
	}
    }
  else
    {
      abs_value = (uint32_t)value;
      return u_to_nstr(abs_value,n,str,base);
    }

}


/*
  Converts an unsigned integer value to a null-terminated string
  using the specified base and stores the result in the array 
  given by str parameter.
  If the resulting string would be longer than n-1 characters
  then the resulting string only contains the character '\0'.

  Returns the length of the resulting string.
*/
size_t u_to_nstr(uint32_t value, size_t n, char* str,  uint16_t base)
{
  uint32_t i = 0;  
  uint32_t j = 0; //beginning's index of the number representation

  if(n<=0)
    {
      return 0;
    }

  if(base < 2 || base > 36)
    { 
      str[0] = '\0';
      return 0;
    }


  /*We add the "0b" prefix if base is 2*/
  if(base == 2)
    {
      if(n > 2)
	{
	  str[0] = '0';
	  str[1] = 'b';
	  i = i + 2;
	  j = 2;
	}
      else
	{
	  str[0] = '\0';
	  return 0;
	}
    }
  /*We add the "0x" prefix if base is 16*/
  else if(base == 16)
    {
      if(n > 2)
	{
	  str[0] = '0';
	  str[1] = 'x';
	  i = i + 2;
	  j = 2;
	}
      else
	{
	  str[0] = '\0';
	  return 0;
	}
    }

  uint32_t r = 0;
  const char* digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

  /*We compute the value representation*/
  do{
    r = value % base;
    value = value / base;
    str[i] = digits[r];
    i++;
  }while(value != 0 && i < n);

  /*If we computed the whole number representation*/
  if(value == 0)
    {
      char c;
      uint32_t length = i;
      str[i] = '\0';

      /*We have to reverse the string to get the representation in
	the proper order.*/
      for(i = i - 1; i > j; i--, j++)
	{
	  c = str[j];
	  str[j] = str[i];
	  str[i] = c;
	}

      return length;
    }
  else
    {
      str[0] = '\0';
      return 0;
    }

}
