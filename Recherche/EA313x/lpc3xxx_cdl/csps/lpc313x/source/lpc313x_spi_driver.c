/***********************************************************************
 * $Id:: lpc313x_spi_driver.c 2278 2009-09-22 23:34:00Z pdurgesh       $
 *
 * Project: LPC313x SPI driver
 *
 * Description:
 *     This file contains driver support for the SPI module on the
 *     LPC313x
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

#include "lpc313x_cgu_driver.h"
#include "lpc313x_intc_driver.h"
#include "lpc313x_spi_driver.h"

/***********************************************************************
 * SPI driver private data and types
 **********************************************************************/
/* SPI device configuration structure type */
typedef struct
{
  BOOL_32 init;          /* Device initialized flag */
  SPI_REGS_T *regptr;    /* Pointer to SPI registers */
  SPI_CB_T cbs;
} SPI_DRVDAT_T;

/* SPI device configuration structure */
static SPI_DRVDAT_T g_spidrv;


/***********************************************************************
 * SPI driver private functions
 **********************************************************************/

SPI_SLAVE_ID_T spi_get_cur_slave(SPI_REGS_T *regptr)
{
  SPI_SLAVE_ID_T sid = SPI_INVALID_ID;
  switch (regptr->slave_enable)
  {
    case SPI_SLV_EN(SPI_SLAVE1_ID):
      sid = SPI_SLAVE1_ID;
      break;
    case SPI_SLV_EN(SPI_SLAVE2_ID):
      sid = SPI_SLAVE2_ID;
      break;
    case SPI_SLV_EN(SPI_SLAVE3_ID):
      sid = SPI_SLAVE3_ID;
      break;
    default:
      sid = SPI_INVALID_ID;
      break;
  }
  return sid;
}

/***********************************************************************
 *
 * Function: spi_set_clock
 *
 * Purpose: Sets or resets the serial clock rate of the SPI interface
 *          (in Hz)
 *
 * Processing:
 *     Determine the best dividers to generate the closest possible
 *     target clock rate for the SPI.
 *
 * Parameters:
 *     pspidrvdat   : Pointer to driver data
 *     target_clock : The value in Hz for the new SPI serial clock
 *
 * Outputs: None
 *
 * Returns: _ERROR if the configuration setup failed, otherwise _NO_ERROR
 *
 * Notes: None
 *
 **********************************************************************/
static STATUS spi_get_clock_div(UNS_32 target_clock,
                                UNS_32 *pDivSet)
{
  UNS_32 div, spi_clk, ps, div1;
  STATUS retvalue = _ERROR;

  /* The SPI clock is derived from the (main system PLL / 2),
     so compute the best divider from that clock */
  spi_clk = cgu_get_clk_freq(CGU_SB_SPI_CLK_ID);

  /* Find closest divider to get at or under the target frequency.
     Use smallest prescaler possible and rely on the divider to get
     the closest target frequency */
  div = (spi_clk + target_clock / 2) / target_clock;

  if ((div < SPI_MAX_DIVIDER) && (div > SPI_MIN_DIVIDER))
  {
    ps = (((div - 1) / 512) + 1) * 2;
    div1 = ((((div + ps / 2) / ps) - 1));

    /* write the divider settings */
    *pDivSet = SPI_SLV1_CLK_PS(ps) | SPI_SLV1_CLK_DIV1((div1));

    retvalue = _NO_ERROR;
  }

  return retvalue;
}
/***********************************************************************
 *
 * Function: spi_slave_configure
 *
 * Purpose: Configure SPI interface
 *
 * Processing:
 *     Setup the general capabilities of the SPI controller.
 *
 * Parameters:
 *     pspicfg : Pointer to an SPI_CONFIG_T structure
 *     pspidrvdat: Pointer to driver data
 *
 * Outputs: None
 *
 * Returns:
 *     _ERROR if the configuration setup failed, otherwise _NO_ERROR
 *
 * Notes: None
 *
 **********************************************************************/
