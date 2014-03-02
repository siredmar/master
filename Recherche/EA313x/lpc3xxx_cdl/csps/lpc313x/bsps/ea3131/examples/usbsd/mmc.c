/***********************************************************************
 * $Id:: mmc.c 6157 2011-01-12 05:33:34Z ing02124                     $
 *
 * Project: Disk I/O functions for SD/MMC
 *
 * Description:
 *     Disk I/O functions for File system.
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
#include <stdio.h>
#include <string.h>

#include "lpc_types.h"
#include "lpc313x_mci_driver.h"
#include "lpc313x_timer_driver.h"
#include "diskio.h"

INT_32 g_mcidev;
static char uart_buff[512];

/* Disk Status */
DSTATUS Stat = STA_NOINIT;

extern BYTE di; //Disk initialized flag
extern INT_32 mci_get_status(INT_32);
extern void uart_string_write(char *pbuff, UNS_32 len);

/***********************************************************************
 *
 * Function: disk_ioctl
 *
 * Purpose: Disk I/O control function
 *
 * Processing:
 *          Read function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: RES_OK: Success
 *          RES_PARERR: Drive not supported
 *          RES_NOTRDY: Disk not initialised
 *          RES_ERROR: Error
 *
 * Notes: None
 *
 **********************************************************************/
DRESULT disk_ioctl(
  BYTE drv,		/* Physical drive nmuber (0) */
  BYTE ctrl,		/* Control code */
  void *buff		/* Buffer to send/receive control data */
)
{
  DRESULT res;
  MCI_CARD_INFO_T* psdcard = (MCI_CARD_INFO_T*)g_mcidev;
  INT_32 status;

  if (drv) return RES_PARERR; //Single drive supported

  res = RES_ERROR;

  if (Stat & STA_NOINIT)
  {
    uart_string_write("Initialize the Disk first! \r\n", 0);
    return RES_NOTRDY;
  }

  switch (ctrl)
  {
    case CTRL_SYNC :		/* Make sure that no pending write process. Do not remove this or written sector might not left updated. */
    {
      int i = 10; // wait for a maximum of 10ms
      while (i--)
      {
        status = mci_get_status(g_mcidev);
        if ((status & 0x01E00) == 0x00800) break;
        timer_wait_ms(NULL, 1);
      }
      if (i)
        res = RES_OK;
    }
    break;

    case GET_SECTOR_COUNT :	/* Get number of sectors on the disk (DWORD) */
      *((DWORD *)buff) = psdcard->blocknr;
      res = RES_OK;
      break;

    case GET_SECTOR_SIZE :	/* Get R/W sector size (WORD) */
      *((DWORD*)buff) = 512;
      res = RES_OK;
      break;
#if 0 //Not a necessity in the ff.c hence skipping it for the moment
    case GET_BLOCK_SIZE :	/* Get erase block size in unit of sector (DWORD) */
      if (CardType & CT_SD2)  	/* SDv2? */
      {
        if (send_cmd(ACMD13, 0) == 0)  	/* Read SD status */
        {
          rcvr_spi();
          if (rcvr_datablock(csd, 16))  				/* Read partial block */
          {
            for (n = 64 - 16; n; n--) rcvr_spi();	/* Purge trailing data */
            *(DWORD*)buff = 16UL << (csd[10] >> 4);
            res = RES_OK;
          }
        }
      }
      else  					/* SDv1 or MMCv3 */
      {
        if ((send_cmd(CMD9, 0) == 0) && rcvr_datablock(csd, 16))  	/* Read CSD */
        {
          if (CardType & CT_SD1)  	/* SDv1 */
          {
            *(DWORD*)buff = (((csd[10] & 63) << 1) + ((WORD)(csd[11] & 128) >> 7) + 1) << ((csd[13] >> 6) - 1);
          }
          else  					/* MMCv3 */
          {
            *(DWORD*)buff = ((WORD)((csd[10] & 124) >> 2) + 1) * (((csd[11] & 3) << 3) + ((csd[11] & 224) >> 5) + 1);
          }
          res = RES_OK;
        }
      }
      break;

    case MMC_GET_TYPE :		/* Get card type flags (1 byte) */
      *(WORD*)buff = psdcard->card_type;
      res = RES_OK;
      break;

    case MMC_GET_CSD :		/* Receive CSD as a data block (16 bytes) */
      if (send_cmd(CMD9, 0) == 0		/* READ_CSD */
          && rcvr_datablock(ptr, 16))
        res = RES_OK;
      break;

    case MMC_GET_CID :		/* Receive CID as a data block (16 bytes) */
      if (send_cmd(CMD10, 0) == 0		/* READ_CID */
          && rcvr_datablock(ptr, 16))
        res = RES_OK;
      break;

    case MMC_GET_OCR :		/* Receive OCR as an R3 resp (4 bytes) */
      if (send_cmd(CMD58, 0) == 0)  	/* READ_OCR */
      {
        for (n = 4; n; n--) *ptr++ = rcvr_spi();
        res = RES_OK;
      }
      break;

    case MMC_GET_SDSTAT :	/* Receive SD statsu as a data block (64 bytes) */
      if (send_cmd(ACMD13, 0) == 0)  	/* SD_STATUS */
      {
        rcvr_spi();
        if (rcvr_datablock(ptr, 64))
          res = RES_OK;
      }
      break;
#endif
    default:
      res = RES_PARERR;

  }
  return res;
}

