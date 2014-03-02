/***********************************************************************
 * $Id:: lpc313x_i2c_driver.c 1659 2009-02-06 17:46:20Z pdurgesh       $
 *
 * Project: LPC3xxx I2C driver
 *
 * Description:
 *     This file contains driver support for the LPC3xxx I2C.
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
#include "lpc313x_i2c_driver.h"

/***********************************************************************
 * I2C driver package data
***********************************************************************/

/* I2C driver data for I2C0 and I2C1 */
static I2C_CFG_T i2cdat[2];
UNS_32   header[3];

/***********************************************************************
 * I2C driver private functions
 **********************************************************************/

/***********************************************************************
 *
 * Function: load_slave_address
 *
 * Purpose: load START, slave address and R/W info into the header array
 *
 * Processing:
 *     See function.
 *
 * Parameters: slave address, address mode, R/W selection
 *
 * Outputs: None
 *
 * Returns: number of bytes in the header
 *
 * Notes: None
 *
 **********************************************************************/
void load_slave_address(INT_32 devid, I2C_ADDR_OPT address_length,
                        UNS_32 address, I2CM_ACTIVITY_OPT activity)
{
  UNS_32 i, cnt;
  if (address_length == ADDR7BIT)
  {
    header[0] = I2C_TXFF_START_CND | (address << 1) | activity;
    cnt = 1;
  }
  else
  {
    header[0] = I2C_TXFF_START_CND | 0xF0 | ((address & 0x300) >> 7);
    header[1] = address & 0xFF;
    cnt = 2;
    if (activity == I2C_M_READ)
    {
      header[2] = header[0] | 1;
      cnt++;
    }
  }
  for (i = 0; i != cnt; i++)
  {
    ((I2C_CFG_T *) devid)->regptr->fifo = header[i];
  }
  return;
}
/***********************************************************************
 *
 * Function: i2c0_user_interrupt
 *
 * Purpose: I2C0 interrupt handler
 *
 * Processing:
 *     See function.
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
void i2c0_user_interrupt(void)
{
  i2cdat[0].handler(&i2cdat[0]);
  return;
}

/***********************************************************************
 *
 * Function: i2c1_user_interrupt
 *
 * Purpose: I2C1 interrupt handler
 *
 * Processing:
 *     See function.
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
void i2c1_user_interrupt(void)
{
  i2cdat[1].handler(&i2cdat[1]);
  return;
}

/***********************************************************************
 *
 * Function: enable_i2C_irq_int
 *
 * Purpose: enable specified I2C irq
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     devid: Pointer to I2C config structure
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void enable_i2c_irq_int(UNS_32 devid)
{
  UNS_32 i2c_id = 0xFF; /*invalid ID */

  if ((I2C_CFG_T *) devid == &i2cdat[0]) i2c_id = 0;
  if ((I2C_CFG_T *) devid == &i2cdat[1]) i2c_id = 1;

  if (i2c_id == 0)
  {
    int_enable(IRQ_I2C0);
  }
  else if (i2c_id == 1)
  {
    int_enable(IRQ_I2C1);
  }
  return;
}

/***********************************************************************
 *
 * Function: disable_i2c_irq_int
 *
 * Purpose: disable specified I2C irq
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     devid: Pointer to I2C config structure
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void disable_i2c_irq_int(UNS_32 devid)
{
  UNS_32 i2c_id = 0xFF; /*invalid ID */

  if ((I2C_CFG_T *) devid == &i2cdat[0]) i2c_id = 0;
  if ((I2C_CFG_T *) devid == &i2cdat[1]) i2c_id = 1;

  if (i2c_id == 0)
  {
    int_disable(IRQ_I2C0);
  }
  else if (i2c_id == 1)
  {
    int_disable(IRQ_I2C1);
  }
  return;
}

/***********************************************************************
 * I2C master transmitter interrupt handler
 **********************************************************************/
