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

#if defined (CONFIG_INTERRUPTS)
extern void (*interrupt_handlers[32])(void);
#endif

#if defined (CONFIG_ARCH_IXP42X)
# include "ixp42x.h"
#endif

/* *** FIXME: ifdef? */
#include "ixp4xx-npe.h"

#if defined (CONFIG_MACH_NSLU2)
# include "nslu2.h"
#endif

#if defined (CONFIG_MACH_NAS100D)
# include "nas100d.h"
#endif

#if defined (CONFIG_MACH_DSMG600)
# include "dsmg600.h"
#endif

#if defined (CONFIG_MACH_FSG3)
# include "fsg3.h"
#endif

/* ----- Types */

/* ----- Globals */

/* ----- Prototypes */

#endif  /* __HARDWARE_H__ */
