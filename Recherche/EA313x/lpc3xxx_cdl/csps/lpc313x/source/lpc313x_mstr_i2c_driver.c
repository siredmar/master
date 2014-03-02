/***********************************************************************
 * $Id:: lpc313x_mstr_i2c_driver.c 6407 2011-02-07 04:34:55Z ing03005  $
 *
 * Project: LPC313x alternate I2C driver (master only)
 *
 * Description:
 *     This file contains an alternate I2C driver for the LPC31XX using
 *     master only mode. The driver allows multiple opens and handle
 *     large transfer sizes.
 *
 * Notes:
 *     It is the requirement of the application to setup and enable
 *     base clocks for the I2C devices as well as pin muxing, interrupt
 *     handlers, and interrupt enable/disable.
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

#include "lpc313x_cgu_driver.h"
#include "lpc313x_intc_driver.h"
#include "lpc313x_mstr_i2c_driver.h"

/***********************************************************************
 * I2C driver package data
***********************************************************************/

typedef struct
{
  int instances;
  INT_32 ch;
  I2C_REGS_T *pregs;
  I2C_MTXRX_SETUP_T *pcurtransfer;
  UNS_32 last_clock_rate;
  UNS_32* cur_tx_data_ptr;
  UNS_32 tx_rem;
  UNS_32* cur_rx_data_ptr;
  UNS_32 current_status;
} I2C_MSTR_DRV_DATA_T;
static I2C_MSTR_DRV_DATA_T drvdata[2];

/***********************************************************************
 * I2C driver private functions
 **********************************************************************/

/***********************************************************************
 *
 * Function: i2c_mstr_interrupt_handler
 *
 * Purpose: Handles the I2C channel interrupt
 *
 * Processing:
 *     Examine the amount of data left to transfer and setup the
 *     the necessary I2C register states to keep the data flowing. If
 *     the transfer is complete or failed, update the status as
 *     appropriate and call the application callback if it exists.
 *
 * Parameters:
 *     ch: I2C channel for the interrupt (0 or 1)
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void i2c_mstr_interrupt_handler(int ch)
{
  I2C_MSTR_DRV_DATA_T *pdrvdata = (I2C_MSTR_DRV_DATA_T *)
                                  & drvdata[ch];

  /* Did a NAK error occur? */
  if (pdrvdata->pregs->sts & I2C_STS_NAI)
  {
    pdrvdata->pregs->ctl = 0;
    pdrvdata->current_status = I2C_MSTR_STATUS_NOACKF;
    if (pdrvdata->pcurtransfer->cb != NULL)
    {
      pdrvdata->pcurtransfer->cb(pdrvdata->current_status);
    }

    return;
  }

  /* Did a arbitration error occur? */
  if (pdrvdata->pregs->sts & I2C_STS_AFI)
  {
    pdrvdata->pregs->ctl = 0;
    pdrvdata->current_status = I2C_MSTR_STATUS_ARBF;
    if (pdrvdata->pcurtransfer->cb != NULL)
    {
      pdrvdata->pcurtransfer->cb(pdrvdata->current_status);
    }

    return;
  }

  /* RX FIFO data request? */
  while ((pdrvdata->pregs->sts & I2C_STS_RFE) == 0)
  {
    *pdrvdata->cur_rx_data_ptr = (pdrvdata->pregs->fifo & 0xFF);
    pdrvdata->cur_rx_data_ptr++;
  }

  /* Transfer more data as needed */
  while (((pdrvdata->pregs->sts & I2C_STS_TFF) == 0) &&
         (pdrvdata->tx_rem > 0))
  {
    /* Send data */
    pdrvdata->pregs->fifo = *pdrvdata->cur_tx_data_ptr;
    pdrvdata->cur_tx_data_ptr++;
    pdrvdata->tx_rem--;
  }

  /* Wait for completion or error */
  if (pdrvdata->tx_rem > 0)
  {
    pdrvdata->pregs->ctl = I2C_CTL_TFFIE | I2C_CTL_NAIE |
                           I2C_CTL_AFIE;
  }
  else
  {
    pdrvdata->pregs->ctl = I2C_CTL_NAIE | I2C_CTL_AFIE |
                           I2C_CTL_TDIE;
  }

  /* Has the transfer completed? */
  if (pdrvdata->pregs->sts & I2C_STS_TDI)
  {
    pdrvdata->pregs->ctl = 0;
    pdrvdata->current_status = I2C_MSTR_STATUS_DONE;
    if (pdrvdata->pcurtransfer->cb != NULL)
    {
      pdrvdata->pcurtransfer->cb(pdrvdata->current_status);
    }
  }
}