void i2c_mtx_handler(INT_32 devid)
{
  UNS_32 length, count;
  I2C_CFG_T * device;
  I2C_MTX_SETUP_T * setup;

  device = ((I2C_CFG_T *) devid);
  setup  = (I2C_MTX_SETUP_T*)(device->txrx_setup);

  /* were there Arbitration or ACK problems? */
  if ((device->regptr->sts & (I2C_STS_NAI | I2C_STS_AFI)) != 0)
  {
    if (setup->retransmissions_count != setup->retransmissions_max)
    {
      /* update status */
      setup->retransmissions_count++;
      setup->status = setup->retransmissions_count;
      /* clear Arbitration and Transaction Done interrupt flags */
      device->regptr->sts = I2C_STS_AFI | I2C_STS_TDI;
      /* clear transmitted byte counter */
      setup->tx_count = 0;
      /* soft reset */
      device->regptr->ctl = I2C_CTL_SOFT_RESET;
      /* self cleared, wait until RESET is gone. */
      while (device->regptr->ctl & I2C_CTL_SOFT_RESET);
      /* load slave address */
      load_slave_address(devid, setup->addr_mode,
                         setup->sl_addr, I2C_M_WRITE);
      /* TxFIFO notfull,M Tx Data R,Tx no ACK,Tx Arb Fl, Tx Done Ints */
      device->regptr->ctl = I2C_CTL_TFFIE | I2C_CTL_DRMIE | I2C_CTL_NAIE |
                            I2C_CTL_AFIE | I2C_CTL_TDIE;
    }
    else
    {
      /* update status */
      setup->status |= (
                         (device->regptr->sts & ((I2C_STS_AFI | I2C_STS_NAI)) << 8) |
                         I2C_SETUP_STATUS_DONE);
      /* Disable I2C interrupt in the interrupt controller */
      disable_i2c_irq_int(devid);
    }
  }
  /* was a STOP generated? */
  else if ((device->regptr->sts & I2C_STS_TDI) != 0)
  {
    /* clear the TDI bit */
    device->regptr->sts |= I2C_STS_TDI;
    /* update status */
    setup->status |= I2C_SETUP_STATUS_DONE;
    /* Disable I2C interrupt in the interrupt controller */
    disable_i2c_irq_int(devid);
  }
  /* provide more data */
  else
  {
    length  = setup->tx_length;
    count   = setup->tx_count;
    while ((length != count) &&
           ((device->regptr->sts & I2C_STS_TFF) == 0))
    {
      if ((count + 1) == length)
      {
        device->regptr->fifo = I2C_TXFF_STOP_CND | (setup->tx_data)[count];
      }
      else
      {
        device->regptr->fifo = (setup->tx_data)[count];
      }
      count++;
    }
    setup->tx_count = count;
    /* have all data been sent? */
    if (count == length)
    {
      /* enable Tx no ACK, Tx Arbitr Fail, Tx Done Ints */
      device->regptr->ctl = I2C_CTL_NAIE | I2C_CTL_AFIE | I2C_CTL_TDIE;
    }
  }
  return;
}

/***********************************************************************
 * I2C master receiver interrupt handler
 **********************************************************************/
