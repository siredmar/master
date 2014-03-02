/***********************************************************************
 * $Id:: lpc_arm922t_cp15_driver.c 3382 2010-05-05 23:58:22Z usb10132  $
 *
 * Project: ARM922T Coprocessor 15 driver
 *
 * Description:
 *     This file contains driver support for the MMU and cache
 *     coprocessor (15) of the ARM922T.
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

#include "lpc_arm922t_cp15_driver.h"

#ifdef __ICCARM__
#include "intrinsics.h"
#endif

/***********************************************************************
 * CP15 driver private data
 **********************************************************************/

/* The address translation functions of this driver require a saved
   pointer to the virtual base address of the MMU table. */
UNS_32 *virtual_tlb_addr;

/***********************************************************************
 * CP15 driver private functions and macros
 **********************************************************************/

#ifdef __ghs__
/* Simple GHS function returning the TTB register */
asm UNS_32 getttb(void)
{
  % reg ttb
  MRC     p15, 0, r0, c2, c0, 0
}

/* Simple GHS function returning the MMU control register */
asm UNS_32 getstatus(void)
{
  % reg status
  MRC     p15, 0, r0, c1, c0, 0
}

/* Simple GHS function used to support cache invalidation */
asm void invalcache(UNS_32 *adr)
{
  % reg adr
  MCR     p15, 0, adr, c7, c14, 1
  MCR     p15, 0, adr, c7, c5, 1
  % con adr
  MOV     r0, adr
  MCR     p15, 0, r0, c7, c14, 1
  MCR     p15, 0, r0, c7, c5, 1
}

/* Simple GHS function used to support cache invalidation */
asm void inval_all_cache(void)
{
  MOV     r0, 0
  MCR     p15, 0, r0, c7, c7, 0
}

/* Simple GHS function used to support translation table base
   register invalidation */
asm void invaltlb(UNS_32 *adr)
{
  % reg adr
  MCR     p15, 0, adr, c8, c5, 1
  NOP
  NOP
  % con adr
  MOV     r0, adr
  MCR     p15, 0, r0, c8, c5, 1
  NOP
  NOP
}

/* Simple GHS function used to invalidate all TLB entries */
asm void inval_all_tlb(void)
{
  MOV     r0, 0
  MCR     p15, 0, r0, c8, c7, 0
}

/* Simple GHS function used to enable/disable MMU */
asm void set_mmu(UNS_32 val)
{
  % reg val
  MCR     p15, 0, val, c1, c0, 0
}

/* Simple GHS function used to set TTB address */
asm void set_ttb(UNS_32 addr)
{
  % reg addr
  MCR     p15, 0, addr, c2, c0, 0
}

/* Simple GHS function used to set domain access control */
asm void set_dac(UNS_32 reg_val)
{
  % reg reg_val
  MCR     p15, 0, reg_val, c3, c0, 0
}
#endif

/***********************************************************************
 * CP15 driver public functions
 **********************************************************************/

/***********************************************************************
 *
 * Function: cp15_decode_level2
 *
 * Purpose:
 *     Given a level2 descriptor and a physical address, return
 *     the virtual address if the level2 descriptor can map the
 *     physical address.
 *
 * Processing:
 *     Translate and return the level 2 virtual address for
 *     the given physical address.
 *
 * Parameters:
 *     level2: MMU level 2 table value
 *     addr:   Physical address to be converted
 *
 * Outputs: None
 *
 * Returns:
 *      The virtual address or 0 if the address does not translate.
 *
 * Notes: None
 *
 **********************************************************************/
static UNS_32 cp15_decode_level2(UNS_32 level2,
                                 UNS_32 addr)
{
  UNS_32 dcd = 0;

  switch (level2 & ARM922T_L2D_TYPE_PAGE_MASK)
  {
    case ARM922T_L2D_TYPE_LARGE_PAGE:
      /* Large page table */
      if ((level2 & ARM922T_L2D_LPAGE_MASK) ==
          (addr & ARM922T_L2D_LPAGE_MASK))
      {
        dcd = (level2 & ARM922T_L2D_LPAGE_MASK) |
              (addr & (~ARM922T_L2D_LPAGE_MASK));
      }
      break;

    case ARM922T_L2D_TYPE_SMALL_PAGE:
      /* Small page table */
      if ((level2 & ARM922T_L2D_SPAGE_MASK) ==
          (addr & ARM922T_L2D_SPAGE_MASK))
      {
        dcd = (level2 & ARM922T_L2D_SPAGE_MASK) |
              (addr & (~ARM922T_L2D_SPAGE_MASK));
      }
      break;

    case ARM922T_L2D_TYPE_TINY_PAGE:
      /* Tiny page table */
      if ((level2 & ARM922T_L2D_TPAGE_MASK) ==
          (addr & ARM922T_L2D_TPAGE_MASK))
      {
        dcd = (level2 & ARM922T_L2D_TPAGE_MASK) |
              (addr & (~ARM922T_L2D_TPAGE_MASK));
      }

    default:
      break;
  }

  return dcd;
}

