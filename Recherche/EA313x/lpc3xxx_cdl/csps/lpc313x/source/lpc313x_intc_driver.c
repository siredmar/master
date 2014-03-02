/***********************************************************************
 * $Id:: lpc313x_intc_driver.c 1180 2008-09-11 19:32:40Z pdurgesh      $
 *
 * Project: LPC313X interrupt driver
 *
 * Description:
 *     This file contains driver support for the LPC313X
 *     interrupt driver.
 *
 * Notes:
 *     This driver requires that the CP15 MMU driver is correctly
 *     working.
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
***********************************************************************/

#include "lpc_arm_arch.h"
/* CP15 driver for ARM922 & ARM926 is same, hence we will re-use.*/
#include "lpc_arm922t_cp15_driver.h"
#include "lpc313x_cgu_driver.h"
#include "lpc313x_intc_driver.h"

/***********************************************************************
 * Interrupt driver package data
***********************************************************************/

/* External vector jump addresses - setting one of these addresses with
   a new address of a function will cause the new function to be called
   when the interrupt or exception occurs */
extern UNS_32 lpc313x_reset_vector;
extern UNS_32 vec_reset_handler;
extern UNS_32 vec_undefined_handler;
extern UNS_32 vec_swi_handler;
extern UNS_32 vec_prefetch_handler;
extern UNS_32 vec_abort_handler;
extern UNS_32 vec_irq_handler;
extern UNS_32 vec_fiq_handler;

/* structure to store interrupt controller driver variables */
typedef struct
{
  /* indicates the driver init state */
  BOOL_32 init;
  /* Pointer to logical interrupt vector area (writable) */
  UNS_32 *vecarea;
  /* INTC regs pointer*/
  INTC_REGS_T* pregs;

} INTC_CTRL_DRV_T;

/* Array of Interrupt handlers */
PFV g_irq_func_ptrs[IRQ_END_OF_INTERRUPTS + 1];

/* Interrupt controller data structure */
INTC_CTRL_DRV_T g_intc_drv;

/***********************************************************************
 * Vectored Interrupt driver private functions
***********************************************************************/


/***********************************************************************
 *
 * Function: int_write_arm_vec_table
 *
 * Purpose: Writes the vector table and jump addresses to vector area
 *
 * Processing:
 *     Copy the shadowed image of the interrupt and exception vector
 *     table from memory to the vector jump area (usually at location
 *     0x00000000). Force out any cached values to external memory.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes:
 *     Ideally, we would check the state of the V bit in the CP15
 *     coprocessor register 1 to determine the address of the where
 *     the vector area is located. If that bit was set, the vectors
 *     would be located at address 0xFFFF0000 instead of 0x00000000.
 *     This function assumes that the vector area is at 0x00000000.
 *
 **********************************************************************/
static void int_write_arm_vec_table(void)
{
  UNS_32 *dst, *dstsave, *src;
  INT_32 vecsize;
  UNS_32 high_vector;

  /* If vector address is automatic address, compute address */
  dst = g_intc_drv.vecarea;
  if ((UNS_32) dst == 0xFFFFFFFF)
  {
    /* Assume that vector table is located at low vector
       (0x00000000) address */
    dst = (UNS_32 *) ARM_RESET_VEC;

    /* Check status of high vector bit in MMU control register and
       set destination address of vector table to high vector
       address if bit is set */
    high_vector = cp15_get_mmu_control_reg();

    /* If high bit is set, use high vector addresses instead */
    if ((high_vector & ARM922T_MMU_CONTROL_V) != 0)
    {
      dst = (UNS_32 *) 0xFFFF0000;
    }
  }

  /* Copy vector block to interrupt vector area */
  dstsave = dst;
  for (src = (UNS_32 *) & lpc313x_reset_vector;
       src <= (UNS_32 *) &vec_fiq_handler; src++)
  {
    *dst = *src;
    dst++;
  }

  /* Write out cached vector table to memory */
  vecsize = ((INT_32) & vec_fiq_handler -
             (INT_32) & lpc313x_reset_vector) / 4;
  cp15_force_cache_coherence(dstsave, (dstsave + vecsize));
}


/***********************************************************************
 * Interrupt driver public functions
***********************************************************************/

