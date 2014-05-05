/* memory.h

   written by Marc Singer
   14 Sep 2008

   Copyright (C) 2007 Marc Singer

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

/* ----- Types */

/* ----- Globals */

/* ----- Prototypes */

#define SDRAM_START_PHYS	(SDRAM_BANK0_PHYS)
#define SDRAM_END_PHYS		(SDRAM_BANK1_PHYS + SDRAM_BANK_SIZE)

#if defined (CONFIG_SDRAM_BANK0)
# define RAM_BANK0_START	SDRAM_BANK0_PHYS
# define RAM_BANK0_LENGTH	SDRAM_BANK_SIZE
#endif

#if defined (CONFIG_SDRAM_BANK1)
# define RAM_BANK1_START	SDRAM_BANK1_PHYS
# define RAM_BANK1_LENGTH	SDRAM_BANK_SIZE
#endif

	/* MMU macro for assigning segment protection bits. */
#define PROTECTION_FOR(p) \
({ int v = 0;   /* uncacheable, unbuffered */ \
   if (   (p) >= SDRAM_BANK0_PHYS \
       && (p) <  SDRAM_BANK1_PHYS + RAM_BANK0_LENGTH)\
     v = 3<<2;/* cacheable, buffered */ \
   v; })

#endif  /* __MEMORY_H__ */