/***********************************************************************
 * CP15 driver public functions
 **********************************************************************/

/***********************************************************************
 *
 * Function: cp15_map_virtual_to_physical
 *
 * Purpose: Return a physical address for a passed virtual address
 *
 * Processing:
 *      Return (UNS_32)addr if MMU is turned off. Otherwise, read the
 *      address of the translation table from the translation table base
 *      address register. Use the upper 12 bits of the addr to index the
 *      translation table and read out the descriptor. If the descriptor
 *      is invalid, return 0. If the descriptor is for a 1 Meg section,
 *      read back the upper 12 bits of the physical address. The lower
 *      20 bits of the physical address is the lower 20 bits of the
 *      virtual address. If the descriptor is for a coarse page table,
 *      read the coarse page table descriptor and use the most
 *      significant 22 bits as the base address of the page table. If
 *      the descriptor is for a fine page table, read the fine page
 *      table descriptor and use the most significant 20 bits as the
 *      base address of the page table.
 *
 *      If not a section base, read the level 2 page descriptor from the
 *      page table.  If bits 1..0 of the level2 descriptor are 01, then
 *      it is a large page table descriptor. The most significant 16
 *      bits of the descriptor are the most significant 16 bits of the
 *      physical address; the least significant 16-bits of the virtual
 *      address are the least significant 16-bits of the address. If
 *      bits 1..0 of the level2 descriptor are 10, then it is a small
 *      page table descriptor. The most significant 20 bits of the
 *      level2 descriptor are the most significant 20 bits of the
 *      physical address; the least significant 12 bits are the least
 *      significant 12 bits of the physical address. If bits 1..0 of
 *      the level2 descriptor are 11, then it is a tiny page table
 *      descriptor.  The most significant 22 bits of the level2
 *      descriptor are the most significant 22 bits of the physical
 *      address; the least significant 10 bits are the least
 *      significant 10 bits of the physical address.  If bits 1..0
 *      of the level2 descriptor are 0, return 0 (invalid).
 *
 * Parameters:
 *     addr: The virtual address to be converted
 *
 * Outputs: None
 *
 * Returns:
 *      The physical address or 0 if the address does not translate.
 *
 * Notes: None
 *
 **********************************************************************/
UNS_32 cp15_map_virtual_to_physical(void *addr)
{
  register UNS_32 status;
  UNS_32 *tlb, tlb_entry, index, virtual_addr;
  UNS_32 *page_table, level2;

  virtual_addr = (UNS_32) addr;

  /* Read the MMU control register */
#ifdef __GNUC__
__asm__ volatile("MRC p15, 0, %0, c1, c0, 0" : "=r"(status));
#endif
#ifdef __ghs__
  status = getstatus();
#endif
#ifdef __arm
  __asm
  {
    MRC p15, 0, status, c1, c0, 0;
  }
#endif
#ifdef __ICCARM__
  /* IAR CC includes some "intrinsic" functions to access ARM CP regs */
  status = __MRC(15, 0, 1, 0, 0);
#endif

  /* Is MMU enabled? */
  if ((status & ARM922T_MMU_CONTROL_M) == 0)
  {
    /* MMU is off; virtual address is physical address */
    return virtual_addr;
  }

  /* Get TLB base address */
  tlb = virtual_tlb_addr;

  /*******************************************************************
   * Get the level 1 translation table entry
   * indexed by bits 31:20 of the address
   ******************************************************************/
  index = virtual_addr;
  index >>= 20;
  tlb_entry = tlb[index];
  /* Perform function based on level 1 page type or section */
  switch (tlb_entry & ARM922T_L1D_TYPE_PG_SN_MASK)
  {
    case ARM922T_L1D_TYPE_FAULT:
      /* Invalid section or page, fault */
      return 0;

    case ARM922T_L1D_TYPE_CPAGE:
      /* Coarse page tables */
      index = (virtual_addr >> 12) & ARM922T_CPT_INDEX_MASK;
      page_table = (UNS_32 *)(tlb_entry &
                              ARM922T_L2D_CP_BASE_MASK);
      level2 = page_table[index];
      break;

    case ARM922T_L1D_TYPE_SECTION:
      /* Section type */
      /***********************************************************
       * Section base -- upper 12 bits of entry is physical
       * memory base lower 20 bits of virtual address is offset
       * rom that base
       **********************************************************/
      return ((tlb_entry & ARM922T_L2D_SN_BASE_MASK) |
              (virtual_addr & ~(ARM922T_L2D_SN_BASE_MASK)));

    case ARM922T_L1D_TYPE_FPAGE:
      /* Fine page tables */
      index = (virtual_addr >> 10) & ARM922T_FPT_INDEX_MASK;
      page_table = (UNS_32 *)(tlb_entry &
                              ARM922T_L2D_FP_BASE_MASK);
      level2 = page_table[index];
      break;

    default:
      /* This should never happen */
      return 0;
  }

  switch (level2 & ARM922T_L2D_TYPE_PAGE_MASK)
  {
    case ARM922T_L2D_TYPE_LARGE_PAGE:
      /* Large page table */
      return ((level2 & ARM922T_L2D_LPAGE_MASK) |
              (virtual_addr & ~(ARM922T_L2D_LPAGE_MASK)));

    case ARM922T_L2D_TYPE_SMALL_PAGE:
      /* Small page table */
      return ((level2 & ARM922T_L2D_SPAGE_MASK) |
              (virtual_addr & ~(ARM922T_L2D_SPAGE_MASK)));

    case ARM922T_L2D_TYPE_TINY_PAGE:
      /* Tiny page table */
      return ((level2 & ARM922T_L2D_TPAGE_MASK) |
              (virtual_addr & ~(ARM922T_L2D_TPAGE_MASK)));

    default:
      break;
  }

  return 0;
}

