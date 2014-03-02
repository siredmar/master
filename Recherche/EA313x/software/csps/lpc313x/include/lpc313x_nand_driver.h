/***********************************************************************
 * $Id:: lpc313x_nand_driver.h 1911 2009-06-16 00:31:46Z pdurgesh    $
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
  NAND_BOOT_CFG_PARAMS_T param;
  UNS_8 cs;           /* chip select */
  UNS_8 init;         /* device initialized flag */
  UNS_8 page_mode;    /* if set (516 mode) includes 4 bytes for wearleveling */
} NAND_DEV_T;

/***********************************************************************
 * NAND device configuration commands (IOCTL commands and arguments)
 **********************************************************************/

/* NAND device commands (IOCTL commands) */
typedef enum
{
  NAND_SET_WP,
} NAND_IOCTL_CMD_T;




/***********************************************************************
 * NAND driver API functions
 **********************************************************************/

/* Open the MCI */
INT_32 nand_open(void *ipbase, INT_32 arg);

/* Close the MCI */
STATUS nand_close(INT_32 devid);

/* MCI configuration block */
STATUS nand_ioctl(INT_32 devid,
                  INT_32 cmd,
                  INT_32 arg);

/* MCI read function - read all 16 data registers */
INT_32 nand_read_blocks(INT_32 devid,
                        void *buffer,
                        INT_32 start_block,
                        INT_32 end_block);

/* MCI write function - writes all 16 data registers */
INT_32 nand_write_blocks(INT_32 devid,
                         void *buffer,
                         INT_32 start_block,
                         INT_32 end_block);

/***********************************************************************
 * Other NAND driver functions
 **********************************************************************/


#ifdef __cplusplus
}
#endif

#endif /* LPC313X_NAND_DRIVER_H */
