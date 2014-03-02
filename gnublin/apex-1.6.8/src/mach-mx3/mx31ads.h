/* mx31ads.h

   written by Marc Singer
   25 Nov 2006

   Copyright (C) 2006 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#if !defined (__MX31ADS_H__)
#    define   __MX31ADS_H__

/* ----- Includes */

#include <asm/reg.h>

/* ----- Types */

/* ----- Globals */

/* ----- Prototypes */

#define CPLD_PHYS	(0xb4000000)
#define CPLD_VERSION	__REG16(CPLD_PHYS + 0x0000)
#define CPLD_STATUS2	__REG16(CPLD_PHYS + 0x0002)
#define CPLD_CTRL1_SET	__REG16(CPLD_PHYS + 0x0004)
#define CPLD_CTRL1_CLR	__REG16(CPLD_PHYS + 0x0006)
#define CPLD_CTRL2_SET	__REG16(CPLD_PHYS + 0x0008)
#define CPLD_CTRL2_CLR	__REG16(CPLD_PHYS + 0x000a)
#define CPLD_CTRL3_SET	__REG16(CPLD_PHYS + 0x000c)
#define CPLD_CTRL3_CLR	__REG16(CPLD_PHYS + 0x000e)
#define CPLD_CTRL4_SET	__REG16(CPLD_PHYS + 0x0010)
#define CPLD_CTRL4_CLR	__REG16(CPLD_PHYS + 0x0012)
#define CPLD_STATUS1	__REG16(CPLD_PHYS + 0x0014)
#define CPLD_ISR	__REG16(CPLD_PHYS + 0x0016)
#define CPLD_ICSR	__REG16(CPLD_PHYS + 0x0018) /* Raw Interrupt Status */
#define CPLD_IMR_SET	__REG16(CPLD_PHYS + 0x001a)
#define CPLD_IMR_CLR	__REG16(CPLD_PHYS + 0x001c)
#define CPLD_UARTA	__REG16(CPLD_PHYS + 0x10000)
#define CPLD_UARTB	__REG16(CPLD_PHYS + 0x10010)
#define CPLD_CS8900_BASE __REG16(CPLD_PHYS + 0x20000)
#define CPLD_CS8900_MEM	__REG16(CPLD_PHYS + 0x21000)
#define CPLD_CS8900_DMA	__REG16(CPLD_PHYS + 0x22000)
#define CPLD_AUDIO	__REG16(CPLD_PHYS + 0x30000)

#define CPLD_CTRL1_ENET_RST		  (1<<1)
#define CPLD_CTRL1_XUART_RST		  (1<<1)
#define CPLD_CTRL1_UARTA_EN		  (1<<2)
#define CPLD_CTRL1_UARTB_EN		  (1<<3)
#define CPLD_CTRL1_UARTC_EN		  (1<<4)
#define CPLD_CTRL1_LED0			  (1<<6)
#define CPLD_CTRL1_LED1			  (1<<7)

#define LED_ON(i)  ({ CPLD_CTRL1_CLR = CPLD_CTRL1_LED0<<(i&1); })
#define LED_OFF(i) ({ CPLD_CTRL1_SET = CPLD_CTRL1_LED0<<(i&1); })


#endif  /* __MX31ADS_H__ */
