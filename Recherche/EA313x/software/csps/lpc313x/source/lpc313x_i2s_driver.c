/***********************************************************************
 * $Id:: lpc313x_i2s_driver.c 1529 2009-01-06 02:08:40Z pdurgesh       $
 *
 * Project: LPC313x standard I2S driver
 *
 * Description:
 *     This file contains driver support for the LPC313x standard UART
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

#include "lpc313x_i2s_driver.h"
#include "lpc313x_intc_driver.h"
//#include "lpc313x_cgu_driver.h"

/***********************************************************************
 * I2S driver package data
***********************************************************************/



/* I2S driver data */
static I2S_CFG_T  i2sdat[4];

/***********************************************************************
 * I2S driver private functions
 **********************************************************************/

/***********************************************************************
 *
 * Function: i2s_ptr_to_i2s_num
 *
 * Purpose: Convert a I2S register pointer to a I2S number
 *
 * Processing:
 *     Based on the passed I2S address, return the I2S number.
 *
 * Parameters:
 *     pi2s : Pointer to a I2S register set
 *
 * Outputs: None
 *
 * Returns: The I2S number (0 to 1) or -1 if register pointer is bad
 *
 *
 **********************************************************************/
INT_32 i2s_ptr_to_i2s_num(I2S_REGS_T *pi2s)
{
  INT_32 i2snum = -1;


  if (pi2s == I2S_TX0)
  {
    i2snum = 0; /* I2S_TX0 */
  }
  else if (pi2s == I2S_TX1)
  {
    i2snum = 1; /* I2S_TX1  */
  }
  else if (pi2s == I2S_RX0)
  {
    i2snum = 2; /* I2S_RX0  */
  }
  else if (pi2s == I2S_RX1)
  {
    i2snum = 3; /* I2S_RX1  */
  }

  return i2snum;
}

/***********************************************************************
 *
 * Function: get_IRQ_source
 *
 * Purpose: Get the IRQ source corresponding to the I2S number
 *
 * Processing:
 *     Based on the passed I2S number, return the I2S IRQ source.
 *
 * Parameters:
 *     i2snum : Numbers (o to 3) according the I2S module
 *
 * Outputs: I2S IRQ source
 *
 * Returns: The I2S IRQ source corresponding to the passed I2S number
 *
 *
 **********************************************************************/
INTERRUPT_SOURCE_T get_IRQ_source(INT_32 i2snum)
{
  INTERRUPT_SOURCE_T irq_source = IRQ_I2S0_OUT;

  /* select the IRQ source according the i2snum */
  switch (i2snum)
  {
    case 0:
      irq_source = IRQ_I2S0_OUT;
      break;
    case 1:
      irq_source = IRQ_I2S1_OUT;
      break;
    case 2:
      irq_source = IRQ_I2S0_IN;
      break;
    case 3:
      irq_source = IRQ_I2S1_IN;
      break;
  }
  return irq_source;
}

/***********************************************************************
 * I2S driver public functions
 **********************************************************************/

/***********************************************************************
 *
 * Function: i2s_open
 *
 * Purpose: Open the I2S
 *
 * Processing:
 *     If init is not FALSE, return 0x00000000 to the caller. Otherwise,
 *     set init to TRUE, save the I2S peripheral register set address,
 *     and initialize the I2S interface and controller to a default
 *     state by calling i2s_default(), and return a pointer to the I2S
 *     config structure to the caller.
 *
 * Parameters:
 *     ipbase: I2S descriptor device address
 *     arg   : Not used
 *
 * Outputs: None
 *
 * Returns: The pointer to a I2S config structure or 0
 *
 * Notes: None
 *
 **********************************************************************/
INT_32 i2s_open(I2S_REGS_T *ipbase,
                PFV handler_ptr)
{
  INT_32 status = 0;
  INT_32 i2snum;


  /* find a matching I2S number based on the passed pointer */
  i2snum = i2s_ptr_to_i2s_num((I2S_REGS_T *) ipbase);

  if (i2snum >= 0)
  {
    /* Has the I2S been previously initialized? */
    if (i2sdat[i2snum].init == FALSE)
    {
      /* Device is valid and not previously initialized */
      i2sdat[i2snum].init = TRUE;

      /* Save address of peripheral block */
      i2sdat[i2snum].regptr = (I2S_REGS_T *) ipbase;

      /* Save i2snum  for later reference */
      i2sdat[i2snum].i2snum = i2snum;

      /* if a handler was passed, install the IRQ handler */
      if (handler_ptr != 0)
      {

        /* save IRQ source    */
        i2sdat[i2snum].source = get_IRQ_source(i2snum);

        /* Install I2S interrupt handler */
        int_install_irq_handler(i2sdat[i2snum].source, (PFV) handler_ptr);

        /* Enable and Clear pending interrupts  */
        int_enable(i2sdat[i2snum].source);
        int_clear(i2sdat[i2snum].source);
      }

      /* Return pointer to I2S configuration structure */
      status = (INT_32) & i2sdat[i2snum];
    }
  }
  return status;
}

