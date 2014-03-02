/***********************************************************************
 * $Id:: lpc313x_nand_driver.h 6112 2011-01-11 04:41:10Z ing02124    $
 *
 * Project: LPC313x NAND controller driver
 *
 * Description:
 *     This file contains driver support for the LPC313x NAND
 *     controller.
 *
 ***********************************************************************
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * NXP Semiconductors assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. NXP Semiconductors
 * reserves the right to make changes in the software without
 * notification. NXP Semiconductors also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
 *********************************************************************/

#ifndef LPC313X_NAND_DRIVER_H
#define LPC313X_NAND_DRIVER_H

#include "lpc313x_nandc.h"
#include "lpc_nandflash_params.h"

#ifdef __cplusplus
extern "C"
{
#endif
/***********************************************************************
 * NAND device structure and it defines
 **********************************************************************/
/* open argument structure */
/* NAND device configuration structure type */
typedef struct
{
  UNS_32	cs;           		/* chip select */
  UNS_8	interface_width;	/* Interface Width - 8 or 16 */
} NAND_DEV_T;

/* NAND flash block/page structure */
typedef struct
{
  BOOL_32 dma;     	  /* use DMA
                         if 0 then dma not used
                         else dma used */
  UNS_32 block_num;   /* Block number */
  UNS_32 page_num;    /* Page number */
  UNS_8 *buffer;      /* data buffer pointer */
  UNS_8 *oobbuf;      /* OOB data buffer pointer - NULL if OOB not required
											 For DMA transfers, set NULL since OOB can not be transfered
											 using DMA */
} NAND_BLOCKPAGE_T;

/* NAND flash device information structure */
typedef struct
{
  UNS_8	 id[4]; 						/* Dev, MAF, Cell, EXT IDs */
  UNS_16 page_size;  				/* Page size in bytes */
  UNS_16 oob_size;					/* OOB size in bytes */
  UNS_16 pages_per_block;		/* pages per block */
  UNS_32 nbr_of_blocks;			/* Number of blocks */
} NAND_DEV_INFO_T;

/* NAND Ready, Busy status Enum */
typedef enum
{
  NAND_READY,						/* NAND Ready */
  NAND_BUSY							/* NAND Busy */
} NAND_STATUS_T;

/* NAND Bad Block Check structure */
typedef struct
{
  UNS_32 blk_num;			/* Block number */
  BOOL_32 blk_bad;		/* Bad Block Flag */
} NAND_BB_CHECK_T;

/***********************************************************************
 * NAND device configuration commands (IOCTL commands and arguments)
 **********************************************************************/

/* NAND device commands (IOCTL commands) */
typedef enum
{
  NAND_GET_STATUS,  	/* Get Status from NAND Flash chip -
										 		use arg a pointer to NAND_STATUS_T enum */
  NAND_GET_DEV_INFO,	/* Read NAND flash device info -
										 		use arg as pointer to NAND_DEV_INFO_T struct */
  NAND_ERASE_BLOCK, 	/* Erase a NAND flash block,
                      		use arg as block number */
  NAND_READ_PAGE, 		/* Read NAND flash page, use arg as a
                      		pointer to a structure of type
                      		NAND_BLOCK_PAGE_T */
  NAND_WRITE_PAGE,		/* Write NAND flash page, use arg as a
                      		pointer to a structure of type
                      		NAND_BLOCK_PAGE_T */
  NAND_CHECK_BB,   	/* Check if block is bad, use arg as a
                      		pointer to a structure of type
                      		NAND_BB_CHECK_T */
  NAND_MARK_BB 				/* Mark block as bad,
                      	 use arg as block number */
} NAND_IOCTL_CMD_T;

/***********************************************************************
 * NAND driver API functions
 **********************************************************************/

/* Open the NAND device */
INT_32 nand_open(void *ipbase, INT_32 arg);

/* Close the NAND device */
STATUS nand_close(INT_32 devid);

/* NAND configuration function */
STATUS nand_ioctl(INT_32 devid,
                  INT_32 cmd,
                  INT_32 arg);

/* NAND read function - not supported */
INT_32 nand_read(INT_32 devid,
                 void *buffer,
                 INT_32 max_bytes);


/* NAND write function - not supported */
INT_32 nand_write(INT_32 devid,
                  void *buffer,
                  INT_32 max_bytes);

/* NAND format command */
STATUS nand_format(INT_32 devid);

/***********************************************************************
 * Other NAND driver functions
 **********************************************************************/


#ifdef __cplusplus
}
#endif

#endif /* LPC313X_NAND_DRIVER_H */
