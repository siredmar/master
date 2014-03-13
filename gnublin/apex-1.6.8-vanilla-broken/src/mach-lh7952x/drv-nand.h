/* drv-nand.h

   written by Marc Singer
   13 October 2006

   Copyright (C) 2006 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   NAND flash hooks.

*/

#if !defined (__DRV_NAND_H__)
#    define   __DRV_NAND_H__

/* ----- Includes */

#include "mach/hardware.h"

/* ----- Types */

/* ----- Globals */

/* ----- Prototypes */

#define NAND_TYPE_ST		/* *** FIXME: make it config'd */

/* 8 bit flash */
#define NAND_PHYS	(0x40000000 | (1<<23))
#define NAND_DATA	__REG8(NAND_PHYS + 0x00)
#define NAND_CLE	__REG8(NAND_PHYS + (1<<4))
#define NAND_ALE	__REG8(NAND_PHYS + (1<<3))

#define NAND_ENABLE\
	({ IOCON_MUXCTL14 |=  (1<<8);\
	   GPIO_MN_PHYS   &= ~(1<<0);\
	   IOCON_MUXCTL7  &= ~(0xf<<12);\
	   IOCON_MUXCTL7  |=  (0xa<<12); /* Boot ROM uses 0xf */ \
	   /*  IOCON_MUXCTL7  |=  (0xf<<12); */\
	   IOCON_RESCTL7  &= ~(0xf<<12);\
	   IOCON_RESCTL7  |=  (0xa<<12); })

#define NAND_WP_DISABLE\

//	({ GPIO_PCD |=  (1<<2); })

#define NAND_WP_ENABLE\
//	({ GPIO_PCD |= ~(1<<2); })

#define NAND_CS_ENABLE\

//	({ GPIO_PCD &= ~(1<<6); })
#define NAND_CS_DISABLE\

//	({ GPIO_PCD |= 1<<6; })

#define NAND_ISBUSY   ({ NAND_CLE = NAND_Status;\
			 (NAND_DATA & NAND_Ready) == 0; })

/* The NAND address map converts the boot code from the CPU into the
   number of address bytes used by the device.  Refer to the LH79524
   documentation for the boot codes.  The map is a 32 bit number where
   each boot code is represented by two bits.  Boot code zero occupies
   the least significant bits of the map.  The values in the map are 0
   for non-NAND boot, 1 for 3 address bytes, 2 for 4 address bytes,
   and 3 for five address bytes.  The simple decode macro will return
   2 for non-NAND boot codes.

   The reason for this is two-fold.  First, we want to be space
   efficient when we can.  This stragety yields a correct result in
   only a few instructions.  Second, a lookup using a byte array is
   infeasible when the loader is still trying to get itself into
   SDRAM.  The compiler wants to put the array into constant storage
   and not in the code segment.  Most importantly, though, this method
   is compact.

 */

#define _NAM(bootcode,address_bytes) \
	    ((((address_bytes) - 2)&0x3)<<(((bootcode) & 0xf)*2))
#define NAND_ADDR_MAP ( _NAM(4,3) | _NAM(5,4)  | _NAM(6,5)\
		      | _NAM(7,3) | _NAM(12,4) | _NAM(13,5))
#define NAM_DECODE(bootcode) (((NAND_ADDR_MAP >> ((bootcode)*2)) & 0x3) + 2)

#endif  /* __DRV_NAND_H__ */
