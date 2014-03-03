/* relocate-onenand.c

   written by Marc Singer
   7 May 2007

   Copyright (C) 2007 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   Implementation of APEX relocator that copies the loader from
   OneNAND flash into SDRAM.  This function will override the default
   version that is part of the architecture library.  Note that we
   *still* have to cope with the possibility that we're running from
   SDRAM and so we need to relocate within memory.  Thus, we play
   nice.

   o DataBuffer0/DataBuffer1.  In APEX, we don't really care about
     which data buffer we use to access the NAND flash.  This
     relocation code uses DataBuffer0 so that we can get more than 1K
     of the boot loader into contiguous memory before SDRAM is
     initialized.

   o Preinitialization.  In order to get a 3k boot loader image, load
     the second and third KiB from NAND flash into DataRam0.  See the
     comment on the preinitialization function for details.

*/

#include <config.h>
#include <apex.h>
#include <asm/bootstrap.h>
#include "mach/hardware.h"
#include "drv-onenand-base.h"
#include <linux/bitops.h>

#include <debug_ll.h>

#define PAGE_SIZE ONENAND_DATA_SIZE

void relocate_apex_exit (void);

static void __section (.rlocate.early.func) wait_on_busy (void)
{
  while (ONENAND_IS_BUSY)
    ;
}


/* relocate_early

   performs a crucial preload of the second and third KiB of APEX into
   DataRAM0.  This allows us to run with 3KiB of boot loader code
   until we get the SDRAM initialized and the whole loader copied to
   SDRAM.

   Note that we always perform this preinitialization because it
   cannot hurt and it is cheaper to just execute the load than deal
   with trying to figure out where APEX is executing.  We already can
   be sure that the OneNAND flash is available.

   Also beware attempting to output data to the console at this stage.
   The setup for the UART is lengthy and tends to exhaust our code
   space for the early initialization.  An alternative would be to
   initialize the UART after we perform the relocation, but then we
   wouldn't see much.

*/

void __naked __section (.rlocate.early) relocate_early (void)
{
  ONENAND_ADDRSETUP (0, 2);
  ONENAND_BUFFSETUP (0, 0, 2);
  ONENAND_INTR = 0;
  ONENAND_CMD = ONENAND_CMD_LOAD;

  wait_on_busy ();

  ONENAND_ADDRSETUP (1, 0);
  ONENAND_BUFFSETUP (0, 2, 2);
  ONENAND_INTR = 0;
  ONENAND_CMD = ONENAND_CMD_LOAD;

  wait_on_busy ();

  __asm volatile ("b relocate_early_exit\n\t");
}

void __naked __section (.rlocate.early.exit) relocate_early_exit (void)
{
}


/* relocate_apex_maybe_onenand

   relocates the loader from either OneNAND flash or from SDRAM to the
   linked VMA.  The determination of our boot method is based on the
   execution address.  For OneNAND flash boot, we assume that the
   system is executing from a known location in memory based on the
   architecture.  Really, we may need to allow the mach- to inject a
   piece of code to perform the check because it may require some
   other technique to determine the boot mode.

   Note that without the name relocate_apex(), this function requires
   inline initialization.

*/

