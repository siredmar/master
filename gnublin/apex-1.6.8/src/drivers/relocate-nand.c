/* relocate-nand.c

   written by Marc Singer
   9 Nov 2004

   Copyright (C) 2004 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   Implementation of APEX relocator that copies the loader from NAND
   flash into SDRAM.  This function will override the default version
   that is part of the architecture library.

*/

#include <config.h>
#include <apex.h>
#include <asm/bootstrap.h>
#include "mach/hardware.h"

#include <drv-nand-base.h>
#include "mach/drv-nand.h"

#include <debug_ll.h>

//#define EMERGENCY
#define USE_NAND
//#define USE_SLOW_COPY


/* wait_on_busy

   wait for the flash device to become ready.  Using this function
   makes the code smaller.

*/

static void __naked __section (.rlocate.func) wait_on_busy (void)
{
  while (NAND_ISBUSY)
    ;
//  __asm volatile ("mov pc, lr");
  __asm volatile ("bx lr");
}

void relocate_apex_exit (void);

/* relocate_apex

   performs a copy of the loader, but it first checks to see if we are
   running from RAM already.  The system hardware (LH7) copies 4K of
   the loader from NAND flash to SRAM at CONFIG_NAND_BOOT_BASE.  If we
   are executing from there, we assume that we need to perform the
   NAND flash relocation.  Otherwise, we perform a straightforward
   copy from our present location to SDRAM because we've been loaded
   via JTAG.

*/

void __naked __section (.rlocate) relocate_apex (unsigned long offset)
{
//  unsigned long lr;
  unsigned long pc;		/* So we can detect the second stage */
//  extern unsigned long reloc_nand;
//  unsigned long offset = (unsigned long) &reloc_nand;

  PUTC ('>');

	/* Setup bootstrap stack, trivially.  We do this so that we
	   can perform some complex operations here in the bootstrap,
	   The C setup will move the stack into SDRAM just after this
	   routine returns. */

  /* This stack does exist when you are using the COMPANION_EVT1 boot,
     but I don't think we need it here. */
//  __asm volatile ("mov sp, %0" :: "r" (&APEX_VMA_BOOTSTRAP_STACK_START));

  __asm volatile ("mov %0, pc" : "=r" (pc));
  PUTHEX_LL (pc);
  PUTC ('>');

  PUTC ('c');

  if (0) {
    /* Dummy test so that the next and final clauses can be elses */
  }

#if defined (USE_NAND)

	/* Jump to NAND loader only if we could be starting from
           NAND. To be most clear, we will only relocate if we're
           executing from the first 4K of the memory described by
           CONFIG_NAND_BOOT_BASE.  This is a reasonable assumption
           given that this relocation is the first thing we do after
           setup of RAM.  If that takes more than 4K, we've got an
           extrordinary situation. */
  else if ((pc >> 12) == (CONFIG_NAND_BOOT_BASE>>12)) {
    PUTC ('N');
    /* Note that we don't care about offset since relocate_apex_nand
       copies all of the loader from NAND flash into memory and then
       jumps to it.  */
    __asm volatile ("b relocate_apex_nand");
  }

#endif

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
//  __asm volatile ("mov pc, %0" : : "r" (&relocate_apex_exit));
  __asm volatile ("bx %0" : : "r" (&relocate_apex_exit));
}

void __naked __section (.rlocate.exit) relocate_apex_exit (void)
{
}


/* relocate_apex_nand

   performs a memory move of the whole loader, presumed to be from
   NAND flash into SDRAM.  The LMA is determined at runtime.  The
   relocator will put the loader at the VMA and then return to the
   relocated address.

   *** FIXME: we can read eight bytes at a time to make the transfer
   *** more efficient.  Probably, this isn't a big deal, but it would
   *** be handy.

*/

void __naked __section (.rlocate.func) relocate_apex_nand (void)
{
  int cPages = (&APEX_VMA_COPY_END
                - &APEX_VMA_COPY_START
                + CONFIG_NAND_BOOT_PAGE_SIZE - 1)
    /CONFIG_NAND_BOOT_PAGE_SIZE;
  void* pv = &APEX_VMA_ENTRY;
  int cAddr = NAM_DECODE (BOOT_PBC);
  int iPage;

  PUTC ('>');

  PUTC_LL ('0' + cAddr);

  NAND_CS_ENABLE;

  for (iPage = 0; iPage < cPages; ++iPage) {
    NAND_CLE = NAND_Reset;
    wait_on_busy ();

    NAND_CLE = NAND_Read1;
    NAND_ALE = 0;
    {
      int page = iPage;
      int i;
      for (i = cAddr - 1; i--; ) {
        NAND_ALE = page & 0xff;
        page >>= 8;
      }
    }
    wait_on_busy ();

    NAND_CLE = NAND_Read1;

    {
      int cb;
      for (cb = CONFIG_NAND_BOOT_PAGE_SIZE; cb--; )
        *((char*) pv++) = NAND_DATA;
    }
  }

  NAND_CS_DISABLE;

#if 0
  NAND_CLE = NAND_Reset;
  wait_on_busy ();

  NAND_CLE = NAND_Read1;
  while (cAddr--)
    NAND_ALE = 0;
  wait_on_busy ();

  while (cPages--) {
    int cb;

    NAND_CLE = NAND_Read1;
    for (cb = CONFIG_NAND_BOOT_PAGE_SIZE; cb--; )
      *((char*) pv++) = NAND_DATA;
    for (cb = 16; cb--; )
      NAND_DATA;
    wait_on_busy ();
  }
#endif

//  __asm volatile ("mov pc, %0" : : "r" (&relocate_apex_exit));
  __asm volatile ("bx %0" : : "r" (&relocate_apex_exit));
}