/***********************************************************************
 *
 * Function: cp15_map_physical_to_virtual
 *
 * Purpose: Return a virtual address for a passed physical address
 *
 * Processing:
 *     Test if MMU is on, return if not.  Search for the virtual
 *     address of the provided physical address. If found, return a
 *     void pointer to virtual address.
 *
 * Parameters:
 *     addr: The physical address to be converted
 *
 * Outputs: None
 *
 * Returns:
 *      The virtual address or 0 if the address does not translate.
 *
 * Notes: None
 *
 **********************************************************************/
void * cp15_map_physical_to_virtual(UNS_32 addr)
{
  register UNS_32 status;
  UNS_32 *tlb, tlb_entry, index;
  UNS_32 *page_table, level2, index2;
  void *virtual_addr;

  /*******************************************************************
   * Do a linear search of the translation table until the level 1
   * descriptor corresponding to the physical address is found
   ******************************************************************/

  /* Read the control register */
#ifdef __GNUC__
__asm__ volatile("MRC p15, 0, %0, c1, c0, 0" : "=r"(status));
#endif
#ifdef __ghs__
  status = getstatus();
#endif
#ifdef __arm
  __asm
  {
    MRC p15, 0, status, c1, c0, 0;
  }
#endif
#ifdef __ICCARM__
  /* IAR CC includes some "intrinsic" functions to access ARM CP regs */
  status = __MRC(15, 0, 1, 0, 0);
#endif

  /* Is MMU enabled? */
  if ((status & ARM922T_MMU_CONTROL_M) == 0)
  {
    /* MMU is off; physical address is virtual address */
    return (void *)addr;
  }

  /* Get TLB base address */
  tlb = virtual_tlb_addr;

  /*******************************************************************
   * Search until found or all 4096 translation
   * table entries are examined.
   ******************************************************************/
  for (index = 0; index < ARM922T_TT_ENTRIES; index ++)
  {
    tlb_entry = tlb[index];
    switch (tlb_entry & ARM922T_L1D_TYPE_PG_SN_MASK)
    {
      case ARM922T_L1D_TYPE_FAULT:
        /* Invalid section or page, fault */
        break;

      case ARM922T_L1D_TYPE_CPAGE:
        /* Course page tables, loop through all entries */
        page_table = (UNS_32 *)(tlb_entry &
                                ARM922T_L2D_CP_BASE_MASK);
        for (index2 = 0; index2 < ARM922T_CPT_ENTRIES; index2++)
        {
          level2 = page_table[index2];
          if (level2)
          {
            virtual_addr = (void *)
                           cp15_decode_level2(level2, addr);

            if (virtual_addr)
            {
              return virtual_addr;
            }
          }
          break;
        }
        break;

      case ARM922T_L1D_TYPE_SECTION:
        /* Section type */
        /*******************************************************
         * Section base -- upper 12 bits of entry is physical
         * memory base lower 20 bits of virtual address is
         * offset from that base
         ******************************************************/
        if ((tlb_entry & ARM922T_L2D_SN_BASE_MASK)
            == (addr & ARM922T_L2D_SN_BASE_MASK))
        {
          return (void *)((index << 20) |
                          (addr & ~(ARM922T_L2D_SN_BASE_MASK)));
        }
        break;

      case ARM922T_L1D_TYPE_FPAGE:
        /* Fine page tables, loop through entries */
        page_table = (UNS_32 *)(tlb_entry &
                                ARM922T_L2D_FP_BASE_MASK);
        for (index2 = 0; index2 < ARM922T_FPT_ENTRIES; index2++)
        {
          level2 = page_table[index2];
          if (level2)
          {
            virtual_addr = (void *)
                           cp15_decode_level2(level2, addr);

            if (virtual_addr)
            {
              return virtual_addr;
            }
          }
        }
        break;

      default:
        break;
    }
  }

  return 0;
}

