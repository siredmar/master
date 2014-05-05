/* memtest.c

   written by Marc Singer
   7 Nov 2006

   Copyright (C) 2006 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   The full memory test now uses a random bit pattern.  The test ought
   to be improved to scan the memory array several times in order to
   cover more ground.  The RNG is a relatively efficient linear
   congruential genrator.  It's low quality for overall entropy, but
   good for our purpose.

   See
   <http://www.gnu.org/software/gsl/manual/html_node/Other-random-number-generators.html>
   for a description of different random number generators.

*/

#include <config.h>
#include <attributes.h>
#include <memtest.h>
#include <debug_ll.h>

/* memory_test_0

   is a data bus/address bus memory verification test for verifying
   that memory is OK before we copy APEX into SDRAM.

*/

int __naked __section (.bootstrap) memory_test_0 (unsigned long address,
						  unsigned long c)
{
  const unsigned long pattern_a = 0xaaaaaaaa;
  const unsigned long pattern_b = 0x55555555;

  volatile unsigned long* p = (volatile unsigned long*) address;
  unsigned long offset;
  unsigned long mark;

  __asm volatile ("mov fp, lr");

  c /= 4;	/* Convert to count of words */

		/* Walking data bit.  Each write must be to a
		   different address to avoid memory capacitance from
		   generating a falsely correct value. */
//  PUTC_LL ('a');
  for (mark = 1; mark < c/4; mark <<= 1)
    *(volatile unsigned long*) (address + mark*4) = mark;
//  PUTC_LL ('b');
  for (mark = 1; mark < c/4; mark <<= 1) {
    if (*(volatile unsigned long*) (address + mark*4) != mark)
      __asm volatile ("mov r0, #1\n\t"
		      "mov pc, fp");
  }

//  PUTC_LL ('c');

		/* Walking address bits */
  for (offset = 1; offset < c; offset <<= 1)
    p[offset] = pattern_a;

  p[0] = pattern_b;

//  PUTC_LL ('d');
  for (offset = 1; offset < c; offset <<= 1)
    if (p[offset] != pattern_a)
	__asm volatile ("mov r0, #2\n\t"
			"mov pc, fp");

  p[0] = pattern_a;

//  PUTC_LL ('e');
  for (mark = 1; mark < c; mark <<= 1) {
    p[mark] = pattern_b;
    if (p[0] != pattern_a)
	__asm volatile ("mov r0, #3\n\t"
			"mov pc, fp");

    for (offset = 1; offset < c; offset <<= 1)
      if (p[offset] != pattern_a && offset != mark)
	__asm volatile ("mov r0, #4\n\t"
			"mov pc, fp");

    p[mark] = pattern_a;
  }

#if defined (CONFIG_BOOTSTRAP_MEMTEST_FULL)

//  PUTC_LL ('f');

	/* Full memory test.  This is probably unnecessary given the
	   fact that failures in memory these days tend to be shorts
	   or cold-solder joints in manufacturing and not 'stuck'
	   cells.  */
  {
    unsigned long v;
#define A 69069
#define C 1
#define M 0xffffffff
#define RND(s) (((s)*A + C)&M)
//#define RND(s) ((s) + 1)

    v = 1;
    for (offset = 0; offset < c; ++offset)
      p[offset] = (v = RND (v));

    v = 1;
    for (offset = 0; offset < c; ++offset) {
      if (p[offset] != (v = RND (v)))
	__asm volatile ("mov r0, %0\n\t"
		      "mov pc, fp" :: "r" (offset*4));
      p[offset] = ~v;
    }
    v = 1;
    for (offset = 0; offset < c; ++offset)
      if (p[offset] != ~(v = RND (v)))
	__asm volatile ("mov r0, %0\n\t"
			"mov pc, fp" :: "r" (offset*4));

  }
#endif

  __asm volatile ("mov r0, #0\n\t"
		  "mov pc, fp");
  return 0;			/* Redundant, but calms the compiler */
}
