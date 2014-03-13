/* memory.h
   
   written by Durgesh Pattamatta
   10 Oct 2008

   Copyright (C) 2008 NXP Semiconductors

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
# define RAM_BANK0_START	EXT_SDRAM_PHYS
# define RAM_BANK0_LENGTH	0x10000000

#endif  /* __MEMORY_H__ */
