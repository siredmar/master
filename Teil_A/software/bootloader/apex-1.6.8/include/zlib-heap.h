/* zlib-heap.h

   written by Marc Singer
   10 Jun 2005

   Copyright (C) 2005 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#if !defined (__ZLIB_HEAP_H__)
#    define   __ZLIB_HEAP_H__

/* ----- Includes */

/* ----- Types */

/* ----- Globals */

/* ----- Prototypes */

extern voidpf zlib_heap_alloc (voidpf opaque, uInt items, uInt size);
extern void zlib_heap_free (voidpf opaque, voidpf address, uInt nbytes);
extern void zlib_heap_reset (void);


#endif  /* __ZLIB_HEAP_H__ */
