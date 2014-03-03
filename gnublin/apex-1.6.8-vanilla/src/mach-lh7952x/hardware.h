/* hardware.h

   written by Marc Singer
   15 Nov 2004

   Copyright (C) 2004 Marc Singer

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

#if defined (CONFIG_INTERRUPTS)
//#define IRQ_COUNT	32
#endif

#if defined (CONFIG_ARCH_LH79520)
# include "lh79520.h"
#endif

#if defined (CONFIG_ARCH_LH79524) || defined (CONFIG_ARCH_LH79525)
# include "lh79524.h"
#endif

#if defined (CONFIG_MACH_LNODE80)
# include "lnode80.h"
#endif

#if defined (CONFIG_MACH_LPD79520)
# include "lpd79520.h"
#endif

#if defined (CONFIG_MACH_LPD79524)
# include "lpd79524.h"
#endif

#if defined (CONFIG_MACH_KEV79524) || defined (CONFIG_MACH_KEV79525)
# include "kev79524.h"
#endif

/* ----- Types */

/* ----- Globals */

/* ----- Prototypes */



#endif  /* __HARDWARE_H__ */
