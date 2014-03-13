/* cp15-armv5te.h

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

*/

#if !defined (__CP15_ARMV5TE_H__)
#    define   __CP15_ARMV5TE_H__

/* ----- Includes */

#include "mach/memory.h"

	/* ---- ID extended */

#define CP15_TCM_TYPE\
  ({ unsigned long l;\
     __asm volatile ("mrc p15, 0, %0, c0, c0, 2" : "=r" (l)); l; })
#define CP15_TLB_TYPE\
  ({ unsigned long l;\
     __asm volatile ("mrc p15, 0, %0, c0, c0, 3" : "=r" (l)); l; })
#define CP15_MPU_TYPE\
  ({ unsigned long l;\
     __asm volatile ("mrc p15, 0, %0, c0, c0, 4" : "=r" (l)); l; })

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

#define CLEANALL_DCACHE ({\
  __asm volatile ("0: mrc p15, 0, r15, c7, c14, 3\n\t" ::: "r15");  \
  __asm volatile ("   bne 0b\n\t"); })

#endif  /* __CP15_ARMV5TE_H__ */
