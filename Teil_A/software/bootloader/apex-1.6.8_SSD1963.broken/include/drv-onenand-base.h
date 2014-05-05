/* drv-onenand-base.h

   written by Marc Singer
   7 May 2007

   Copyright (C) 2007 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#if !defined (__DRV_ONENAND_BASE_H__)
#    define   __DRV_ONENAND_BASE_H__

/* ----- Includes */

/* ----- Types */

/* ----- Globals */

/* ----- Prototypes */

#define ONENAND_MULTIPLIER	(CONFIG_DRIVER_ONENAND_MULTIPLIER)

#if ONENAND_MULTIPLIER != 2
# error "OneNAND driver only works with 16 bit interface and MULTIPLIER of 2."
#endif

#define ONENAND_BASE		(CONFIG_DRIVER_ONENAND_BASE)
#define ONENAND_BOOTRAM		(ONENAND_BASE + 0x0000*ONENAND_MULTIPLIER)
#define ONENAND_DATARAM0	(ONENAND_BASE + 0x0200*ONENAND_MULTIPLIER)
#define ONENAND_DATARAM1	(ONENAND_BASE + 0x0600*ONENAND_MULTIPLIER)

#define PAGES_PER_BLOCK		(64)

#define _ONR_(x)		__REG16(ONENAND_BASE + (x)*ONENAND_MULTIPLIER)

#define ONENAND_OP		_ONR_(0x0000)
#define ONENAND_MAN_ID		_ONR_(0xf000)
#define ONENAND_DEV_ID		_ONR_(0xf001)
#define ONENAND_VER_ID		_ONR_(0xf002)
#define ONENAND_DATA_SIZE	_ONR_(0xf003)
#define ONENAND_BOOT_SIZE	_ONR_(0xf004)
#define ONENAND_BUFF_CNT	_ONR_(0xf005)
#define ONENAND_TECH_INFO	_ONR_(0xf006)
#define ONENAND_SA_1		_ONR_(0xf100)
#define ONENAND_SA_2		_ONR_(0xf101)
#define ONENAND_SA_3		_ONR_(0xf102)
#define ONENAND_SA_4		_ONR_(0xf103)
#define ONENAND_SA_5		_ONR_(0xf104)
#define ONENAND_SA_6		_ONR_(0xf105)
#define ONENAND_SA_7		_ONR_(0xf106)
#define ONENAND_SA_8		_ONR_(0xf107)
#define ONENAND_SB		_ONR_(0xf200)
#define ONENAND_CMD		_ONR_(0xf220)
#define ONENAND_CONFIG_1	_ONR_(0xf221)
#define ONENAND_CONFIG_2	_ONR_(0xf222)
#define ONENAND_STATUS		_ONR_(0xf240)
#define ONENAND_INTR		_ONR_(0xf241)
#define ONENAND_SBA		_ONR_(0xf24c)
#define ONENAND_WP_STATUS	_ONR_(0xf24e)

#define ONENAND_CMD_LOAD	0x00
#define ONENAND_CMD_LOAD_SPARE	0x13
#define ONENAND_CMD_PROGRAM	0x80
#define ONENAND_CMD_PROGRAM_SPARE 0x1a
#define ONENAND_CMD_COPY_BACK	0x1b
#define ONENAND_CMD_RESET_CORE	0xf0
#define ONENAND_CMD_RESET_ALL	0xf3
#define ONENAND_CMD_UNLOCK	0x23
#define ONENAND_CMD_UNLOCKALL	0x27
#define ONENAND_CMD_BLOCKERASE	0x94


#define ONENAND_STATUS_TO	(1<<0)
#define ONENAND_STATUS_PLANE2CURR (1<<1)
#define ONENAND_STATUS_PLANE2PREV (1<<2)
#define ONENAND_STATUS_PLANE1CURR (1<<3)
#define ONENAND_STATUS_PLANE1PREV (1<<4)
#define ONENAND_STATUS_OTPBL	(1<<5)
#define ONENAND_STATUS_OTPL	(1<<6)
#define ONENAND_STATUS_RSTB	(1<<7)
#define ONENAND_STATUS_SUSPEND	(1<<9)
#define ONENAND_STATUS_ERROR	(1<<10)
#define ONENAND_STATUS_ERASE	(1<<11)
#define ONENAND_STATUS_PROG	(1<<12)
#define ONENAND_STATUS_LOAD	(1<<13)
#define ONENAND_STATUS_LOCK	(1<<14)
#define ONENAND_STATUS_ONGO	(1<<15)

#define ONENAND_INTR_READY	(1<<15)

#define DFS_FBA(x)		(((x)&0x3ff) | ((x)&0x400 << 4))
#define DBS(x)			(((x)&1)<<15)
#define FPA_FSA(x)		(x)
/** Assemble the bits for the BufferStartAddress_BufferStartCount
    register. 'd' is 0 for BootRAM, and 1 for DataRAM.  's' is the
    starting sector of the buffer (0-3).  'c' is the count of
    sectors (0-3). */
#define BSA_BSC(d,s,c)		(((d)?(1<<11):0)|(((s)&0x7)<<8)|((c)&0x3))
#define SBA(x)			(((x)/PAGES_PER_BLOCK)&0x3ff)

#define ONENAND_IS_ERROR	(ONENAND_STATUS & ONENAND_STATUS_ERROR)
#define ONENAND_IS_BUSY		(!(ONENAND_INTR & ONENAND_INTR_READY))

/** Setup array addressing registers for OneNAND.  'p' is the 2KiB
    page to address, and 's' is the 512B sector within that page. */
#define ONENAND_ADDRSETUP(p,s)\
	({ ONENAND_SA_1 = DFS_FBA ((p)/PAGES_PER_BLOCK);\
	   ONENAND_SA_2 = DBS (0);\
	   ONENAND_SA_8 = FPA_FSA (((p)%PAGES_PER_BLOCK)*4 + (s)); })

/** Shortcut to setup OneNAND array addressing for the start of a page. */
#define ONENAND_PAGESETUP(p) ONENAND_ADDRSETUP((p),0)

/** Setup the buffer to receive the data from the OneNAND array.  'b'
    is the Data RAM buffer (0-1), 's' is the sector within that buffer
    (0-3), and 'c' is the count of 512B sectors. */
#define ONENAND_BUFFSETUP(b,s,c)\
	({ ONENAND_SB = BSA_BSC (1, (((b)&1)*4) + ((s)&0x3), (c)); })

#endif  /* __DRV_ONENAND_BASE_H__ */
