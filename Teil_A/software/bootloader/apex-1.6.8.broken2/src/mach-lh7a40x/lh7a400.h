/* lh7a400.h
     $Id$

   written by Marc Singer
   8 Jul 2005

   Copyright (C) 2005 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#if !defined (__LH7A400_H__)
#    define   __LH7A400_H__

/* ----- Includes */

#include <asm/reg.h>

/* ----- Types */

#define HRTFTC_PHYS	(0x80001000)	/* High-res TFT Controller */

#define HRTFTC_SETUP	__REG (HRTFTC_PHYS + 0x00)
#define HRTFTC_CON	__REG (HRTFTC_PHYS + 0x04)
#define HRTFTC_TIMING1	__REG (HRTFTC_PHYS + 0x08)
#define HRTFTC_TIMING2	__REG (HRTFTC_PHYS + 0x0c)

#endif  /* __LH7A400_H__ */