void i2c_mrx_handler(INT_32 devid)
{
  UNS_32 length, count;
  I2C_CFG_T * device;
  I2C_MRX_SETUP_T * setup;

  device = ((I2C_CFG_T *) devid);
  setup  = (I2C_MRX_SETUP_T*)(device->txrx_setup);

  /* were there Arbitration or ACK problems? */
  if ((device->regptr->sts & (I2C_STS_NAI | I2C_STS_AFI)) != 0)
  {
    if (setup->retransmissions_count != setup->retransmissions_max)
    {
      /* update status */
      setup->retransmissions_count++;
      setup->status = setup->retransmissions_count;
      /* clear Arbitration and Transaction Done interrupt flags */
      device->regptr->sts = I2C_STS_AFI | I2C_STS_TDI;
      /* clear received byte counter */
      setup->rx_count = 0;
      /* set transmitted byte counter */
      setup->tx_count = 1;
      /* soft reset */
      device->regptr->ctl = I2C_CTL_SOFT_RESET;
      /* self cleared */
      while (device->regptr->ctl & I2C_CTL_SOFT_RESET);

      /* load slave address */
      load_slave_address(devid, setup->addr_mode,
                         setup->sl_addr, I2C_M_WRITE);
      /* en RxD Av,RxFIFO Fl,M Tx DataR,Tx noACK,Tx ArbF,Tx Done Ints */
      /* write a dummy byte to trigger actual data transfer */
      if (setup->rx_length == setup->tx_count)
      {
        /* If rx_length is 1, dummy + STOP on TX FIFO */
        device->regptr->fifo = I2C_TXFF_STOP_CND | 0xAA;
      }
      else
      {
        device->regptr->fifo = 0xAA;
      }
      device->regptr->ctl = I2C_CTL_DAIE | I2C_CTL_RFFIE | I2C_CTL_DRMIE |
                            I2C_CTL_NAIE | I2C_CTL_AFIE | I2C_CTL_TDIE;
    }
    else
    {
      /* update status */
      setup->status |=
        ((device->regptr->sts & ((I2C_STS_AFI | I2C_STS_NAI)) << 8) |
         I2C_SETUP_STATUS_DONE);
      /* Disable I2C interrupt in the interrupt controller */
      disable_i2c_irq_int(devid);
    }
  }
  /* was a STOP generated? */
  else if ((device->regptr->sts & I2C_STS_TDI) != 0)
  {
    /* clear the TDI bit */
    device->regptr->sts |= I2C_STS_TDI;
    /* update status */
    setup->status |= I2C_SETUP_STATUS_DONE;
    /* Disable I2C interrupt in the interrupt controller */
    disable_i2c_irq_int(devid);
  }
  /* read more data */
  else
  {
    length  = setup->rx_length;
    count   = setup->rx_count;
    while ((length != count) &&
           ((device->regptr->sts & I2C_STS_RFE) == 0))
    {
      (setup->rx_data)[count] = device->regptr->fifo;
      count++;
    }
    setup->rx_count = count;
    /* have all data been received? */
    if (count == length)
    {
      /* enable Tx no ACK, Tx Arbitr Fail, Tx Done Ints */
      device->regptr->ctl = I2C_CTL_NAIE | I2C_CTL_AFIE | I2C_CTL_TDIE;
    }
    else
    {
      // not all data received, fill the Tx FIFO with more dummies
      while ((setup->tx_count != length) &&
             ((device->regptr->sts & I2C_STS_TFF) == 0))
      {
        if (((setup->tx_count) + 1) == length)
        {
          device->regptr->fifo = I2C_TXFF_STOP_CND | 0xAA;
        }
        else
        {
          device->regptr->fifo = 0xAA;
        }
        setup->tx_count++;
      }
    }
  }
  return;
}

/***********************************************************************
 * I2C master transmitter followed by master receiver interrupt handler
 **********************************************************************/
