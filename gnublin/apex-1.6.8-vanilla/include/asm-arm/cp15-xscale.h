/* cp15-xscale.h

   written by Marc Singer
   13 Jan 2006

   Copyright (C) 2006 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   This cache flush alrogithm comes from Nicolas Pitre's work in the
   Linux kernel as well as from email he sent to the linux arm ports
   list.  We flush twice since we don't know how the caller, perhaps
   RedBoot, was flushing cache.  Therefore, we perform the ping-pong
   flush as described by Nico so that we are certain that the cache
   has been purged.

*/

#if !defined (__CP15_XSCALE_H__)
#    define   __CP15_XSCALE_H__

/* ----- Includes */

#include "mach/memory.h"

	/* ---- Cache control */

#define INVALIDATE_ICACHE\
  __asm volatile ("mcr p15, 0, %0, c7, c5, 0\n\t" :: "r" (0))
#define INVALIDATE_ICACHE_VA(a)\
  __asm volatile ("mcr p15, 0, %0, c7, c5, 1\n\t" :: "r" (a))

#define INVALIDATE_DCACHE\
  __asm volatile ("mcr p15, 0, %0, c7, c6, 0\n\t" :: "r" (0))
#define INVALIDATE_DCACHE_VA(a)\
  __asm volatile ("mcr p15, 0, %0, c7, c6, 1\n\t" :: "r" (a))

#define INVALIDATE_CACHE\
  __asm volatile ("mcr p15, 0, %0, c7, c7, 0\n\t" :: "r" (0))

#define CLEAN_DCACHE_VA(a)\
  __asm volatile ("mcr p15, 0, %0, c7, c10, 1\n\t" :: "r" (a))
#define DRAIN_WRITE_BUFFER\
  __asm volatile ("mcr p15, 0, %0, c7, c10, 4\n\t" :: "r" (0))

#define PREFETCH_ICACHE_VA(a)\
  __asm volatile ("mcr p15, 0, %0, c7, c13, 1\n\t" :: "r" (a))

	/* ---- Cache lockdown */

#define UNLOCK_CACHE\
  __asm volatile ("mcr p15, 0, %0, c9, c1, 1\n\t"\
		  "mcr p15, 0, %0, c9, c2, 1\n\t" :: "r" (0))

	/* ---- TLB control */

#define INVALIDATE_TLB\
  __asm volatile ("mcr p15, 0, %0, c8, c7, 0\n\t" :: "r" (0))
#define INVALIDATE_ITLB\
  __asm volatile ("mcr p15, 0, %0, c8, c5, 0\n\t" :: "r" (0))
#define INVALIDATE_ITLB_VA(a)\
  __asm volatile ("mcr p15, 0, %0, c8, c5, 1\n\t" :: "r" (a))
#define INVALIDATE_DTLB\
  __asm volatile ("mcr p15, 0, %0, c8, c6, 0\n\t" :: "r" (0))
#define INVALIDATE_DTLB_VA(a)\
  __asm volatile ("mcr p15, 0, %0, c8, c6, 1\n\t" :: "r" (a))


/* --- */

#define INVALIDATE_BTB(i)\
  __asm volatile ("mcr p15, 0, %0, c7, c5, 6\n\t" :: "r" (i))

#define ALLOCATE_DCACHE_VA(a)\
  __asm volatile ("mcr p15, 0, %0, c7, c2, 5" :: "r" (a));

#define LOCK_ICACHE_VA(a)\
  __asm volatile ("mcr p15, 0, %0, c9, c1, 0\n\t" :: "r" (a))
#define ENABLE_DCACHE_LOCK\
  __asm volatile ("mcr p15, 0, %0, c9, c2, 0\n\t" :: "r" (1));
#define DISABLE_DCACHE_LOCK\
  __asm volatile ("mcr p15, 0, %0, c9, c2, 0\n\t" :: "r" (0));


/* This coprocessor wait is require on the XScale when it is necessary
   to guarantee that the coprocessor has finished before continuing.
   AFAIK, this is not required on other ARM architectures. */

#define CP15_WAIT\
 ({ unsigned long v; \
    __asm volatile ("mrc p15, 0, %0, c2, c0, 0\n\t" \
		    "mov %0, %0\n\t" \
		    "sub pc, pc, #4" : "=r" (v)); })


  /* The cache cleaning algorithm comes from the XScale documentation
     as well as from the Linux kernel.  */
#undef  MVA_CACHE_CLEAN
#define MVA_CACHE_CLEAN		(0xfffe0000)

#define CACHE_LINE_SIZE		(32)
#define CACHE_SIZE		(32768)

#define _CLEANALL_DCACHE(b) \
  ({ unsigned long p = MVA_CACHE_CLEAN ^ (b ? CACHE_SIZE : 0);\
     int line;\
     for (line = CACHE_SIZE/CACHE_LINE_SIZE; line--; p += CACHE_LINE_SIZE) \
       ALLOCATE_DCACHE_VA (p); })

#define CLEANALL_DCACHE\
  ({ _CLEANALL_DCACHE (0) ; _CLEANALL_DCACHE (1); })

#endif  /* __CP15_XSCALE_H__ */
