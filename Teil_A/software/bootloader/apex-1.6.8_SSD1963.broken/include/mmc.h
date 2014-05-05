/* mmc.h

   written by Marc Singer
   26 May 2006

   Copyright (C) 2006 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#if !defined (__MMC_H__)
#    define   __MMC_H__

/* ----- Includes */

#include "driver.h"

/* ----- Constants */

  /* MMC codes cribbed from the Linux MMC driver */

/* Standard MMC commands (3.1)           type  argument     response */
   /* class 1 */
#define	MMC_GO_IDLE_STATE         0   /* bc                          */
#define MMC_SEND_OP_COND          1   /* bcr  [31:0]  OCR        R3  */
#define MMC_ALL_SEND_CID          2   /* bcr                     R2  */
#define MMC_SET_RELATIVE_ADDR     3   /* ac   [31:16] RCA        R1  */
#define MMC_SET_DSR               4   /* bc   [31:16] RCA            */
#define MMC_SELECT_CARD           7   /* ac   [31:16] RCA        R1  */
#define MMC_SEND_EXT_CSD          8   /* bc                      R1  */        
#define MMC_SEND_CSD              9   /* ac   [31:16] RCA        R2  */
#define MMC_SEND_CID             10   /* ac   [31:16] RCA        R2  */
/* Deprecated */
//#define MMC_READ_DAT_UNTIL_STOP  11   /* adtc [31:0]  dadr       R1  */
#define MMC_STOP_TRANSMISSION    12   /* ac                      R1b */
#define MMC_SEND_STATUS	         13   /* ac   [31:16] RCA        R1  */
#define MMC_GO_INACTIVE_STATE    15   /* ac   [31:16] RCA            */

  /* class 2 */
#define MMC_SET_BLOCKLEN         16   /* ac   [31:0]  block len  R1  */
#define MMC_READ_SINGLE_BLOCK    17   /* adtc [31:0]  data addr  R1  */
#define MMC_READ_MULTIPLE_BLOCK  18   /* adtc [31:0]  data addr  R1  */

  /* class 3 */
#define MMC_WRITE_DAT_UNTIL_STOP 20   /* adtc [31:0]  data addr  R1  */

  /* class 4 */
#define MMC_SET_BLOCK_COUNT      23   /* adtc [31:0]  data addr  R1  */
#define MMC_WRITE_BLOCK          24   /* adtc [31:0]  data addr  R1  */
#define MMC_WRITE_MULTIPLE_BLOCK 25   /* adtc                    R1  */
#define MMC_PROGRAM_CID          26   /* adtc                    R1  */
#define MMC_PROGRAM_CSD          27   /* adtc                    R1  */

  /* class 6 */
#define MMC_SET_WRITE_PROT       28   /* ac   [31:0]  data addr  R1b */
#define MMC_CLR_WRITE_PROT       29   /* ac   [31:0]  data addr  R1b */
#define MMC_SEND_WRITE_PROT      30   /* adtc [31:0]  wpdata addr R1  */

  /* class 5 */
#define MMC_ERASE_GROUP_START    35   /* ac   [31:0]  data addr  R1  */
#define MMC_ERASE_GROUP_END      36   /* ac   [31:0]  data addr  R1  */
#define MMC_ERASE                37   /* ac                      R1b */

  /* class 9 */
#define MMC_FAST_IO              39   /* ac   <Complex>          R4  */
#define MMC_GO_IRQ_STATE         40   /* bcr                     R5  */

  /* class 7 */
#define MMC_LOCK_UNLOCK          42   /* adtc                    R1b */

  /* class 8 */
#define MMC_APP_CMD              55   /* ac   [31:16] RCA        R1  */
#define MMC_GEN_CMD              56   /* adtc [0]     RD/WR      R1b */

/* SD commands                           type  argument     response */
  /* class 8 */
/* This is basically the same command as for MMC with some quirks. */
#define SD_SEND_RELATIVE_ADDR     3   /* ac                      R6  */
#define SD_CMD8                   8   /* bcr  [31:0]  OCR        R3  */        

  /* Application commands */
