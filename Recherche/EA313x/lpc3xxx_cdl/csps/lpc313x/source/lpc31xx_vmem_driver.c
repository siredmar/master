/***********************************************************************
 * $Id:: lpc31xx_vmem_driver.c 6166 2011-01-12 08:16:12Z ing02124      $
 *
 * Project: Virtual memory management driver
 *
 * Description:
 *     This file contains driver support for the LPC313x virtual memory
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
#include "lpc31xx_vmem_driver.h"
#include "lpc_arm922t_cp15_driver.h"
#include "string.h"

/***********************************************************************
 * Virtual memory driver package data
***********************************************************************/

/* Memory region Start and End */
struct vmregion
{
  UNS_32 start;
  UNS_32 end;
};

/* Memory region mapping structure */
struct vmem
{
  struct vmregion phy;
  struct vmregion virt;
};

/* List of virtual regions and corresponding physical regions */
static struct vmem lpc31xx_mem_regions[] =
{
  /* Physical Addr region */  /* Virtual addr region */
  {{0x11000000, 0x110FFFFF}, {0x11100000, 0x111FFFFF}},  /* IRAM 0 & 1 */
  {{0x12000000, 0x120FFFFF}, {0x12100000, 0x121FFFFF}},  /* ISROM-0 EROM-0 */
  {{0x20000000, 0x200FFFFF}, {0x20100000, 0x201FFFFF}},  /* External SRAM 0 & 1 */
  {{0x30000000, 0x3FFFFFFF}, {0x40000000, 0x4FFFFFFF}},  /* External SDRAM/DDR */
};

/***********************************************************************
 * Virtual Memroy driver private functions
 **********************************************************************/

/***********************************************************************
 *
 * Function: lpc31xx_get_mem_region
 *
 * Purpose: Search memory region list and get the required index
 *
 * Processing:
 *     Searches the lpc31xx_mem_regions structure and retrieves the
 *     index of the region to which parameter @addr belongs
 *
 * Parameters: @addr - Addr of location to which region to be found
 *             @phy  - if 0 the @addr holds virtual address else it
 *                     holds a physical address
 *
 * Outputs: None
 *
 * Returns: Index to the region to which @addr belongs
 *
 **********************************************************************/
static int lpc31xx_get_mem_region(UNS_32 addr, int phy)
{
  int i;
  for (i = 0; i < sizeof(lpc31xx_mem_regions) / sizeof(lpc31xx_mem_regions[0]); i ++)
  {
    struct vmregion * reg = phy ? &lpc31xx_mem_regions[i].phy : &lpc31xx_mem_regions[i].virt;
    if (addr >= reg->start && addr <= reg->end)
      return i;
  }
  return -1;
}

/***********************************************************************
 * Vmem driver public functions
 **********************************************************************/

/***********************************************************************
 *
 * Function: lpc31xx_pa_to_va
 *
 * Purpose: Gets the virtual address corresponding to a physical address
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     @pa   : Physical address to which the virtual address to be found
 *
 * Outputs: None
 *
 * Returns: Virtual address corresponding to @pa
 *
 * Notes: If the physical address @pa does not belong to any virtual
 *        address region the same address is returned
 *
 **********************************************************************/
void * lpc31xx_pa_to_va(UNS_32 pa)
{
  int index;
  index = lpc31xx_get_mem_region(pa, 1);
  if (index >= 0)
  {
    UNS_32 pstart = lpc31xx_mem_regions[index].phy.start;
    UNS_32 vstart = lpc31xx_mem_regions[index].virt.start;
    return (void *)((pa - pstart) + vstart);
  }
  return (void *) pa;
}

/***********************************************************************
 *
 * Function: lpc31xx_va_to_pa
 *
 * Purpose: Gets the physical address corresponding to a virtual address
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     @va   : Virtual address to which the physical address to be found
 *
 * Outputs: None
 *
 * Returns: Physical address corresponding to @va
 *
 * Notes: If the virtual address @va does not belong to any physical
 *        address region the same address is returned
 *
 **********************************************************************/
UNS_32 lpc31xx_va_to_pa(void * va)
{
  int index;
  index = lpc31xx_get_mem_region((UNS_32)va, 0);
  if (index >= 0)
  {
    UNS_32 pstart = lpc31xx_mem_regions[index].phy.start;
    UNS_32 vstart = lpc31xx_mem_regions[index].virt.start;
    return ((UNS_32)va - vstart) + pstart;
  }
  return (UNS_32) va;
}

/***********************************************************************
 *
 * Function: lpc31xx_is_cache_on
 *
 * Purpose: Checks if cache(MMU) is on
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: 0 - When Cache(MMU) is off, 1 - When cache(MMU) is on
 *
 * Notes: When we say "cache on" we check if MMU and D-Cache are on
 *        and the code is running inside a virtual memory region
 *
 **********************************************************************/
int lpc31xx_is_cache_on(void)
{
  int index;
  index = lpc31xx_get_mem_region((UNS_32)lpc31xx_is_cache_on, 0);
  return !(index < 0);
}

/***********************************************************************
 *
 * Function: lpc31xx_force_cache_coherence
 *
 * Purpose:
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     @start   : Start address of data memory region
 *     @end     : End address of data memory region
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 **********************************************************************/
void lpc31xx_force_cache_coherence(void * start, void * end)
{
  if (lpc31xx_is_cache_on())
  {
    /* Call CP15 driver to do the task */
    cp15_force_cache_coherence(start, end);
  }
}

