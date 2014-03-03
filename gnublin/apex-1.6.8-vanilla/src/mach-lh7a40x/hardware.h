/* hardware.h
     $Id$

   written by Marc Singer
   4 Dec 2004

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

#if defined (CONFIG_ARCH_LH7A40X)
# include "lh7a40x.h"
#endif

#if defined (CONFIG_ARCH_LH7A400)
# include "lh7a400.h"
#endif

#if defined (CONFIG_ARCH_LH7A404)
# include "lh7a404.h"
#endif

#if  defined (CONFIG_MACH_LPD7A400) \
  || defined (CONFIG_MACH_LPD7A404) \
  || defined (CONFIG_MACH_LPD7A40X)
# include "lpd7a40x.h"
#endif

#include "iobarrier.h"

/* ----- Types */

/* ----- Globals */

/* ----- Prototypes */



#endif  /* __HARDWARE_H__ */
