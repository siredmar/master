/* attributes.h

   written by Marc Singer
   10 Feb 2005

   Copyright (C) 2005 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#if !defined (__ATTRIBUTES_H__)
#    define   __ATTRIBUTES_H__

/* ----- Includes */

#include <linux/compiler.h>

/* ----- Types */

/* ----- Globals */

/* ----- Prototypes */


// *** Be cautions about uncommenting __used.  Some systems may
// *** already have it defined, but it is necessary on sime systems.
// *** So, either detect the need to comment this line out, or find a
// *** work-around, e.g. renaming the attribute macro.

#undef __used
#undef __section
#undef __aligned

#define __weak			__attribute__((weak))
#define __naked			__attribute__((naked))
#define __used			__attribute__((used))
#define __section(s)		__attribute__((section(#s)))
#define __xbss(s)		__attribute__((section("." #s ".xbss")))
#define __rodata	  const __section(.rodata)
#define __irq_handler		__attribute__((interrupt ("IRQ")))
#define __fiq_handler		__attribute__((interrupt ("FIQ")))
#define __swi_handler		__attribute__((interrupt ("SWI")))
#define __abort_handler		__attribute__((interrupt ("ABORT")))
#define __undef_handler		__attribute__((interrupt ("UNDEF")))

#define __aligned		__attribute__((aligned (32)))

#endif  /* __ATTRIBUTES_H__ */