/***********************************************************************
 *
 * Function: cp15_force_cache_coherence
 *
 * Purpose:
 *     Force the CPU to recognize the block of code that was just
 *     written to memory between start_adr and end_adr even if caching
 *     and write buffering is on.
 *
 * Processing:
 *     Cache lines are 32-bytes (8 words); clean and invalidate each
 *     line of D-cache and invalidate each line of I-cache within the
 *     address range.
 *
 *     Invalidate the I-TLB within the the address range. The I-TLB has
 *     256 word granularity.
 *
 * Parameters:
 *     start_adr:  The first address in the code block
 *     end_adr:    The last address in the code block
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void cp15_force_cache_coherence(UNS_32 *start_adr,
                                UNS_32 *end_adr)
{
  register UNS_32 * addr;

  /*******************************************************************
   * Cache lines are 32-bytes (8 words); clean and invalidate each
   * line of D-cache and invalidate each line of I-cache within the
   * address range. Make sure addresses are 32-bit aligned.
   ******************************************************************/
  for (addr = (UNS_32 *)((UNS_32)start_adr & 0xFFFFFFE0);
       addr < end_adr;
       addr += 8)
  {
    /* p15 is MMU coprocessor, Cache OPS is c7, TLB OPS is c8 */
#ifdef __GNUC__
__asm__ volatile("MOV r0, %0" : : "r"(addr));
    /* Clean and Invalidate D-Cache single entry using MVA format */
    asm("MCR p15, 0, r0, c7, c14, 1");
    /* Invalidate I-Cache single entry using MVA format */
    asm("MCR p15, 0, r0, c7, c5, 1");
#endif
#ifdef __ghs__
    invalcache(addr);
#endif
#ifdef __arm
    UNS_32 trx;
    __asm
    {
      MOV     trx, addr
      /* Clean and Invalidate D-Cache single entry using MVA
         format */
      MCR     p15, 0, trx, c7, c14, 1
      /* Invalidate I-Cache single entry using MVA format */
      MCR     p15, 0, trx, c7, c5, 1
    }
#endif
#ifdef __ICCARM__

    /* Use IAR intrinsic functions */
    __MCR(15, 0, (UNS_32)addr, 7, 14, 1);
    __MCR(15, 0, (UNS_32)addr, 7, 5, 1);
#endif

  }
  /*******************************************************************
   * Invalidate the I-TLB within the the address range. The I-TLB has
   * 256 word granularity. Make sure addresses are '256 word' aligned.
   ******************************************************************/
  for (addr = (UNS_32 *)((UNS_32)start_adr & 0xFFFFFC00);
       addr < end_adr;
       addr += 256)
  {
#ifdef __GNUC__
__asm__ volatile("MOV r0, %0" : : "r"(addr));
    /* Invalidate I-TLB using MVA format */
    asm("MCR p15, 0, r0, c8, c5, 1");
    asm("NOP");
    asm("NOP");
#endif
#ifdef __ghs__
    invaltlb(addr);
#endif
#ifdef __arm
    UNS_32 trx;
    __asm
    {
      MOV     trx, addr
      /* Invalidate I-TLB using MVA format */
      MCR      p15, 0, trx, c8, c5, 1
      NOP
      NOP
    }
#endif
#ifdef __ICCARM__

    /* Invalidate I-TLB using MVA format */
    /* Use IAR intrinsic functions */
    __MCR(15, 0, (UNS_32)addr, 8, 5, 1);
    __no_operation();
    __no_operation();
#endif
  }
}

/***********************************************************************
 *
 * Function: cp15_init_mmu_trans_table
 *
 * Purpose: Initializes the MMU page table
 *
 * Processing:
 *     Return error if MMU is enabled. Return error if target
 *     Translation Table address is not 16K aligned. Clear the
 *     Translation Table area. Build the Translation Table from the
 *     initialization data in the Section Block array. Return no error.
 *
 * Parameters:
 *      tt:    address of Translation Table in RAM.
 *      ttsbp: address of the beginning of the initialization array
 *
 * Outputs: None.
 *
 * Returns:
 *     This function returns _ERROR when the MMU is enabled, or the
 *     target address is not 16K aligned. Otherwise, it returns
 *     _NO_ERROR.
 *
 * Notes:
 *     This function is not intended to be used when the MMU is
 *     enabled.
 *
 **********************************************************************/
