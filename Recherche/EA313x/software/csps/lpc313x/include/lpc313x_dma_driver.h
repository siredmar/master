/***********************************************************************
 * $Id:: lpc313x_dma_driver.h 1180 2008-09-11 19:32:40Z pdurgesh       $
 *
 * Project: LPC313X DMA driver
 *
 * Description:
 *     This file contains driver support for the LPC313X DMA controller.
 *
 * Notes:
 *     This driver is a helper driver only for DMA support. To use DMA
 *     in a peripheral will require writing some DMA support code for
 *     that peripheral in it's driver using these supporting functions.
 *     This driver provides the following DMA support functions:
 *       - Initial DMA controller and driver setup
 *       - Channel allocation and deallocation
 *       - DMA controller enable/disable
 *       - Automatic DMA clock control
 *       - Interrupt clearning and routing to user functions
 *     This driver uses physical addresses only.
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

#ifndef LPC313X_DMA_DRIVER_H
#define LPC313X_DMA_DRIVER_H

#include "lpc313x_dmac.h"

#ifdef __cplusplus
extern "C"
{
#endif


/***********************************************************************
 * DMA driver types
 **********************************************************************/
typedef void (*PF_DMACB)(DMAC_REGS_T*);

/***********************************************************************
 * DMA driver functions
 **********************************************************************/

/* Initial DMA controller and driver */
STATUS dma_init(void);

/* Allocate a channel for DMA, use ch for selected channel (0 to 7) or
   -1 to use the highest priority available channel. Also sets up the
   user callback function for the channel's DMA interrupt. */
INT_32 dma_alloc_channel(INT_32 ch, PF_DMACB cb);

/* Return (free) an allocated DMA channel */
STATUS dma_free_channel(INT_32 ch);

/* Return pointer to DMA registers */
DMAC_REGS_T *dma_get_base(void);

#ifdef __cplusplus
}
#endif

#endif /* LPC313X_DMA_DRIVER_H */
