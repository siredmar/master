/* drv-ata.h

   written by Marc Singer
   21 Sep 2008

   Copyright (C) 2008 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#if !defined (__MACH_DRV_ATA_H__)
#    define   __MACH_DRV_ATA_H__

/* ----- Includes */

#include <mach/hardware.h>

/* ----- Types */

/* ----- Globals */

/* ----- Prototypes */

#define ATA_PHYS(i)		(ATAX_PHYS (i))

#define ATA_WIDTH		(32)
#define ATA_ADDR_MULT		(2)
#define ATA_DEVICE_MAX		(2)

//#define ATA_REG			(1<<12)	/* REG line for register access */
//#define ATA_ALT			(1<<10)	/* A10 line for data read */
//#define ATA_ATTRIB		(1<<9) /* A9 line for attribute access */

#endif  /* __MACH_DRV_ATA_H__ */
