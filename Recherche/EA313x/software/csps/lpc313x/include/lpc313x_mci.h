/***********************************************************************
* $Id:: lpc313x_mci.h 1756 2009-03-25 18:39:38Z pdurgesh              $
*
* Project: LPC313X Memory Card Interface (MCI) definitions
*
* Description:
*     This file contains the structure definitions and manifest
*     constants for the LPC313X chip family component:
*         Memory Card Interface (MCI)
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

#ifndef LPC313X_MCI_H
#define LPC313X_MCI_H

#include "lpc_types.h"
#include "lpc313x_chip.h"

#ifdef __cplusplus
extern "C"
{
#endif

/***********************************************************************
* Memory Card Interface (MCI) Register Structures
**********************************************************************/

/* Memory Card Interface (MCI) Module Register Structure */
typedef volatile struct
{
  volatile UNS_32 ctrl;    /* control register (R/W) */
  volatile UNS_32 pwren;   /* power-enable register (R/W) */
  volatile UNS_32 clkdiv;  /* clock-divider register (R/W) */
  volatile UNS_32 clksrc;  /* clock-source register (R/W) */
  volatile UNS_32 clkena;  /* clock enable register (R/W) */
  volatile UNS_32 tmout;   /* time-out  register (R/W) */
  volatile UNS_32 ctype;   /* card-type register (R/W) */
  volatile UNS_32 blksiz;  /* blck-size register (R/W) */
  volatile UNS_32 bytcnt;  /* byte-count register (R/W) */
  volatile UNS_32 intmsk;  /* interrupt-mask    register (R/W) */
  volatile UNS_32 cmdarg;  /* command-argument register (R/W) */
  volatile UNS_32 cmd;     /* command register (R/W) */
  volatile UNS_32 resp0;   /* response-0 register (R) */
  volatile UNS_32 resp1;   /* response-1 register (R) */
  volatile UNS_32 resp2;   /* response-2 register (R) */
  volatile UNS_32 resp3;   /* response-3 register (R) */
  volatile UNS_32 mintsts; /* masked interrupt-status register (R) */
  volatile UNS_32 rintsts; /* raw interrupt-status register (R/W) */
  volatile UNS_32 status;  /* status; mainly for debug register (R) */
  volatile UNS_32 fifoth;  /* FIFO treshold register (R/W) */
  volatile UNS_32 cdetec;  /* card-detect   register (R) */
  volatile UNS_32 wrtprt;  /* write-protect register (R) */
  volatile UNS_32 gpio;    /* GPIO register (R/W) */
  volatile UNS_32 tcbcnt;  /* transferred CIU card byte count register (R) */
  volatile UNS_32 tbbcnt;  /* transferred host/DMA to/from BIU-FIFO byte count register (R) */
  volatile UNS_32 debnce;  /* card detect debounce  register (R/W) */
  volatile UNS_32 usrid;   /* user ID register (R/W) */
  volatile UNS_32 verid;   /* synopsys version ID register (R) */
  volatile UNS_32 hcon;    /* hardware configuration register (R) */
  volatile UNS_32 reserved[35]; /* Data FIFO (R) */
  volatile UNS_32 data;    /* Data FIFO (R) */
} MCI_REGS_T;

#ifdef __cplusplus
}
#endif
/***********************************************************************
* useful defines
***********************************************************************/
#define SD_MMC_ENUM_CLOCK       400000
#define MMC_MAX_CLOCK           20000000
#define MMC_LOW_BUS_MAX_CLOCK   26000000
#define MMC_HIGH_BUS_MAX_CLOCK  52000000
#define SD_MAX_CLOCK            25000000
#define MCI_FIFO_SZ             32
#define SYS_REG_SD_CARD_DELAY   0x1B
#define SYS_REG_MMC_CARD_DELAY  0x16