static STATUS spi_slave_configure(SPI_DRVDAT_T *pspidrvdat,
                                  SPI_SLAVE_CONFIG_T *pSlaveCfg)
{
  UNS_32 set1, set2;
  STATUS setup = _ERROR;
  SPI_REGS_T *pSpiRegs = pspidrvdat->regptr;

  /* get clock divider setting */
  if (spi_get_clock_div(pSlaveCfg->clk, &set1) == _NO_ERROR)
  {
    /* set no of words for SMS mode and inter frame delay timing */
    set1 |= SPI_SLV1_NUM_WORDS(pSlaveCfg->words) |
            SPI_SLV1_INTER_TX_DLY(pSlaveCfg->inter_delay);

    /* set data width & pre-post delay timing */
    set2 = SPI_SLV2_WD_SZ(pSlaveCfg->databits - 1) |
           SPI_SLV2_PPCS_DLY(pSlaveCfg->pp_delay);

    /* set the transfer mode */
    switch (pSlaveCfg->mode)
    {
      case SSI_MODE:
        set2 = SPI_SLV2_SSI_MODE;
        break;
      case SPI_MODE1:
        set2 |= SPI_SLV2_SPH;
        break;
      case SPI_MODE2:
        set2 |= SPI_SLV2_SPO;
        break;
      case SPI_MODE3:
        set2 |= SPI_SLV2_SPO | SPI_SLV2_SPH;
        break;
      default:
        break;
    }
    /* check is CS high is needed */
    if (pSlaveCfg->cs_high)
      set2 |= SPI_SLV2_CS_HIGH;

    /* finally write to the register */
    pSpiRegs->slv_setting[pSlaveCfg->sid].setting1 = set1;
    pSpiRegs->slv_setting[pSlaveCfg->sid].setting2 = set2;

    setup = _NO_ERROR;
  }
  return setup;
}


/***********************************************************************
 *
 * Function: spi_configure
 *
 * Purpose: Configure SPI interface
 *
 * Processing:
 *     Setup the general capabilities of the SPI controller.
 *
 * Parameters:
 *     pspicfg : Pointer to an SPI_CONFIG_T structure
 *     pspidrvdat: Pointer to driver data
 *
 * Outputs: None
 *
 * Returns:
 *     _ERROR if the configuration setup failed, otherwise _NO_ERROR
 *
 * Notes: None
 *
 **********************************************************************/
static STATUS spi_configure(SPI_DRVDAT_T *pspidrvdat,
                            SPI_CONFIG_T *pspicfg)
{
  STATUS setup = _NO_ERROR;
  SPI_REGS_T *pSpiRegs = pspidrvdat->regptr;
  UNS_32 i;

  /* reset the SPI block */
  pSpiRegs->spi_config = SPI_CFG_SW_RESET;

  /* Master/slave mode control */
  if (pspicfg->master_mode != FALSE)
  {
    /* configure each slave settings. In master mode slave[0] settings
    are used. */
    for (i = 0; i < SPI_NUM_SLAVES; i++)
    {
      /* if databits is zero then no slave connected to this CS */
      if ((pspicfg->slave[i].databits != 0) &&
          (pspicfg->slave[i].sid == i))
      {
        setup = spi_slave_configure(pspidrvdat, &pspicfg->slave[i]);

        if (setup == _NO_ERROR)
        {
          /* enable the last configured slave only */
          pSpiRegs->slave_enable = SPI_SLV_EN(i);
          /* inform the SPI block about changes in slave enable */
          pSpiRegs->spi_config |= SPI_CFG_UPDATE_EN;
        }
      }
    }
    /* set master mode & also set inter slave delay */
    pSpiRegs->spi_config |= SPI_CFG_INTER_DLY(pspicfg->inter_slave_delay);
  }
  else
  {
    /* in slave mode slave[0] setting registers are used for config */
    pspicfg->slave[SPI_DEFAULT_ID].sid = SPI_DEFAULT_ID;
    setup = spi_slave_configure(pspidrvdat, &pspicfg->slave[SPI_DEFAULT_ID]);
    if (setup == _NO_ERROR)
    {
      /* set master mode & also set inter slave delay */
      pSpiRegs->spi_config |= SPI_CFG_SLAVE_MODE |
                              SPI_CFG_INTER_DLY(pspicfg->inter_slave_delay);
      pSpiRegs->slave_enable = 0;
    }
  }


  return setup;
}

/***********************************************************************
 *
 * Function: spi_int
 *
 * Purpose: SPI standard interrupt function
 *
 * Processing:
 *     Handle the SPI interrupt. Route to the necessary callback
 *     function as needed. Disable interrupt if a callback is not
 *     associated with it.
 *
 * Parameters:
 *     pspidrvdat : Pointer to an SPI driver data
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
static void spi_int(SPI_DRVDAT_T *pspidrvdat)
{
  SPI_REGS_T *pSpiRegs = pspidrvdat->regptr;

  /* Interrupt was due to a receive data FIFO service request */
  if ((pSpiRegs->int_status & (SPI_RX_INT | SPI_OVR_INT)) != 0)
  {
    if (pspidrvdat->cbs.rxcb == NULL)
    {
      /* Disable interrupt, no support for it */
      pSpiRegs->int_clr_enable = (SPI_RX_INT | SPI_OVR_INT);
    }
    else
    {
      /* Handle callback */
      pspidrvdat->cbs.rxcb();
      pSpiRegs->int_clr_status = (SPI_RX_INT | SPI_OVR_INT);
    }
  }

  /* Interrupt was due to a transmit data FIFO service request */
  if ((pSpiRegs->int_status & SPI_TX_INT) != 0)
  {
    if (pspidrvdat->cbs.txcb == NULL)
    {
      /* Disable interrupt, no support for it */
      pSpiRegs->int_clr_enable = SPI_TX_INT;
    }
    else
    {
      /* Handle callback */
      pspidrvdat->cbs.txcb();
      pSpiRegs->int_clr_status = (SPI_TX_INT);
    }
  }
}

