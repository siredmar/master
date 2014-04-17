/* memory.h
     $Id$

   written by Marc Singer
   22 Feb 2005

   with modifications by David Anders
   06 Nov 2005

   Copyright (C) 2005 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

    memory definitions

*/

#if !defined (__MEMORY_H__)
#    define   __MEMORY_H__

/* ----- Includes */

#include <config.h>
#include <mach/hardware.h>

/* ----- Constants */

#if defined (CONFIG_SDRAM_BANK0)
# define RAM_BANK0_START	SDRAM_BANK0_PHYS
# define RAM_BANK0_LENGTH	0x4000000
#endif


#endif				/* __MEMORY_H__ */
