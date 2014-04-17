/* relocate-companion.c

   written by Marc Singer
   26 May 2006

   Copyright (C) 2006 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   Implementation of APEX relocator that copies the loader from I2C
   EEPROM or SD/MMC depending on our needs.  This function will
   override the default version that is part of the architecture
   library.  This implementation was used on the EVT1 hardware.  The
   EVT2 hardware included a NAND flash part in order to avoid this
   mess.

   Bootstrap Loader Selection
   --------------------------

   There are three ways in which this APEX binary may be loaded.  We
   attempt to make sense of each of them.

   Production always depends on the I2C EEPROM to get the system on
   it's feet.  The CPU ROM copies the first 4K from EEPROM to
   0xb0000000 and jumps to the first instruction.  At that point, we
   may want to either read the whole loader from EEPROM and continue
   execution at the same instruction in SDRAM, or we may want to read
   the loader from an SD/MMC card.  The third way we may want to run
   APEX is to copy it into SRAM (via JTAG) and run that version
   without accessing the EEPROM or SD/MMC.

   Distinguishing between the first two cases may be possible using a
   unique FAT partition table layout that signals to the loader that a
   better version of APEX resides there.

   The third case is a little trickier since we don't have a good way
   to know if we only have 4K of the loader, copied from EEPROM, or a
   whole copy of the loader in SRAM.  Unless we implement some sort of
   checksum operation, we'll probably not be able to do this.  Until
   that time, we'll probably depend on a second entry point to signal
   that the loader is already all present and a simple copy from where
   it is desired.

   Relocating APEX from SD/MMC
   ---------------------------

   Relocation of APEX from SD/MMC has several elements.  The first
   4KiB of the loader are copied from I2C memory to address 0xb0000000
   by the CPU Boot ROM.  The relocate_apex () implementation here is
   guaranteed to be able to execute from that section of the program.

   The relocator reads the partition table of the SD card.  If it
   finds that the first partition is type 0, it reads 80K - 12K (4K
   for the code from I2C, 4K for stack, and 4K for data), from that
   partition to address 0xb0000000+4k and jumps to it.  This
   guarantees that the loader, as read from SD/MMC, is able fully
   initialize the system including SDRAM as needed.

   If a larger boot loader is needed, the temporary stack may be
   relocated to SDRAM, allowing for another 4K of boot loader code.

   Loading APEX to SRAM
   --------------------

   The best method is to load the copy of APEX from SD/MMC into SRAM
   at 0xb0000000+4k, but there is some question as to whether or not
   this will work.  An initial test of the loader showed that it does
   work, but just after this successful test and after writing to the
   EEPROM, the board is no longer able to boot and nor will it respond
   to JTAG.

*/

#include <config.h>
#include <apex.h>
#include <asm/bootstrap.h>
#include <mmc.h>
#include "hardware.h"

#include <debug_ll.h>

#define USE_MMC
//#define USE_I2C

//#define EMERGENCY

#if defined (CONFIG_DEBUG_LL)
//# define USE_LDR_COPY		/* Simpler copy loop, more free registers */
# define USE_SLOW_COPY		/* Simpler copy loop, more free registers */
# define USE_COPY_VERIFY
#endif

#define MMC_BOOTLOADER_SIZE		(1024*(80\
		/* 4k from EEPROM */	  - 4\
		/* 4k stack */		  - 4\
		/* 4k bootstrap data */	  - 4))
//#define MMC_BOOTLOADER_SIZE		(64*1024)
#define MMC_BOOTLOADER_LOAD_ADDR	(0xb0000000 + 4*1024)
//#define MMC_BOOTLOADER_LOAD_ADDR	(0xc0000000)

void relocate_apex (unsigned long);
int relocate_apex_mmc (void);
void relocate_apex_exit (void);


/* relocate_apex

   performs a copy of the loader
   performs a memory move of the whole loader, presumed to be from NOR
   flash into SDRAM.  The LMA is determined at runtime.  The relocator
   will put the loader at the VMA and then return to the relocated address.

   *** FIXME: we can read eight bytes at a time to make the transfer
   *** more efficient.  Probably, this isn't a big deal, but it would
   *** be handy.

*/

void __naked __section (.rlocate) relocate_apex (unsigned long offset)
{
  unsigned long pc;		/* So we can detect the second stage */

  PUTC ('>');

  __asm volatile ("mov %0, pc" : "=r" (pc));

	/* Setup bootstrap stack, trivially.  We do this so that we
	   can perform some complex operations here in the bootstrap,
	   e.g. copying from SD/MMC.  The C setup will move the stack
	   into SDRAM just after this routine returns. */

  __asm volatile ("mov sp, %0" :: "r" (&APEX_VMA_BOOTSTRAP_STACK_START));

  PUTC_LL ('c');

  if (0) {
    /* Dummy test so that the final clause can be an else */
  }

#if defined (USE_MMC)

	/* Read loader from SD/MMC only if we could be starting from I2C. */
  else if ((pc >> 12) == (0xb0000000>>12) && relocate_apex_mmc ()) {
    PUTC ('@');			/* Let 'em know we're jumping */
//    __asm volatile ("mov pc, %0" :: "r" (MMC_BOOTLOADER_LOAD_ADDR));
    __asm volatile ("bx %0" :: "r" (MMC_BOOTLOADER_LOAD_ADDR));
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
		  "stmia %1!, {r3}\n\t"
		  "cmp %1, %2\n\t"
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
  /* *** FIXME: tidy up the exit code; check for code density */

				/* Return to SDRAM */
  PUTC ('@');			/* Let 'em know we're jumping */
//  __asm volatile ("mov pc, %0" : : "r" (&relocate_apex_exit));
  __asm volatile ("bx %0" : : "r" (&relocate_apex_exit));
}

int __section (.rlocate.func) relocate_apex_mmc (void)
{
  struct descriptor_d d;
  size_t cb;
  unsigned char rgb[16];	/* Partition table entry */

  PUTC ('M');			/* Feedback */

  mmc_init ();
  PUTC_LL ('i');

  if (!mmc_card_acquired ()) {
    PUTC ('0');
    return 0;
  }

  PUTC_LL ('a');

  d.start = 512 - 2 - 4*16;
  d.length = 16;
  d.index = 0;
  cb = mmc_read (&d, (void*) rgb, d.length);
  if (cb != d.length) {
    PUTC ('1');
    return 0;
  }

  if (rgb[4] != 0) {		/* Must be type 0 */
    PUTC ('2');
    return 0;
  }

  d.start = *((unsigned long*) &rgb[8])*512;	/* Start of first partition */
  d.length = MMC_BOOTLOADER_SIZE;
  d.index = 0;

  PUTHEX_LL (d.start);
  PUTC_LL ('p');
  PUTC ('r');
  cb = mmc_read (&d, (void*) MMC_BOOTLOADER_LOAD_ADDR, d.length);

  PUTHEX_LL (cb);
  PUTC_LL ('\r');
  PUTC_LL ('\n');

  PUTC ((cb == d.length) ? '+' : '-');

  return (cb == d.length) ? 1 : 0;
}


void __naked  __section (.rlocate.exit) relocate_apex_exit (void)
{
//  __asm volatile ("nop");
}