void i2c_mtxrx_handler(INT_32 devid)
{
  UNS_32 length, count;
  I2C_CFG_T * device;
  I2C_MTXRX_SETUP_T * setup;
  static UNS_32 txrx_state = 0;

  device = ((I2C_CFG_T *) devid);
  setup  = (I2C_MTXRX_SETUP_T*)(device->txrx_setup);

  /* were there Arbitration or ACK problems? */
  if ((device->regptr->sts & (I2C_STS_NAI | I2C_STS_AFI)) != 0)
  {
    if (setup->retransmissions_count != setup->retransmissions_max)
    {
      /* update status */
      setup->retransmissions_count++;
      setup->status = setup->retransmissions_count;
      /* clear Arbitration and Transaction Done interrupt flags */
      device->regptr->sts = I2C_STS_AFI | I2C_STS_TDI;
      /* clear transmitted & received byte counter */
      setup->tx_count = 0;
      setup->rx_count = 0;
      /* set the state indicator */
      txrx_state = 0;
      /* soft reset */
      device->regptr->ctl = I2C_CTL_SOFT_RESET;
      /* self cleared */
      while (device->regptr->ctl & I2C_CTL_SOFT_RESET);
      /* load slave address */
      load_slave_address(devid, setup->addr_mode,
                         setup->sl_addr, I2C_M_WRITE);
      /* en TxFIFO not full,M TxDataR,Tx noACK,Tx ArbF,Tx Done Ints */
      device->regptr->ctl = I2C_CTL_TFFIE | I2C_CTL_DRMIE
                            | I2C_CTL_NAIE | I2C_CTL_AFIE | I2C_CTL_TDIE;
    }
    else
    {
      /* update status */
      setup->status |=
        ((device->regptr->sts & ((I2C_STS_AFI | I2C_STS_NAI)) << 8) |
         I2C_SETUP_STATUS_DONE);
      /* Disable I2C interrupt in the interrupt controller */
      disable_i2c_irq_int(devid);
      /* set txrx state for a new transfer */
      txrx_state = 0;
    }
  }
  /* was a STOP generated? */
  else if ((device->regptr->sts & I2C_STS_TDI) != 0)
  {
    /* clear the TDI bit */
    device->regptr->sts |= I2C_STS_TDI;
    /* update status */
    setup->status |= I2C_SETUP_STATUS_DONE;
    /* Disable I2C interrupt in the interrupt controller */
    disable_i2c_irq_int(devid);
    /* set txrx state for a new transfer */
    txrx_state = 0;
  }
  /* transfer triggered interrupt */
  else
  {
    switch (txrx_state)
    {
        /* transmitting data */
      case 0:
        length  = setup->tx_length;
        count   = setup->tx_count;
        while ((length != count) &&
               ((device->regptr->sts & I2C_STS_TFF) == 0))
        {
          device->regptr->fifo = (setup->tx_data)[count];
          count++;
        }
        setup->tx_count = count;
        /* have all data been sent? */
        if (count == length)
        {
          /* switch to receiving data */
          txrx_state = 1;
          setup->tx_count = 0;
          /* en M Tx Data Req, Tx no ACK, Tx Arb Fail, Tx Done Ints */
          device->regptr->ctl = I2C_CTL_DRMIE | I2C_CTL_NAIE |
                                I2C_CTL_AFIE | I2C_CTL_TDIE;
        }
        break;
        /* receiving data */
      default:
        /* is repeated START + slave address + READ needed? */
        if (setup->tx_count == 0)
        {
          if (setup->addr_mode == ADDR7BIT)
          {
            device->regptr->fifo =
              I2C_TXFF_START_CND | (setup->sl_addr << 1) | I2C_M_READ;
          }
          else
          {
            device->regptr->fifo = I2C_TXFF_START_CND | 0xF0 |
                                   ((setup->sl_addr & 0x30) >> 7) | I2C_M_READ;
          }
          /* en RxDt Av,RxFIFO Fl,MTx DtR,Tx noACK,Tx ArbF,TxDn Ints */
          device->regptr->ctl = I2C_CTL_DAIE | I2C_CTL_RFFIE |
                                I2C_CTL_DRMIE | I2C_CTL_NAIE | I2C_CTL_AFIE | I2C_CTL_TDIE;
          /* write a dummy byte to trigger actual data transfer */
          setup->tx_count = 1;
          setup->rx_count = 0;
          if (setup->rx_length == setup->tx_count)
          {
            /* If rx_length is 1, dummy + STOP on TX FIFO */
            device->regptr->fifo = I2C_TXFF_STOP_CND | 0xAA;
          }
          else
          {
            device->regptr->fifo = 0xAA;
          }
        }
        else
        {
          length  = setup->rx_length;
          count   = setup->rx_count;
          while ((length != count) &&
                 ((device->regptr->sts & I2C_STS_RFE) == 0))
          {
            (setup->rx_data)[count] = device->regptr->fifo;
            count++;
          }
          setup->rx_count = count;
          /* have all data been received? */
          if (count == length)
          {
            /* enable Tx no ACK, Tx Arbitr Fail, Tx Done Ints */
            device->regptr->ctl = I2C_CTL_NAIE |
                                  I2C_CTL_AFIE | I2C_CTL_TDIE;
          }
          else
          {
            // not all data received, fill the Tx FIFO with more dummies
            while ((setup->tx_count != length) &&
                   ((device->regptr->sts & I2C_STS_TFF) == 0))
            {
              if (((setup->tx_count) + 1) == length)
              {
                device->regptr->fifo = I2C_TXFF_STOP_CND | 0xAA;
              }
              else
              {
                device->regptr->fifo = 0xAA;
              }
              setup->tx_count++;
            }
          }
        }
        break;
    }
  }
  return;
}

