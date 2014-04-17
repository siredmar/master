/* memtest.h

   written by Marc Singer
   7 Nov 2006

   Copyright (C) 2006 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#if !defined (__MEMTEST_H__)
#    define   __MEMTEST_H__

/* ----- Includes */

/* ----- Types */

/* ----- Globals */

/* ----- Prototypes */

int __naked __section (.bootstrap) memory_test_0 (unsigned long address,
						  unsigned long cb);

#endif  /* __MEMTEST_H__ */
