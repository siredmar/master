/***********************************************************************
 * $Id:: lpc313x_evt_driver.c 1388 2008-12-01 19:20:06Z pdurgesh       $
 *
 * Project: LPC313X event router driver
 *
 * Description:
 *     This file contains driver support for the LPC313X event router
 *     block.
 *
 * Notes:
 *     This driver is tightly coupled with interrupt controller driver.
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

#include "lpc313x_cgu_driver.h"
#include "lpc313x_intc_driver.h"
#include "lpc313x_evt_driver.h"

/***********************************************************************
 * Interrupt driver package data
***********************************************************************/

/* structure to store Event router driver variables */
typedef struct
{
  /* indicates the driver init state */
  BOOL_32 init;
  /* INTC regs pointer*/
  EVENT_ROUTER_REGS_T* pregs;

} EVTR_CTRL_DRV_T;

/* Interrupt controller data structure */
EVTR_CTRL_DRV_T g_evtr_drv;

/***********************************************************************
 * Event router driver private functions
***********************************************************************/


/***********************************************************************
 * Event router driver public functions
***********************************************************************/

/***********************************************************************
 *
 * Function: evtr_int_initialize
 *
 * Purpose: Initialize the Event router
 *
 * Processing:
 *     For all IRQ interrupt sources, clear the dispatcher jump address
 *     and disable the interrupt in the Event router. Copy the
 *     vector table and vector branch instructions to the interrupt
 *     and exception area with a call to int_write_table.
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void evtr_int_initialize(void)
{
  int i, j;

  if (g_evtr_drv.init == FALSE)
  {
    /* enable clock to Event router */
    cgu_clk_en_dis(CGU_SB_EVENT_ROUTER_PCLK_ID, TRUE);

    /* Initialize Event router*/
    g_evtr_drv.pregs = (EVENT_ROUTER_REGS_T*)EVENT_ROUTER_BASE;

    for (i = 0; i < EVT_MAX_VALID_BANKS; i++)
    {
      /* mask all events */
      g_evtr_drv.pregs->maskClr[i] = 0xFFFFFFFF;
      /* clear all pending events */
      g_evtr_drv.pregs->intClr[i] = 0xFFFFFFFF;

      for (j = 0; j < EVT_MAX_VALID_INT_OUT; j++)
      {
        /* mask all events */
        g_evtr_drv.pregs->intoutMaskClr[j][i] = 0xFFFFFFFF;
      }
    }
    /* Set the default Activation polarity & type for inernal signals*/
    g_evtr_drv.pregs->apr[0] = EVT_APR_BANK0_DEF;
    g_evtr_drv.pregs->apr[1] = EVT_APR_BANK1_DEF;
    g_evtr_drv.pregs->apr[2] = EVT_APR_BANK2_DEF;
    g_evtr_drv.pregs->apr[3] = EVT_APR_BANK3_DEF;

    g_evtr_drv.pregs->atr[0] = EVT_ATR_BANK0_DEF;
    g_evtr_drv.pregs->atr[1] = EVT_ATR_BANK1_DEF;
    g_evtr_drv.pregs->atr[2] = EVT_ATR_BANK2_DEF;
    g_evtr_drv.pregs->atr[3] = EVT_ATR_BANK3_DEF;

    /* make sure interrupt controller driver is initialised */
    int_initialize(0xFFFFFFFF);
  }
}


/***********************************************************************
 *
 * Function: evtr_int_install_handler
 *
 * Purpose: Install an interrupt handler for an external interrupt
 *
 * Processing:
 *     For the selected event, sets the activation polarity/type and
 *  also installs the passed function as vector handler for the selected
 *  output_vector.
 *
 * Parameters:
 *     evt_setup   : Event setup structure
 *
 * Outputs: None
 *
 * Returns: Returns TRUE or FALSE.
 *
 * Notes: User should call evtr_int_install_handler() twice
 *  for an event if the event should, both wakeup the chip from sleep
 *  and also cause interrupt.
 *
 **********************************************************************/
