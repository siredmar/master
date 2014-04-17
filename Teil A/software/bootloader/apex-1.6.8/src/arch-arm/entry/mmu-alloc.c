/* mmu-alloc.c

   written by Marc Singer
   22 Dec 2005

   Copyright (C) 2005 Marc Singer

   -----------
   DESCRIPTION
   -----------

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

*/

#include <config.h>
#include <debug_ll.h>
#include <attributes.h>
#include <linux/string.h>
#include <service.h>
#include <apex.h>
#include <asm/cp15.h>
#include <asm/mmu.h>
#include <drv-mem.h>

//#define TALK

void* pvAlloc;			/* Address of next allocatable address */

void* alloc_uncached (size_t cb, size_t alignment)
{
  void* pv;

  if (alignment == 0)
    alignment = 1;

  if (pvAlloc == 0) {
    extern char APEX_VMA_END;
    pvAlloc = (void*) (((unsigned long) &APEX_VMA_END + (1024*1024 - 1))
	       & ~(1024*1024 - 1));
  }

  pv = (void*) (((unsigned long) pvAlloc + (alignment - 1))
		& ~(alignment - 1));
  pvAlloc = pv + cb;

#if defined (TALK)
  printf ("%s: <- %p, 0x%x (%u)\n", __FUNCTION__, pv, cb, cb);
#endif

#if defined (CONFIG_MMU)
  mmu_protsegment (pv, 0, 0);
#endif

  return pv;
}

/* alloc_uncached_top_retain

   allocates an aligned block of memory from the top of the probed
   regions, removes it from the caches, and returns it.

   *** FIXME: should verify that the allocation being requested is
   *** available among the regions.  I'm not fixing it now because it
   *** either works, or it doesn't.

*/

void* alloc_uncached_top_retain (size_t cb, size_t alignment)
{
  void* pv;
  struct mem_region* region = NULL;
  int i;

  if (alignment == 0)
    alignment = 1;

  for (i = 0; i < sizeof (memory_regions)/sizeof (*memory_regions); ++i)
    if (memory_regions[i].length)
      region = &memory_regions[i];
    else
      break;

  pv = (void*) ((region->start + region->length - cb) & ~(alignment - 1));
  region->length = (unsigned long) pv - region->start;

#if defined (CONFIG_MMU)
  mmu_protsegment (pv, 0, 0);
#endif

  return pv;
}
