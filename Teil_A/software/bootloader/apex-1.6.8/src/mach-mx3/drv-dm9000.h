/* drv-dm9000.h

   written by Marc Singer
   6 April 2006

   Copyright (C) 2006 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#if !defined (__MACH_DRV_DM9000_H__)
#    define   __MACH_DRV_DM9000_H__

/* ----- Includes */

/* ----- Types */

/* ----- Globals */

/* ----- Prototypes */

#define DM_WIDTH	16

#define DM_PHYS		(0xb4000000) /* Dev MAC/PHY on CS4 */
#define DM_WEIM_CS	(4)          /* CS4 controls the DM9000 */
#define DM_PHYS_INDEX	(DM_PHYS + 0) /* Index register */

#if defined (CONFIG_MACH_EXBIBLIO_ROSENCRANTZ)
	/* There are two versions of the NFF board, one has a shifted
           address bus on the OneNAND and Ethernet controller and one
           does not.  The correct data offset is +4.  The older,
           broken, and shifted version uses +8.

           The schematic will show that A2 drives the COMMAND pin of
           the DM9000.  Based on the way that the MX31 handles the
           address lines for 16 bit memory devices, this would
           normally be address +8.  However, an error in the schematic
           incorrectly drove the OneNAND flash address lines.  Our
           solution was to shift all of the address lines up so that
           A1 from the processor drove A2 on the OneNAND flash.  This
           also meant that the DM9000 addressing changed such that +4
           would drive COMMAND. */
# define DM_PHYS_DATA	(DM_PHYS + 4) /* Data register */
#endif

#if defined (CONFIG_MACH_KARMA5)
	/* A2 out of the processor drives address bit 3 (+8) for 16
           bit devices.  This should be the default for all
           designs. */
# define DM_PHYS_DATA	(DM_PHYS + 8) /* Data register */
#endif


//#define DM_NAME		"dev"

#endif  /* __MACH_DRV_DM9000_H__ */
