/***********************************************************************
 * $Id:: lpc313x_intc_driver.h 1342 2008-11-24 20:25:40Z pdurgesh      $
 *
 * Project: LPC313X interrupt driver
 *
 * Description:
 *     This file contains driver support for the LPC32xx interrupt
 *     driver.
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

#ifndef LPC313X_INTC_DRIVER_H
#define LPC313X_INTC_DRIVER_H

#include "lpc313x_intc.h"

#ifdef __cplusplus
extern "C"
{
#endif

/***********************************************************************
 * Interrupt driver enums & structures
 **********************************************************************/
/* ARM exception vector enumerations */
typedef enum
{
  RESET_VEC,
  UNDEFINED_INST_VEC,
  SWI_VEC,
  PREFETCH_ABORT_VEC,
  DATA_ABORT_VEC,
  IRQ_VEC,
  FIQ_VEC
} VECTOR_T;


/***********************************************************************
 * Interrupt driver functions
 **********************************************************************/

/* Initialize the interrupt controller */
void int_initialize(UNS_32 vectbladdr);

/* Install an new ARM exception vector handler */
void int_install_arm_vec_handler(VECTOR_T handler_id,
                                 PFV handler_ptr);

/* Install an IRQ interrupt function for an internal interrupt */
BOOL_32 int_install_irq_handler(INTERRUPT_SOURCE_T source,
                                PFV func_ptr);

/* Enable an interrupt */
void int_enable(INTERRUPT_SOURCE_T source);

/* Disable an interrupt */
void int_disable(INTERRUPT_SOURCE_T source);

/* Check to see if a unmasked interrupt is pending */
BOOL_32 int_pending(INTERRUPT_SOURCE_T source);

/* Check to see if an interrupt is enabled */
BOOL_32 int_enabled(INTERRUPT_SOURCE_T source);

/* Clear a pending (latched) interrupt */
BOOL_32 int_clear(INTERRUPT_SOURCE_T source);

/* Setup an interrupt as an IRQ (FALSE) or and FIQ (TRUE) */
BOOL_32 int_setup_irq_fiq(INTERRUPT_SOURCE_T source,
                          BOOL_32 use_fiq);

#ifdef __cplusplus
}
#endif

#endif /* LPC313X_INTC_DRIVER_H */