void __naked __section (.rlocate)
  relocate_apex_maybe_onenand (unsigned long offset)
{
  unsigned long pc;		/* So we can detect the stage */

  PUTC ('>');

  __asm volatile ("mov %0, pc" : "=r" (pc));

  PUTHEX_LL (pc);
  PUTC_LL ('>');

  PUTC ('c');

	/* Jump to OneNAND loader only if we could be starting from NAND. */
  if ((pc >> 12) == (CONFIG_DRIVER_ONENAND_BASE>>12)) {
    PUTC ('N');
    /* Note that we don't care about offset because
       relocate_apex_onenand doesn't use it.  Instead, it copies the
       while loader from OneNAND to SDRAM and then jumps to it. */
    __asm volatile ("b relocate_apex_onenand\n");
  }

  /* *** FIXME: it might be good to allow this code to exist in a
     subroutine so that we can, optionally, use it here.  The linkage
     tricks preclude this at the moment, but hope is not lost. */

      /* Relocate from current copy in memory, probably SRAM. */
  else {
    unsigned long d = (unsigned long) &APEX_VMA_COPY_START;
    unsigned long s = (unsigned long) &APEX_VMA_COPY_START - offset;
#if defined USE_LDR_COPY
    unsigned long index
      = (&APEX_VMA_COPY_END - &APEX_VMA_COPY_START + 3 - 4) & ~3;
    unsigned long v;
    PUTC ('R');
    __asm volatile (
		    "0: ldr %3, [%0, %2]\n\t"
		       "str %3, [%1, %2]\n\t"
		       "subs %2, %2, #4\n\t"
		       "bpl 0b\n\t"
		       : "+r" (s),
			 "+r" (d),
			 "+r" (index),
			 "=&r" (v)
		       :: "cc"
		    );
#elif defined (USE_SLOW_COPY)
    PUTC ('R');
  __asm volatile (
	       "0: ldmia %0!, {r3}\n\t"
		  "stmia %1!, {r3}\n\t"		  "cmp %1, %2\n\t"
		  "bls 0b\n\t"
		  : "+r" (s),
		    "+r" (d)
		  :  "r" (&APEX_VMA_COPY_END)
		  : "r3", "cc"
		  );
#else
    PUTC ('R');
  __asm volatile (
	       "0: ldmia %0!, {r3-r10}\n\t"
		  "stmia %1!, {r3-r10}\n\t"
		  "cmp %1, %2\n\t"
		  "bls 0b\n\t"
		  : "+r" (s),
		    "+r" (d)
		  :  "r" (&APEX_VMA_COPY_END)
		  : "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "cc"
		  );
#endif
  }

				/* Return to SDRAM */
  PUTC ('@');			/* Let 'em know we're jumping */
//  __asm volatile ("mov pc, %0" :: "r" (&relocate_apex_exit));
  __asm volatile ("bx %0" :: "r" (&relocate_apex_exit));
}


/* relocate_apex_onenand

   performs the relocation from OneNAND into SDRAM.  The LMA is
   determined at runtime.  The relocator will put the loader at the
   VMA and then return to the relocated address.

   We're register constrained when the DEBUG_LL configuration option
   is enabled.  We'd like to use eight registers for the copy
   function, but four will have to suffice.  The number must be a
   power of 2 so that we copy exactly the size of the OneNAND page.

*/

void __naked __section (.rlocate.func) relocate_apex_onenand (void)
{
  int page_size = PAGE_SIZE;
  int cPages = (&APEX_VMA_COPY_END - &APEX_VMA_COPY_START
		+ page_size - 1);
  int page = 0;
  void* pv = &APEX_VMA_ENTRY;

  PUTC ('>');

  {
    int v;
	/* Divide by the page size without resorting to a function call */
    for (v = page_size >> 1; v; v = v>>1)
      cPages >>= 1;
  }

  PUTHEX_LL (page_size);
  PUTC_LL ('|');
  PUTHEX_LL (cPages);
  PUTC_LL ('|');
  PUTHEX_LL (pv);
  PUTC_LL ('|');
  for (; page < cPages; ++page) {
      /* Use this to see how many blocks we're copying from flash */
//    PUTC ('A' + (page&0xf));
    PUTC_LL ('A' + (page&0xf));

    ONENAND_PAGESETUP (page);
    ONENAND_BUFFSETUP (1, 0, 4);
    ONENAND_INTR = 0;
    ONENAND_CMD = ONENAND_CMD_LOAD;

    /* We don't use the function wait_on_busy() because we cannot
       honor register usage.  So, we let that be the problem of the
       compiler. */
    while (ONENAND_IS_BUSY)
      ;

    __asm volatile (
		 "0: ldmia %1!, {r3-r6}\n\t"
		    "stmia %0!, {r3-r6}\n\t"
		    "cmp %0, %2\n\t"
		    "blo 0b\n\t"
		 : "+r" (pv)
		 :  "r" (ONENAND_DATARAM1),
		    "r" (pv + page_size)
		 : "r3", "r4", "r5", "r6",
//		   "r7", "r8", "r9", "r10",
		   "cc"
		 );

    /* Note that we don't need to increment pv as it is incremented by
       the stmia instruction.  */
  }

  PUTC_LL('!');

//  __asm volatile ("mov pc, %0" :: "r" (&relocate_apex_exit));
  __asm volatile ("bx %0" :: "r" (&relocate_apex_exit));
}

void __naked __section (.rlocate.exit) relocate_apex_exit (void)
{
}
