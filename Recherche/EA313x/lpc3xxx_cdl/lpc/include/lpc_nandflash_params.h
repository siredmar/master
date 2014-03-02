/**********************************************************************
*
* Project: NAND flash definitions
*
* Description:
*     This file contains the structure definitions and manifest
*     constants for the following component:
*         NAND flash
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
**********************************************************************/

#ifndef LPC_NANDFLASH_PARAMS_H
#define LPC_NANDFLASH_PARAMS_H

#if defined (__cplusplus)
extern "C"
{
#endif

#include "lpc_types.h"

/**********************************************************************
* Defines
**********************************************************************/

/* Vendor IDs */
#define LPCNAND_VENDOR_SAMSUNG      0xEC
#define LPCNAND_VENDOR_STMICRO      0x20
#define LPCNAND_VENDOR_MICRON       0x2C

/**********************************************************************
* NAND flash commands
**********************************************************************/
#define NAND_CMD_READ1        0x00 /* Read mode (1) command */
#define NAND_CMD_READ2        0x01 /* Read mode (2) command */
#define NAND_CMD_PAGEPROG     0x10 /* Auto program command */
#define NAND_CMD_READSTART    0x30 /* Read start command */
#define NAND_CMD_READ3        0x50 /* Read mode (3) command */
#define NAND_CMD_ERASE1ST     0x60 /* Auto block erase 1-st command */
#define NAND_CMD_STATUS       0x70 /* Status read (1) command */
#define NAND_CMD_STATUS_MULTI 0x71 /* Status read (2) command */
#define NAND_CMD_SDIN         0x80 /* Serial data input command */
#define NAND_CMD_READID       0x90 /* ID read (1) command */
#define NAND_CMD_ERASE2ND     0xD0 /* Auto block erase 2-nd command */
#define NAND_CMD_RESET        0xFF /* Reset */

/* Alternate NAND command defines */
#define LPCNAND_CMD_PAGE_READ1    0x00
#define LPCNAND_CMD_PAGE_READ2    0x30
#define LPCNAND_CMD_RANDOM_READ1  0x05
#define LPCNAND_CMD_RANDOM_READ2  0xE0
#define LPCNAND_CMD_STATUS        0x70
#define LPCNAND_CMD_READ_ID       0x90
#define LPCNAND_CMD_PAGE_WRITE1   0x80
#define LPCNAND_CMD_PAGE_WRITE2   0x10
#define LPCNAND_CMD_RANDOM_WRITE1 0x85
#define LPCNAND_CMD_ERASE1        0x60
#define LPCNAND_CMD_ERASE2        0xD0
#define LPCNAND_CMD_RESET         0xFF

/**********************************************************************
* NAND flash main/spare area size
**********************************************************************/
#define SMALL_BLOCK_PAGE_SIZE             528
#define SMALL_BLOCK_PAGE_MAIN_AREA_SIZE   512
#define SMALL_BLOCK_PAGE_SPARE_AREA_SIZE   16
#define LARGE_BLOCK_PAGE_SIZE            2112
#define LARGE_BLOCK_PAGE_MAIN_AREA_SIZE  2048
#define LARGE_BLOCK_PAGE_SPARE_AREA_SIZE   64
#define ID_SIZE                             4

/**********************************************************************
* NAND flash ecc value location
**********************************************************************/
#define SMALL_BLOCK_PAGE_1ST_ECC           520
#define SMALL_BLOCK_PAGE_2ND_ECC           524
#define LARGE_BLOCK_PAGE_1ST_ECC(n)       (2048 +  8 + 16 * n)
#define LARGE_BLOCK_PAGE_2ND_ECC(n)       (2048 + 12 + 16 * n)

/**********************************************************************
* NAND flash status register bit field
**********************************************************************/
/* NAND flash status register -  operation failed */
#define NAND_FLASH_FAILED              _BIT(0)
/* NAND flash status register -  device ready */
#define NAND_FLASH_READY               _BIT(6)

/**********************************************************************
* Types
**********************************************************************/

/* NAND flash parameter */
typedef enum
{
  BUS_WIDTH_8,                   /* 8 bit bus width */
  BUS_WIDTH_16                   /* 16 bit bus width */
} BUS_WIDTH_T;

/* NAND flash parameter */
typedef enum
{
  BLOCK_PAGE_SMALL,              /* 528 byte/264 word page */
  BLOCK_PAGE_LARGE               /* 2112 byte/1056 word page */
} BLOCK_PAGE_T;

/* NAND flash parameter */
typedef enum
{
  ADDR_CYCLES_3,                 /* 3 address cycles */
  ADDR_CYCLES_4,                 /* 4 address cycles */
  ADDR_CYCLES_5                  /* 5 address cycles */
} ADDR_CYCLES_T;

/* Structure containing the parameters for the NAND flash */
typedef struct
{
  BUS_WIDTH_T     bus_width;     /* Bus width */
  BLOCK_PAGE_T    block_page;    /* Block page (small/large) */
  ADDR_CYCLES_T   addr_cycles;   /* Address cycles (3/4/5) */
} NANDFLASH_PARAM_T;

/* ST NAND256R3A */
extern const NANDFLASH_PARAM_T st_nand256r3a;

/* MICRON MT29F2G08AACWP */
extern const NANDFLASH_PARAM_T micron_mt29f2g08aacwp;

/* Optional structure for NAND FLASH geometry */
typedef struct
{
  UNS_32 num_blocks;
  UNS_32 pages_per_block;
  UNS_32 data_bytes_per_page;
  UNS_32 spare_bytes_per_page;
  UNS_8  address_cycles;
} NAND_GEOM_T;

#if defined (__cplusplus)
}
#endif /*__cplusplus */

#endif /* LPC_NANDFLASH_PARAMS_H */