/***********************************************************************
 *
 * Function: int_initialize
 *
 * Purpose: Initialize the interrupt controller
 *
 * Processing:
 *     For all IRQ interrupt sources, clear the dispatcher jump address
 *     and disable the interrupt in the interrupt controller. Copy the
 *     vector table and vector branch instructions to the interrupt
 *     and exception area with a call to int_write_table.
 *
 * Parameters:
 *     vectbladdr: Pointer to interrupt vector area, or 0xFFFFFFFF to
 *                 have driver determine address
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void int_initialize(UNS_32 vectbladdr)
{
  UNS_32 source;

  if (g_intc_drv.init == FALSE)
  {
    /* enable clock to interrupt controller */
    cgu_clk_en_dis(CGU_SB_AHB2INTC_CLK_ID, TRUE);
    cgu_clk_en_dis(CGU_SB_INTC_CLK_ID, TRUE);

    /* Initialize interrupt controller*/
    g_intc_drv.pregs = INTC;

    /* Set the vector base (we don't use direct vectoring, so this is 0) */
    g_intc_drv.pregs->irq_vec = 0x00000000;
    g_intc_drv.pregs->fiq_vec = 0x00000000;

    /* Set the priority treshold to 0, i.e. don't mask any interrupt */
    /* on the basis of priority level, for both targets (IRQ/FIQ)    */
    g_intc_drv.pregs->irq_prio_mask = 0;
    g_intc_drv.pregs->fiq_prio_mask = 0;

    /* Clear the IRQ vector table and disable all interrupts. Start from
    index 1 since 0 is unused.*/
    g_irq_func_ptrs[0] = (PFV) NULL;
    for (source = 1; source <= IRQ_END_OF_INTERRUPTS; source++)
    {
      /* Note: start counting from 0 since the first configuration     */
      /* register in the table corresponds to interrupt request line 1 */

      /* Set the initial control values */

      /* Initialize as high-active, Disable the interrupt,
       * Set target to IRQ , Set priority level to 1 (= lowest) for
       * all the interrupt lines */
      g_intc_drv.pregs->request[source] =
        INTC_REQ_WE_ACT_LOW |
        INTC_REQ_WE_ENABLE |
        INTC_REQ_TARGET_IRQ |
        INTC_REQ_PRIO_LVL(1) |
        INTC_REQ_WE_PRIO_LVL;

      g_irq_func_ptrs[source] = (PFV) NULL;
    }

    /* Save user passed vector area pointer */
    g_intc_drv.vecarea = (UNS_32 *) vectbladdr;

    /* write ARM vector table */
    int_write_arm_vec_table();
  }
}

/***********************************************************************
 *
 * Function: int_install_arm_vec_handler
 *
 * Purpose: Install an new ARM exception vector handler
 *
 * Processing:
 *     If the passed fiq_handler_ptr pointer is not NULL, then
 *     set the handler jump address for the specific interrupt or
 *     exception to handler_ptr.  Recopy the vector table and vector
 *     branch instructions to the interrupt and exception area with a
 *     call to int_write_table.
 *
 * Parameters:
 *     handler_id  : Must be an enumeration of type VECTOR_T
 *     handler_ptr : Pointer to new interrupt or exception handler
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void int_install_arm_vec_handler(VECTOR_T handler_id,
                                 PFV handler_ptr)
{
  /* Update address only if it is not NULL */
  if (handler_ptr != (PFV) NULL)
  {
    switch (handler_id)
    {
      case RESET_VEC:
        vec_reset_handler = (UNS_32) handler_ptr;
        cp15_force_cache_coherence(
          (UNS_32 *) &vec_reset_handler,
          (UNS_32 *) &vec_reset_handler);
        break;

      case UNDEFINED_INST_VEC:
        vec_undefined_handler = (UNS_32) handler_ptr;
        cp15_force_cache_coherence(
          (UNS_32 *) &vec_undefined_handler,
          (UNS_32 *) &vec_undefined_handler);
        break;

      case SWI_VEC:
        vec_swi_handler = (UNS_32) handler_ptr;
        cp15_force_cache_coherence(
          (UNS_32 *) &vec_swi_handler,
          (UNS_32 *) &vec_swi_handler);
        break;

      case PREFETCH_ABORT_VEC:
        vec_prefetch_handler = (UNS_32) handler_ptr;
        cp15_force_cache_coherence(
          (UNS_32 *) &vec_prefetch_handler,
          (UNS_32 *) &vec_prefetch_handler);
        break;

      case DATA_ABORT_VEC:
        vec_abort_handler = (UNS_32) handler_ptr;
        cp15_force_cache_coherence(
          (UNS_32 *) &vec_abort_handler,
          (UNS_32 *) &vec_abort_handler);
        break;

      case IRQ_VEC:
        vec_irq_handler = (UNS_32) handler_ptr;
        cp15_force_cache_coherence(
          (UNS_32 *) &vec_irq_handler,
          (UNS_32 *) &vec_irq_handler);
        break;

      case FIQ_VEC:
        vec_fiq_handler = (UNS_32) handler_ptr;
        cp15_force_cache_coherence(
          (UNS_32 *) &vec_fiq_handler,
          (UNS_32 *) &vec_fiq_handler);
        break;

      default:
        break;
    }

    /* Update ARM vector table */
    int_write_arm_vec_table();
  }
}

