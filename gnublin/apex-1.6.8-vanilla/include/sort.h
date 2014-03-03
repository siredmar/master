/* sort.h

   written by Marc Singer
   17 May 2005

   Copyright (C) 2005 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   Linkage for kernel's sort routine.  We import it for the sake of
   drv-jffs2.

*/

#if !defined (__SORT_H__)
#    define   __SORT_H__

/* ----- Includes */

/* ----- Types */

/* ----- Globals */

/* ----- Prototypes */

extern void sort (void *base, size_t num, size_t size,
		  int (*cmp)(const void *, const void *),
		  void (*swap)(void *, void *, int size));

#endif  /* __SORT_H__ */
