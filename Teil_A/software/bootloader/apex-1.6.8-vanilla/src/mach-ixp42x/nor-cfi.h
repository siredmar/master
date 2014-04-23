/* nor-cfi.h

   written by Marc Singer
   9 Mar 2005

   Copyright (C) 2005 Marc Singer

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

/* ----- Constants */

#define NOR_WIDTH	CONFIG_NOR_BUSWIDTH
#define NOR_0_PHYS	CONFIG_NOR_BANK0_START
#define NOR_0_LENGTH	CONFIG_NOR_BANK0_LENGTH

#define NOR_CHIP_MULTIPLIER	(1)	/* Number of chips at REGA */

#define NOR_BIGENDIAN		/* NOR flash is inherently big-endian */

#endif  /* __NOR_CFI_H__ */
