/* fsg3.h

   written by Rod Whitby
   04 Dec 2006

   based on nslu2.h
   written by Marc Singer
   11 Feb 2005

   Copyright (C) 2005 Marc Singer
   Copyright (C) 2006 Rod Whitby

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
   USA.

   -----------
   DESCRIPTION
   -----------

*/

#if !defined (__FSG3_H__)
#    define   __FSG3_H__

/* ----- Includes */

/* ----- Types */

/* ----- Globals */

/* ----- Prototypes */

/* The FSG-3 LEDS are not directly accessible as GPIO pins */
#define _L(l)

#define GPIO_OER_OUTPUTS	(0)

#define GPIO_I_SYNCBUTTON	4	/* 0: pressed */
#define GPIO_I_PCI_INTC		5
#define GPIO_I_PCI_INTA		6
#define GPIO_I_PCI_INTB		7
#define GPIO_I_RESETBUTTON	9	/* 0: pressed */
#define GPIO_I_UNPLUGBUTTON	10	/* 0: pressed */
#define GPIO_I_I2C_SDA		12
#define GPIO_I_I2C_SCL		13

#endif  /* __FSG3_H__ */
