/* debug_ll.h

   written by Marc Singer
   22 Feb 2007

   Copyright (C) 2007 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#if !defined (__DEBUG_LL_H__)
#    define   __DEBUG_LL_H__

/* ----- Includes */

#include <config.h>
#include <asm/reg.h>

#if defined (CONFIG_MX31_UART1)
#include <mach/uart.h>
# define PUTC(c)\
  ({ __REG (UART + UART_TXD) = c; \
     while (!(__REG (UART + UART_SR2) & UART_SR2_TXDC)) ; })
#endif

#if defined (CONFIG_MX31ADS_UARTA)
# include "sc16c652.h"
# define PUTC(c)\
  ({ SC_UART_HR = c;\
     while (!(SC_UART_LSR & SC_UART_LSR_TXE)) ; })
#endif

#endif  /* __DEBUG_LL_H__ */
