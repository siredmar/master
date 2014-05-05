/* lh79520.h

   written by Marc Singer
   14 Nov 2004

   Copyright (C) 2004 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#if !defined (__LH79520_H__)
#    define   __LH79520_H__

/* ----- Includes */

#include <asm/reg.h>

/* ----- Types */

/* ----- Globals */

/* ----- Prototypes */

#define RCPC_PHYS			0xfffe2000

#define RCPC_CTRL			__REG (RCPC_PHYS + 0x00)
#define RCPC_CHIPID			__REG (RCPC_PHYS + 0x04)
#define RCPC_REMAP			__REG (RCPC_PHYS + 0x08)
#define RCPC_SOFTRESET			__REG (RCPC_PHYS + 0x0c)
#define RCPC_RESETSTATUS		__REG (RCPC_PHYS + 0x10)
#define RCPC_RESETSTATUSCLR		__REG (RCPC_PHYS + 0x14)
#define RCPC_HCLKCLKPRESCALE		__REG (RCPC_PHYS + 0x18)
#define RCPC_CPUCLKPRESCALE		__REG (RCPC_PHYS + 0x1C)
#define RCPC_PERIPHCLKCTRL		__REG (RCPC_PHYS + 0x24)
#define RCPC_PERIPHCLKCTRL2		__REG (RCPC_PHYS + 0x28)
#define RCPC_AHBCLKCTRL			__REG (RCPC_PHYS + 0x2C)
#define RCPC_PERIPHCLKSEL		__REG (RCPC_PHYS + 0x30)
#define RCPC_PERIPHCLKSEL2		__REG (RCPC_PHYS + 0x34)
#define RCPC_LCDCLKPRESCALE		__REG (RCPC_PHYS + 0x40)
#define RCPC_CORECLKCONFIG		__REG (RCPC_PHYS + 0x88)

#define RCPC_CTRL_UNLOCK		(1<<9)
#define RCPC_AHBCLK_SDC			(1<<1)
#define RCPC_AHBCLK_DMA			(1<<0)
#define RCPC_PERIPHCLK_RTC		(1<<9)
#define RCPC_PERIPHCLK_T23		(1<<5)
#define RCPC_PERIPHCLK_T01		(1<<4)
#define RCPC_PERIPHCLK_U2		(1<<2)
#define RCPC_PERIPHCLK_U1		(1<<1)
#define RCPC_PERIPHCLK_U0		(1<<0)
#define RCPC_PERIPHCLKSEL_RTC_32KHZ	(3<<7)
#define RCPC_PERIPHCLKSEL_RTC_1HZ	(0<<7)
#define RCPC_PERIPHCLKSEL_RTC_MASK	(3<<7)

#define RCPC_CPUCLKPRESCALE_78		0x2
#define RCPC_CPUCLKPRESCALE_52		0x3
#define RCPC_CPUCLKPRESCALE_39		0x4
#define RCPC_HCLKCLKPRESCALE_52		0x3
#define RCPC_HCLKCLKPRESCALE_39		0x4
//#define RCPC_CPUCLK_PRESCALE_DEFAULT		RCPC_CPUCLK_PRESCALE_52
//#define RCPC_HCLK_PRESCALE_DEFAULT		RCPC_HCLK_PRESCALE_52

#if 0 // run the CPU at 52 MHz, and the bus at 52 MHz
# define RCPC_CPUCLKPRESCALE_V	RCPC_CPUCLKPRESCALE_52
# define RCPC_HCLKHCLKPRESCALE_V	RCPC_HCLKCLKPRESCALE_52
# define RCPC_CORECLKCONFIG_V	3			/* FastBus */
# define HCLK			(309657600/6)
#else // run the CPU at 78 MHz, and the Bus at 52 MHz
# define RCPC_CPUCLKPRESCALE_V	RCPC_CPUCLKPRESCALE_78
# define RCPC_HCLKCLKPRESCALE_V	RCPC_HCLKCLKPRESCALE_52
# define RCPC_CORECLKCONFIG_V	0			/* std mode, async */
# define HCLK			(309657600/6)
#endif