BOOL_32 cp15_init_mmu_trans_table(TRANSTABLE_T *tt,
                                  TT_SECTION_BLOCK_T *ttsbp)
{
  register UNS_32 control;
  UNS_32 idx;
  UNS_32 va_idx;
  UNS_32 pa_addr;
  UNS_32 *uiptr;
  UNS_32 ret = _NO_ERROR;

  /*******************************************************************
   * The following check returns an error if the MMU is enabled.
   * This condition is not necessarily an error, but an existing
   * Translation Table for an enabled MMU should not be overwritten
   * while the MMU is enabled.
   ******************************************************************/

  /* Read the control register */
#ifdef __GNUC__
__asm__ volatile("MRC p15, 0, %0, c1, c0, 0" : "=r"(control));
#endif
#ifdef __ghs__
  control = getstatus();
#endif
#ifdef __arm
  __asm
  {
    MRC p15, 0, control, c1, c0, 0;
  }
#endif
#ifdef __ICCARM__
  /* Use IAR intrinsic functions */
  control = __MRC(15, 0, 1, 0, 0);
#endif

  /* Exit if MMU is already enabeld */
  if ((control & ARM922T_MMU_CONTROL_M) != 0)
  {
    return (_ERROR);
  }

  /* Make sure table address is on a 16K boundary */
  if (((INT_32) tt & ~(ARM922T_TT_ADDR_MASK)) != 0)
  {
    return (_ERROR);
  }

  /*******************************************************************
   * Clear the entire Translation Table.
   * This results in L1D_TYPE_FAULT being the default for any
   * uninitialized entries.
   ******************************************************************/
  uiptr = (UNS_32 *) tt;
  for (idx = 0; idx < ARM922T_TT_ENTRIES; idx++)
    *uiptr++ = ARM922T_L1D_TYPE_FAULT;

  /* Build the translation table from user provided
     TT_SECTION_BLOCK_TYPE array */
  while (ttsbp->num_sections != 0)
  {
    switch ((ttsbp->entry) & ARM922T_L1D_TYPE_PG_SN_MASK)
    {
      case ARM922T_L1D_TYPE_SECTION:
        va_idx = ttsbp->virt_addr >> 20;
        pa_addr = ttsbp->phys_addr & ARM922T_L2D_SN_BASE_MASK;
        for (idx = 0; idx < ttsbp->num_sections; idx++)
        {
          tt->vidx[va_idx] = (pa_addr | ttsbp->entry | 0x10);
          va_idx++;
          pa_addr += 0x100000;
        }
        break;

      case(ARM922T_L1D_TYPE_CPAGE):
        va_idx = ttsbp->virt_addr >> 20;
        pa_addr = ttsbp->phys_addr & ARM922T_L2D_CP_BASE_MASK;
        for (idx = 0; idx < ttsbp->num_sections; idx++)
        {
          tt->vidx[va_idx] = pa_addr | ttsbp->entry;
          va_idx++;
          pa_addr += 0x100000;
        }
        break;

      case ARM922T_L1D_TYPE_FAULT:
      default:
        break;
    }

    ttsbp++;
  }

  return ret;
}

/***********************************************************************
 *
 * Function: cp15_set_vmmu_addr
 *
 * Purpose: Set the virtual address of the MMU table
 *
 * Processing:
 *     Set the saved virtual MMU table address to the passed value.
 *
 * Parameters:
 *      addr: Virtual address of start of MMU table
 *
 * Outputs: None.
 *
 * Returns: Nothing
 *
 * Notes:
 *     This function must be called if the driver MMU functions are
 *     being used. This should be set after the call to the
 *     cp15_init_mmu_trans_table() function.
 *
 **********************************************************************/
void cp15_set_vmmu_addr(UNS_32 *addr)
{
  virtual_tlb_addr = addr;
}

/***********************************************************************
 *
 * Function: cp15_get_ttb
 *
 * Purpose: Return the physical address of the MMU translation table
 *
 * Processing:
 *     Read the TTB register from coprocessor 15 and return it to the
 *     caller.
 *
 * Parameters: None
 *
 * Outputs: None.
 *
 * Returns: The base address of the MMU translation table
 *
 * Notes: None
 *
 **********************************************************************/
UNS_32 *cp15_get_ttb(void)
{
  register UNS_32 ttb;

#ifdef __GNUC__
__asm__ volatile("MRC p15, 0, %0, c2, c0, 0" : "=r"(ttb));
#endif
#ifdef __ghs__
  ttb = getttb();
#endif
#ifdef __arm
  __asm
  {
    MRC p15, 0, ttb, c2, c0, 0;
  }
#endif
#ifdef __ICCARM__
  /* use IAR CC intrinsic function to read CP15 reg */
  ttb = __MRC(15, 0, 2, 0, 0);
#endif

  return (UNS_32 *) ttb;
}

