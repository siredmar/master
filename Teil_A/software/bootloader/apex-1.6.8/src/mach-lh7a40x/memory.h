/* memory.h
     $Id$

   written by Marc Singer
   22 Feb 2005

   Copyright (C) 2005 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#if !defined (__MEMORY_H__)
#    define   __MEMORY_H__

/* ----- Includes */

#include <config.h>
#include <mach/hardware.h>

/* ----- Constants */

#if defined (CONFIG_SDRAM_BANK0)
# define RAM_BANK0_START	SDRAM_BANK0_PHYS
# define RAM_BANK0_LENGTH	0x10000000
#endif

#if defined (CONFIG_SDRAM_BANK1)
# define RAM_BANK1_START	SDRAM_BANK1_PHYS
# define RAM_BANK1_LENGTH	0x10000000
#endif

#if defined (CONFIG_SDRAM_BANK2)
# define RAM_BANK2_START	SDRAM_BANK2_PHYS
# define RAM_BANK2_LENGTH	0x10000000
#endif

	/* MMU macro for assigning segment protection bits. */
#define PROTECTION_FOR(p) \
({ int v = 0;   /* uncacheable, unbuffered */ \
   if (   (p) >= SDRAM_BANK0_PHYS \
       && (p) <  SDRAM_BANK2_PHYS + 0x10000000)\
     v = 3<<2;/* cacheable, buffered */ \
   v; })

#endif  /* __MEMORY_H__ */