/***********************************************************************
 *
 * Function: i2s_close
 *
 * Purpose: Close the I2S
 *
 * Processing:
 *     If init is not TRUE, then return _ERROR to the caller as the
 *     device was not previously opened. Otherwise, disable the I2S
 *     interface and controller and return
 *     _NO_ERROR to the caller.
 *
 * Parameters:
 *     devid: Pointer to I2S config structure
 *
 * Outputs: None
 *
 * Returns: The status of the close operation
 *
 * Notes: None
 *
 **********************************************************************/
STATUS i2s_close(INT_32 devid)
{
  I2S_CFG_T *pi2s = (I2S_CFG_T *) devid;
  STATUS status = _ERROR;


  /* Close and disable device if it was previously initialized */
  if (pi2s->init == TRUE)
  {

    /* Free I2S and Disable clock */
    pi2s->init = FALSE;
    pi2s->regptr = 0;

    /* Disable interrupts */
    int_disable(pi2s->source);

    /* Everything was Ok... */
    status = _NO_ERROR;
  }

  return status;
}

/***********************************************************************
 *
 * Function: i2s_ioctl
 *
 * Purpose: I2S configuration block
 *
 * Processing:
 *     This function is a large case block. Based on the passed function
 *     and option values, set or get the appropriate I2S parameter.
 *
 * Parameters:
 *     devid: Pointer to I2S config structure
 *     cmd:   ioctl command
 *     arg:   ioctl argument
 *
 * Outputs: None
 *
 * Returns: The status of the ioctl operation
 *
 * Notes: None
 *
 **********************************************************************/
STATUS i2s_ioctl(INT_32 devid,
                 INT_32 cmd,
                 INT_32 arg)
{
  I2S_CFG_T *pi2s = (I2S_CFG_T *) devid;
  AUDIOSS_REGS_T *pcfg = (AUDIOSS_REGS_T *) ADSS_CFG_BASE;
  STATUS status = _ERROR;

  /* Close and disable device if it was previously initialized */
  if (pi2s->init == TRUE)
  {
    status = _NO_ERROR;

    switch (cmd)
    {
      case I2S_FORMAT_SETTINGS:
        switch (pi2s->i2snum)
        {
          case 0:
            pcfg->i2s_format_settings &= ~(7 << 6);
            pcfg->i2s_format_settings |= (arg << 6);
            break;
          case 1:
            pcfg->i2s_format_settings &= ~(7 << 9);
            pcfg->i2s_format_settings |= (arg << 9);
            break;
          case 2:
            pcfg->i2s_format_settings &= ~(7 << 0);
            pcfg->i2s_format_settings |= (arg << 0);
            break;
          case 3:
            pcfg->i2s_format_settings &= ~(7 << 3);
            pcfg->i2s_format_settings |= (arg << 3);
            break;
        }
        break;

      case I2S_RX_MASTER_SLAVE_MODE:
        switch (pi2s->i2snum)
        {
          case 0:
            pcfg->audioss_mux_settings &= ~(1 << 1);
            pcfg->audioss_mux_settings |= (arg << 1);
            break;
          case 1:
            pcfg->audioss_mux_settings &= ~(1 << 2);
            pcfg->audioss_mux_settings |= (arg << 2);
            break;
        }
        break;

      case I2S_NSOF_EDGE_DETECTION:
        pcfg->audioss_mux_settings &= ~(1 << 0);
        pcfg->audioss_mux_settings |= (arg << 0);
        break;

      case I2S_INT_MASK_ALL:
        pi2s->regptr->int_mask = 0x03FF;
        break;

      case I2S_INT_MASK:
        pi2s->regptr->int_mask = 0;
        pi2s->regptr->int_mask |= arg;
        break;

      case I2S_INT_UNMASK_ALL:
        pi2s->regptr->int_mask = 0;
        break;

      case I2S_INT_UNMASK:
        pi2s->regptr->int_mask = 0x03FF;
        pi2s->regptr->int_mask &= arg;
        break;

      case I2S_GET_STATUS:
        /* Return a I2S status */
        switch (arg)
        {
          case I2S_GET_INT_STATUS:
            status = pi2s->regptr->int_status;
            break;

          case I2S_GET_NSOF_COUNTER:
            status = pcfg->n_sof_counter;
            break;

          default:
            /* Unsupported parameter */
            status = LPC_BAD_PARAMS;
            break;
        }
        break;

      default:
        /* Unsupported parameter */
        status = LPC_BAD_PARAMS;
    }
  }
  return status;
}