/***********************************************************************
 * SPI driver public functions
 **********************************************************************/

/***********************************************************************
 *
 * Function: spi_open
 *
 * Purpose: Open the SPI
 *
 * Processing:
 *     Initializes the SPI clocks and default state.
 *
 * Parameters:
 *     ipbase: SPI descriptor device address
 *     arg   : Pointer to config structure, or NULL if not used
 *
 * Outputs: None
 *
 * Returns: The pointer to a SPI config structure or 0
 *
 * Notes: None
 *
 **********************************************************************/
INT_32 spi_open(void *ipbase,
                INT_32 arg)
{
  SPI_CONFIG_T spi_cfg, *pspicfg;
  INT_32 status = 0;

  /* Map SPI registers to data index */
  if (g_spidrv.init == FALSE)
  {
    /* Save and return address of peripheral block */
    g_spidrv.regptr = (SPI_REGS_T *) ipbase;
    /* No initial callbacks */
    g_spidrv.cbs.txcb = NULL;
    g_spidrv.cbs.rxcb = NULL;

    /* Enable SPI clock */
    cgu_clk_en_dis(CGU_SB_SPI_PCLK_ID, TRUE);
    cgu_clk_en_dis(CGU_SB_SPI_PCLK_GATED_ID, TRUE);
    cgu_clk_en_dis(CGU_SB_SPI_CLK_ID, TRUE);
    cgu_clk_en_dis(CGU_SB_SPI_CLK_GATED_ID, TRUE);

    /* Initialize device */
    if (arg == 0)
    {
      /* Create and use defaults */
      spi_cfg.master_mode = TRUE;
      spi_cfg.inter_slave_delay = 1;
      spi_cfg.slave[0].sid = SPI_SLAVE1_ID;
      spi_cfg.slave[0].databits = 8;
      spi_cfg.slave[0].mode = SPI_MODE0;
      spi_cfg.slave[0].cs_high = FALSE;
      spi_cfg.slave[0].pp_delay = 0;
      spi_cfg.slave[0].inter_delay = 0;
      spi_cfg.slave[0].clk = 1000000;
      spi_cfg.slave[1].databits = 0;
      spi_cfg.slave[2].databits = 0;

      pspicfg = &spi_cfg;
    }
    else
    {
      pspicfg = (SPI_CONFIG_T *) arg;
    }
    if (spi_configure(&g_spidrv, pspicfg) == _NO_ERROR)
    {
      /* Device is valid */
      g_spidrv.init = TRUE;
      status = (INT_32) & g_spidrv;

      /* Install SPI interrupt handler */
      int_install_irq_handler(IRQ_SPI, spi_int);

      /* Empty FIFO */
      g_spidrv.regptr->tx_fifo_flush = SPI_TXFF_FLUSH;

      /* Clear latched interrupts */
      g_spidrv.regptr->int_clr_enable = SPI_ALL_INTS;
      g_spidrv.regptr->int_clr_status = SPI_ALL_INTS;

      /* Enable interrupts */
      g_spidrv.regptr->int_set_enable = (SPI_OVR_INT |
                                         SPI_RX_INT | SPI_TX_INT);
      /* finally SPI block */
      //g_spidrv.regptr->spi_config |= SPI_CFG_ENABLE;
    }
  }

  return status;
}

/***********************************************************************
 *
 * Function: spi_close
 *
 * Purpose: Close the SPI
 *
 * Processing:
 *     Disable the SPI clock and device.
 *
 * Parameters:
 *     devid: Pointer to SPI config structure
 *
 * Outputs: None
 *
 * Returns: The status of the close operation
 *
 * Notes: None
 *
 **********************************************************************/