#define SDRC_REFTIMER_V	((HCLK*16)/1000000)

#define IOCON_PHYS	0xfffe5000
#define IOCON_MEMMUX	__REG (IOCON_PHYS + 0x00)
#define IOCON_LCDMUX	__REG (IOCON_PHYS + 0x04)
#define IOCON_DMAMUX	__REG (IOCON_PHYS + 0x0c)
#define IOCON_UARTMUX	__REG (IOCON_PHYS + 0x10)

#define SMC_PHYS	0xffff1000
#define SMC_BCR0	__REG (SMC_PHYS + 0x00)
#define SMC_BCR1	__REG (SMC_PHYS + 0x04)
#define SMC_BCR2	__REG (SMC_PHYS + 0x08)
#define SMC_BCR3	__REG (SMC_PHYS + 0x0c)
#define SMC_BCR4	__REG (SMC_PHYS + 0x10)
#define SMC_BCR5	__REG (SMC_PHYS + 0x14)
#define SMC_BCR6	__REG (SMC_PHYS + 0x18)

#define SDRAM_BANK0_PHYS	0x20000000
#define SDRAM_BANK1_PHYS	0x28000000
#define SDRAM_BANK_SIZE		0x08000000

#define SDRC_PHYS	0xffff2000
#define SDRC_CONFIG0	__REG (SDRC_PHYS + 0x00)
#define SDRC_CONFIG1	__REG (SDRC_PHYS + 0x04)
#define SDRC_REFTIMER	__REG (SDRC_PHYS + 0x08)
#define SDRC_WBTIMEOUT	__REG (SDRC_PHYS + 0x0c)

#define TIMER0_PHYS	0xfffc4000
#define TIMER1_PHYS	0xfffc4020
#define TIMER2_PHYS	0xfffc5000
#define TIMER3_PHYS	0xfffc4020

#define TIMER_LOAD	0x00
#define TIMER_VALUE	0x04
#define TIMER_CONTROL	0x08
#define TIMER_CLEAR	0x0c

#define TIMER_ENABLE	(1<<7)
#define TIMER_PERIODIC	(1<<6)	// !FREERUNNING
#define TIMER_CASCADE	(1<<4)
#define TIMER_SCALE_MASK (3<<2)
#define TIMER_SCALE_1	 (0<<2)
#define TIMER_SCALE_16	 (1<<2)
#define TIMER_SCALE_256	 (2<<2)

#define RTC_PHYS	(0xfffe0000)
#define RTC_DR		__REG (RTC_PHYS + 0x00)

#define UART_PHYS	(0xfffc0000)

#define UART0_PHYS	(UART_PHYS + 0x0000)
#define UART1_PHYS	(UART_PHYS + 0x1000)
#define UART2_PHYS	(UART_PHYS + 0x2000)
#define UART		(UART1_PHYS)

#define DMA_PHYS	(0xfffe1000)
#define DMA0_PHYS	(0xfffe1000)
#define DMA1_PHYS	(0xfffe1040)
#define DMA2_PHYS	(0xfffe1080)
#define DMA3_PHYS	(0xfffe10c0)

#define DMA_SOURCELO(c)	__REG(DMA_PHYS + (c)*0x40 + 0x00)
#define DMA_SOURCEHI(c)	__REG(DMA_PHYS + (c)*0x40 + 0x04)
#define DMA_DESTLO(c)	__REG(DMA_PHYS + (c)*0x40 + 0x08)
#define DMA_DESTHI(c)	__REG(DMA_PHYS + (c)*0x40 + 0x0c)
#define DMA_MAX(c)	__REG(DMA_PHYS + (c)*0x40 + 0x10)
#define DMA_CTRL(c)	__REG(DMA_PHYS + (c)*0x40 + 0x14)
#define DMA_CURSLO(c)	__REG(DMA_PHYS + (c)*0x40 + 0x18)
#define DMA_CURSHI(c)	__REG(DMA_PHYS + (c)*0x40 + 0x1c)
#define DMA_CURDLO(c)	__REG(DMA_PHYS + (c)*0x40 + 0x20)
#define DMA_CURDHI(c)	__REG(DMA_PHYS + (c)*0x40 + 0x24)
#define DMA_TCNT(c)	__REG(DMA_PHYS + (c)*0x40 + 0x28)

