/* initialize-sdram.c

   written by Marc Singer
   17 Feb 2007

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   Copyright (C) 2007 Marc Singer

   -----------
   DESCRIPTION
   -----------

   This is a special command/function to initialize SDRAM after the
   system has been started.  While it would be nice to be able to
   perform this magic before the system were running commands on the
   command line, there isn't really a good way to do this.  From the
   XScale Core Developer's Manual:

    There are several requirements for locking down code:
      7.[sic] the routine used to lock lines down in the cache must be
	placed in non-cacheable memory, which means the MMU is
	enabled. As a corollary: no fetches of cacheable code should
	occur while locking instructions into the cache.the code being
	locked into the cache must be cacheable.

      8.[sic] the instruction cache must be enabled and invalidated
	prior to locking down lines

    Failure to follow these requirements will produce unpredictable
    results when accessing the instruction cache.

    System programmers should ensure that the code to lock
    instructions into the cache does not reside closer than 128 bytes
    to a non-cacheable/cacheable page boundary. If the processor
    fetches ahead into a cacheable page, then the first requirement
    noted above could be violated.

  So, the plan is to copy APEX to a new region of SDRAM, 0x0, make
  this memory uncached, then jump to it.  The routine the locks the
  original copy of itself into both caches and returns to that version
  of itself.  At this point, we can reinitialize the SDRAM controller
  at will as long as we don't leave the function we're executing and
  we don't attempt to use the stack.

  o Determining memory chip size and banks.  We do a little dance in
    order to deduce the correct memory chip size.  We start by
    initializing memory for the largest chip size and a single bank
    which gives us the broadest addressing using a single chip select.
    We sum addressable space in the regions found and we compare this
    the expected sizes for the RAM chips.  Then, we initialize with
    the correct memory size at two banks and decide if, in fact, there
    are two banks.  The result appears to be a reliable heuristic for
    initializing RAM regardless of the chip size..

*/

#include <config.h>
#include <asm/bootstrap.h>
#include <apex.h>
#include <command.h>
#include <service.h>
#include "hardware.h"
#include <linux/kernel.h>
#include <asm/mmu.h>
#include <asm/cp15.h>
#include "sdram.h"
#include <drv-mem.h>
#include <linux/string.h>

#include <debug_ll.h>

static void __section (.bootstrap) do_sdram_initialization (int mode)
{
  unsigned long s = (unsigned long) &APEX_VMA_START;
  unsigned long d = (unsigned long) 0;
  const unsigned long diff = d - s;

//  PUTC ('A');

  CLEANALL_DCACHE;
  DRAIN_WRITE_BUFFER;

  mmu_protsegment ((void*) d, 0, 0);			  /* Disable caching */
  INVALIDATE_DTLB_VA (0);
  INVALIDATE_ITLB_VA (0);

//  PUTC ('B');

		/* Copy APEX to uncacheable memory  */
  __asm volatile (
	       "0: ldmia %0!, {r3-r10}\n\t"
		  "stmia %1!, {r3-r10}\n\t"
		  "cmp %0, %2\n\t"
		  "bls 0b\n\t"
	       : "+r" (s),
		 "+r" (d)
	       :  "r" (&APEX_VMA_COPY_END)
	       : "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "cc"
	       );
  CLEANALL_DCACHE;			/* Force copy to SDRAM */

  /* *** Jump to 0x0 copy of APEX  */
  __asm volatile ("add pc, pc, %0\n\t" :: "r" (diff - 4));
//  PUTC ('C');

#if 1
  /* *** Perform lockdown on this function.  Also should read it into
     *** dcache.  */
  {
    unsigned long p = (unsigned long) (&APEX_VMA_START) & ~0x1f;
    unsigned long c = 4096/32;

    INVALIDATE_ICACHE;
    ENABLE_DCACHE_LOCK;
    CP15_WAIT;

    for (; c--; p += 32) {
      LOCK_ICACHE_VA (p);
      CLEAN_DCACHE_VA (p);
      INVALIDATE_DCACHE_VA (p);
      *(volatile unsigned long*) p;
    }

    DISABLE_DCACHE_LOCK;
    CP15_WAIT;
  }

#endif
  /* *** Jump back to original copy of APEX; we're running from cache. */

   __asm volatile ("sub pc, pc, %0\n\t" :: "r" (diff + 4));
//  PUTC ('X');
//  PUTC ('Y');

  /* *** Reinitialize SDRAM. */

  SDR_CONFIG = SDR_CONFIG_RAS3 | SDR_CONFIG_CAS3 | mode;
//  PUTC ('a');
  SDR_REFRESH = 0;		/* Disable refresh */
  SDR_IR = SDR_IR_NOP;
  usleep (200);			/* datasheet: maintain 200us of NOP */

//  PUTC ('b');
  /* 133MHz timer cycle count, 0x81->969ns == ~1us */
  /* datasheet: not clear what the refresh requirement is.  */
  SDR_REFRESH = 0x81;		/* *** FIXME: calc this */

//  PUTC ('c');
  SDR_IR = SDR_IR_PRECHARGE_ALL;

  usleep (1);			/* datasheet: wait for Trp (<=20ns)
				   before starting AUTO REFRESH */

  /* datasheet: needs at least 8 refresh (bus) cycles.  Timing diagram
     shows only two AUTO_REFRESH commands, each is Trc (20ns) long. */

//  PUTC ('d');
  SDR_IR = SDR_IR_AUTO_REFRESH;
  usleep (1);
  SDR_IR = SDR_IR_AUTO_REFRESH;
  usleep (1);

//  PUTC ('e');
  SDR_IR = SDR_IR_MODE_CAS3;
  SDR_IR = SDR_IR_NORMAL;
  usleep (1);			/* Must wait for 3 CPU cycles before
				   SDRAM access */

//  PUTC ('E');

  /* *** Reenable caching at 0x0 */
  d = 0;
  mmu_protsegment ((void*) d, 1, 1);			  /* Enable caching */
  INVALIDATE_ITLB_VA (0);
  INVALIDATE_DTLB_VA (0);

  /* *** Unlock caches */
  UNLOCK_CACHE;

  /* *** Return. */
//  PUTC ('\r');
//  PUTC ('\n');
}