#define SD_APP_SET_BUS_WIDTH      6   /* ac   [1:0]   bus width  R1	  */
#define SD_APP_OP_COND           41   /* bcr  [31:0]  OCR        R1 (R4)  */
#define SD_APP_SEND_SCR          51   /* adtc                    R1	  */

/*
  MMC status in R1
  Type
	e : error bit
	s : status bit
	r : detected and set for the actual command response
	x : detected and set during command execution. the host must poll
	    the card by sending status command in order to read these bits.
  Clear condition
	a : according to the card state
	b : always related to the previous command. Reception of
	    a valid command will clear it (with a delay of one command)
	c : clear by read
 */

#define R1_OUT_OF_RANGE		(1 << 31)	/* er, c */
#define R1_ADDRESS_ERROR	(1 << 30)	/* erx, c */
#define R1_BLOCK_LEN_ERROR	(1 << 29)	/* er, c */
#define R1_ERASE_SEQ_ERROR      (1 << 28)	/* er, c */
#define R1_ERASE_PARAM		(1 << 27)	/* ex, c */
#define R1_WP_VIOLATION		(1 << 26)	/* erx, c */
#define R1_CARD_IS_LOCKED	(1 << 25)	/* sx, a */
#define R1_LOCK_UNLOCK_FAILED	(1 << 24)	/* erx, c */
#define R1_COM_CRC_ERROR	(1 << 23)	/* er, b */
#define R1_ILLEGAL_COMMAND	(1 << 22)	/* er, b */
#define R1_CARD_ECC_FAILED	(1 << 21)	/* ex, c */
#define R1_CC_ERROR		(1 << 20)	/* erx, c */
#define R1_ERROR		(1 << 19)	/* erx, c */
#define R1_UNDERRUN		(1 << 18)	/* ex, c */
#define R1_OVERRUN		(1 << 17)	/* ex, c */
#define R1_CID_CSD_OVERWRITE	(1 << 16)	/* erx, c, CID/CSD overwrite */
#define R1_WP_ERASE_SKIP	(1 << 15)	/* sx, c */
#define R1_CARD_ECC_DISABLED	(1 << 14)	/* sx, a */
#define R1_ERASE_RESET		(1 << 13)	/* sr, c */
#define R1_STATUS(x)            (x & 0xFFFFE000)
#define R1_CURRENT_STATE(x)	((x & 0x00001E00) >> 9)	/* sx, b (4 bits) */
#define R1_READY_FOR_DATA	(1 << 8)	/* sx, a */
#define R1_APP_CMD		(1 << 5)	/* sr, c */

#define MMC_IDLE_ST             0   /* Idle state */
#define MMC_READY_ST            1   /* Ready state */
#define MMC_IDENT_ST            2   /* Identification State */
#define MMC_STBY_ST             3   /* standby state */
#define MMC_TRAN_ST             4   /* transfer state */
#define MMC_DATA_ST             5   /* Sending-data State */
#define MMC_RCV_ST              6   /* Receive-data State */
#define MMC_PRG_ST              7   /* Programming State */
#define MMC_DIS_ST              8   /* Disconnect State */

