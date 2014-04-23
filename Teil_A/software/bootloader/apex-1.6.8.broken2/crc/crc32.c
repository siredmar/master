/* crc32.cc

   written by Marc Singer
   6 November 2004

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   An original reference for information about implementating CRC32 is
   a document written by Ross Williams in 1993, "A Painless Guide to
   CRC Error Detection Algorithms."  Using his nomenclature, the CRC
   algorithm implemented by Ethernet and Zip compression has the
   following features:

     Width  : 32
     Poly   : 04C11DB7
     iPoly  : EDB88320		// reverse bit order
     Init   : FFFFFFFF
     RefIn  : True
     RefOut : True
     XorOut : FFFFFFFF
     Check  : CBF43926

   Width parameter is the length, in bits, of the polynomial.  The
   Poly is a bit mask indicating which terms have non-zero
   coefficients.  In this case,

     g(x) = x^32 + x^26 + x^23 + x^22 + x^16 + x^12 + x^11 + x^10 + x^8 +
	    x^7 + x^5 + x^4 + x^2 + x + 1

   Init is value used to initialize the CRC function.  RefIn and
   RefOut, being true, indicate that the algorithm interprets the MSB
   of each byte as an MSB.  XorOut is the XOR mask applied to the CRC
   on completion.  Check is the CRC for the sequence "123456789".

   The implementation used herein comes from BLOB and was the work of
   several people, Erik Muow, Jan-Derk Bakker, and Russell King.  It
   was found to be faster than the table algorithm though a little
   larger, ~124 bytes.

*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>

#define POLY		(0x04c11db7)
#define IPOLY		(0xedb88320)

#include "crctable.out"

#if 0

/* Allegedly faster version, we want the smallest code possible */

unsigned long compute_crc32_x (unsigned long crc, const void* pv, int cb)
{
  unsigned char* pb = (unsigned char*) pv;
  unsigned long poly = IPOLY;	// Hack to get a register allocated

  crc = crc ^ 0xffffffff;	// Invert because we're continuing

#define DO_CRC\
  if (crc & 1) { \
    crc >>= 1; crc ^= poly; }\
  else { crc >>= 1; }

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

  return crc ^ 0xffffffff;
}

#endif


/* compute_crc32

   calculates a standard 32 bit CRC checksum over a sequence of bytes.
   This implementation is optimized for the ARM architecture.  Other
   architectures, e.g. i386 will probably not be well served by this
   version as it depends on excellent optimization by the compiler as
   well as a adequate number of registers.

*/

unsigned long compute_crc32 (unsigned long crc, const void *pv, int cb)
{
  const unsigned char* pb = (const unsigned char *) pv;

  //  crc ^= 0xffffffff;

  while (cb--) {
    int i;
    crc ^= *pb++;

    for (i = 8; i--; ) {
      if (crc & 1) {
	crc >>= 1;
	crc ^= IPOLY;
      }
      else
	crc >>= 1;
    }
  }

  //  return crc ^ 0xffffffff;
  return crc;
}

/* compute_crc32_0

   is the first version of the CRC computation that appears to
   generate the same CRC as the GNU cksum.  It doesn't seem to produce
   the same check value as the "Painless Guide to CRC" does, but it's
   a start.

   In order for this routine to produce a correct CRC, it must have a
   word of zeros shifted in.  Unlike the byte oriented versions, this
   one does not implicitly shift in zeros at the end of the input.

*/

unsigned long compute_crc32_0 (unsigned long crc, const void *pv, int cb)
{
  const unsigned char* pb = (const unsigned char *) pv;

  for (; cb--; ++pb) {
    int i;
    unsigned char mask;

    for (mask = 1<<7; mask; mask >>= 1) {
      int f = (crc & (1<<31));
      crc = crc << 1;
      if (*pb & mask)
	crc |= 1;
      if (f)
	crc ^=POLY;
    }
  }

  return crc;
}

/* compute_crc32_1

*/

unsigned long compute_crc32_1 (unsigned long crc, const void *pv, int cb)
{
  const unsigned char* pb = (const unsigned char *) pv;

  for (; cb--; ++pb) {
    int i;
    unsigned char mask;

    unsigned char b = (crc >> 24);
    crc <<= 8;
    b ^= *pb;

    for (mask = 1<<7; mask; mask >>= 1) {
      int f = (b & mask);
      crc = crc << 1;
      if (f)
	crc ^=POLY;
    }
  }

  //  return crc ^ 0xffffffff;
  return crc;
}