#define MODE(size,banks) (((ffs ((size)/128) - 1) << 1)\
			  | ((banks) == 2 ? 1 : 0))

static int cmd_initialize_sdram (int argc, const char** argv)
{
  int size = 0;
  int banks =
#if defined (CONFIG_SDRAM_BANK1)
    2
#else
    1
#endif
    ;
  int mode = -1;

//  PUTC ('A');

	/* This code only supports 128Mib,256MiB, and 512MiB. */
  if (argc > 1) {
    size = simple_strtoul (argv[1], NULL, 10);
    if (size >= 512)
      size = 512;
    else if (size >= 256)
      size = 256;
    else
      size = 128;
  }
  if (argc > 2) {
    banks = simple_strtoul (argv[2], NULL, 10);
    if (banks >= 2)
      banks = 2;
    else
      banks = 1;
  }

  if (size == 0) {
    struct mem_region regions[16];
    int c;
    int cb = 0;			/* Total memory size */
    memset (regions, 0, sizeof (regions));
    do_sdram_initialization (MODE (512, 1));
    c = memory_scan (regions, sizeof (regions)/sizeof (*regions),
		     0, 256*1024*1024);
    {
      int i;
      for (i = c; i--; )
	cb += regions[i].length;
    }

    if (cb >= 128*1024*1024)
      size = 512;
    else if (cb >= 64*1024*1024)
      size = 256;
    else
      size = 128;
    do_sdram_initialization (MODE (size, 2));
    memset (regions, 0, sizeof (regions));
    c = memory_scan (regions, sizeof (regions)/sizeof (*regions),
		     0, 256*1024*1024);
//    printf ("size %d\n", regions[0].length);
    if (regions[0].length >= 2*size*1024*1024*2/8)
      banks = 2;
    else
      banks = 1;
//    printf ("banks %d\n", regions[0].length);
  }

  mode = MODE (size, banks);
//  printf ("(%d,%d) -> %d\n", size, banks, mode);
  do_sdram_initialization (mode);

  printf (" %d bank%s of 2 %dMib chips\n",
	  banks, banks > 1 ? "s" : "", size);

  return 0;
}

static __command struct command_d c_initialize_sdram = {
  .command = "sdram-init",
  .func = cmd_initialize_sdram,
  COMMAND_DESCRIPTION ("reinitialize the SDRAM controller")
  COMMAND_HELP(
"sdram-init [SIZE [BANKS]]\n"
"  reinitialize SDRAM controller using cache to hold the \n"
"  program code while SDRAM is off-line.  The optional SIZE argument\n"
"  sets the bank size.  The optional BANKS argument can be either\n"
"  1 or 2 for the number of banks of memory chips.  The original\n"
"  NSLU2 system memory configuration has 1 bank of 128Mib chips.\n"
  )
};