/***********************************************************************
 * I2C slave transmitter interrupt handler
 **********************************************************************/
void i2c_stx_handler(INT_32 devid)
{
  UNS_32 length, count;
  I2C_CFG_T * device;
  I2C_STX_SETUP_T * setup;

  device = ((I2C_CFG_T *) devid);
  setup  = (I2C_STX_SETUP_T*)(device->txrx_setup);

  length  = setup->tx_length;
  count   = setup->tx_count;

  while ((length != count) &&
         ((device->regptr->sts & I2C_STS_TFFS) == 0))
  {
    device->regptr->txs = (setup->tx_data)[count];
    count++;
  }
  setup->tx_count = count;
  /* have all data been transmitted? */
  if (count == length)
  {
    /* update status */
    setup->status |= I2C_SETUP_STATUS_DONE;
    /* Disable I2C interrupt in the interrupt controller */
    disable_i2c_irq_int(devid);
  }

  return;
}

/***********************************************************************
 * I2C slave receiver interrupt handler
 **********************************************************************/
void i2c_srx_handler(INT_32 devid)
{
  UNS_32 length, count;
  I2C_CFG_T * device;
  I2C_SRX_SETUP_T * setup;

  device = ((I2C_CFG_T *) devid);
  setup  = (I2C_SRX_SETUP_T*)(device->txrx_setup);

  length  = setup->rx_length;
  count   = setup->rx_count;
  while ((length != count) &&
         ((device->regptr->sts & I2C_STS_RFE) == 0))
  {
    (setup->rx_data)[count] = device->regptr->fifo;
    count++;
  }
  setup->rx_count = count;
  /* have all data been received? */
  if (count == length)
  {
    /* update status */
    setup->status |= I2C_SETUP_STATUS_DONE;
    /* Disable I2C interrupt in the interrupt controller */
    disable_i2c_irq_int(devid);
  }
  return;
}

/***********************************************************************
 * I2C slave receiver followed by slave transmitter interrupt handler
 **********************************************************************/
void i2c_srxtx_handler(INT_32 devid)
{
}

/***********************************************************************
 * I2C driver public functions
 **********************************************************************/

/***********************************************************************
 *
 * Function: i2c_open
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
INT_32 i2c_open(void *ipbase, INT_32 arg)
{
  UNS_32 i2c_id = 9999;
  INT_32 tptr = (INT_32) NULL;

  if ((I2C_REGS_T *) ipbase == I2C0) i2c_id = 0;
  if ((I2C_REGS_T *) ipbase == I2C1) i2c_id = 1;
  if (i2c_id != 9999)
  {
    /* has the selected I2C been previously initialized? */
    if (i2cdat[i2c_id].init == FALSE)
    {
      /* Device not initialized and it is usable, so set it to
         used */
      i2cdat[i2c_id].init = TRUE;

      /* Save address of register block */
      if (i2c_id == 0) i2cdat[i2c_id].regptr = I2C0;
      if (i2c_id == 1) i2cdat[i2c_id].regptr = I2C1;
      tptr = (INT_32) & i2cdat[i2c_id];
    }
  }
  return tptr;
}

/***********************************************************************
 *
 * Function: i2c_close
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
STATUS i2c_close(INT_32 devid)
{
  UNS_32 i2c_id = 9999;
  STATUS status = _ERROR;

  if ((I2C_CFG_T *) devid == &i2cdat[0]) i2c_id = 0;
  if ((I2C_CFG_T *) devid == &i2cdat[1]) i2c_id = 1;
  {
    if (i2cdat[i2c_id].init == TRUE)
    {
      /* Set I2C as uninitialized */
      i2cdat[i2c_id].init = FALSE;

      /* disable I2C interrupt */
      disable_i2c_irq_int(devid);

      /* Successful operation */
      status = _NO_ERROR;
    }
  }
  return status;
}

