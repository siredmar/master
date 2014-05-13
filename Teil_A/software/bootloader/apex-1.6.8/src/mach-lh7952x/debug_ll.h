/* debug_ll.h

   written by Marc Singer
   12 Feb 2005

   Copyright (C) 2005 The Buici Company

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   This header is for debug only.  As such, it conflicts with the
   normal serial includes.  Thus these macros are protected from being
   included when the uart macros have previously been included.

   Do not include this header in source files.  Use

      #include <debug_ll.h>

   which will get the global header that has additional macros.

*/

#if !defined (__DEBUG_LL_H__)
#    define   __DEBUG_LL_H__

/* ----- Includes */

#include <config.h>
#include <asm/reg.h>

/* ----- Types */

/* ----- Globals */

/* ----- Prototypes */

#if !defined (RCPC_PHYS)

#define RCPC_PHYS		(0xfffe2000)
#define RCPC_CTRL		__REG(RCPC_PHYS + 0x00)
#define RCPC_CTRL_UNLOCK	(1<<9)
#define RCPC_PCLKCTRL0		__REG(RCPC_PHYS + 0x00)
#define RCPC_PCLKCTRL0_U0	(1<<0)

#endif

#if !defined (UART_PHYS)

#define UART_PHYS	(0xfffc0000)
#define UART0_PHYS	(UART_PHYS + 0x0000)
#define UART1_PHYS	(UART_PHYS + 0x1000)
#define UART2_PHYS	(UART_PHYS + 0x2000)

#if defined (CONFIG_MACH_LNODE80)
# define UART		(UART1_PHYS)
#endif

#if !defined (UART) && defined (CONFIG_ARCH_LH79520)
# define UART		(UART1_PHYS)
#endif

#if !defined (UART) && defined (CONFIG_ARCH_LH79524)
# define UART		(UART0_PHYS)
#endif

#if !defined (UART) && defined (CONFIG_ARCH_LH79525)
# define UART		(UART0_PHYS)
#endif

#endif

#if !defined (UART_DR)

#define UART_DR			__REG(UART + 0x00)
#define UART_IBRD		__REG(UART + 0x24)
#define UART_FBRD		__REG(UART + 0x28)
#define UART_LCR_H		__REG(UART + 0x2c)
#define UART_CR			__REG(UART + 0x30)
#define UART_FR			__REG(UART + 0x18)
#define UART_IMSC		__REG(UART + 0x38)
#define UART_ICR		__REG(UART + 0x44)

#define UART_FR_TXFE		(1<<7)
#define UART_FR_RXFF		(1<<6)
#define UART_FR_TXFF		(1<<5)
#define UART_FR_RXFE		(1<<4)
#define UART_FR_BUSY		(1<<3)
#define UART_DR_PE		(1<<9)
#define UART_DR_OE		(1<<11)
#define UART_DR_FE		(1<<8)
#define UART_CR_EN		(1<<0)
#define UART_CR_TXE		(1<<8)
#define UART_CR_RXE		(1<<9)
#define UART_LCR_WLEN8		(3<<5)
#define UART_LCR_FEN		(1<<4)
#define UART_DR_DATAMASK	(0xff)

#endif

#define PUTC(c)		({ while (UART_FR & UART_FR_TXFF) ; \
			   UART_DR = c; \
			   while (UART_FR & UART_FR_BUSY) ; })

#endif  /* __DEBUG_LL_H__ */
