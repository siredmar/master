/* drv-cf.h
     $Id$

   written by Marc Singer
   7 Feb 2005

   Copyright (C) 2005 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#if !defined (__MACH_DRV_CF_H__)
#    define   __MACH_DRV_CF_H__

/* ----- Includes */

#include "mach/iobarrier.h"

/* ----- Types */

/* ----- Globals */

/* ----- Prototypes */

# if !defined (CF_PHYS)
#  define CF_PHYS		(0x60200000)
#  define CF_IOBARRIER_PHYS	(IOBARRIER_PHYS)
# endif

# define CF_WIDTH		(16)
# define CF_ADDR_MULT		(1)
# define CF_REG			(1<<12)	/* REG line for register access */
# define CF_ALT			(1<<10)	/* A10 line for data read */
# define CF_ATTRIB		(1<<9) /* A9 line for attribute access */

#endif  /* __MACH_DRV_CF_H__ */
