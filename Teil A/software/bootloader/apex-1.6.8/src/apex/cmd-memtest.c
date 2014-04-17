/* cmd-memtest.c

   written by Marc Singer
   22 Jul 2007

   Copyright (C) 2007 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   MMU
   ---

   If the MMU is enabled, we need to disable it for the memory test.
   This can be done by either clearing the MMU enable bit, or by
   changing the segment attribute bits.  I think it is probably best
   to to the latter since we can leave the MMU on (for I-CACHE for
   example) for the segments we are not inspecting.

   Until this is implemented, the memory test will only work when the
   MMU is disabled.

   Walking Bit Tests
   -----------------

   These tests don't work on the Xscale (NSLU2) and I suspect that
   even the memory_test_0() version is broken.

*/

#include <config.h>
#include <driver.h>
#include <service.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <error.h>
#include <command.h>
#include <spinner.h>
#include <apex.h>
#if defined (CONFIG_MMU)
# include <asm/cp15.h>
#endif

#include <drv-mem.h>
#include <mach/memory.h>
#include <memtest.h>

#include <debug_ll.h>

//#define TALK

#if defined (TALK)
# define PRINTF(f...) printf (f)
#else
# define PRINTF(f...) do {} while (0)
#endif

//#define CB_BLOCK	     (1024*1024)

/* memory_test

   is a data bus/address bus memory verification test for verifying
   that memory is OK before we copy APEX into SDRAM.

*/

int memory_test (unsigned long address, unsigned long c)
{
//  const unsigned long pattern_a = 0xaaaaaaaa;
//  const unsigned long pattern_b = 0x55555555;

  volatile unsigned long* p = (volatile unsigned long*) (address & ~3);
  unsigned long offset;
//  unsigned long mark;

  c /= 4;	/* Convert to count of words */
//  printf ("0: 0x%08lx 0x%08lx\n", address, c);

#if 0
		/* Walking data bit.  Each write must be to a
		   different address to avoid memory capacitance from
		   generating a falsely correct value. */
  /* *** FIXME: the problem here is we're not limiting these loops to
     *** the proscribed address range. */

  for (mark = 1; mark; mark <<= 1) {
//    printf ("m 0x%08lx 0x%08lx\n", (address + mark*4), mark);
    *(volatile unsigned long*) (address + mark*4) = mark;
  }
  PRINTF ("wd\n");

  for (mark = 1; mark; mark <<= 1) {
    if (*(volatile unsigned long*) (address + mark*4) != mark)
      return 1;
  }

		/* Walking address bits */
  PRINTF ("wa0\n");
  for (offset = 1; offset < c; offset <<= 1)
    p[offset] = pattern_a;

  p[0] = pattern_b;

  PRINTF ("wa1\n");
  for (offset = 1; offset < c; offset <<= 1)
    if (p[offset] != pattern_a)
      return 2;

  p[0] = pattern_a;

  PRINTF ("wa2\n");
  for (mark = 1; mark < c; mark <<= 1) {
    p[mark] = pattern_b;
    if (p[0] != pattern_a)
      return 3;

    for (offset = 1; offset < c; offset <<= 1)
      if (p[offset] != pattern_a && offset != mark)
	return 4;

    p[mark] = pattern_a;
  }
#endif

		/* Full memory test.  This is probably unnecessary
		   (and ineffective) given the fact that failures in
		   memory these days tend to be shorts or cold-solder
		   joints in manufacturing and not 'stuck' cells.  */
  PRINTF ("f0\n");
  for (offset = 0; offset < c; ++offset) {
    p[offset] = offset + 1;
    if ((offset & 0xfff) == 0)
      SPINNER_STEP;
  }

  PRINTF ("f1\n");
  for (offset = 0; offset < c; ++offset) {
    if (p[offset] != offset + 1)
      return offset*4;
    p[offset] = ~(offset + 1);
    if ((offset & 0xfff) == 0)
      SPINNER_STEP;
  }
  PRINTF ("f2\n");
  for (offset = 0; offset < c; ++offset) {
    if (p[offset] != ~(offset + 1))
      return offset*4;
    if ((offset & 0xfff) == 0)
      SPINNER_STEP;
  }

  return 0;
}


static int cmd_memtest (int argc, const char** argv)
{
  struct descriptor_d d;
  int result;

#if 0
  /* Parse arguments */
  while (argc > 1 && *argv[1] == '-') {
    switch (argv[1][1]) {
    case 'u':
      update = 1;
      break;
    default:
      return ERROR_PARAM;
      break;
    }
    --argc;
    ++argv;
  }
#endif

  if (argc != 2)
    return ERROR_PARAM;

  result = parse_descriptor (argv[1], &d);
  if (result) {
    printf ("Unable to open descriptor %s\n", argv[1]);
    return result;
  }

  if (strcmp (d.driver_name, "memory"))
    ERROR_RETURN (ERROR_UNSUPPORTED, "descriptor must refer to memory");

#if defined (CB_BLOCK)
  if (d.length < CB_BLOCK)
    ERROR_RETURN (ERROR_PARAM, "region too small");
#endif

  PRINTF ("before mmu disable\n");

#if defined (CONFIG_MMU)
  CLEANALL_DCACHE;
  DRAIN_WRITE_BUFFER;
  INVALIDATE_ICACHE;
  INVALIDATE_DCACHE;
  INVALIDATE_TLB;
  MMU_DISABLE;
  CP15_WAIT;
#endif

  PRINTF ("before test 0x%08lx 0x%08lx\n", d.start, d.length);

//  result = 0;
  result = memory_test (d.start, d.length);

#if defined (CONFIG_MMU)
  MMU_ENABLE;
  CP15_WAIT;
#endif

  if (result) {
    PRINTF ("test returned error 0x%x\n", result);
    ERROR_RETURN (ERROR_FAILURE, "memory test failed\n");
  }

  return 0;
}

static __command struct command_d c_memtest = {
  .command = "memtest",
  .description = "perform a memory test over the a memory region",
  .func = cmd_memtest,
  COMMAND_HELP(
"memtest REGION\n"
"  Perform memory test.\n"
"  This command performs a memory test over a region of RAM.  It\n"
"  first does the essential check for broken or swapped traces, then it\n"
"  does a complete memory test of the region.\n"
"  e.g.  memtest 4m+60m                  # Test 60MiB starting at 4MiB\n"
  )
};
