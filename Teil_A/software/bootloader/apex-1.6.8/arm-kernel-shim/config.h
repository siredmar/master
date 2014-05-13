/* config.h

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

*/

#if !defined (__CONFIG_H__)
#    define   __CONFIG_H__

	/* CREATE_ATAGS indicates that the shim ought to build the
	   ATAGS list.  If it is not defined, the ATAGS may be passed
	   by another piece of code, e.g. a boot loader. */
#define CREATE_ATAGS		/* Construct ATAGS from fields herein */

	/* GUARANTEE_ATAG_CMDLINE makes sure that a command line is
	   present in the ATAGS list.  If none is found, the
	   COMMANDLINE below is added appended to the end of the
	   ATAGS. */
#define GUARANTEE_ATAG_CMDLINE

#define PHYS_PARAMS		0x00000100 /* Address for the parameter list */

#define RAM_BANK0_START		0x00000000
#define RAM_BANK0_LENGTH	(32*1024*1024)

//#define RAM_BANK1_START	   0xd0000000
//#define RAM_BANK1_LENGTH   0x10000000

#define INITRD_START		(0x01000000)
#define INITRD_LENGTH		(0x00400000)

#define COMMANDLINE\
 "console=ttyS0,115200 rtc-x1205,probe0,0x6f noirqdebug"

#define MACH_TYPE		   999

/* Uncomment one of these if you need to switch the CPU into a
   specific mode. */
#define FORCE_LITTLEENDIAN
//#define FORCE_BIGENDIAN

/* Uncomment on of these if you want to reprogram the SDRAM controller
   to take advantage of memory installed, but not detected by the
   primary boot loader.  Also, make sure to se the RAM_BANK0_LENGTH
   appropriately */

//#define IXP4XX_SDR_CONFIG	0x18 /* 64MiB */
//#define IXP4XX_SDR_CONFIG	0x1c /* 128MiB */

#endif  /* __CONFIG_H__ */
