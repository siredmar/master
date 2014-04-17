/* debug_ll.h

   written by Marc Singer
   10 Mar 2005

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
#include "mach/hardware.h"	/* For UART constants */

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

#define UART1_PHYS	(0x80000600)
#define UART2_PHYS	(0x80000700)
#define UART3_PHYS	(0x80000800)

#define UART		(UART2_PHYS)

#endif

#if !defined (UART_DATA)

#define UART_DATA		__REG(UART + 0x00)
#define UART_FCON		__REG(UART + 0x04)
#define UART_BRCON		__REG(UART + 0x08)
#define UART_CON		__REG(UART + 0x0c)
#define UART_STATUS		__REG(UART + 0x10)
#define UART_RAWISR		__REG(UART + 0x14)
#define UART_INTEN		__REG(UART + 0x18)
#define UART_ISR		__REG(UART + 0x1c)

#endif

#define PUTC(c)		({ UART_DATA = c; \
			   while (UART_STATUS & UART_STATUS_BUSY) ; })
//#define PUTC(c)		({ __asm volatile ("


#endif  /* __DEBUG_LL_H__ */