/***********************************************************************
 *
 * Function: cp15_dcache_flush
 *
 * Purpose: Force an data cache flush
 *
 * Processing:
 *     Flush each data cache entry using the segment/index method.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void cp15_dcache_flush(void)
{
#ifdef __GNUC__

#endif
#ifdef __ghs__

#endif
#ifdef __arm
  INT_32 segment, index, comp, cache_sz, total_seg;

  __asm
  {
    MRC p15, 0, cache_sz, c0, c0, 1
  }

  /* For 8KB cache the size field is 4 and for 16 it is 5.
  */
  total_seg = (1 << (ARM922T_MMU_DC_SIZE(cache_sz) - 2));

  for (segment = 0; segment < total_seg; segment++)
  {
    for (index = 0; index < 64; index++)
    {
      /* For each segment and index, flush the data cache */
      comp = (segment << 5) | (index << 26);
      __asm
      {
        MCR p15, 0, comp, c8, c5, 1
      }
    }
  }
#endif
#ifdef __ICCARM__

#endif
}

/***********************************************************************
 *
 * Function: cp15_write_buffer_flush
 *
 * Purpose: Force an write buffer flush
 *
 * Processing:
 *     Flush the write buffer and wait for completion of the flush.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void cp15_write_buffer_flush(void)
{
#ifdef __GNUC__

#endif
#ifdef __ghs__

#endif
#ifdef __arm
  UNS_32 trx;
  __asm
  {
    MOV trx, #0
    MCR p15, 0, trx, c7, c10, 4
  }
#endif
#ifdef __ICCARM__

#endif
}

/***********************************************************************
 *
 * Function: cp15_mmu_enabled
 *
 * Purpose:
 *  Checks to see if the MMU is enabled
 *
 * Processing:
 *  Read the MMU control register and check if the MMU enable bit
 *  (bit 0) is set.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns:
 *  TRUE if the MMU is enabled
 *  FALSE if the MMU is disabled
 *
 * Notes: None
 *
 **********************************************************************/
BOOL_32 cp15_mmu_enabled(void)
{
  UNS_32 mmu_reg;

#ifdef __GNUC__
__asm__ volatile("MRC p15, 0, %0, c1, c0, 0" : "=r"(mmu_reg));
#endif

#ifdef __ghs__
  mmu_reg = getstatus();
#endif

#ifdef __arm
  __asm
  {
    MRC     p15, 0, mmu_reg, ARM922T_MMU_REG_CONTROL, c0, 0;
  }
#endif

#ifdef __ICCARM__
  /* use IAR CC intrinsic function to read CP15 reg */
  mmu_reg = __MRC(15, 0, 1, 0, 0);
#endif


  return ((mmu_reg & _BIT(0)) == _BIT(0));
}

/***********************************************************************
 *
 * Function: cp15_get_mmu_control_reg
 *
 * Purpose:
 *  To return the current value of  the MMU Coprocessor (CP15) Control
 *  register.
 *
 * Processing:
 *  Fetch the MMU control register to a variable and return it
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns:
 *  The current value of the MMU Control register (cp15) as an UNS_32
 *
 * Notes: None
 *
 **********************************************************************/
UNS_32 cp15_get_mmu_control_reg(void)
{
  register UNS_32 mmu_reg;

#ifdef __GNUC__
__asm__ volatile("MRC p15, 0, %0, c1, c0, 0" : "=r"(mmu_reg));
#endif

#ifdef __ghs__
  mmu_reg = getstatus();

#endif
#ifdef __arm
  __asm
  {
    MRC     p15, 0, mmu_reg, ARM922T_MMU_REG_CONTROL, c0, 0;
  }
#endif
#ifdef __ICCARM__
  /* use IAR CC intrinsic function to read CP15 reg */
  mmu_reg = __MRC(15, 0, 1, 0, 0);
#endif

  return (mmu_reg);
}

/***********************************************************************
 *
 * Function: cp15_set_mmu_control_reg
 *
 * Purpose:
 *  To set MMU Coprocessor (CP15) Control register.
 *
 * Processing:
 *  Set the MMU control register to a value passed as parameter.
 *
 * Parameters:
 *  mmu_reg - The value to be set in the MMU Control register (cp15).
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 **********************************************************************/
void cp15_set_mmu_control_reg(UNS_32 mmu_reg)
{
#ifdef __GNUC__
__asm__ volatile("MCR p15, 0, %0, c1, c0, 0" : "=r"(mmu_reg));
#endif

#ifdef __ghs__
  set_mmu(mmu_reg);
#endif

#ifdef __arm
  __asm
  {
    MCR     p15, 0, mmu_reg, ARM922T_MMU_REG_CONTROL, c0, 0
  }
#endif
#ifdef __ICCARM__

  /* Use IAR intrinsic functions */
  __MCR(15, 0, (UNS_32)mmu_reg, 1, 0, 0);
#endif
}

