/* main.c

   written by Marc Singer
   23 Jun 2006

   Copyright (C) 2006 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   ARM kernel bootstrap shim.

   This code may be prepended to a 2.6 Linux kernel in order to setup
   the ATAGS and the machine type in the EXTREME circumstance that the
   platform bootloader cannot be replaced.

   Offset
   ------

   Because we don't know where we're going to be executing, we do a
   little shenanigans to figure out the execution offset.  With it, we
   can reference data that was fixed-up with an absolute address even
   though we are not executing from that address.

   Endianness
   ----------

   This code can be used with either big or little endian kernels, and
   it can be used to switch the endan-ness of the system from the
   boot-up default to the desired orientation for the kernel.
   However, you must swap the bytes of this file and the kernel if you
   are going to write this data to flash.

*/

#define __KERNEL__

#include "config.h"
#include "types.h"		/* include/asm-arm/types.h */
#include "setup.h"		/* include/asm-arm/setup.h */

#define NAKED		__attribute__((naked))
#define BOOT		__attribute__((section(".boot")))

#if defined (COMMANDLINE)
const char cmdline[] = COMMANDLINE;
#endif


void NAKED BOOT boot (u32 r0, u32 r1, u32 r2)
{
  __asm volatile ("nop");	/* Require to get compiler to jump here */
}

int NAKED start (void)
{
  struct tag* p;
  extern unsigned long reloc;
  unsigned long offset = (unsigned long) &reloc;

#if defined (FORCE_BIGENDIAN)

  {
    unsigned long v;
    __asm volatile ("mrc p15, 0, %0, c1, c0, 0\n\t"
		    "orr %0, %0, #(1<<7)\n\t" /* Switch to bigendian */
		    "mcr p15, 0, %0, c1, c0, 0" : "=&r" (v));
  }
#endif

#if defined (FORCE_LITTLEENDIAN)

  {
    unsigned long v;
    __asm volatile ("mrc p15, 0, %0, c1, c0, 0\n\t"
		    "bic %0, %0, #(1<<7)\n\t" /* Switch to littleendian */
		    "mcr p15, 0, %0, c1, c0, 0" : "=&r" (v));
  }
#endif

  __asm volatile ("bl reloc\n\t"
	   "reloc: sub %0, lr, %0\n\t"
	   ".globl reloc\n\t"
		  :  "+r" (offset)
		  :: "lr", "cc");

#if defined (IXP4XX_SDR_CONFIG)
  *(volatile unsigned long*) 0xcc000000 = IXP4XX_SDR_CONFIG;
#endif

#if defined (CREATE_ATAGS) && defined (PHYS_PARAMS)
  p = (struct tag*) PHYS_PARAMS;

	/* Always start with the CORE tag */
  p->hdr.tag		= ATAG_CORE;
  p->hdr.size		= tag_size (tag_core);
  p->u.core.flags	= 0;
  p->u.core.pagesize	= 0;
  p->u.core.rootdev	= 0;
  p = tag_next (p);

	/* Memory tags are always second */
  p->hdr.tag		= ATAG_MEM;
  p->hdr.size		= tag_size (tag_mem32);
  p->u.mem.size		= RAM_BANK0_LENGTH;
  p->u.mem.start	= RAM_BANK0_START;
  p = tag_next (p);

# if defined (RAM_BANK1_START)
  p->hdr.tag		= ATAG_MEM;
  p->hdr.size		= tag_size (tag_mem32);
  p->u.mem.size		= RAM_BANK1_LENGTH;
  p->u.mem.start	= RAM_BANK1_START;
  p = tag_next (p);
# endif

# if defined (INITRD_START)
  p->hdr.tag		= ATAG_INITRD2;
  p->hdr.size		= tag_size (tag_initrd);
  p->u.initrd.start	= INITRD_START;
  p->u.initrd.size	= INITRD_LENGTH;
  p = tag_next (p);
# endif

	/* Command line */
# if defined (COMMANDLINE)
  p->hdr.tag		= ATAG_CMDLINE;
  p->hdr.size		= tag_size (tag_cmdline)
    + (sizeof (cmdline)+3)/4 - 1;
  {
    const char* sz = cmdline + offset;
    int i;
    for (i = 0; i < sizeof (cmdline); ++i)
      p->u.cmdline.cmdline[i] = sz[i];
  }
  p = tag_next (p);
# endif

	/* End */
  p->hdr.tag		= ATAG_NONE;
  p->hdr.size		= 0;

#endif

#if !defined (CREATE_ATAGS) && defined (GUARANTEE_ATAG_CMDLINE)\
 &&  defined (PHYS_PARAMS)  && defined (COMMANDLINE)

  {
    char* cmdlineFound = 0;
    for_each_tag (p, (struct tag*) PHYS_PARAMS)
      if (p->hdr.tag == ATAG_CMDLINE)
	cmdlineFound = p->u.cmdline.cmdline;

    /* The pointer p is left at the start of the terminating
       ATAG_NONE.  If cmdlineFound is non-null, we've found an
       instance of the ATAG_CMDLINE.  If not, we append the default
       command line and a new ATAG_NONE.

       The original author was concerned about the possibility that
       there may be more than one ATAG_CMDLINE record.  There is no
       stipulation in the documentation for the cmdline tags, but it
       makes sense that there would be only one.  Even if there were
       more than one, I'm not sure it would matter since we won't
       append one if we find any. */

    if (!cmdlineFound || !*cmdlineFound) {
      p->hdr.tag = ATAG_CMDLINE;
      p->hdr.size	= tag_size (tag_cmdline)
	+ (sizeof (cmdline)+3)/4 - 1;
      {
	const char* sz = cmdline + offset;
	int i;
	for (i = 0; i < sizeof (cmdline); ++i)
	  p->u.cmdline.cmdline[i] = sz[i];
      }
      p = tag_next (p);
      p->hdr.tag = ATAG_NONE;
      p->hdr.size = 0;
    }
  }

#endif

	/* Pass control to the kernel.  The compile ought to optimize
	   this to a branch (instead of a branch-link), but it doesn't
	   matter if it doesn't. */
  boot (0, MACH_TYPE, PHYS_PARAMS);
}