STATUS spi_close(INT_32 devid)
{
  SPI_DRVDAT_T *pspidrvdat = (SPI_DRVDAT_T *) devid;
  STATUS status = _ERROR;

  if (pspidrvdat->init != FALSE)
  {
    /* 'Uninitialize' device */
    pspidrvdat->init = FALSE;
    status = _NO_ERROR;

    /* Disable device */
    pspidrvdat->regptr->spi_config &= ~SPI_CFG_ENABLE;

    /* Disable clock */
    cgu_clk_en_dis(CGU_SB_SPI_PCLK_ID, FALSE);
    cgu_clk_en_dis(CGU_SB_SPI_PCLK_GATED_ID, FALSE);
    cgu_clk_en_dis(CGU_SB_SPI_CLK_ID, FALSE);
    cgu_clk_en_dis(CGU_SB_SPI_CLK_GATED_ID, FALSE);
  }

  return status;
}

/***********************************************************************
 *
 * Function: spi_ioctl
 *
 * Purpose: SPI configuration block
 *
 * Processing:
 *     This function is a large case block. Based on the passed function
 *     and option values, set or get the appropriate SPI parameter.
 *
 * Parameters:
 *     devid: Pointer to SPI config structure
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
STATUS spi_ioctl(INT_32 devid,
                 INT_32 cmd,
                 INT_32 arg)
{
  SPI_REGS_T *spiregs;
  SPI_CB_T *pspicb;
  UNS_32 spiclk, tmp, tmp2, i;
  SPI_DRVDAT_T *spidrvdat = (SPI_DRVDAT_T *) devid;
  STATUS status = _ERROR;

  if (spidrvdat->init != FALSE)
  {
    status = _NO_ERROR;
    spiregs = spidrvdat->regptr;

    switch (cmd)
    {
      case SPI_ENABLE:
        if (arg == 1)
        {
          /* Enable SPI */
          spiregs->spi_config |= SPI_CFG_ENABLE;
        }
        else
        {
          /* Disable SPI */
          spiregs->spi_config &= ~SPI_CFG_ENABLE;
        }
        break;

      case SPI_SET_ACTIVE_SLAVE:
        /* enable the last configured slave only */
        spiregs->slave_enable = SPI_SLV_EN(arg);
        /* inform the SPI block about changes in slave enable */
        spiregs->spi_config |= SPI_CFG_UPDATE_EN;
        break;

      case SPI_CONFIG:
        status = spi_configure(spidrvdat,
                               (SPI_CONFIG_T *) arg);
        break;

      case SPI_SLAVE_CONFIG:
        status = spi_slave_configure(spidrvdat,
                                     (SPI_SLAVE_CONFIG_T *) arg);
        break;

      case SPI_ENABLE_LOOPB:
        /* Enable or disable loopback mode */
        if (arg == 1)
        {
          /* Enable SPI loopback mode */
          spiregs->spi_config |= SPI_CFG_LOOPBACK;
        }
        else
        {
          /* Disable SPI loopback mode */
          spiregs->spi_config &= ~SPI_CFG_LOOPBACK;
        }
        break;

      case SPI_SO_DISABLE:
        /* Slave output disable */
        if (arg != 0)
        {
          spiregs->spi_config |= SPI_CFG_SLAVE_DISABLE;
        }
        else
        {
          spiregs->spi_config &= ~SPI_CFG_SLAVE_DISABLE;
        }
        break;

      case SPI_FLUSH_RX_FIFO:
        while ((spiregs->status & SPI_ST_RX_EMPTY) == 0)
        {
          tmp = spiregs->fifo_data;
        }
        break;
      case SPI_FLUSH_TX_FIFO:
        while ((spiregs->status & SPI_ST_TX_EMPTY) == 0)
        {
          spiregs->tx_fifo_flush = SPI_TXFF_FLUSH;
        }
        break;
      case SPI_SET_CALLBACKS:
        pspicb = (SPI_CB_T *) arg;
        spidrvdat->cbs.txcb = pspicb->txcb;
        spidrvdat->cbs.rxcb = pspicb->rxcb;
        break;

      case SPI_CLEAR_INTS:
        spiregs->int_clr_status = ((UNS_32) arg) & SPI_ALL_INTS;
        break;

      case SPI_GET_STATUS:
        /* Return an SPI status */
        i = 0;
        switch (arg)
        {
          case SPI_SLV3_CLOCK_ST:
            i++;
          case SPI_SLV2_CLOCK_ST:
            i++;
          case SPI_SLV1_CLOCK_ST:
          case SPI_CLOCK_ST:
            /* Return clock speed of SPI interface */
            tmp = SPI_SLV1_CLK_DIV1_GET(spiregs->slv_setting[i].setting1);
            tmp2 = SPI_SLV1_CLK_PS_GET(spiregs->slv_setting[i].setting1);
            /* Compute SPI bit clock rate */
            spiclk = cgu_get_clk_freq(CGU_SB_SPI_CLK_ID);
            status = spiclk / (tmp2 * (tmp + 1));
            break;

          case SPI_PENDING_INTS_ST:
            status = (spiregs->int_status & spiregs->int_enable);
            break;

          case SPI_RAW_INTS_ST:
            status = spiregs->int_status;
            break;
          case SPI_FIFO_ST:
          default:
            status = spiregs->status;
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

/***********************************************************************
 *
 * Function: spi_read
 *
 * Purpose: SPI read function
 *
 * Processing:
 *     Reads data from the SPI FIFO.
 *
 * Parameters:
 *     devid:     Pointer to SPI config structure
 *     buffer:    Pointer to data buffer to copy to (2 byte aligned)
 *     max_fifo:  Number of items (of programmed data width) to read
 *
 * Outputs: None
 *
 * Returns: Number of items read from the SPI FIFO
 *
 * Notes: None
 *
 **********************************************************************/
INT_32 spi_read(INT_32 devid,
                void *buffer,
                INT_32 max_fifo)
{
  INT_32 count = 0;
  SPI_DRVDAT_T *spicfgptr = (SPI_DRVDAT_T *) devid;
  SPI_REGS_T* pregs;
  UNS_16 *data16 = NULL;
  UNS_8 *data8 = NULL;
  SPI_SLAVE_ID_T sid;

  if (spicfgptr->init == TRUE)
  {
    pregs = spicfgptr->regptr;
    /* get current active slave */
    sid = spi_get_cur_slave(pregs);

    if (sid != SPI_INVALID_ID)
    {
      /* determine sample width */
      if (SPI_SLV2_WD_SZ(pregs->slv_setting[sid].setting2) > 8)
      {
        data16 = (UNS_16 *)buffer;
      }
      else
      {
        data8 = (UNS_8 *)buffer;
      }
      /* Loop until transmit ring buffer is full or until n_bytes
         expires */
      while ((max_fifo > 0) &&
             ((pregs->status & SPI_ST_RX_EMPTY) != SPI_ST_RX_EMPTY))
      {
        if (data16 == NULL)
        {
          *data8 = (UNS_8)pregs->fifo_data;
          data8++;
        }
        else
        {
          *data16 = (UNS_16)pregs->fifo_data;
          data16++;
        }

        /* Increment data count and decrement buffer size count */
        count++;
        max_fifo--;
      }
    }
  }

  return count;
}

/***********************************************************************
 *
 * Function: spi_write
 *
 * Purpose: SPI write function
 *
 * Processing:
 *     Write data to the SPI FIFO.
 *
 * Parameters:
 *     devid:   Pointer to SPI config structure
 *     buffer:  Pointer to data buffer to copy from (2 byte aligned)
 *     n_fifo:  Number of times to write data to the transmit fifo
 *
 * Outputs: None
 *
 * Returns: Number of items written to the transmit fifo
 *
 * Notes: None
 *
 **********************************************************************/
INT_32 spi_write(INT_32 devid,
                 void *buffer,
                 INT_32 n_fifo)
{
  SPI_REGS_T* pregs;
  INT_32 count = 0;
  SPI_DRVDAT_T *spicfgptr = (SPI_DRVDAT_T *) devid;
  UNS_16 *data16 = NULL;
  UNS_8 *data8 = NULL;
  SPI_SLAVE_ID_T sid;

  if (spicfgptr->init == TRUE)
  {
    pregs = spicfgptr->regptr;
    /* get current active slave */
    sid = spi_get_cur_slave(pregs);

    if (sid != SPI_INVALID_ID)
    {
      if (SPI_SLV2_WD_SZ(pregs->slv_setting[sid].setting2) > 8)
      {
        data16 = (UNS_16 *)buffer;
      }
      else
      {
        data8 = (UNS_8 *)buffer;
      }
      /* restrict single to max fifo depth */
      n_fifo = (n_fifo > SPI_FIFO_DEPTH) ? SPI_FIFO_DEPTH : n_fifo;
      /* Loop until transmit ring buffer is full or until n_bytes
         expires */
      while ((n_fifo > 0) &&
             ((pregs->status & SPI_ST_TX_FF) != SPI_ST_TX_FF))
      {
        if (data16 == NULL)
        {
          pregs->fifo_data = (UNS_32) * data8;
          data8++;
        }
        else
        {
          pregs->fifo_data = (UNS_32) * data16;
          data16++;
        }

        /* Increment data count and decrement buffer size count */
        count++;
        n_fifo--;
      }
    }
  }

  return count;
}