/***********************************************************************
 *
 * Function: cp15_set_mmu
 *
 * Purpose:
 *  To enable or disable the MMU as specified.
 *
 * Processing:
 *  Fetch the MMU control register to a variable. If the argument passed
 *  is true, set the MMU enable bit, otherwise, clear it. Write the
 *  resultant value back to the control register.
 *
 * Parameters:
 *  enable - TRUE if the MMU must be enabled
 *           FALSE if the MMU must be disabled
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void cp15_set_mmu(BOOL_32 enable)
{
  register UNS_32 mmu_reg;

  /* Read-Modify-Write the MMU register */
  mmu_reg = cp15_get_mmu_control_reg();

  if (enable)
  {
    mmu_reg |= ARM922T_MMU_CONTROL_M;
  }
  else
  {
    mmu_reg &= ~ARM922T_MMU_CONTROL_M;
  }

#ifdef __GNUC__
__asm__ volatile("MCR p15, 0, %0, c1, c0, 0" : "=r"(mmu_reg));
#endif

#ifdef __ghs__
  set_mmu(mmu_reg);
#endif

#ifdef __arm
  __asm
  {
    MCR     p15, 0, mmu_reg, ARM922T_MMU_REG_CONTROL, c0, 0
  }
#endif
#ifdef __ICCARM__

  /* Use IAR intrinsic functions */
  __MCR(15, 0, (UNS_32)mmu_reg, 1, 0, 0);
#endif

}

/***********************************************************************
 *
 * Function: cp15_invalidate_cache
 *
 * Purpose:
 *  Invalidates the Instruction and Data caches
 *
 * Processing:
 *  Use the ARM instruction to unconditionally invalidate the entire
 *  cache.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes:
 *  This function invalidates all cache data including dirty data (data
 *  that has been modified in cache but not yet written to main memory).
 *  Use with caution. See ARM922T TRM.
 *
 **********************************************************************/
void cp15_invalidate_cache(void)
{
#ifdef __GNUC__
  // Invalidate both I- and D-cache completely
  __asm__ volatile("MOV r0, #0");
  __asm__ volatile("MCR p15, 0, r0, c7, c7, 0");
#endif
#ifdef __ghs__
  inval_all_cache();
#endif

#ifdef __arm
  UNS_32 trx;
  __asm
  {
    MOV     trx, 0
    MCR     p15, 0, trx, c7, c7, 0
  }
#endif
#ifdef __ICCARM__
  {
    register UNS_32 temp = 0;

    /* Use IAR intrinsic functions */
    __MCR(15, 0, (UNS_32)temp, 7, 7, 0);
  }
#endif
}

/***********************************************************************
 *
 * Function: cp15_invalidate_tlb
 *
 * Purpose:
 *  Invalidates the Translation Lookaside Buffers
 *
 * Processing:
 *  Use the ARM instruction to unconditionally invalidate the I- and D-
 *  TLBs.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes:
 *  See the ARM922T TRM.
 *
 **********************************************************************/
void cp15_invalidate_tlb(void)
{
#ifdef __GNUC__
  __asm__ volatile
  (
    "MOV r0, #0" "\n\t"
    "MCR p15, 0, r0, c8, c7, 0" "\n\t"
    "NOP" "\n\t"
    "NOP" "\n\t"
    "NOP" "\n\t"
  );
#endif
#ifdef __ghs__
  inval_all_tlb();
#endif

#ifdef __arm
  UNS_32 trx;
  __asm
  {
    MOV     trx, 0
    // Invalidate TLBs
    MCR      p15, 0, trx, ARM922T_MMU_REG_TLB_OPS, c7, 0
    NOP
    NOP
    NOP
  }
#endif
#ifdef __ICCARM__
  {
    register UNS_32 temp = 0;

    /* Use IAR intrinsic functions */
    __MCR(15, 0, (UNS_32)temp, 8, 7, 0);
    __no_operation();
    __no_operation();
    __no_operation();
  }
#endif
}

/***********************************************************************
 *
 * Function: cp15_set_transtable_base
 *
 * Purpose:
 *  Sets the first-level translation table base address
 *
 * Processing:
 *  Masks out the lower 12 bits of the address passed. Writes register
 *  2 of CP15 with the base address passed as parameter.
 *
 * Parameters:
 *  addr - Translation table base address
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes:
 *  The address must be aligned on a 16K boundary. See ARM922T TRM.
 *
 **********************************************************************/
