/***********************************************************************
 * $Id:: lpc31xx_vmem_driver.h 5902 2010-12-15 10:56:30Z ing03005      $
 *
 * Project: LPC31XX Virtual memory management routines
 *
 * Description:
 *     This file contains driver support for the LPC313X Vmem.
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

#ifndef LPC31XX_VMEM_DRIVER_H
#define LPC31XX_VMEM_DRIVER_H

#include "lpc_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define CACHE_DLINESIZE                32    /* 32 Bytes D-Cache line size */
#define CACHE_DLIMIT                   16384 /* 16-K D-Cache */

/**
 * Macros to help with allocation of DMA-capable buffers
 * Usage: DMA_BUFFER char mybuffer[40];
 **/
#ifdef __GNUC__
#define DMA_BUFFER                     __attribute__((aligned(CACHE_DLINESIZE)))
#elif defined(__CC_ARM)
#define DMA_BUFFER                     __align(CACHE_DLINESIZE)
#endif

/***********************************************************************
 * VMem driver API functions
 **********************************************************************/
/* Get VirtAddr corresponding to PhyAddr */
void * lpc31xx_pa_to_va(UNS_32 pa);

/* Get PhyAddr corresponding to VirtAddr */
UNS_32 lpc31xx_va_to_pa(void * va);

/* Get status of MMU/D-Cache */
int lpc31xx_is_cache_on(void);

/* Make cache coherent */
void lpc31xx_force_cache_coherence(void * start, void * end);

/* Invalidate D-Cache for the given virtual address range */
void lpc31xx_invalidate_dcache_range(UNS_32 *begin, UNS_32 *stop);

/* Flush D-Cache for the given virtual address range */
void lpc31xx_flush_dcache_range(UNS_32 *begin, UNS_32 *stop);

#ifdef __cplusplus
}
#endif

#endif