/***********************************************************************
 *
 * Function: disk_status
 *
 * Purpose: Return status of the disk
 *
 * Processing:
 *     Return the status
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Status of disk
 *
 * Notes: None
 *
 **********************************************************************/

DSTATUS disk_status(
  BYTE drv		/* Physical drive nmuber (0) */
)
{
  if (drv)
  {
    return STA_NOINIT;		/* Supports only single drive */
  }
  return Stat;
}

/***********************************************************************
 *
 * Function: disk_initialize
 *
 * Purpose: SDMMC init
 *
 * Processing:
 *     Initialize the SDMMC card
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Status of the disk
 *
 * Notes: None
 *
 **********************************************************************/
DSTATUS disk_initialize(UNS_8 drv)
{
  MCI_CARD_INFO_T* psdcard;

  if (drv)
    return Stat; // We support only single drive

  /* open MCI device */
  g_mcidev = mci_open((void*)SD_MMC_BASE, 0);

  psdcard = (MCI_CARD_INFO_T*)g_mcidev;

  /* check if card is acquired */
  if (psdcard->cid[0] != 0)
  {

    /* print card info */
    sprintf(uart_buff, "%s card acquired \r\n", (psdcard->card_type & CARD_TYPE_SD) ? "sd" : "mmc");
    uart_string_write(uart_buff, 0);

    Stat &= ~STA_NOINIT;
    Stat &= ~STA_NODISK;
  }
  else
  {
    uart_string_write("no card acquired\r\n", 0);
    Stat = STA_NODISK;
  }
  return Stat;
}


/***********************************************************************
 *
 * Function: disc_read
 *
 * Purpose: SD/MMC sector read
 *
 * Processing:
 *     Read from the SD/MMC sectors
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: RES_OK: Success
 *          RES_ERROR: Error in reading
 *          RES_PARERR: Invalid parameter
 *          RES_NOTRDY: Disk not initialised
 *
 * Notes: None
 *
 **********************************************************************/
BOOL_32 disk_read(UNS_8 drv, void *buff, UNS_32 sector, UNS_32 sec_cnt)
{
  if (drv || !sec_cnt) return RES_PARERR;
  if (Stat & STA_NOINIT) return RES_NOTRDY;

  if (mci_read_blocks(g_mcidev,
                      buff,
                      sector,
                      sector + sec_cnt) < 0)
  {
    return RES_ERROR;
  }

  return RES_OK;
}


/***********************************************************************
 *
 * Function: disk_write
 *
 * Purpose: SD/MMC sector write
 *
 * Processing:
 *     Writes into SD/MMC sectors
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: RES_OK: Success
 *          RES_PARERR: Invalid parameter
 *          RES_NOTRDY: Disk not initialised
 *          RES_WRPRT: Write protected
 *          RES_ERROR: Error is writing
 *
 * Notes: None
 *
 **********************************************************************/
BOOL_32 disk_write(UNS_8 drv, void *buff, UNS_32 sector, UNS_32 sec_cnt)
{

  if (drv || !sec_cnt) return RES_PARERR;
  if (Stat & STA_NOINIT) return RES_NOTRDY;
  if (Stat & STA_PROTECT) return RES_WRPRT;

  if (mci_write_blocks(g_mcidev,
                       buff,
                       sector,
                       sector + sec_cnt) < 0)
  {
    return RES_ERROR;
  }

  return RES_OK;

}

