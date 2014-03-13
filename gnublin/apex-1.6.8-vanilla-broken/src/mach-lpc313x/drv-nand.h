/* drv-nand.h

Copyright (C) 2008 NXP Semiconductors

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
#include <drv-nand-base.h>

/* ----- Types */
/* NAND Flash configuration parameters structure used by bootROM */
typedef struct
{
  char   tag[8];
  u8  interface_width;
  u8  reserv_1;
  u16 page_size_in_bytes;
  u16 page_size_in_32bit_words;
  u16 pages_per_block;
  u32 nbr_of_blocks;
  u8  amount_of_address_bytes;
  u8  amount_of_erase_address_bytes;
  u8  support_read_terminate;
  u8  page_increment_byte_nr;
  char   device_name[40];
  u32 timing1;
  u32 timing2;
  u8  ecc_mode;
  u8  id_mask;
  u8  reserv_2[2];

} NAND_BOOT_CFG_PARAMS_T;

/* ----- Globals */
#define LPC313x_NAND_BPARAM_PAGE_SZ	256
#define NANDFLASH_BADBLOCK_LIST_LENGTH 4094  //1 block = min 32 pages = min 16Kbytes (32x512bytes)
                                             //=> 4094 blocks = min 64Mbytes (should be enough for the image)


/* ----- Prototypes */

#define NAND_PHYS	(NANDC_PHYS)
#define NAND_DATA	(unsigned char) (NANDC_READ_DATA & 0xFF)
#define NAND_DATA_W	NANDC_WRITE_DATA
#define NAND_CLE	NANDC_SET_CMD
#define NAND_ALE	NANDC_SET_ADDR

#define NAND_CS_ENABLE {NANDC_SET_CE &= ~NAND_SETCE_CV(0);}
#define NAND_CS_DISABLE {NANDC_SET_CE |= NAND_SETCE_CV(0);}

#define NAND_ENABLE nandsys_init()

#define NAND_ISBUSY ((NANDC_CHECK_STS & NAND_CHK_STS_RB1_LVL) == 0)

#endif  /* __DRV_NAND_H__ */
