/***********************************************************************
* $Id:: lpc313x_dmac.h 2025 2009-07-22 22:56:13Z wellsk               $
*
* Project: LPC313X Simple DMA controller definitions
*
* Description:
*     This file contains the structure definitions and manifest
*     constants for the LPC313X chip family component:
*         DMA Controller
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

#ifndef LPC313X_DMAC_H
#define LPC313X_DMAC_H

#include "lpc_types.h"
#include "lpc313x_chip.h"

#ifdef __cplusplus
extern "C"
{
#endif

/***********************************************************************
* DMA controller Register Structures
**********************************************************************/

/* DMA channel Register Structure */
typedef volatile struct
{
  volatile UNS_32 source;       /* source address register (R/W) */
  volatile UNS_32 destination;  /* destination address register (R/W) */
  volatile UNS_32 length;       /* Transfer length register (R/W) */
  volatile UNS_32 config;       /* Configuration register (R/W) */
  volatile UNS_32 enable;       /* Channel Enable register (R/W) */
  volatile UNS_32 _unused1[2];  /* reserved (R) */
  volatile UNS_32 counter;      /* Counter status and clear register (WO) */
} DMA_CH_REGS_T;

/* DMA channel alternate Register Structure */
typedef volatile struct
{
  volatile UNS_32 alt_source;     /* Mirror source address register (R/W) */
  volatile UNS_32 alt_destination;/* Mirror destination address register (R/W) */
  volatile UNS_32 alt_length;     /* Mirror Transfer length register (R/W) */
  volatile UNS_32 alt_config;     /* Mirror Configuration register (R/W) */
} DMA_CH_ALT_REGS_T;

/* DMA Controller Module Register Structure */
typedef volatile struct
{
  DMA_CH_REGS_T channel[12];       /* 12 DMA channels (R/W) */
  DMA_CH_REGS_T _unused_ch[4];
  DMA_CH_ALT_REGS_T altchannel[12];/* Mirror channels (W-only) */
  DMA_CH_ALT_REGS_T _unused1[20];
  volatile UNS_32 alt_enable;      /* Alternative enable register (R/W) */
  volatile UNS_32 irq_status_clear;/* IRQ status info(R), and IRQ clear (W) */
  volatile UNS_32 irq_mask;        /* IRQ mask register. */
  volatile UNS_32 test_resp_status;
  volatile UNS_32 soft_int;        /* Instruction word register (WO) */
} DMAC_REGS_T;

#ifdef __cplusplus
}
#endif


/***********************************************************************
* Useful values
***********************************************************************/
/* Number of DMA channels */
#define DMA_MAX_CHANNELS        12

/***********************************************************************
* Channel TRANSFER_LENGTH register defines
***********************************************************************/
#define DMA_TX_LEN_MASK         0x1FFFFF

/***********************************************************************
* Channel CONFIGURATION register defines
***********************************************************************/
#define DMA_CFG_CIRC_BUF        _BIT(18)
#define DMA_CFG_CMP_CH_EN       _BIT(17)
#define DMA_CFG_CMP_CH_NR(n)    _SBF(13, ((n) & 0x7))
#define DMA_CFG_INV_ENDIAN      _BIT(12)
#define DMA_CFG_TX_WORD         _SBF(10, 0x00)
#define DMA_CFG_TX_HWORD        _SBF(10, 0x01)
#define DMA_CFG_TX_BYTE         _SBF(10, 0x02)
#define DMA_CFG_TX_BURST        _SBF(10, 0x03)
#define DMA_CFG_RD_SLV_NR(n)    _SBF(5, ((n) & 0x1F))
#define DMA_CFG_WR_SLV_NR(n)    _SBF(0, ((n) & 0x1F))

/***********************************************************************
* Channel ENABLE register defines
***********************************************************************/
#define DMA_CH_ENABLE           _BIT(0)

/***********************************************************************
* Channel ALT_ENABLE register defines
***********************************************************************/
#define DMA_ALT_CH_EN(n)        _BIT(((n) & 0xF))

/***********************************************************************
* Channel IRQ_STATUS register defines
***********************************************************************/
#define DMA_IRQ_ABORT           _BIT(31)
#define DMA_IRQ_SOFT            _BIT(30)
#define DMA_IRQ_DONE(n)         _BIT(2 * (n))
#define DMA_IRQ_HDONE(n)        _BIT(2 * (n) + 1)
#define DMA_IRQ_CH(n)           _SBF(2 * (n), 0x3)
#define DMA_IRQ_CH_MASK         0x3
#define DMA_IRQ_ALL_MASK        0xC0FFFFFF

#endif /* LPC313X_DMAC_H */
