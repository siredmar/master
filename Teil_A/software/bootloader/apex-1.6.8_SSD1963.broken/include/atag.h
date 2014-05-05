/* atag.h

   written by Marc Singer
   6 Nov 2004

   Copyright (C) 2004 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#if !defined (__ATAG_H__)
#    define   __ATAG_H__

/* ----- Includes */

#include <linux/types.h>
#include <asm-arm/setup.h>
#include <attributes.h>

/* ----- Types */

/* ----- Globals */

/* ----- Prototypes */

struct atag_d {
  struct tag* (*append_atag) (struct tag*);
};


#define __atag_0 __used __section(.atag.0)
#define __atag_1 __used __section(.atag.1)
#define __atag_2 __used __section(.atag.2)
#define __atag_3 __used __section(.atag.3)
#define __atag_4 __used __section(.atag.4)
#define __atag_5 __used __section(.atag.5)
#define __atag_6 __used __section(.atag.6)
#define __atag_7 __used __section(.atag.7)

#endif  /* __ATAG_H__ */