/* compute_crc32_2

   implements the un-reversed table lookup version of the algorithm.
   Note that this version does not need to have zeros shifted in at
   the end in order to compute the remainder on the augmented message.
   Instead, that aspect is implicit.

*/

unsigned long compute_crc32_2 (unsigned long crc, const void *pv, int cb)
{
  unsigned char *blk_adr;
  unsigned long  blk_len;

  const char* pb = (const char*) pv;

  while (cb--)
    crc = crctable[((crc >> 24) ^ *pb++) & 0xff] ^ (crc << 8);
  return crc;
}


/* compute_crc32_3

   is a working, byte oriented version of the crc computation routine
   that does not require zero shifting.  It is suitable for APEX,
   though I wonder if it isn't worthwhile to use the table
   implementation even though the latter is much longer, due to the
   lookup table.  I suppose we can use CONFIG_SMALL to select one or
   the other.

*/

unsigned long compute_crc32_3 (unsigned long crc, const void *pv, int cb)
{
  const unsigned char* pb = (const unsigned char *) pv;

  while (cb--) {
    int i;
    crc ^= *pb++ << 24;

    for (i = 8; i--; ) {
      if (crc & (1<<31))
	crc = (crc << 1) ^ POLY;
      else
	crc <<= 1;
    }
  }

  return crc;
}


#define CALC compute_crc32_3

inline unsigned long cksum (unsigned long (*p)(unsigned long,
					       const void*, int),
			    const void* pv, int cb)
{
  unsigned long crc = p (0,pv,cb);
  unsigned long v;
  for (v = cb; v; v >>= 8)
    crc = p (crc, &v, 1);
  return ~crc;
}

int main (int argc, char** argv)
{
  unsigned long crc = 0;
  unsigned long zero = 0;
  int fh;
  void* pv;
  size_t cb;

  if (argc < 2) {
    printf ("usage: foo FILE\n");
    exit (1);
  }

  fh = open (argv[1], O_RDONLY);
  if (fh == -1) {
    printf ("unable to open file\n");
    exit (1);
  }
  cb = lseek (fh, 0, SEEK_END);
  lseek (fh, 0, SEEK_SET);
  pv = mmap (NULL, cb, PROT_READ, MAP_PRIVATE, fh, 0);

  printf ("check 0x%08x\n", CALC (~0, "123456789", 9) ^ 0xffffffff);

  crc = CALC (crc, pv, cb);
  printf ("crc %x\n", crc);

  {
    unsigned long v = CALC (crc, &zero, 4);
    unsigned long y = htonl (crc);
//    unsigned long x = CALC (v, &v, 4);
    unsigned long x = CALC (crc, &y, 4);
    printf ("crc w/zero %x (%u)  w/zero&crc %x (%u)\n", v, v, x, x);
  }

//printf ("crc w/zeros %x (%u)\n", CALC (crc, &zero, 4), CALC (crc, &zero, 4));

  {
    unsigned long v = cb;
    while (v) {
      printf ("  length byte 0x%x\n", ((char*)&v)[0]);
      crc = CALC (crc, &v, 1);
      v >>= 8;
    }
  }

  printf ("crc w/len 0x%x (%u %u)\n", crc, crc, ~crc);

  crc = CALC (crc, &zero, 4);

  printf ("crc w/len&zeros 0x%x (%u)\n", crc, crc);

  printf ("checksum 0x%x (%u) 0x%x (%u) %d %s\n",
	  crc, crc, ~crc, ~crc, cb, argv[1]);

  printf ("3/2 %u %u\n",
	  cksum(compute_crc32_3,pv,cb),
	  cksum(compute_crc32_2,pv,cb));

#if 0
  printf ("rev/1/2 0x%08x 0x%08x 0x%08x\n",
	  compute_crc32 (0, pv, cb),
	  compute_crc32_1 (0, pv, cb),
	  compute_crc32_2 (0, pv, cb));
#endif

  return 0;
}
