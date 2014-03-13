/* nand.h

   written by Marc Singer
   9 Nov 2004

   Copyright (C) 2004 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   Testing the NAND flash should be a matter sending a couple of
   commands.

   First, without CPLD support, the IOCON has to be setup
   0xfffe5068 <- 0x100 //
   0xfffe5030 <- 0xf555 //
   0xfffe5034 <- 0xa555 //

   0x40800010 <- 0xff  // Reset
   0x40800010 <- 0x70  // Status
   0x40800010 <- 0x90  // ReadID
   0x40800000 -> 0x98  //
   0x40800000 -> 0x75  //
   0x40800000 -> 0xa5  //
   0x40800000 -> 0xba  //
   0x40800000 -> 0x22  //
   0x40800010 <- 0x00  // Read1
   0x40800008 <- 0x00  // Address0
   0x40800008 <- 0x00  // Address1
   0x40800008 <- 0x00  // Address2
   0x40800000 -> ...

*/

#if !defined (__NAND_H__) && 0
#    define   __NAND_H__

/* ----- Includes */

/* ----- Types */

/* ----- Globals */

/* ----- Prototypes */

#define NAND_PHYS	(0x40000000 | (1<<23))
#define NAND_DATA	__REG8(NAND_PHYS + 0x00)
#define NAND_CLE	__REG8(NAND_PHYS + 0x10)
#define NAND_ALE	__REG8(NAND_PHYS + 0x08)

#define Reset		(0xff)
#define ReadID		(0x90)
#define Status		(0x70)
#define Read1		(0x00)	/* Start address in first 256 bytes of page */
#define Read2		(0x01)	/* Start address in second 256 bytes of page */
#define Read3		(0x50)	/* Start address in last 16 bytes, (ECC?) */
#define Erase		(0x60)
#define EraseConfirm	(0xd0)
#define AutoProgram	(0x10)
#define SerialInput	(0x80)

#define Fail		(1<<0)
#define Ready		(1<<6)
#define Writable	(1<<7)


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

#endif  /* __NAND_H__ */
