/* arch.c

   written by Marc Singer
   5 Dec 2004

   Copyright (C) 2004 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   Architecture support

*/

#include <config.h>
#include "hardware.h"
#include <linux/kernel.h>
#include "alias.h"

/* determine_arch_number

   queries the CPU to determine which architecture ID to give to the
   kernel.  As the LPD7A40X boards are similar enough for the
   bootloader, there can be one bootloader binary for both.

*/

void determine_arch_number (void)
{
  int arch_id = 0;
  char sz[10];

  switch (((CSC_PWRSR >> CSC_PWRSR_CHIPID_SHIFT)
	   & CSC_PWRSR_CHIPID_MASK)
	  & 0xf0) {
  default:
  case 0x00:
    arch_id = 389;              /* LPD7A400 */
  case 0x20:
    arch_id = 390;              /* LPD7A404 */
  }

  snprintf (sz, 10, "%d", arch_id);
  alias_set ("arch-number", sz);
}
