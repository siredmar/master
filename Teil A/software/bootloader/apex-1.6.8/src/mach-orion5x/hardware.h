/* hardware.h

   written by Marc Singer
   14 Jan 2005

   Copyright (C) 2005 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#if !defined (__HARDWARE_H__)
#    define   __HARDWARE_H__

/* ----- Includes */

#include <config.h>
#include <linux/types.h>

#if defined (CONFIG_INTERRUPTS)
extern void (*interrupt_handlers[32])(void);
#endif

#if defined (CONFIG_ARCH_ORION5X)
# include "orion5x.h"
#endif

/* ----- Types */

/* ----- Globals */

/* ----- Prototypes */

uint32_t get_tclk (void);
uint32_t get_cpu_clk (void);


#endif  /* __HARDWARE_H__ */
