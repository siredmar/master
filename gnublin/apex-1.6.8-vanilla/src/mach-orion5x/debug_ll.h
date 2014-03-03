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
#include <mach/hardware.h>
#include <mach/uart.h>

# define PUTC(c)\
  ({ __REG (UART_PHYS + 0x00) = c;\
     while (!(__REG (UART_PHYS + 0x14) & (1<<6))) ; })

#endif  /* __DEBUG_LL_H__ */
