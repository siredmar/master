/* crc32-lsb.cc

   written by Marc Singer
   12 September 2008

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   This LSB order version of the CRC32 computation is compatible with
   the way that UBOOT performs the computation.  This version is built
   into the program when necessary.

*/

#undef  TALK
//#define TALK 2

#include <config.h>
#include <linux/types.h>
#include <apex.h>
#include <talk.h>

/** Compute CRC for UBOOT images.  UBOOT uses an LSB ordered CRC
    computation that is incompatible with the CRC calculations used
    throughout APEX.  Both algorithms are considered 'standard', but
    the UBOOT version will not result in 0 when the CRC is appended to
    the byte stream. */

uint32_t compute_crc32_lsb (uint32_t crc, const void* pv, int cb)
{
#define IPOLY		(0xedb88320)
  unsigned char* pb = (unsigned char*) pv;
  uint32_t poly = IPOLY;	// Hack to get a register allocated

  //  dumpw (pv, cb < 128 ? cb : 128, 0, 0);
  DBG (2, "%s: 0x%x * 0x%p + %d", __FUNCTION__, crc, pv, cb);

  crc = ~crc;                   // Invert because we're continuing

#define DO_CRC\
  if (crc & 1) { crc >>= 1; crc ^= poly; }\
  else	       { crc >>= 1; }

  while (cb--) {
    crc ^= *pb++;

    DO_CRC;
    DO_CRC;
    DO_CRC;
    DO_CRC;
    DO_CRC;
    DO_CRC;
    DO_CRC;
    DO_CRC;
  }

  DBG (2, " -> 0x%08x\n", ~crc);

  return ~crc;
}

