/* zlib-heap.c

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

   Heap helper for zlib.  It wants to have a memory allocator, but we
   don't have one.  We fake it with these routines.

   o Heap

     A very crude heap is made available to the zlib routines.  It
     allocates, but doesn't deallocate.  As we don't care much about
     using memory, this is OK.  However, it is quite dumb.  The zlib
     stuff here should be replaced with the inflate code from the
     kernel which uses static buffers.

*/


#include <config.h>
#include <apex.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <zlib.h>
#include <zlib-heap.h>
#include <attributes.h>

/* *** FIXME: this heap can't really cut it, can it? */
#define CB_HEAP   (512*1024)
static unsigned char __xbss(zlib) heap [CB_HEAP]; /* Fake heap */
static size_t heap_allocated;	/* Bytes allocated on the heap */

voidpf zlib_heap_alloc (voidpf opaque, uInt items, uInt size)
{
  int cb = (items*size + 3) & ~3;
  voidpf result = heap + heap_allocated;

  if (heap_allocated + cb > sizeof (heap)) {
    printf ("%s: heap overflow %d %d\n", __FUNCTION__, heap_allocated, cb);
    return Z_NULL;
  }

  heap_allocated += cb;
//  printf ("%s: %p <= %d %d = %d (%d)\n", __FUNCTION__,
//	  result, items, size, cb, heap_allocated);
  return result;
}

void zlib_heap_free (voidpf opaque, voidpf address, uInt nbytes)
{
//  printf ("%s: %p %d\n", __FUNCTION__, address, nbytes);
}

void zlib_heap_reset (void)
{
  /* *** FIXME: not sure why, but we need to initialize the allocated
	 memory or the decompression has problems.  Instead of
	 amortizing the cost, we clear memory in one pass.  It means
	 that the decompression part, if visible, will be as fast as
	 possible.  I suspect that the time will be negligible in any
	 case. */
  memset (heap, 0, CB_HEAP);
  heap_allocated = 0;
}
