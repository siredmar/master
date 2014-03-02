/***********************************************************************
 * $Id:: lpc_cf_card.h 745 2008-05-13 19:59:29Z pdurgesh               $
 *
 * Project: CompactFlash card definitions
 *
 * Description:
 *     This file contains the structure definitions and manifest
 *     constants for the following component:
 *         CompactFlash cards
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

#ifndef LPC_CF_CARD_H
#define LPC_CF_CARD_H

#include "lpc_types.h"

#if defined (__cplusplus)
extern "C"
{
#endif

/***********************************************************************
 * CF structures
 **********************************************************************/

/* CF card memory mapped structure - used when the CF card is in
   memory mapped mode and supports 8-bit reads/writes */
typedef struct
{
  volatile CHAR   dataeven;
  volatile CHAR   error_features;
  volatile CHAR   sectorcount;
  volatile CHAR   sectornum;
  volatile CHAR   cyllo;
  volatile CHAR   cylhi;
  volatile CHAR   head;
  volatile CHAR   status_command;
  volatile CHAR   dupdataeven;
  volatile CHAR   dupdataodd;
  volatile CHAR   reserved1 [4];
  volatile CHAR   duperror_dup_features;
  volatile CHAR   altstat_devctl;
  volatile CHAR   driveraddr;
} CF_MMAPREG_T;

/* CompactFlash memory mapped mode interface register structure for
   the SDK7A400 (for 16-bit read accesses only) */
typedef struct
{
  volatile UNS_16 data;           /* CF read data */
  volatile UNS_16 sec_count_no;   /* CF sector count and low 8 bits
                                       of number (LBA mode) */
  volatile UNS_16 sec_mid;        /* CF sector middle 16 bits */
  volatile UNS_16 sec_high_sts;   /* Sector high 8 bits and status
                                       value */
  volatile UNS_16 dup_data;       /* Duplicated data */
  volatile UNS_16 rsvd1;
  volatile UNS_16 dup_error;      /* Duplicated error value */
  volatile UNS_16 dup_sts_da;     /* Duplicated status and drive
                                       address  */
} CF_STATUS_READ_REG_T;

/* CompactFlash memory mapped mode interface register structure for
   the SDK7A400 (for 16-bit write accesses) */
typedef struct
{
  volatile UNS_16 data;           /* CF write data */
  volatile UNS_16 sec_count_no;   /* CF sector count and low 8 bits
                                       of number (LBA mode) */
  volatile UNS_16 sec_mid;        /* CF sector middle 16 bits */
  volatile UNS_16 sec_high_cmd;   /* Sector high 8 bits and command
                                       value */
  volatile UNS_16 dup_data;       /* Duplicated data */
  volatile UNS_16 rsvd1;
  volatile UNS_16 dup_features;   /* Duplicated features value */
  volatile UNS_16 device_ctl;     /* Device control */
} CF_STATUS_WRITE_REG_T;

/* CF card attribute memory structure */
typedef struct
{
  volatile UNS_16 config_option;
  volatile UNS_16 card_config_sts;
  volatile UNS_16 pin_replacement;
  volatile UNS_16 socket_copy;
} CF_ATTB_T;

/***********************************************************************
 * CF status register defines
 **********************************************************************/

/* CompactFlash status register busy bit */
#define CF_BUSY            _BIT(7)
/* CompactFlash status register command ready bit */
#define CF_RDY             _BIT(6)
/* CompactFlash status register Write fault bit */
#define CF_DWF             _BIT(5)
/* CompactFlash status register card ready bit */
#define CF_DSC             _BIT(4)
/* CompactFlash status register data request bit */
#define CF_DRQ             _BIT(3)
/* CompactFlash status register correctable error bit */
#define CF_CORR            _BIT(2)
/* CompactFlash status register reserved bit, alwats 0 */
#define CF_IDX             _BIT(1)
/* CompactFlash status register noncorrectable error bit */
#define CF_ERR             _BIT(0)

/***********************************************************************
 * CF status register defines
 **********************************************************************/

/* CompactFlash error register bad block bit */
#define CF_BBK             _BIT(7)
/* CompactFlash error register bauncorrectable error bit */
#define CF_UNC             _BIT(6)
/* CompactFlash error register bad sector bit */
#define CF_IDNF            _BIT(4)
/* CompactFlash error register abort error bit */
#define CF_ABORT           _BIT(2)
/* CompactFlash error register general error bit */
#define CF_AMNF            _BIT(1)

/***********************************************************************
 * CF status register defines
 **********************************************************************/

/* CompactFlash head register LBA mode select bit */
#define CF_LBA             _BIT(4)
/* CompactFlash Device control register reset bit */
#define CF_RST             _BIT(2)
/* CompactFlash Device control register Interrupt enable bit */
#define CF_IEN             _BIT(1)

/***********************************************************************
 * CF command defines
 **********************************************************************/

/* CompactFlash Read sector command */
#define CFC_R_SECT         0x20
/* CompactFlash Write sector command */
#define CFC_W_SECT         0x30
/* CompactFlash card identification command */
#define CFC_IDENT          0xEC
/* CompactFlash Read multiple sectors command */
#define CFC_R_MULT         0xC4
/* CompactFlash Erase sector command */
#define CFC_E_SECT         0xC0
/* CompactFlash Set Multiple command */
#define CFC_S_MULT         0xC6

/***********************************************************************
 * Miscellaneous CF defines
 **********************************************************************/

#define CF_SECTOR_SIZE     512   /* Size of a CF sector */

/* CompactFlash LBA mode enable in the sector register */
#define CF_LBA_MODE        0x40

#if defined (__cplusplus)
}
#endif /*__cplusplus */

#endif /* LPC_CF_CARD_H */