#define DMA0_SOURCELO	__REG(DMA0_PHYS + 0x00)
#define DMA0_SOURCEHI	__REG(DMA0_PHYS + 0x04)
#define DMA0_DESTLO	__REG(DMA0_PHYS + 0x08)
#define DMA0_DESTHI	__REG(DMA0_PHYS + 0x0c)
#define DMA0_MAX	__REG(DMA0_PHYS + 0x10)
#define DMA0_CTRL	__REG(DMA0_PHYS + 0x14)
#define DMA0_CURSLO	__REG(DMA0_PHYS + 0x18)
#define DMA0_CURSHI	__REG(DMA0_PHYS + 0x1c)
#define DMA0_CURDLO	__REG(DMA0_PHYS + 0x20)
#define DMA0_CURDHI	__REG(DMA0_PHYS + 0x24)
#define DMA0_TCNT	__REG(DMA0_PHYS + 0x28)

#define DMA1_SOURCELO	__REG(DMA1_PHYS + 0x00)
#define DMA1_SOURCEHI	__REG(DMA1_PHYS + 0x04)
#define DMA1_DESTLO	__REG(DMA1_PHYS + 0x08)
#define DMA1_DESTHI	__REG(DMA1_PHYS + 0x0c)
#define DMA1_MAX	__REG(DMA1_PHYS + 0x10)
#define DMA1_CTRL	__REG(DMA1_PHYS + 0x14)
#define DMA1_CURSLO	__REG(DMA1_PHYS + 0x18)
#define DMA1_CURSHI	__REG(DMA1_PHYS + 0x1c)
#define DMA1_CURDLO	__REG(DMA1_PHYS + 0x20)
#define DMA1_CURDHI	__REG(DMA1_PHYS + 0x24)
#define DMA1_TCNT	__REG(DMA1_PHYS + 0x28)

#define DMA_MASK	__REG(DMA_PHYS + 0xf0)
#define DMA_CLR		__REG(DMA_PHYS + 0xf4)
#define DMA_STATUS	__REG(DMA_PHYS + 0xf8)

#define ALI_PHYS	(0xfffe4000) /* Advanced LCD Interface */
#define ALI_SETUP	__REG(ALI_PHYS + 0x00)
#define ALI_CTRL	__REG(ALI_PHYS + 0x04)
#define ALI_TIMING1	__REG(ALI_PHYS + 0x08)
#define ALI_TIMING2	__REG(ALI_PHYS + 0x0c)

#define CLCDC_PHYS	(0xffff4000)
#define CLCDC_TIMING0	__REG(CLCDC_PHYS + 0x00)
#define CLCDC_TIMING1	__REG(CLCDC_PHYS + 0x04)
#define CLCDC_TIMING2	__REG(CLCDC_PHYS + 0x08)
#define CLCDC_UPBASE	__REG(CLCDC_PHYS + 0x10)
#define CLCDC_LPBASE	__REG(CLCDC_PHYS + 0x14)
#define CLCDC_INTREN	__REG(CLCDC_PHYS + 0x18)
#define CLCDC_CTRL	__REG(CLCDC_PHYS + 0x1c)
#define CLCDC_STATUS	__REG(CLCDC_PHYS + 0x20)
#define CLCDC_INTERRUPT	__REG(CLCDC_PHYS + 0x24)
#define CLCDC_UPCURR	__REG(CLCDC_PHYS + 0x28)
#define CLCDC_LPCURR	__REG(CLCDC_PHYS + 0x2c)
#define CLCDC_PALETTE	__REG(CLCDC_PHYS + 0x200)

#endif  /* __LH79520_H__ */