/***********************************************************************
 *
 * Function: i2c_mstr_setup_clock
 *
 * Purpose: Sets the I2C channel clock rate
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     pdrvdata : Pointer to channel driver data structure
 *     newclock : New clock rate to set
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
static void i2c_mstr_setup_clock(I2C_MSTR_DRV_DATA_T *pdrvdata,
                                 UNS_32 newclock)
{
  UNS_32 baserate, div, clkv;

  /* Get the current I2C clock base rate */
  if (pdrvdata->ch == 0)
  {
    baserate = cgu_get_clk_freq(CGU_SB_I2C0_PCLK_ID);
  }
  else
  {
    baserate = cgu_get_clk_freq(CGU_SB_I2C1_PCLK_ID);
  }
  // FIXME TBD functions above return 0, not sure why
  baserate = 12000000; // HACK

  /* Compute divider to get rate */
  div = baserate / newclock;

  /* Set high and low registers to get correct rate */
  clkv = (div / 2);
  pdrvdata->pregs->ckl = clkv;
  pdrvdata->pregs->ckh = div - clkv;

  pdrvdata->last_clock_rate = newclock;
}

/***********************************************************************
 *
 * Function: i2c_mstr_interrupt_handler
 *
 * Purpose: Handles the I2C channel interrupt
 *
 * Processing:
 *     Examine the amount of data left to transfer and setup the
 *     the necessary I2C register states to keep the data flowing. If
 *     the transfer is complete or failed, update the status as
 *     appropriate and call the application callback if it exists.
 *
 * Parameters:
 *     pdrvdata: Pointer to channel driver data structure
 *     psetup  : Pointer to transfer setup structure
 *
 * Outputs: None
 *
 * Returns:
 *     _NO_ERROR if the device was setup ok, or _ERROR if the channel
 *     is currently busy or it can't be setup.
 *
 * Notes: None
 *
 **********************************************************************/
static INT_32 i2c_mstr_setup_xfer(I2C_MSTR_DRV_DATA_T *pdrvdata,
                                  I2C_MTXRX_SETUP_T *psetup)
{
  volatile UNS_32 tmp;
  INT_32 status = _ERROR;

  /* Is channel idle or complete? */
  if ((pdrvdata->current_status & ~I2C_MSTR_STATUS_DONE) == 0)
  {
    /* Copy over transfer parameters */
    pdrvdata->pcurtransfer = psetup;
    pdrvdata->cur_tx_data_ptr = psetup->tx_data;
    pdrvdata->tx_rem = psetup->tx_length;
    pdrvdata->cur_rx_data_ptr = psetup->rx_data;

    /* Does the clock rate need to be updated? */
    if (pdrvdata->last_clock_rate != psetup->clock_rate)
    {
      i2c_mstr_setup_clock(pdrvdata, psetup->clock_rate);
    }

    /* Flush the RX FIFOs and soft reset I2C */
    while ((pdrvdata->pregs->sts & I2C_STS_RFE) == 0)
    {
      tmp = pdrvdata->pregs->fifo;
    }
    pdrvdata->pregs->ctl = I2C_CTL_SOFT_RESET;

    /* Status is now busy */
    status = _NO_ERROR;
    pdrvdata->current_status = I2C_MSTR_STATUS_BUSY;

    /* Enable TDI interrupt to start transfer */
    pdrvdata->pregs->ctl = I2C_CTL_TFFIE;
  }

  return status;
}

/***********************************************************************
 * I2C driver public functions
 **********************************************************************/

/***********************************************************************
 *
 * Function: i2c_mstr_open
 *
 * Purpose: Open the I2C
 *
 * Processing:
 *     If the passed register base is valid and the I2C
 *     driver isn't already initialized, then setup the I2C
 *     into a default initialized state that is disabled. Return
 *     a pointer to the driver's data structure or NULL if driver
 *     initialization failed.
 *
 * Parameters:
 *     ipbase: Pointer to a I2C peripheral block
 *     arg   : Not used
 *
 * Outputs: None
 *
 * Returns: The pointer to a I2C config structure or NULL
 *
 * Notes: None
 *
 **********************************************************************/
