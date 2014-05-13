/* nor-cfi.h
     $Id$

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

#if defined CONFIG_MACH_TROUNCER
# define NOR_WIDTH		(16)
# define NOR_CHIP_MULTIPLIER	(1)
#endif

#if defined CONFIG_MACH_KARMA
# define NOR_WIDTH		(16)
# define NOR_CHIP_MULTIPLIER	(1)
#endif

#if !defined (NOR_WIDTH)
# define NOR_WIDTH		(32)
#endif
#if !defined (NOR_0_PHYS)
# define NOR_0_PHYS		(0x00000000)
#endif
#if !defined (NOR_CHIP_MULTIPLIER)
# define NOR_CHIP_MULTIPLIER	(2)	/* Number of chips at REGA */
#endif

#if defined (CPLD_FLASH)
# define VPEN_ENABLE		(CPLD_FLASH |=  CPLD_FLASH_FL_VPEN)
# define VPEN_DISABLE		(CPLD_FLASH &= ~CPLD_FLASH_FL_VPEN)
#endif

#endif  /* __NOR_CFI_H__ */
