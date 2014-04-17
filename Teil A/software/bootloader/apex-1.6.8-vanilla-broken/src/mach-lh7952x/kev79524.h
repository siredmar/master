/* kev79524.h

   written by Marc Singer
   12 Nov 2004

   Copyright (C) 2004 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#if !defined (__KEV79524_H__)
#    define   __KEV79524_H__

/* ----- Includes */

/* ----- Types */

/* ----- Globals */

/* ----- Prototypes */

#define CPLD_PHYS			(0x4c000000 + 0x00600000)

#define CPLD_CTRL1			__REG16(CPLD_PHYS + 0x00)
#define CPLD_CTRL2			__REG16(CPLD_PHYS + 0x02)
#define CPLD_STAT1			__REG16(CPLD_PHYS + 0x04)
#define CPLD_STAT2			__REG16(CPLD_PHYS + 0x06)
#define CPLD_STAT3			__REG16(CPLD_PHYS + 0x08)
#define CPLD_STAT4			__REG16(CPLD_PHYS + 0x0a)

#define CPLD_CTRL1_LCD_BACKLIGHT_EN	(1<<0)
#define CPLD_CTRL1_LCD_OE		(1<<1)
#define CPLD_CTRL1_LCD_POWER_EN		(1<<2)
#define CPLD_CTRL1_CF_POWER		(1<<3)
#define CPLD_CTRL1_CF_RESET		(1<<4)
#define CPLD_CTRL1_DMA_ON		(1<<5)

#define CPLD_STAT1_DIPSW_MASK		((1<<8) - 1)
#define CPLD_STAT1_DIPSW1		(1<<0)
#define CPLD_STAT1_DIPSW2		(1<<1)
#define CPLD_STAT1_DIPSW3		(1<<2)
#define CPLD_STAT1_DIPSW4		(1<<3)
#define CPLD_STAT1_DIPSW5		(1<<4)
#define CPLD_STAT1_DIPSW6		(1<<5)
#define CPLD_STAT1_DIPSW7		(1<<6)
#define CPLD_STAT1_DIPSW8		(1<<7)
#define CPLD_STAT1_USERSW1		(1<<8)
#define CPLD_STAT1_USERSW2		(1<<9)
#define CPLD_STAT1_USERSW3		(1<<10)
#define CPLD_STAT1_USERSW4		(1<<11)

#define CPLD_STAT2_SW_UP		(1<<0)
#define CPLD_STAT2_SW_DOWN		(1<<1)
#define CPLD_STAT2_SW_LEFT		(1<<2)
#define CPLD_STAT2_SW_RIGHT		(1<<3)
#define CPLD_STAT2_SW_SEL		(1<<4)
#define CPLD_STAT2_NAV_INT		(1<<5)
#define CPLD_STAT2_CF_CD1		(1<<6)
#define CPLD_STAT2_CF_CD2		(1<<7)
#define CPLD_STAT2_CF_RDY		(1<<8)
#define CPLD_STAT2_INT7			(1<<9)

#define CPLD_STAT4_DMA_STATE		((1<<4) - 1)
#define CPLD_STAT4_DMA_WER		(1<<4)
#define CPLD_STAT4_DMA_ACK		(1<<4)

#endif  /* __KEV79524_H__ */