#define MMC_VDD_145_150	0x00000001	/* VDD voltage 1.45 - 1.50 */
#define MMC_VDD_150_155	0x00000002	/* VDD voltage 1.50 - 1.55 */
#define MMC_VDD_155_160	0x00000004	/* VDD voltage 1.55 - 1.60 */
#define MMC_VDD_160_165	0x00000008	/* VDD voltage 1.60 - 1.65 */
#define MMC_VDD_165_170	0x00000010	/* VDD voltage 1.65 - 1.70 */
#define MMC_VDD_17_18	0x00000020	/* VDD voltage 1.7 - 1.8 */
#define MMC_VDD_18_19	0x00000040	/* VDD voltage 1.8 - 1.9 */
#define MMC_VDD_19_20	0x00000080	/* VDD voltage 1.9 - 2.0 */
#define MMC_VDD_20_21	0x00000100	/* VDD voltage 2.0 ~ 2.1 */
#define MMC_VDD_21_22	0x00000200	/* VDD voltage 2.1 ~ 2.2 */
#define MMC_VDD_22_23	0x00000400	/* VDD voltage 2.2 ~ 2.3 */
#define MMC_VDD_23_24	0x00000800	/* VDD voltage 2.3 ~ 2.4 */
#define MMC_VDD_24_25	0x00001000	/* VDD voltage 2.4 ~ 2.5 */
#define MMC_VDD_25_26	0x00002000	/* VDD voltage 2.5 ~ 2.6 */
#define MMC_VDD_26_27	0x00004000	/* VDD voltage 2.6 ~ 2.7 */
#define MMC_VDD_27_28	0x00008000	/* VDD voltage 2.7 ~ 2.8 */
#define MMC_VDD_28_29	0x00010000	/* VDD voltage 2.8 ~ 2.9 */
#define MMC_VDD_29_30	0x00020000	/* VDD voltage 2.9 ~ 3.0 */
#define MMC_VDD_30_31	0x00040000	/* VDD voltage 3.0 ~ 3.1 */
#define MMC_VDD_31_32	0x00080000	/* VDD voltage 3.1 ~ 3.2 */
#define MMC_VDD_32_33	0x00100000	/* VDD voltage 3.2 ~ 3.3 */
#define MMC_VDD_33_34	0x00200000	/* VDD voltage 3.3 ~ 3.4 */
#define MMC_VDD_34_35	0x00400000	/* VDD voltage 3.4 ~ 3.5 */
#define MMC_VDD_35_36	0x00800000	/* VDD voltage 3.5 ~ 3.6 */
#define OCR_ALL_READY	0x80000000	/* Card Power up status bit */
#define OCR_HC_CCS    0x40000000  /* High capacity card */

/*
 * Card Command Classes (CCC)
 */
#define CCC_BASIC		(1<<0)	/* (0) Basic protocol functions */
					/* (CMD0,1,2,3,4,7,9,10,12,13,15) */
#define CCC_STREAM_READ		(1<<1)	/* (1) Stream read commands */
					/* (CMD11) */
#define CCC_BLOCK_READ		(1<<2)	/* (2) Block read commands */
					/* (CMD16,17,18) */
#define CCC_STREAM_WRITE	(1<<3)	/* (3) Stream write commands */
					/* (CMD20) */
#define CCC_BLOCK_WRITE		(1<<4)	/* (4) Block write commands */
					/* (CMD16,24,25,26,27) */
#define CCC_ERASE		(1<<5)	/* (5) Ability to erase blocks */
					/* (CMD32,33,34,35,36,37,38,39) */
#define CCC_WRITE_PROT		(1<<6)	/* (6) Able to write protect blocks */
					/* (CMD28,29,30) */
#define CCC_LOCK_CARD		(1<<7)	/* (7) Able to lock down card */
					/* (CMD16,CMD42) */
#define CCC_APP_SPEC		(1<<8)	/* (8) Application specific */
					/* (CMD55,56,57,ACMD*) */
#define CCC_IO_MODE		(1<<9)	/* (9) I/O mode */
					/* (CMD5,39,40,52,53) */
#define CCC_SWITCH		(1<<10)	/* (10) High speed switch */
					/* (CMD6,34,35,36,37,50) */
					/* (11) Reserved */
					/* (CMD?) */

/*
 * CSD field definitions
 */

#define CSD_STRUCT_VER_1_0  0           /* Valid for system spec 1.0 - 1.2 */
#define CSD_STRUCT_VER_1_1  1           /* Valid for system spec 1.4 - 2.2 */
#define CSD_STRUCT_VER_1_2  2           /* Valid for system spec 3.1       */

#define CSD_SPEC_VER_0      0           /* Implements system spec 1.0 - 1.2 */
#define CSD_SPEC_VER_1      1           /* Implements system spec 1.4 */
#define CSD_SPEC_VER_2      2           /* Implements system spec 2.0 - 2.2 */
#define CSD_SPEC_VER_3      3           /* Implements system spec 3.1 */



/* *** These clock values aren't used at the moment.  They should be
   held as upper limits on the rate and determined from HCLK and the
   predivisor. */
#define CLOCK_DETECT		(300*1024)	 /* Clock rate during detect */
#define CLOCK_DATA		(20*1024*1024)	 /* Clock rate during I/O */

