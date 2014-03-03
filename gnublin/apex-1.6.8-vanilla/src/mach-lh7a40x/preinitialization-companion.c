/* preinitialization-companion.c

   written by Marc Singer
   16 Oct 2006

   Copyright (C) 2006 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   The bad news is that the NAND flash IO code with SDRAM init code is
   too large to fit in 512 bytes , surprise, surprise.  A feature
   (bug) in the LH7A404 boot ROM means that we can only read 512 bytes
   from NAND flash into SRAM after reset.  Preinitialization executes
   before SDRAM initialization, so we have to copy APEX from NAND
   flash into SRAM which limits us to about 64KiB (IIRC).  Once APEX
   is copied to SRAM, we continue execution, initialize SDRAM and then
   call the bonafide relocation routine that copies the loader from
   NAND flash to SDRAM where we can get crazy.

*/

#include <config.h>
#include <apex.h>
#include <asm/bootstrap.h>
#include "mach/hardware.h"
#include "mach/drv-nand.h"

#include <debug_ll.h>


/* wait_on_busy

   wait for the flash device to become ready.  Using this function
   makes the code smaller.

*/

static void __naked __section (.rlocate.early.func) wait_on_busy (void)
{
  while (NAND_ISBUSY)
    ;
//  __asm volatile ("mov pc, lr");
  __asm volatile ("bx lr");
}

void __naked __section (.rlocate.early) relocate_early (void)
{
  unsigned long pc;

  __asm volatile ("mov %0, pc" : "=r" (pc));

	/* Also, only perform this special relocation when we're
	   running from the base of SRAM.  */
  if ((pc >> 12) != (CONFIG_NAND_BOOT_BASE>>12))
    __asm volatile ("b relocate_early_exit");

  PUTC ('N');

	/* Relocate one page of the loader from NAND to SRAM.  This
           routine is necessary because the CPU ROM will load only
           512B, one page, of flash into SRAM before starting
           execution.  We could load a lot more than 4KiB, but we want
           to always relocate directly from NAND into SDRAM so that
           the loader isn't size limited.  The 4KiB limit is a policy
           choice as there is no performance impact.  Also note that
           we don't write over the page we're executing. */
  {
    int cPages = 4096/CONFIG_NAND_BOOT_PAGE_SIZE - 1;
    void* pv = (void*) (CONFIG_NAND_BOOT_BASE + CONFIG_NAND_BOOT_PAGE_SIZE);
    int cAddr = NAM_DECODE (BOOT_PBC);
    int iPage;

    NAND_CS_ENABLE;

    for (iPage = 1; iPage < cPages; ++iPage) {
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
  }

  NAND_CS_DISABLE;

  PUTC ('n');

  __asm volatile ("b relocate_early_exit");
}

void __naked __section (.rlocate.early.exit) relocate_early_exit (void)
{
}
