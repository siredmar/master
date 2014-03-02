/***********************************************************************
 * $Id:: lpc313x_mci_driver.h 1756 2009-03-25 18:39:38Z pdurgesh     $
 *
 * Project: LPC313x SD/MMC/SDIO controller driver
 *
 * Description:
 *     This file contains driver support for the LPC313x SD/MMC/SDIO
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

#ifndef LPC313X_MCI_DRIVER_H
#define LPC313X_MCI_DRIVER_H

#include "lpc313x_mci.h"
#include "lpc_sdmmc.h"

#ifdef __cplusplus
extern "C"
{
#endif
/***********************************************************************
 * MCI device structure and it defines
 **********************************************************************/
typedef struct  _mci_card_struct MCI_CARD_INFO_T;
typedef UNS_32(*MCI_CMD_WAIT_FUNC_T)(MCI_CARD_INFO_T* , UNS_32);
struct  _mci_card_struct
{
  UNS_32 response[4];	/* Most recent response */
  UNS_32 cid[4];			/* CID of acquired card  */
  UNS_32 csd[4];			/* CSD of acquired card */
  UNS_32 ext_csd[MMC_SECTOR_SIZE/4];
  UNS_32 card_type;
  UNS_32 rca;			    /* Relative address assigned to card */
  UNS_32 speed;
  UNS_32 block_len;
  UNS_32 device_size;
  UNS_32 blocknr;
  MCI_CMD_WAIT_FUNC_T wait_func;

};

/* card type defines */
#define CARD_TYPE_SD    (1 << 0)
#define CARD_TYPE_4BIT  (1 << 1)
#define CARD_TYPE_8BIT  (1 << 2)
#define CARD_TYPE_HC    (OCR_HC_CCS) /* high capacity card > 2GB */


/***********************************************************************
 * MCI device configuration commands (IOCTL commands and arguments)
 **********************************************************************/

/* MCI device commands (IOCTL commands) */
typedef enum
{
  MCI_GET_CSD,
  MCI_GET_EXT_CSD
} MCI_IOCTL_CMD_T;




/***********************************************************************
 * MCI driver API functions
 **********************************************************************/

/* Open the MCI */
INT_32 mci_open(void *ipbase, INT_32 arg);

/* Close the MCI */
STATUS mci_close(INT_32 devid);

/* MCI configuration block */
STATUS mci_ioctl(INT_32 devid,
                 INT_32 cmd,
                 INT_32 arg);

/* MCI read function - read all 16 data registers */
INT_32 mci_read_blocks(INT_32 devid,
                       void *buffer,
                       INT_32 start_block,
                       INT_32 end_block);

/* MCI write function - writes all 16 data registers */
INT_32 mci_write_blocks(INT_32 devid,
                        void *buffer,
                        INT_32 start_block,
                        INT_32 end_block);

/***********************************************************************
 * Other SDMMC driver functions
 **********************************************************************/
INT_32 mci_execute_command(MCI_CARD_INFO_T* pdev,
                           UNS_32 cmd,
                           UNS_32 arg,
                           UNS_32 wait_status);


#ifdef __cplusplus
}
#endif

#endif /* LPC313X_MCI_DRIVER_H */