#if defined(CONFIG_DRIVER_LH_MMC)
#define MMC_CLKC_START_CLK	(1<<1)
#define MMC_CLKC_STOP_CLK	(1<<0)

#define MMC_STATUS_ENDRESP	(1<<13)
#define MMC_STATUS_DONE		(1<<12)
#define MMC_STATUS_TRANDONE	(1<<11)
#define MMC_STATUS_CLK_DIS	(1<<8)
#define MMC_STATUS_FIFO_FULL	(1<<7)
#define MMC_STATUS_FIFO_EMPTY	(1<<6)
#define MMC_STATUS_CRC		(1<<5)
#define MMC_STATUS_CRCREAD	(1<<3)
#define MMC_STATUS_CRCWRITE	(1<<2)
#define MMC_STATUS_TORES	(1<<1)
#define MMC_STATUS_TOREAD	(1<<0)
#define MMC_STATUS_TIMED_OUT	(1<<16)	/* Synthetic status */

#define MMC_PREDIV_APB_RD_EN	(1<<5)
#define MMC_PREDIV_MMC_EN	(1<<4)
#define MMC_PREDIV_MMC_PREDIV_SHIFT	(0)
#define MMC_PREDIV_MMC_PREDIV_MASK	(0xf)

#define MMC_CMDCON_ABORT	(1<<13)
#define MMC_CMDCON_SET_READ_WRITE (1<<12)
#define MMC_CMDCON_MULTI_BLK4_INTEN (1<<11)
#define MMC_CMDCON_READ_WAIT_EN	(1<<10)
#define MMC_CMDCON_SDIO_EN	(1<<9)
#define MMC_CMDCON_BIG_ENDIAN	(1<<8)
#define MMC_CMDCON_WIDE		(1<<7)
#define MMC_CMDCON_INITIALIZE	(1<<6)
#define MMC_CMDCON_BUSY		(1<<5)
#define MMC_CMDCON_STREAM	(1<<4)
#define MMC_CMDCON_WRITE	(1<<3)
#define MMC_CMDCON_DATA_EN	(1<<2)
#define MMC_CMDCON_RESPONSE_FORMAT_SHIFT (0)
#define MMC_CMDCON_RESPONSE_FORMAT_MASK (0x3)

#define MMC_CMDCON_RESPONSE_NONE (0 << MMC_CMDCON_RESPONSE_FORMAT_SHIFT) //   0
#define MMC_CMDCON_RESPONSE_R1	 (1 << MMC_CMDCON_RESPONSE_FORMAT_SHIFT) //  48
#define MMC_CMDCON_RESPONSE_R2	 (2 << MMC_CMDCON_RESPONSE_FORMAT_SHIFT) // 136
#define MMC_CMDCON_RESPONSE_R3	 (3 << MMC_CMDCON_RESPONSE_FORMAT_SHIFT) //  48


/*  HCLK is usually 99993600 */

#define MMC_RATE_IO_V		(0)			/* 0 -> MCLK/1  */
#define MMC_RATE_ID_V		(6)			/* 6 -> MCLK/64 */
//#define MMC_RATE_ID_V		(5)			/* 5 -> MCLK/32 */
//#define MMC_PREDIV_V		(4)
#define MMC_PREDIV_V		(8)			/* HCLK/N */
#define MMC_RES_TO_V		(0x7f)
//#define MMC_RES_TO_V		(64)
//#define MMC_READ_TO_V		(0x7fff)
#define MMC_READ_TO_V		(0xffff)

#endif  /*CONFIG_DRIVER_LH_MMC*/


#define MS_ACQUIRE_DELAY	(10)
#define MMC_OCR_ARG_MAX		(0x00ffff00)

#define CMD_BIT_AUTO_STOP	 (1<<26)
#define CMD_BIT_APP		 (1<<23)
#define CMD_BIT_INIT		 (1<<22)
#define CMD_BIT_BUSY		 (1<<21)
#define CMD_BIT_LS		 (1<<20) /* Low speed, used during acquire */
#define CMD_BIT_DATA		 (1<<19)
#define CMD_BIT_WRITE		 (1<<18)
#define CMD_BIT_STREAM		 (1<<17)
#define CMD_MASK_RESP		 (3)
#define CMD_SHIFT_RESP		 (24)
#define CMD_MASK_CMD		 (0xff)
#define CMD_SHIFT_CMD		 (0)

