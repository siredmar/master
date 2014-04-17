/* nas100d.h

   written by Rod Whitby
   05 Nov 2006

   based on nslu2.h
   written by Marc Singer
   11 Feb 2005

   Copyright (C) 2005 Marc Singer
   Copyright (C) 2006 Rod Whitby

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#if !defined (__NAS100D_H__)
#    define   __NAS100D_H__

/* ----- Includes */

/* ----- Types */

/* ----- Globals */

/* ----- Prototypes */

/* We haven't worked out how to control the LEDs properly yet */
#define _L(l)

#define GPIO_OER_OUTPUTS	((1<<0)		/* WLAN LED */\
				|(1<<3)		/* Disk LED */\
				|(1<<15))	/* Power LED */

#define GPIO_I_LEDWLAN		0
#define GPIO_I_LEDDISK		3
#define GPIO_I_RESETBUTTON	4	/* 0: pressed */
#define GPIO_I_I2C_SDA		5
#define GPIO_I_I2C_SCL		6
#define GPIO_I_PCI_INTD		8
#define GPIO_I_PCI_INTC		9
#define GPIO_I_PCI_INTB		10
#define GPIO_I_PCI_INTA		11
#define GPIO_I_POWEROFF		12
#define GPIO_I_PCI_RESET	13
#define GPIO_I_POWERBUTTON	14	/* 1: pressed */
#define GPIO_I_LEDPOWER		15

#endif  /* __NAS100D_H__ */