INT_32 i2c_mstr_open(void *ipbase, INT_32 arg)
{
  I2C_MSTR_DRV_DATA_T *pdrvdat = NULL;
  int i2c_id = -1;

  if ((I2C_REGS_T *) ipbase == I2C0) i2c_id = 0;
  if ((I2C_REGS_T *) ipbase == I2C1) i2c_id = 1;

  if (i2c_id != -1)
  {
    pdrvdat = &drvdata[i2c_id];

    /* Increment instance counter */
    pdrvdat->instances++;

    /* If this is the first instance, setup some data */
    pdrvdat->ch = i2c_id;
    if (i2c_id == 0)
    {
      pdrvdat->pregs = (I2C_REGS_T *) I2C0;
    }
    else
    {
      pdrvdat->pregs = (I2C_REGS_T *) I2C1;
      /**
       * FIXME: UDA1380 uses address 0x1A hence the slave
       * address should be set to something else.
       */
      pdrvdat->pregs->adr = 0x06E;
    }

    pdrvdat->pcurtransfer = NULL;
    pdrvdat->current_status = 0;
    pdrvdat->last_clock_rate = 0;
  }

  return (INT_32) pdrvdat;
}

/***********************************************************************
 *
 * Function: i2c_mstr_close
 *
 * Purpose: Close the I2C
 *
 * Processing:
 *     If init is not TRUE, then return _ERROR to the caller as the
 *     device was not previously opened. Otherwise, disable the
 *     I2C, set init to FALSE, and return _NO_ERROR to the caller.
 *
 * Parameters:
 *     devid: Pointer to I2C config structure
 *
 * Outputs: None
 *
 * Returns: The status of the close operation
 *
 * Notes: None
 *
 **********************************************************************/
STATUS i2c_mstr_close(INT_32 devid)
{
  STATUS status = _ERROR;
  I2C_MSTR_DRV_DATA_T *pdrvdat = (I2C_MSTR_DRV_DATA_T *) devid;

  if (pdrvdat != NULL)
  {
    if (pdrvdat->instances > 0)
      pdrvdat->instances--;
  }

  return status;
}

/***********************************************************************
 *
 * Function: i2c_mstr_ioctl
 *
 * Purpose: I2C configuration block
 *
 * Processing:
 *     This function is a large case block. Based on the passed function
 *     and option values, set or get the appropriate timer parameter.
 *
 * Parameters:
 *     devid: Pointer to I2C config structure
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
STATUS i2c_mstr_ioctl(INT_32 devid,
                      INT_32 cmd,
                      INT_32 arg)
{
  INT_32 status = _ERROR;
  I2C_MSTR_DRV_DATA_T *pdrvdata = (I2C_MSTR_DRV_DATA_T *) devid;

  if (pdrvdata != NULL)
  {
    if (pdrvdata->instances > 0)
    {
      switch (cmd)
      {
        case I2C_MSTR_TRANSFER:
          if (arg != 0)
          {
            status = i2c_mstr_setup_xfer(pdrvdata,
                                         (I2C_MTXRX_SETUP_T *) arg);
          }
          break;

        case I2C_MSTR_GET_STATUS:
          status = pdrvdata->current_status;
          break;

        default:
          break;
      }
    }
  }

  return status;
}

/***********************************************************************
 *
 * Function: i2c_mstr_read
 *
 * Purpose: I2C read function (stub only)
 *
 * Processing:
 *     Return 0 to the caller.
 *
 * Parameters:
 *     devid:     Pointer to millisecond timer descriptor
 *     buffer:    Pointer to data buffer to copy to
 *     max_bytes: Number of bytes to read
 *
 * Outputs: None
 *
 * Returns: Number of bytes actually read (always 0)
 *
 * Notes: None
 *
 **********************************************************************/
INT_32 i2c_mstr_read(INT_32 devid,
                     void *buffer,
                     INT_32 max_bytes)
{
  return 0;
}

/***********************************************************************
 *
 * Function: i2c_mstr_write
 *
 * Purpose: I2C write function (stub only)
 *
 * Processing:
 *     Return 0 to the caller.
 *
 * Parameters:
 *     devid:   Pointer to millisecond timer descriptor
 *     buffer:  Pointer to data buffer to copy from
 *     n_bytes: Number of bytes to write
 *
 * Outputs: None
 *
 * Returns: Number of bytes actually written (always 0)
 *
 * Notes: None
 *
 **********************************************************************/
INT_32 i2c_mstr_write(INT_32 devid,
                      void *buffer,
                      INT_32 n_bytes)
{
  return 0;
}

/***********************************************************************
 *
 * Function: i2c0_mstr_int_hanlder
 *
 * Purpose: Interrupt handlers for I2C channel 0
 *
 * Processing:
 *     Calls commin I2C interrupt handler with channel 0.
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
void i2c0_mstr_int_hanlder(void)
{
  i2c_mstr_interrupt_handler(0);
}

/***********************************************************************
 *
 * Function: i2c1_mstr_int_hanlder
 *
 * Purpose: Interrupt handlers for I2C channel 1
 *
 * Processing:
 *     Calls commin I2C interrupt handler with channel 1.
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
void i2c1_mstr_int_hanlder(void)
{
  i2c_mstr_interrupt_handler(1);
}
