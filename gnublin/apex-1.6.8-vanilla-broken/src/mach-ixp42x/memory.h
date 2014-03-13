/* memory.h

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
#include <mach/sdram.h>

/* ----- Constants */

#define SDRAM_START_PHYS	(SDRAM_BANK0_PHYS)
#define SDRAM_END_PHYS		(SDRAM_BANK0_PHYS + SDRAM_BANK_SIZE)

#if defined (CONFIG_SDRAM_BANK0)
# define RAM_BANK0_START	SDRAM_BANK0_PHYS
# define RAM_BANK0_LENGTH	CONFIG_SDRAM_BANK_LENGTH
#endif

#if defined (CONFIG_SDRAM_BANK1)
# define RAM_BANK1_START	(RAM_BANK0_START + RAM_BANK0_LENGTH)
# define RAM_BANK1_LENGTH	CONFIG_SDRAM_BANK_LENGTH
#endif

	/* MMU macro for assigning segment protection bits. */
#define PROTECTION_FOR(p) \
({ int v = 0;   /* uncacheable, unbuffered */ \
   if (   (p) >= 0x0 \
       && (p) <  2*CONFIG_SDRAM_BANK_LENGTH)\
     v = 3<<2;/* cacheable, buffered */ \
   v; })

#endif  /* __MEMORY_H__ */
