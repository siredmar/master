/* lpd7a40x.h
     $Id$

   written by Marc Singer
   4 Dec 2004

   Copyright (C) 2004 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#if !defined (__LPD7A40X_H__)
#    define   __LPD7A40X_H__

/* ----- Includes */

/* ----- Types */

/* ----- Globals */

/* ----- Prototypes */

#define CPLD_CONTROL		__REG16(0x70200000)
#define CPLD_FLASH		__REG16(0x71000000)
#define CPLD_REVISION		__REG8(0x71400000)

#define CPLD_CONTROL_WLPEN	(1<<0)
#define CPLD_CONTROL_LCD_VEEEN	(1<<1)

#define CPLD_FLASH_FL_VPEN	(1<<0)
#define CPLD_FLASH_FST1		(1<<1)
#define CPLD_FLASH_FST2		(1<<2)


#endif  /* __LPD7A40X_H__ */
