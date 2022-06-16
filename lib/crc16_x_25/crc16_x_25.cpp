#include "crc16_x_25.h"

#define order 16
#define polynom 0x1021
#define crcinit 0xffff
#define crcxor 0xffff

uint16_t crc16_x_25::reflect(uint16_t crc, int bitnum)
{

  // reflects the lower 'bitnum' bits of 'crc'

  uint16_t i, j = 1, crcout = 0;

  for (i = (uint16_t)1 << (bitnum - 1); i; i >>= 1)
  {
    if (crc & i)
      crcout |= j;
    j <<= 1;
  }
  return (crcout);
}

// http://www.zorc.breitbandkatze.de/crc.html
// http://www.zorc.breitbandkatze.de/crctester.c
uint16_t crc16_x_25::calc_checksum(unsigned char *p, uint16_t len)
{
  uint16_t crchighbit;
  uint16_t i, j, c, bit, crc;
  uint16_t crcmask;

  crcmask = ((((uint16_t)1 << (order - 1)) - 1) << 1) | 1;
  crchighbit = (uint16_t)1 << (order - 1);

  crc = crcinit;
  for (i = 0; i < order; i++)
  {

    bit = crc & 1;
    if (bit)
      crc ^= polynom;
    crc >>= 1;
    if (bit)
      crc |= crchighbit;
  }

  // bit by bit algorithm with augmented zero bytes.
  // does not use lookup table, suited for polynom orders between 1...32.

  for (i = 0; i < len; i++)
  {

    c = (uint16_t)*p++;
    c = this->reflect(c, 8);

    for (j = 0x80; j; j >>= 1)
    {

      bit = crc & crchighbit;
      crc <<= 1;
      if (c & j)
        crc |= 1;
      if (bit)
        crc ^= polynom;
    }
  }

  for (i = 0; i < order; i++)
  {
    bit = crc & crchighbit;
    crc <<= 1;
    if (bit)
      crc ^= polynom;
  }

  crc = this->reflect(crc, order);
  crc ^= crcxor;
  crc &= crcmask;

  return (crc);
}