/***********************************************************************
 *
 * Function: int_install_irq_handler
 *
 * Purpose: Install an IRQ interrupt handler for an internal interrupt
 *
 * Processing:
 *     For the selected interrupt, sets the function called as the
 *     passed value.
 *
 * Parameters:
 *     source   : Interrupt source of type INTERRUPT_SOURCE_T
 *     func_ptr : Pointer to a void function
 *
 * Outputs: None
 *
 * Returns: Returns TRUE or FALSE.
 *
 * Notes: None
 *
 **********************************************************************/
BOOL_32 int_install_irq_handler(INTERRUPT_SOURCE_T source,
                                PFV func_ptr)
{
  BOOL_32 ret_value = FALSE;

  if (source <= IRQ_END_OF_INTERRUPTS)
  {
    g_irq_func_ptrs[source] = func_ptr;
    ret_value = TRUE;
  }
  return ret_value;
}

/***********************************************************************
 *
 * Function: int_enable
 *
 * Purpose: Enable an interrupt
 *
 * Processing:
 *     Enables the interrupt in the controller for the selected source.
 *
 * Parameters:
 *     source   : Interrupt source of type INTERRUPT_SOURCE_T
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void int_enable(INTERRUPT_SOURCE_T source)
{
  g_intc_drv.pregs->request[source] = INTC_REQ_ENABLE | INTC_REQ_WE_ENABLE;
}

/***********************************************************************
 *
 * Function: int_disable
 *
 * Purpose: Disable an interrupt
 *
 * Processing:
 *     Masks the interrupt in the controller for the selected source.
 *
 * Parameters:
 *     source   : Interrupt source of type INTERRUPT_SOURCE_T
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void int_disable(INTERRUPT_SOURCE_T source)
{
  g_intc_drv.pregs->request[source] = INTC_REQ_WE_ENABLE;
}

/***********************************************************************
 *
 * Function: int_pending
 *
 * Purpose: Check to see if a unmasked interrupt is pending
 *
 * Processing:
 *     If the status for the selected interrupt source is set,
 *     a TRUE is returned. Otherwise, FALSE is returned.
 *
 * Parameters:
 *     source   : Interrupt source of type INTERRUPT_SOURCE_T
 *
 * Outputs: None
 *
 * Returns: Returns TRUE or FALSE.
 *
 * Notes: None
 *
 **********************************************************************/
BOOL_32 int_pending(INTERRUPT_SOURCE_T source)
{
  return (g_intc_drv.pregs->request[source] & INTC_REQ_PEND);
}


/***********************************************************************
 *
 * Function: int_enabled
 *
 * Purpose: Check to see if an interrupt is enabled
 *
 * Processing:
 *     If the selected interrupt source is enabled, a TRUE is returned.
 *     Otherwise, FALSE is returned.
 *
 * Parameters:
 *     source   : Interrupt source of type INTERRUPT_SOURCE_T
 *
 * Outputs: None
 *
 * Returns: Returns TRUE or FALSE.
 *
 * Notes: None
 *
 **********************************************************************/
BOOL_32 int_enabled(INTERRUPT_SOURCE_T source)
{
  return (g_intc_drv.pregs->request[source] & INTC_REQ_ENABLE);
}

/***********************************************************************
 *
 * Function: int_clear
 *
 * Purpose: Clear a pending (latched) interrupt
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     source   : Interrupt source of type INTERRUPT_SOURCE_T
 *
 * Outputs: None
 *
 * Returns: Returns TRUE or FALSE.
 *
 * Notes: None
 *
 **********************************************************************/
BOOL_32 int_clear(INTERRUPT_SOURCE_T source)
{
  g_intc_drv.pregs->request[source] = INTC_REQ_CLR_SWINT;

  return TRUE;
}

/***********************************************************************
 *
 * Function: int_setup_irq_fiq
 *
 * Purpose: Setup an interrupt as an IRQ (FALSE) or and FIQ (TRUE)
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     source  : Interrupt source of type INTERRUPT_SOURCE_T
 *     use_fiq : TRUE to use an FIQ interrupt type, FALSE for IRQ
 *
 * Outputs: None
 *
 * Returns: Returns TRUE or FALSE.
 *
 * Notes: None
 *
 **********************************************************************/
BOOL_32 int_setup_irq_fiq(INTERRUPT_SOURCE_T source,
                          BOOL_32 use_fiq)
{
  if (use_fiq == TRUE)
  {
    /* FIQ interrupt type */
    g_intc_drv.pregs->request[source] = INTC_REQ_TARGET_FIQ;
  }
  else
  {
    /* IRQ interrupt type */
    g_intc_drv.pregs->request[source] = INTC_REQ_TARGET_IRQ;
  }

  return TRUE;
}
