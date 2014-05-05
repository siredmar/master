/* nor-cfi.h

   written by Marc Singer
   26 Feb 2007

   Copyright (C) 2007 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#if !defined (__NOR_CFI_H__)
#    define   __NOR_CFI_H__

/* ----- Includes */

#include <config.h>
#include <mach/hardware.h>

/* ----- Constants */

/* These must be defined in the configuration for the board. */
//#undef CONFIG_DRIVER_NOR_CFI_TYPE_INTEL
//#undef CONFIG_DRIVER_NOR_CFI_TYPE_SPANSION
//#define CONFIG_DRIVER_NOR_CFI_TYPE_SPANSION

#if !defined (NOR_WIDTH)
# define NOR_WIDTH		(16)
#endif
#if !defined (NOR_0_PHYS)
# define NOR_0_PHYS		(0xa0000000)
#endif

#if !defined (NOR_CHIP_MULTIPLIER)
# define NOR_CHIP_MULTIPLIER	(1)	/* Number of chips at REGA */
#endif

#endif  /* __NOR_CFI_H__ */
