/* debug_ll.h

   written by Marc Singer
   12 Feb 2005

   Copyright (C) 2005 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#if !defined (__INCLUDE_DEBUG_LL_H__)
#    define   __INCLUDE_DEBUG_LL_H__

/* ----- Includes */

#include <config.h>

#if defined (CONFIG_STARTUP_UART)
# include <mach/debug_ll.h>

# define PUTHEX(value)	 ({ unsigned long v = (unsigned long) (value); \
			     int i; unsigned char ch; \
			     for (i = 8; i--; ) {\
			     ch = ((v >> (i*4)) & 0xf);\
			     ch += (ch >= 10) ? 'a' - 10 : '0';\
			     PUTC (ch); }})
#else
# define PUTC(c) do {} while (0)
# define PUTHEX(v) do {} while (0)

#endif

#if defined (CONFIG_DEBUG_LL)
# define PUTHEX_LL(value) PUTHEX(value)
# define PUTC_LL(c) PUTC(c)
#else
# define PUTC_LL(c) do {} while (0)
# define PUTHEX_LL(v) do {} while (0)
#endif

/* ----- Types */

/* ----- Globals */

/* ----- Prototypes */



#endif  /* __INCLUDE_DEBUG_LL_H__ */