/***********************************************************************
 *
 * Function: i2c_ioctl
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
STATUS i2c_ioctl(INT_32 devid,
                 INT_32 cmd,
                 INT_32 arg)
{
  volatile UNS_32 i, high, low, tmp1;
  UNS_32 i2c_id = 9999;
  INT_32 status = _ERROR;
  I2C_CFG_T * device;
  I2C_SETUP_T * param;
  I2C_MTX_SETUP_T*   setup_mtx;
  I2C_MRX_SETUP_T*   setup_mrx;
  I2C_MTXRX_SETUP_T* setup_mtxrx;
  I2C_SRX_SETUP_T*   setup_srx;
  I2C_STX_SETUP_T*   setup_stx;

  if ((I2C_CFG_T *) devid == &i2cdat[0]) i2c_id = 0;
  if ((I2C_CFG_T *) devid == &i2cdat[1]) i2c_id = 1;

  if (i2c_id != 9999)
  {
    if (i2cdat[i2c_id].init == TRUE)
    {
      status = _NO_ERROR;
      device = ((I2C_CFG_T *) devid);
      param  = (I2C_SETUP_T *) arg;

      switch (cmd)
      {
        case I2C_SETUP:
          /* soft reset */
          device->regptr->ctl = I2C_CTL_SOFT_RESET;
          /* self cleared */
          while (device->regptr->ctl & I2C_CTL_SOFT_RESET);
          /* set the slave address */
          device->regptr->adr = param->sl_addr;
          /* set the I2C clock */
          if (param->rate_option == I2C_RATE_ABSOLUTE)
          {
            device->regptr->ckl = param->low_phase;
            device->regptr->ckh = param->high_phase;
          }
          else
          {
            low = param->low_phase;
            high = param->high_phase;
            tmp1 = cgu_get_clk_freq(CGU_SB_I2C1_PCLK_ID);
            device->regptr->ckl = ((low * tmp1) / (high + low)) / param->rate;
            device->regptr->ckh = ((low * tmp1) / (high + low)) / param->rate;
          }
          device->ip_setup = param;
          break;

        case I2C_MASTER_TX:
          /* disable I2C interrupt */
          disable_i2c_irq_int(devid);
          setup_mtx = (I2C_MTX_SETUP_T*) arg;
          /* link setup parameters */
          device->txrx_setup = (UNS_32) setup_mtx;
          /* clear transmitted byte counter */
          setup_mtx->tx_count = 0;
          /* clear retransmissions counter */
          setup_mtx->retransmissions_count = 0;
          /* prepare actual interrupt handler */
          device->handler = i2c_mtx_handler;
          /* reset status */
          setup_mtx->status = 0x00000000;
          /* clear AFI and TDI bits */
          device->regptr->sts = I2C_STS_AFI | I2C_STS_TDI;
          /* en TxFIFO nt Fll,M Tx Dt Rq,Tx noACK,Tx Arb,Tx Dn Ints */
          device->regptr->ctl = I2C_CTL_TFFIE | I2C_CTL_DRMIE |
                                I2C_CTL_NAIE | I2C_CTL_AFIE | I2C_CTL_TDIE;
          /* load slave address */
          load_slave_address(devid, setup_mtx->addr_mode,
                             setup_mtx->sl_addr, I2C_M_WRITE);
          /* Enable I2C interrupt in the interrupt controller */
          enable_i2c_irq_int(devid);
          break;

        case I2C_MASTER_RX:
          /* disable I2C interrupt */
          disable_i2c_irq_int(devid);
          setup_mrx = (I2C_MRX_SETUP_T*) arg;
          /* link setup parameters */
          device->txrx_setup = (UNS_32) setup_mrx;
          /* clear received byte counter */
          setup_mrx->rx_count = 0;
          /* set transmitted byte counter */
          setup_mrx->tx_count = 1;
          /* clear retransmissions counter */
          setup_mrx->retransmissions_count = 0;
          /* prepare actual interrupt handler */
          device->handler = i2c_mrx_handler;
          /* reset status */
          setup_mrx->status = 0x00000000;
          /* clear AFI and TDI bits */
          device->regptr->sts = I2C_STS_AFI | I2C_STS_TDI;
          /* en RxDtAv,Rx FIFO Fll,M TxDt Rq,Tx noACK,TxArb,TxDn Ints */
          device->regptr->ctl = I2C_CTL_DAIE | I2C_CTL_RFFIE |
                                I2C_CTL_DRMIE | I2C_CTL_NAIE | I2C_CTL_AFIE | I2C_CTL_TDIE;
          /* load slave address */
          load_slave_address(devid, setup_mrx->addr_mode,
                             setup_mrx->sl_addr, I2C_M_READ);
          /* write a dummy byte to trigger actual data transfer */
          device->regptr->fifo = 0xAA;
          /* Enable I2C interrupt in the interrupt controller */
          enable_i2c_irq_int(devid);
          break;

        case I2C_SLAVE_TX:
          /* disable I2C interrupt */
          disable_i2c_irq_int(devid);
          setup_stx = (I2C_STX_SETUP_T*) arg;
          /* link setup parameters */
          device->txrx_setup = (UNS_32) setup_stx;
          /* clear transmitted byte counter */
          setup_stx->tx_count = 0;
          /* prepare actual interrupt handler */
          device->handler = i2c_stx_handler;
          /* reset status */
          setup_stx->status = 0x00000000;
          /* enable Slave Tx FIFO Not Full, Slave Tx Data Req Ints */
          device->regptr->ctl = I2C_CTL_TFFSIE | I2C_CTL_DRSIE;
          /* Enable I2C interrupt in the interrupt controller */
          enable_i2c_irq_int(devid);
          break;

        case I2C_SLAVE_RX:
          /* disable I2C interrupt */
          disable_i2c_irq_int(devid);
          setup_srx = (I2C_SRX_SETUP_T*) arg;
          /* link setup parameters */
          device->txrx_setup = (UNS_32) setup_srx;
          /* clear received byte counter */
          setup_srx->rx_count = 0;
          /* prepare actual interrupt handler */
          device->handler = i2c_srx_handler;
          /* reset status */
          setup_srx->status = 0x00000000;
          /* enable Rx Data Available, Rx FIFO Full Ints */
          device->regptr->ctl = I2C_CTL_DAIE | I2C_CTL_RFFIE;
          /* Enable I2C interrupt in the interrupt controller */
          enable_i2c_irq_int(devid);
          break;

        case I2C_MASTER_TXRX:
          /* disable I2C interrupt */
          disable_i2c_irq_int(devid);
          setup_mtxrx = (I2C_MTXRX_SETUP_T*) arg;
          /* link setup parameters */
          device->txrx_setup = (UNS_32) setup_mtxrx;
          /* clear transmitted & received byte counter */
          setup_mtxrx->tx_count = 0;
          setup_mtxrx->rx_count = 0;
          /* clear retransmissions counter */
          setup_mtxrx->retransmissions_count = 0;
          /* prepare actual interrupt handler */
          device->handler = i2c_mtxrx_handler;
          /* reset status */
          setup_mtxrx->status = 0x00000000;
          /* clear AFI and TDI bits */
          device->regptr->sts = I2C_STS_AFI | I2C_STS_TDI;
          /* en TxFIFO notFll,M TxDtReq,Tx noACK,TxArb Fail,TxDn Ints */
          device->regptr->ctl = I2C_CTL_TFFIE | I2C_CTL_DRMIE |
                                I2C_CTL_NAIE | I2C_CTL_AFIE | I2C_CTL_TDIE;
          /* load slave address */
          load_slave_address(devid, setup_mtxrx->addr_mode,
                             setup_mtxrx->sl_addr, I2C_M_WRITE);
          /* Enable I2C interrupt in the interrupt controller */
          enable_i2c_irq_int(devid);
          break;

        case I2C_SLAVE_RXTX:
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
 * Function: i2c_read
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
INT_32 i2c_read(INT_32 devid,
                void *buffer,
                INT_32 max_bytes)
{
  return 0;
}

/***********************************************************************
 *
 * Function: i2c_write
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
INT_32 i2c_write(INT_32 devid,
                 void *buffer,
                 INT_32 n_bytes)
{
  return 0;
}
