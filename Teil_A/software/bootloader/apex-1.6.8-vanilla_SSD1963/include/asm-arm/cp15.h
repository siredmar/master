/* cp15.h

   written by Marc Singer
   19 Dec 2005

   Copyright (C) 2005 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   The macros here are for accessing registers and functions of the
   standard CP15 coprocessor.  These registers control the MMU and
   cache functions of most ARM microprocessors.

   Standard MACROS are here that will tend to be a superset of those
   supported by any given core.  Be careful to #undef macros that are
   not available on a given architecture.

*/

#if !defined (__CP15_H__)
#    define   __CP15_H__

/* ----- Includes */

#include <linux/types.h>

#define CP15_ID\
  ({ unsigned long l;\
     __asm volatile ("mrc p15, 0, %0, c0, c0, 0" : "=r" (l)); l; })
#define CP15_CACHE_TYPE\
  ({ unsigned long l;\
     __asm volatile ("mrc p15, 0, %0, c0, c0, 1" : "=r" (l)); l; })
#define CP15_CTRL\
  ({ unsigned long l;\
     __asm volatile ("mrc p15, 0, %0, c1, c0, 0" : "=r" (l)); l; })
#define CP15_TEST\
  ({ unsigned long l;\
     __asm volatile ("mrc p15, 0, %0, c15, c0, 0" : "=r" (l)); l; })
#define CPSR\
  ({ unsigned long l;\
     __asm volatile ("mrs %0, cpsr" : "=r" (l)); l; })

#define STORE_TTB(a)\
  __asm volatile ("mcr p15, 0, %0, c2, c0, 0\n\t" :: "r" (a))
#define LOAD_TTB\
  ({ unsigned long l;\
     __asm volatile ("mrc p15, 0, %0, c2, c0, 0\n\t" :  "=r" (l)); l; })

#define STORE_DOMAIN(a)\
  __asm volatile ("mcr p15, 0, %0, c3, c0, 0\n\t" :: "r" (a))
#define LOAD_DOMAIN\
  ({ unsigned long l;\
     __asm volatile ("mrc p15, 0, %0, c3, c0, 0\n\t" :  "=r" (l)); l; })

#define MMU_DISABLE\
  ({ unsigned long l;\
     __asm volatile ("mrc p15, 0, %0, c1, c0, 0\n\t"\
		     "bic %0, %0, #(1<<0)\n\t"\
		     "mcr p15, 0, %0, c1, c0, 0\n\t"\
		     : "=&r" (l)); } )

#define MMU_ENABLE\
  ({ unsigned long l;\
     __asm volatile ("mrc p15, 0, %0, c1, c0, 0\n\t"\
		     "orr %0, %0, #(1<<0)\n\t"\
		     "mcr p15, 0, %0, c1, c0, 0\n\t"\
		     : "=&r" (l)); } )

#define WAIT_FOR_INTERRUPT\
  __asm volatile ("mcr p15, 0, %0, c7, c0, 4\n\t" :: "r" (0));


/* ----- Architecture specific coprocessor support */

#if defined (CONFIG_CPU_ARMV4)
# include <asm/cp15-armv4.h>
#endif

#if defined (CONFIG_CPU_XSCALE)
# include <asm/cp15-xscale.h>
#endif

#if defined (CONFIG_CPU_ARMV5TE)
# include <asm/cp15-armv5te.h>
#elif defined (CONFIG_CPU_ARMV5)
# include <asm/cp15-armv5.h>
#endif

#if defined (CONFIG_CPU_ARMV6)
# include <asm/cp15-armv6.h>
#endif

#if !defined (CP15_WAIT)
# define CP15_WAIT
#endif

extern void cleanall_dcache (void);
extern void cp15_wait (void);
extern void invalidate_dcache_va (uint32_t);

#endif  /* __CP15_H__ */
