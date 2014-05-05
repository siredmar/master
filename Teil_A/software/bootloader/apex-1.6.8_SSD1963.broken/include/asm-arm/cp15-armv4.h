/* cp15-armv4.h

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

#if !defined (__CP15_ARMV4_H__)
#    define   __CP15_ARMV4_H__

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
#define CLEAN_DCACHE_I(i)\
  __asm volatile ("mcr p15, 0, %0, c7, c10, 2\n\t" :: "r" (i))
#define DRAIN_WRITE_BUFFER\
  __asm volatile ("mcr p15, 0, %0, c7, c10, 4\n\t" :: "r" (0))

#define PREFETCH_ICACHE_VA(a)\
  __asm volatile ("mcr p15, 0, %0, c7, c13, 1\n\t" :: "r" (a))

#define CLEAN_INV_DCACHE_VA(a)\
  __asm volatile ("mcr p15, 0, %0, c7, c14, 1\n\t" :: "r" (a))
#define CLEAN_INV_DCACHE_I(i)\
  __asm volatile ("mcr p15, 0, %0, c7, c14, 2\n\t" :: "r" (i))

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


#define _DLEN(l)	((l>>(12+0))&3)
#define _DSIZE(l)	((l>>(12+6))&7)
#define _DASSOC(l)	((l>>(12+3))&7)
#define _DM(l)		((l>>(12+2))&1)

/* *** This code may not work on the ARM720T.  According to the TRM
   *** documentation, the only operation available in the cache is
   *** invalidatation of the combined cache in one statement.
   *** Cleaning isn't addressed at all. */

#define CLEANALL_DCACHE\
  ({ unsigned long cache; int set, index; int linelen; int assoc;\
    __asm volatile ("mrc p15, 0, %0, c0, c0, 1" : "=r" (cache));\
    linelen = _DLEN(cache) + 3;\
    assoc = 32 - _DASSOC(cache);\
/*    if (DM(l)) */ \
/*      assoc = 3*assoc/2; */ \
    for (set = 1<<(_DSIZE(cache) + 6 - _DASSOC(cache) - _DLEN(cache)); \
	 set--; )\
      for (index = 1<<_DASSOC(cache); index--;) {\
	 __asm volatile ("mcr p15, 0, %0, c7, c10, 2" \
			:: "r" ((index<<assoc)|(set<<linelen))); } })

	/* ---- Some function are not available */

#if defined (CONFIG_CPU_ARM720T)
# undef INVALIDATE_ICACHE_VA
# undef INVALIDATE_ICACHE_i
# undef INVALIDATE_DCACHE_VA
# undef INVALIDATE_DCACHE_i
#endif


#endif  /* __CP15_ARMV4_H__ */
