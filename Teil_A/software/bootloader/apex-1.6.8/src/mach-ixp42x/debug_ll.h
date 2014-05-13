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

#if !defined (UART0_PHYS)

#define UART0_PHYS	(0xc8000000)
#define UART1_PHYS	(0xc8001000)
#define UART_PHYS	(UART0_PHYS) /* Console */

#endif

#if !defined (UART_DR)

#define UART_DR		__REG(UART_PHYS + 0x00)
#define UART_DLL	__REG(UART_PHYS + 0x00)
#define UART_DLH	__REG(UART_PHYS + 0x04)
#define UART_IER	__REG(UART_PHYS + 0x04)	/* Interrupt enable */
#define UART_FCR	__REG(UART_PHYS + 0x08)	/* FIFO control */
#define UART_LCR	__REG(UART_PHYS + 0x0c)	/* Line control */
#define UART_LSR	__REG(UART_PHYS + 0x14)	/* Line status */
#define UART_ISR	__REG(UART_PHYS + 0X20)	/* Interrupt status */

#define UART_IER_UUE	(1<<6)

#define UART_LCR_DLAB	(1<<7)
#define UART_LCR_WLS_8	(0x3<<0)
#define UART_LCR_STB_1	(0<<2)

#define UART_FCR_RESETTF (1<<2)
#define UART_FCR_RESETRF (1<<1)
#define UART_FCR_TRFIFOE (1<<0)

#define UART_LSR_TEMT	 (1<<6)
#define UART_LSR_TDRQ	 (1<<5)
#define UART_LSR_FE	 (1<<3)
#define UART_LSR_PE	 (1<<2)
#define UART_LSR_OE	 (1<<1)
#define UART_LSR_DR	 (1<<0)

#endif

/* Assembler coded PUTC optimizes out one load and one unneeded
   register.  Silly GCC.  Then again, this version causes the program
   to misbehave and I'm not sure why. */
#if 0
#define PUTC(c)\
  ({ unsigned long v;\
     __asm volatile ("str %2, [%1]\n\t"\
		  "00: ldr %0, [%1, #20]\n\t"\
		     "tst %0, #64\n\t"\
		     "beq 00b\n\t"\
		     : "=r" (v)\
		     : "r"  (UART_PHYS),\
		       "r"  (c)); })

#else
#define PUTC(c)		({ UART_DR = c;\
			   while ((UART_LSR & UART_LSR_TEMT) == 0) ; })
#endif

#endif  /* __DEBUG_LL_H__ */
