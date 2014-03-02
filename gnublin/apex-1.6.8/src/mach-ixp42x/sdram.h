/* sdram.h

   written by Marc Singer
   20 Feb 2007

   Copyright (C) 2007 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#if !defined (__SDRAM_H__)
#    define   __SDRAM_H__

/* ----- Includes */

/* ----- Types */

/* ----- Globals */

/* ----- Prototypes */

	/* The core has 1GiB of address space for SDRAM, but only
	   256MiB may be used. */
#define SDRAM_BANK0_PHYS	(0x00000000)
#define SDRAM_BANK_SIZE		(0x10000000) /* 256MiB */

#if CONFIG_SDRAM_BANK_LENGTH == (32*1024*1024)
# if defined CONFIG_SDRAM_BANK1
#  define SDR_CONFIG_CHIPS	SDR_CONFIG_4x8Mx16
# else
#  define SDR_CONFIG_CHIPS	SDR_CONFIG_2x8Mx16
# endif
#endif

#if CONFIG_SDRAM_BANK_LENGTH == (64*1024*1024)
# if defined CONFIG_SDRAM_BANK1
#  define SDR_CONFIG_CHIPS	SDR_CONFIG_4x16Mx16
# else
#  define SDR_CONFIG_CHIPS	SDR_CONFIG_2x16Mx16
# endif
#endif

#if CONFIG_SDRAM_BANK_LENGTH == (128*1024*1024)
# if defined CONFIG_SDRAM_BANK1
#  define SDR_CONFIG_CHIPS	SDR_CONFIG_4x32Mx16
# else
#  define SDR_CONFIG_CHIPS	SDR_CONFIG_2x32Mx16
# endif
#endif


/* *** FIXME: sdram config constants should go here.  The Integrated
   Circuit Solution Inc IC42S16800 DRAM can do CAS2.  Later. */



#endif  /* __SDRAM_H__ */
