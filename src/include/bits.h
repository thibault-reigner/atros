/**
 * \file include/bits.h
 * \brief Contains definitions related to bits management.
 */
#ifndef BITS_H
#define BITS_H

#include <bits.h>

#ifndef __ASM__

inline uint32_t most_significant_bit_of(uint32_t i);

/**
 * \fn inline uint32_t most_significant_bit_of(uint32_t i)
 * \brief Returns the index of the most significant bit of 
 *        the given integer (between 1 and 32)
 * \param i The integer
 * \return The index of the most significant bit, 0 if the given
 *         integer is 0.
 */
inline uint32_t most_significant_bit_of(uint32_t i)
{
  uint32_t bits = 0;
  if (i > 32767) {
    i >>= 16;
    bits += 16;
  }
  if (i > 127) {
    i >>= 8;
    bits += 8;
  }
  if (i > 7) {
    i >>= 4;
    bits += 4;
  }
  if (i > 1) {
    i >>= 2;
    bits += 2;
  }
  if (i > 0) {
    bits++;
  }
  
  return bits;
}

#endif //_ASM__

#endif