#define CMD(c,r)		(  ((c) &  CMD_MASK_CMD)\
				 | ((r) << CMD_SHIFT_RESP)\
				 )

#define CMD_IDLE	 CMD(MMC_GO_IDLE_STATE,0) | CMD_BIT_LS	 | CMD_BIT_INIT
#define CMD_SD_OP_COND	 CMD(SD_APP_OP_COND,1)      | CMD_BIT_LS | CMD_BIT_APP
#define CMD_SD_SEND_IF_COND	 CMD(SD_CMD8,1)      | CMD_BIT_LS 
#define CMD_MMC_OP_COND	 CMD(MMC_SEND_OP_COND,3)    | CMD_BIT_LS | CMD_BIT_INIT
#define CMD_ALL_SEND_CID CMD(MMC_ALL_SEND_CID,2)    | CMD_BIT_LS
#define CMD_MMC_SET_RCA	 CMD(MMC_SET_RELATIVE_ADDR,1) | CMD_BIT_LS
#define CMD_SD_SEND_RCA	 CMD(SD_SEND_RELATIVE_ADDR,1) | CMD_BIT_LS
#define CMD_SEND_CSD	 CMD(MMC_SEND_CSD,2) | CMD_BIT_LS
#define CMD_SEND_EXT_CSD	 CMD(MMC_SEND_EXT_CSD,1) | CMD_BIT_LS | CMD_BIT_DATA
#define CMD_DESELECT_CARD CMD(MMC_SELECT_CARD,0)
#define CMD_SELECT_CARD	 CMD(MMC_SELECT_CARD,1)
#define CMD_SET_BLOCKLEN CMD(MMC_SET_BLOCKLEN,1)
#define CMD_SEND_STATUS  CMD(MMC_SEND_STATUS,1)
#define CMD_READ_SINGLE  CMD(MMC_READ_SINGLE_BLOCK,1) | CMD_BIT_DATA
#define CMD_READ_MULTIPLE CMD(MMC_READ_MULTIPLE_BLOCK,1) | CMD_BIT_DATA | CMD_BIT_AUTO_STOP
#define CMD_SD_SET_WIDTH CMD(SD_APP_SET_BUS_WIDTH,1)| CMD_BIT_APP
#define CMD_STOP	 CMD(MMC_STOP_TRANSMISSION,1) | CMD_BIT_BUSY
#define CMD_WRITE_SINGLE CMD(MMC_WRITE_BLOCK,1) | CMD_BIT_DATA | CMD_BIT_WRITE

#define MMC_SECTOR_SIZE 512	/* *** FIXME: should come from card */

/* card type defines */
#define CARD_TYPE_SD    (1 << 0)
#define CARD_TYPE_4BIT  (1 << 1)
#define CARD_TYPE_8BIT  (1 << 2)
#define CARD_TYPE_HC    (OCR_HC_CCS) /* high capacity card > 2GB */  

/* ----- Types */

struct mmc_info {
  char response[20];		/* Most recent response */
  char cid[16];			/* CID of acquired card  */
  char csd[16];			/* CSD of acquired card */
  int acquire_time;		/* Count of delays to acquire card */
  int cmdcon_sd;		/* cmdcon bits for data IO */
  int card_type; 
  int rca;			/* Relative address assigned to card */
  int acquired;                 /* Boolean for marking that card has been acquired */

  int c_size;
  int c_size_mult;
  int read_bl_len;
  int mult;
  int blocknr;
  int block_len;
  unsigned long device_size;

		/* *** FIXME: should be in .xbss section */
//  char rgb[512*2];		/* Sector buffer(s) */
  unsigned long ib;		/* Index of cached data */
  int speed;
};

/* ----- Globals */

extern struct mmc_info mmc;	/* Single, global context structure */


/* ----- Prototypes */

static inline int mmc_card_acquired (void) {
  return mmc.acquired != 0; }

void mmc_init (void);
void mmc_acquire (void);
ssize_t mmc_read (struct descriptor_d* d, void* pv, size_t cb);

#endif  /* __MMC_H__ */