/***********************************************************************
 *
 * Function: lpc31xx_flush_dcache_range
 *
 * Purpose: Flush the D-Cache for the given address range
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     @start   : Start address of data memory region
 *     @end     : End address of data memory region
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 **********************************************************************/
void lpc31xx_flush_dcache_range(UNS_32 *begin, UNS_32 *stop)
{
  UNS_32 start = (UNS_32) begin;
  UNS_32 end = (UNS_32) stop;
#if defined(__CC_ARM)
  UNS_32 r0_val = start;
#endif
  if (!lpc31xx_is_cache_on())
  {
    return ;
  }

  start = start & ~(CACHE_DLINESIZE - 1);
#ifdef __GNUC__
  for (; start < end; start += CACHE_DLINESIZE)
  {
__asm__ volatile("    MOV r0, %0     " : : "r"(start): "r0");
    asm("    MCR p15, 0, r0, c7, c14, 1    ");   /* Flush D-Cache line */
  }
  asm("    MCR p15, 0, r0, c7, c10, 4    ");  /* Flush Write Buffer */
#elif defined(__CC_ARM)
  for (; start < end; start += CACHE_DLINESIZE)
  {
    __asm
    {
      MOV r0_val, start
      MCR p15, 0, r0_val, c7, c14, 1
    }
  }
  __asm {    MCR p15, 0, r0_val, c7, c10, 4    }; /* Flush Write Buffer */
#else
#error Unsupported tool!
#endif
}

/***********************************************************************
 *
 * Function: lpc31xx_invalidate_dcache_range
 *
 * Purpose: Invalidates D-Cache for the given virtual address range
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     @start   : Start address of data memory region
 *     @end     : End address of data memory region
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes:  Handling non-cache-line aligned buffers
 *		   is very tricky. This function has a software workaround
 *		   that will take care of such a situation.
 *		   Users must use DMA_BUFFER to declare the
 *		   DMA buffers to do an optimized DMA transfers.
 *
 **********************************************************************/
void lpc31xx_invalidate_dcache_range(UNS_32 *begin, UNS_32 *stop)
{
  UNS_32 start = (UNS_32) begin;
  UNS_32 end = (UNS_32) stop;
  void * pstart, *pstop;
  UNS_8 tbuf[CACHE_DLINESIZE];
  UNS_32 sextra, eextra;
#if defined(__CC_ARM)
  UNS_32 r0_val = start;
#endif

  /* If D-Cache and MMU are not used just return */
  if (!(lpc31xx_is_cache_on()))
  {
    return ;
  }

  pstart = (void *) lpc31xx_va_to_pa(begin);
  pstop  = (void *) lpc31xx_va_to_pa(stop);
  if (begin == pstart || stop == pstop)
  {
    /* Inconsistant region */
    return ;
  }
  sextra = start & (CACHE_DLINESIZE - 1);
  eextra = end   & (CACHE_DLINESIZE - 1);
#ifdef __GNUC__
  if (sextra)
  {
    memcpy(tbuf, pstart, CACHE_DLINESIZE - sextra);
__asm__ volatile("    MOV r0, %0    " : : "r"(start): "r0");
    asm("    MCR p15, 0, r0, c7, c10, 1    ");  /* Clean D-Entry */
    memcpy(pstart, tbuf, CACHE_DLINESIZE - sextra);
  }

  if (eextra)
  {
    memcpy(tbuf, (void *)((UNS_32)pstop & ~(CACHE_DLINESIZE - 1)), eextra);
__asm__ volatile("    MOV r0, %0    " : : "r"(end): "r0");
    asm("    MCR p15, 0, r0, c7, c10, 1    ");  /* Clean D-Entry */
    memcpy((void *)((UNS_32)pstop & ~(CACHE_DLINESIZE - 1)), tbuf, eextra);
  }

  for (start = start & ~(CACHE_DLINESIZE - 1); start < end; start += CACHE_DLINESIZE)
  {
__asm__ volatile("    MOV r0, %0    " : : "r"(start): "r0");
    asm("    MCR p15, 0, r0, c7, c6, 1    ");  /* Invalidate D-Entry */
  }
  asm("    MCR p15, 0, r0, c7, c10, 4    ");  /* Drain Write Buffer */
#elif defined(__CC_ARM)
  if (sextra)
  {
    memcpy(tbuf, pstart, CACHE_DLINESIZE - sextra);
    __asm
    {
      MOV r0_val, start
      MCR p15, 0, r0_val, c7, c10, 1
    }; /* Clean D-Entry */
    memcpy(pstart, tbuf, CACHE_DLINESIZE - sextra);
  }

  if (eextra)
  {
    memcpy(tbuf, (void *)((UNS_32)pstop & ~(CACHE_DLINESIZE - 1)), eextra);
    __asm
    {
      MOV r0_val, end
      MCR p15, 0, r0_val, c7, c10, 1
    }; /* Clean D-Entry */
    memcpy((void *)((UNS_32)pstop & ~(CACHE_DLINESIZE - 1)), tbuf, eextra);
  }

  for (start = start & ~(CACHE_DLINESIZE - 1); start < end; start += CACHE_DLINESIZE)
  {
    __asm
    {
      MOV r0_val, start
      MCR p15, 0, r0_val, c7, c6, 1
    } /* Clean D-Entry */
  }
  __asm {    MCR p15, 0, r0_val, c7, c10, 4    }; /* Flush Write Buffer */
#else
#error Unsupported tool!
#endif
}
