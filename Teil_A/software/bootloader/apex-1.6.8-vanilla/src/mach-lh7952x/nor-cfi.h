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
#include <mach/hardware.h>

/* ----- Constants */

#define NOR_WIDTH	CONFIG_NOR_BUSWIDTH
#define NOR_0_PHYS	CONFIG_NOR_BANK0_START
#define NOR_0_LENGTH	CONFIG_NOR_BANK0_LENGTH

#if defined (CONFIG_NOR_BANK1_START)
# define NOR_1_PHYS	CONFIG_NOR_BANK1_START
# define NOR_1_LENGTH	CONFIG_NOR_BANK1_LENGTH
#endif

#define NOR_CHIP_MULTIPLIER	(1)	/* Number of chips at REGA */

#if defined (CPLD_FLASH)
# define VPEN_ENABLE		(CPLD_FLASH |=  CPLD_FLASH_FL_VPEN)
# define VPEN_DISABLE		(CPLD_FLASH &= ~CPLD_FLASH_FL_VPEN)
#endif

#endif  /* __NOR_CFI_H__ */