/***********************************************************************
* Control register defines
***********************************************************************/
#define MCI_CTRL_CEATA_INT_EN   _BIT(11)
#define MCI_CTRL_SEND_AS_CCSD   _BIT(10)
#define MCI_CTRL_SEND_CCSD      _BIT(9)
#define MCI_CTRL_ABRT_READ_DATA _BIT(8)
#define MCI_CTRL_SEND_IRQ_RESP  _BIT(7)
#define MCI_CTRL_READ_WAIT      _BIT(6)
#define MCI_CTRL_DMA_ENABLE     _BIT(5)
#define MCI_CTRL_INT_ENABLE     _BIT(4)
#define MCI_CTRL_DMA_RESET      _BIT(2)
#define MCI_CTRL_FIFO_RESET     _BIT(1)
#define MCI_CTRL_RESET          _BIT(0)

/***********************************************************************
* Clock Enable register defines
***********************************************************************/
#define MCI_CLKEN_LOW_PWR      _BIT(16)
#define MCI_CLKEN_ENABLE       _BIT(0)

/***********************************************************************
* time-out register defines
***********************************************************************/
#define MCI_TMOUT_DATA(n)      _SBF(8, (n))
#define MCI_TMOUT_DATA_MSK     0xFFFFFF00
#define MCI_TMOUT_RESP(n)      ((n) & 0xFF)
#define MCI_TMOUT_RESP_MSK     0xFF

/***********************************************************************
* card-type register defines
***********************************************************************/
#define MCI_CTYPE_8BIT         _BIT(16)
#define MCI_CTYPE_4BIT         _BIT(0)

/***********************************************************************
* Interrupt status & mask register defines
***********************************************************************/
#define MCI_INT_SDIO           _BIT(16)
#define MCI_INT_EBE            _BIT(15)
#define MCI_INT_ACD            _BIT(14)
#define MCI_INT_SBE            _BIT(13)
#define MCI_INT_HLE            _BIT(12)
#define MCI_INT_FRUN           _BIT(11)
#define MCI_INT_HTO            _BIT(10)
#define MCI_INT_DTO            _BIT(9)
#define MCI_INT_RTO            _BIT(8)
#define MCI_INT_DCRC           _BIT(7)
#define MCI_INT_RCRC           _BIT(6)
#define MCI_INT_RXDR           _BIT(5)
#define MCI_INT_TXDR           _BIT(4)
#define MCI_INT_DATA_OVER      _BIT(3)
#define MCI_INT_CMD_DONE       _BIT(2)
#define MCI_INT_RESP_ERR       _BIT(1)
#define MCI_INT_CD             _BIT(0)
#define MCI_INT_ERROR          0xbfc2

/***********************************************************************
* Command register defines
***********************************************************************/
#define MCI_CMD_START         _BIT(31)
#define MCI_CMD_CCS_EXP       _BIT(23)
#define MCI_CMD_CEATA_RD      _BIT(22)
#define MCI_CMD_UPD_CLK       _BIT(21)
#define MCI_CMD_UPD_CLK       _BIT(21)

#define MCI_CMD_INIT          _BIT(15)
#define MCI_CMD_STOP          _BIT(14)
#define MCI_CMD_PRV_DAT_WAIT  _BIT(13)
#define MCI_CMD_SEND_STOP     _BIT(12)
#define MCI_CMD_STRM_MODE     _BIT(11)
#define MCI_CMD_DAT_WR        _BIT(10)
#define MCI_CMD_DAT_EXP       _BIT(9)
#define MCI_CMD_RESP_CRC      _BIT(8)
#define MCI_CMD_RESP_LONG     _BIT(7)
#define MCI_CMD_RESP_EXP      _BIT(6)
#define MCI_CMD_INDX(n)       ((n) & 0x1F)

/***********************************************************************
* status register defines
***********************************************************************/
#define MCI_STS_GET_FCNT(x)	  (((x)>>17) & 0x1FF)

/**********************************************************************
* Macro to access MCI registers
**********************************************************************/
#define MCI        ((MCI_REGS_T*)SD_MMC_BASE)


#endif /* LPC313X_MCI_H */