BOOL_32 evtr_int_install_handler(EVENT_SETUP_T* evt_setup)
{
  BOOL_32 ret_value = FALSE;
  int bank, bit_pos, i;

  if (evt_setup->pin <= EVT_LAST)
  {
    bank = EVT_GET_BANK(evt_setup->pin);
    bit_pos = evt_setup->pin & 0x1F;
    /* set activation polarity & type */
    switch (evt_setup->sense)
    {
      case ACTIVE_LOW:
        g_evtr_drv.pregs->apr[bank] &= ~_BIT(bit_pos);
        g_evtr_drv.pregs->atr[bank] &= ~_BIT(bit_pos);
        break;
      case ACTIVE_HIGH:
        g_evtr_drv.pregs->apr[bank] |= _BIT(bit_pos);
        g_evtr_drv.pregs->atr[bank] &= ~_BIT(bit_pos);
        break;
      case FALLING_EDGE:
        g_evtr_drv.pregs->apr[bank] &= ~_BIT(bit_pos);
        g_evtr_drv.pregs->atr[bank] |= _BIT(bit_pos);
        break;
      case RISING_EDGE:
        g_evtr_drv.pregs->apr[bank] |= _BIT(bit_pos);
        g_evtr_drv.pregs->atr[bank] |= _BIT(bit_pos);
        break;
      default:
        return FALSE;
    }
    /* if CGU wakeup event don't clear other vectors. So that an
    can wake the chip and also generate interrupt on selected
    vector out. User should call evtr_int_install_handler() twice
    for an event if he wants both wakeup and interrupt fucntion.
    */
    if (evt_setup->vector == EVT_VEC_CGU_WAKEUP)
    {
      g_evtr_drv.pregs->intoutMaskSet[EVT_VEC_CGU_WAKEUP][bank] = _BIT(bit_pos);
    }
    else
    {
      /* enable interrupt on the select vector output */
      for (i = 0; i < EVT_VEC_CGU_WAKEUP; i++)
      {
        if (evt_setup->vector == i)
        {
          g_evtr_drv.pregs->intoutMaskSet[i][bank] = _BIT(bit_pos);
          /* install interrupt handler if valid */
          if (evt_setup->func_ptr != NULL)
          {
            int_install_irq_handler((INTERRUPT_SOURCE_T)(i + 1), evt_setup->func_ptr);
            /* also enable vector interrupt in the interrupt controller */
            int_enable((INTERRUPT_SOURCE_T)(i + 1));
          }
        }
        else
        {
          g_evtr_drv.pregs->intoutMaskClr[i][bank] = _BIT(bit_pos);
        }
      }
    }

    ret_value = TRUE;
  }
  return ret_value;
}

/***********************************************************************
 *
 * Function: evtr_int_set_sense
 *
 * Purpose: Sets the activation polarity/type for an external interrupt
 *
 * Processing:
 *     For the selected event, sets the activation polarity/type.
 *
 * Parameters:
 *     source   : Event source of type EVENT_T
 *     sense    : Sensitivity of the event
 *
 * Outputs: None
 *
 * Returns: Returns TRUE or FALSE.
 *
 * Notes: None.
 *
 **********************************************************************/
BOOL_32 evtr_int_set_sense(EVENT_T source, EVENT_TYPE_T sense)
{
  BOOL_32 ret_value = FALSE;
  int bank, bit_pos;

  if (source <= EVT_LAST)
  {
    bank = EVT_GET_BANK(source);
    bit_pos = source & 0x1F;
    ret_value = TRUE;
    /* set activation polarity & type */
    switch (sense)
    {
      case ACTIVE_LOW:
        g_evtr_drv.pregs->apr[bank] &= ~_BIT(bit_pos);
        g_evtr_drv.pregs->atr[bank] &= ~_BIT(bit_pos);
        break;
      case ACTIVE_HIGH:
        g_evtr_drv.pregs->apr[bank] |= _BIT(bit_pos);
        g_evtr_drv.pregs->atr[bank] &= ~_BIT(bit_pos);
        break;
      case FALLING_EDGE:
        g_evtr_drv.pregs->apr[bank] &= ~_BIT(bit_pos);
        g_evtr_drv.pregs->atr[bank] |= _BIT(bit_pos);
        break;
      case RISING_EDGE:
        g_evtr_drv.pregs->apr[bank] |= _BIT(bit_pos);
        g_evtr_drv.pregs->atr[bank] |= _BIT(bit_pos);
        break;
      default:
        ret_value = FALSE;
        break;
    }
  }
  return ret_value;
}