void cp15_set_transtable_base(UNS_32 addr)
{
  addr &= 0xFFFFC000;

#ifdef __GNUC__
__asm__ volatile("MCR p15, 0, %0, c2, c0, 0": "=r"(addr));
#endif

#ifdef __ghs__
  set_ttb(addr);
#endif

#ifdef __arm
  __asm
  {
    MCR     p15, 0, addr, ARM922T_MMU_REG_TTB, c0, 0
  }
#endif

#ifdef __ICCARM__
  /* Use IAR intrinsic functions */
  __MCR(15, 0, (UNS_32)addr, 2, 0, 0);
#endif
}

/***********************************************************************
 *
 * Function: cp15_set_icache
 *
 * Purpose:
 *  Enables or disables the instruction cache
 *
 * Processing:
 *  Fetch the MMU control register to a variable. If the argument passed
 *  is true, set the I-cache enable bit, otherwise, clear it. Write the
 *  resultant value back to the control register.
 *
 * Parameters:
 *  enable - TRUE if the I-cache must be enabled
 *           FALSE if the I-cache must be disabled
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void cp15_set_icache(BOOL_32 enable)
{
  UNS_32 mmu_reg;

  mmu_reg = cp15_get_mmu_control_reg();

  if (enable)
    mmu_reg |= ARM922T_MMU_CONTROL_I;
  else
    mmu_reg &= ~ARM922T_MMU_CONTROL_I;

#ifdef __GNUC__
__asm__ volatile("MCR p15, 0, %0, c1, c0, 0" : "=r"(mmu_reg));
#endif

#ifdef __ghs__
  set_mmu(mmu_reg);
#endif

#ifdef __arm
  __asm
  {
    MCR     p15, 0, mmu_reg, ARM922T_MMU_REG_CONTROL, c0, 0
  }
#endif
#ifdef __ICCARM__
  /* Use IAR intrinsic functions */
  __MCR(15, 0, (UNS_32)mmu_reg, 1, 0, 0);
#endif
}

/***********************************************************************
 *
 * Function: cp15_set_dcache
 *
 * Purpose:
 *  Enables or disables the data cache
 *
 * Processing:
 *  Fetch the MMU control register to a variable. If the argument passed
 *  is true, set the D-cache enable bit, otherwise, clear it. Write the
 *  resultant value back to the control register.
 *
 * Parameters:
 *  enable - TRUE if the D-cache must be enabled
 *           FALSE if the D-cache must be disabled
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void cp15_set_dcache(BOOL_32 enable)
{
  UNS_32 mmu_reg;

  mmu_reg = cp15_get_mmu_control_reg();

  if (enable)
    mmu_reg |= ARM922T_MMU_CONTROL_C;
  else
    mmu_reg &= ~ARM922T_MMU_CONTROL_C;

#ifdef __GNUC__
__asm__ volatile("MCR p15, 0, %0, c1, c0, 0" : "=r"(mmu_reg));
#endif

#ifdef __ghs__
  set_mmu(mmu_reg);
#endif

#ifdef __arm
  __asm
  {
    MCR     p15, 0, mmu_reg, ARM922T_MMU_REG_CONTROL, c0, 0
  }
#endif
#ifdef __ICCARM__
  /* Use IAR intrinsic functions */
  __MCR(15, 0, (UNS_32)mmu_reg, 1, 0, 0);
#endif
}

/***********************************************************************
 *
 * Function: cp15_set_domain_access
 *
 * Purpose:
 *  Define the access permissions for the 16 MMU domains.
 *
 * Processing:
 *  Use the ARM instruction to write the value passed as argument to
 *  the domain access control regsiter.
 *
 * Parameters:
 *  dac - 32-bit value encoded as follows:
 *
 *   31  29  27  25  23  21  19  17  15  13  11  9 8 7 6 5 4 3 2 1 0
 *   ---------------------------------------------------------------
 *  |15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
 *   ---------------------------------------------------------------
 *   For each domain (2 bits),
 *     00 - No access
 *     01 - Client
 *     10 - Reserved (same as no access)
 *     11 - Manager
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes:
 *  See the ARM922T TRM.
 *
 **********************************************************************/
void cp15_set_domain_access(UNS_32 dac)
{
#ifdef __GNUC__
__asm__ volatile("MCR p15, 0, %0, c3, c0, 0" : "=r"(dac));
#endif
#ifdef __ghs__
  set_dac(dac);
#endif

#ifdef __arm
  __asm
  {
    MCR     p15, 0, dac, ARM922T_MMU_REG_DAC, c0, 0
  }
#endif
#ifdef __ICCARM__
  /* Use IAR intrinsic functions */
  __MCR(15, 0, (UNS_32)dac, 3, 0, 0);
#endif
}

