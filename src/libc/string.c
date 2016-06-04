#include <types.h>
#include <string.h>

/**
 * \fn void * memcpy(void *dest, const void *src, size_t num)
 * \brief Copy num bytes from src to dest.
 * \param dest The destination buffer
 * \param src The source buffer
 * \param num Number of bytes to copy.
 * \return Pointer to the destination buffer
 *
 * src and dest buffers must not overlap, use memmove() instead
 */
void * memcpy(void *dest, const void *src, size_t num)
{
  const uint8_t* s = (const uint8_t*)src;
  uint8_t* d = (uint8_t*)dest;

  while (num--)
    d[num] = s[num];
		
  return dest;
}

/**
 * \fn void * memmove(void *dest, const void *src, size_t num)
 * \brief Copy num bytes from the src buffer to the dest buffer.
 * \param dest The destination buffer
 * \param src The source buffer
 * \param num The number of bytes to copy.
 * \return Pointer to the destination buffer.
 *
 * src and dest buffers can overlap.
 */
void * memmove(void *dest, const void *src, size_t num)
{
  //Indicate the direction of the copy
  int32_t delta;
  size_t i;
  const byte_t *s = (byte_t*)src;
  byte_t *d = (byte_t*)dest;

  if ((vaddr_t)s >= (vaddr_t)d)
    {
      delta = 1;
      i = 0;
    }
  else
    {
      delta = -1;
      i = num - 1;
    }

  while (num--)
    {
      d[i] = s[i];
      i += delta;
    }

  return dest;
}

/**
 * \fn void * memset(void *ptr, int32_t value, size_t num)
 * \brief Set the first num bytes of ptr to the given value.
 * \param ptr Buffer to initialise
 * \param value The value to set to each byte.
 * \param num Number of bytes to set.
 * \return Pointer to ptr.
 */
void * memset(void *ptr, int_t value, size_t num)
{
  byte_t  *d = (byte_t*) ptr;
  
  while (num--)
    {
      *d = (byte_t)value;
      d++;
    }

  return ptr;
}


char * strncpy(char * dest, const char * src, size_t num )
{
  uint32_t i;

  for (i=0; src[i] != '\0' && i < num; i++)
    {
      dest[i] = src[i];
    }
  //Fill the remaining characters with '\0';
  for (;i < num;i++)
    {
      dest[i] = '\0';
    }
  return dest;
}

int32_t strcmp( const char * str1, const char * str2 )
{
  uint32_t i;

  for (i=0; str1[i] == str2[i] && str1[i] != '\0' && str2[i] != '\0'; i++);

  return (str1[i] - str2[i]);
}
