/* hardware.h
     $Id$

   written by Marc Singer
   22 Dec 2006

   Copyright (C) 2006 Marc Singer

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

#if defined (CONFIG_MACH_MX31ADS)
# include "mx31ads.h"
#endif

#if defined (CONFIG_MACH_EXBIBLIO_ROSENCRANTZ)
# include "rosencrantz.h"
#endif

#if defined (CONFIG_MACH_KARMA5)
//# include "karma.h"
#endif

#include "mx31.h"
#include "mx31-pins.h"

/* ----- Types */

/* ----- Globals */

/* ----- Prototypes */



#endif  /* __HARDWARE_H__ */