/***********************************************************************
 *
 * Function: evtr_int_enable
 *
 * Purpose: Enable an interrupt
 *
 * Processing:
 *     Enables the interrupt in the event router for the selected source.
 *
 * Parameters:
 *     source   : Event source of type EVENT_T
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void evtr_int_enable(EVENT_T source)
{
  int bank, bit_pos;
  if (source <= EVT_LAST)
  {
    bank = EVT_GET_BANK(source);
    bit_pos = source & 0x1F;
    g_evtr_drv.pregs->maskSet[bank] = _BIT(bit_pos);
  }
}

/***********************************************************************
 *
 * Function: evtr_int_disable
 *
 * Purpose: Disable an interrupt
 *
 * Processing:
 *     Masks the interrupt in the event router for the selected source.
 *
 * Parameters:
 *     source   : Event source of type EVENT_T
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void evtr_int_disable(EVENT_T source)
{
  int bank, bit_pos;
  if (source <= EVT_LAST)
  {
    bank = EVT_GET_BANK(source);
    bit_pos = source & 0x1F;
    g_evtr_drv.pregs->maskClr[bank] = _BIT(bit_pos);
  }
}

/***********************************************************************
 *
 * Function: evtr_int_pending
 *
 * Purpose: Check to see if a unmasked interrupt is pending
 *
 * Processing:
 *     If the status for the selected interrupt source is set,
 *     a TRUE is returned. Otherwise, FALSE is returned.
 *
 * Parameters:
 *     source   : Interrupt source of type EVENT_T
 *
 * Outputs: None
 *
 * Returns: Returns TRUE or FALSE.
 *
 * Notes: None
 *
 **********************************************************************/
BOOL_32 evtr_int_pending(EVENT_T source)
{
  BOOL_32 ret_val = FALSE;
  int bank, bit_pos;
  if (source <= EVT_LAST)
  {
    bank = EVT_GET_BANK(source);
    bit_pos = source & 0x1F;
    ret_val = g_evtr_drv.pregs->pend[bank] & _BIT(bit_pos);
  }
  return ret_val;
}

/***********************************************************************
 *
 * Function: evtr_int_raw_pending
 *
 * Purpose: Check to see if a raw interrupt is pending
 *
 * Processing:
 *     If the status for the selected interrupt source is set,
 *     a TRUE is returned. Otherwise, FALSE is returned.
 *
 * Parameters:
 *     source   : Interrupt source of type EVENT_T
 *
 * Outputs: None
 *
 * Returns: Returns TRUE or FALSE.
 *
 * Notes: None
 *
 **********************************************************************/
BOOL_32 evtr_int_raw_pending(EVENT_T source)
{
  BOOL_32 ret_val = FALSE;
  int bank, bit_pos;
  if (source <= EVT_LAST)
  {
    bank = EVT_GET_BANK(source);
    bit_pos = source & 0x1F;
    ret_val = g_evtr_drv.pregs->rsr[bank] & _BIT(bit_pos);
  }
  return ret_val;
}

/***********************************************************************
 *
 * Function: evtr_int_enabled
 *
 * Purpose: Check to see if an interrupt is enabled
 *
 * Processing:
 *     If the selected interrupt source is enabled, a TRUE is returned.
 *     Otherwise, FALSE is returned.
 *
 * Parameters:
 *     source   : Interrupt source of type EVENT_T
 *
 * Outputs: None
 *
 * Returns: Returns TRUE or FALSE.
 *
 * Notes: None
 *
 **********************************************************************/
BOOL_32 evtr_int_enabled(EVENT_T source)
{
  BOOL_32 ret_val = FALSE;
  int bank, bit_pos;
  if (source <= EVT_LAST)
  {
    bank = EVT_GET_BANK(source);
    bit_pos = source & 0x1F;
    ret_val = ((g_evtr_drv.pregs->mask[bank] & _BIT(bit_pos)) == 0);
  }
  return ret_val;
}

/***********************************************************************
 *
 * Function: evtr_int_clear
 *
 * Purpose: Clear a pending (latched) interrupt
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     source   : Interrupt source of type EVENT_T
 *
 * Outputs: None
 *
 * Returns: Returns TRUE or FALSE.
 *
 * Notes: None
 *
 **********************************************************************/
BOOL_32 evtr_int_clear(EVENT_T source)
{
  BOOL_32 ret_val = FALSE;
  int bank, bit_pos;
  if (source <= EVT_LAST)
  {
    bank = EVT_GET_BANK(source);
    bit_pos = source & 0x1F;
    g_evtr_drv.pregs->intClr[bank] = _BIT(bit_pos);
    ret_val = TRUE;
  }
  return ret_val;
}

