/***********************************************************************
 * $Id:: lpc_heap.h 745 2008-05-13 19:59:29Z pdurgesh                  $
 *
 * Project: Simple heap manager
 *
 * Description:
 *     This package provides a simple heap manager with the first-fit
 *     algorithm. Before the package can be used, a call to
 *     lpc_heap_init must be performed with the base heap address and
 *     the size of the heap in bytes.
 *
 *     All returned allocation areas are 32-bit aligned.
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

#ifndef LPC_HEAP_H
#define LPC_HEAP_H

#include "lpc_types.h"

#if defined (__cplusplus)
extern "C"
{
#endif


/***********************************************************************
 * Heap statistics
 **********************************************************************/

/* Return the size of the heap area */
UNS_32 lpc_get_heapsize(void);

/* Return the size of the largest unallocated heap chunk */
UNS_32 lpc_get_largest_chunk(void);

/* Return the number of allocated items in the heap */
UNS_32 lpc_get_allocated_count(void);

/* Return the heap base address */
void *lpc_get_heap_base(void);

/***********************************************************************
 * Heap functions
 **********************************************************************/

/* Setup the heap area */
void lpc_heap_init(void *base_addr, UNS_32 heap_size);

/* Get an allocated area from the heap */
void *lpc_new(UNS_32 size_in_bytes);

/* Return an allocated area to the heap */
INT_32 lpc_free(void *free_addr);

#if defined (__cplusplus)
}
#endif /*__cplusplus */

#endif /* LPC_HEAP_H */
