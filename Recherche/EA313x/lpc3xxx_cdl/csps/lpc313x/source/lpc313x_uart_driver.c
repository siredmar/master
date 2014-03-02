/***********************************************************************
 * $Id:: lpc313x_uart_driver.c 6289 2011-01-28 10:26:08Z ing02124      $
 *
 * Project: LPC313x standard UART driver
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

#include "lpc313x_uart_driver.h"
#include "lpc313x_intc_driver.h"
#include "lpc313x_cgu_driver.h"

/***********************************************************************
 * UART driver package data
***********************************************************************/

/* UART device configuration structure type */
typedef struct
{
  UART_REGS_T *regptr;
  UART_CBS_T cbs;
  UNS_32 baudrate;
  BOOL_32 uart_init;
} UART_CFG_T;

/* UART driver data */
static UART_CFG_T uartdat;

/***********************************************************************
 * UART driver private functions
 **********************************************************************/

/***********************************************************************
 *
 * Function: uart_int_handler
 *
 * Purpose: General UART interrupt handler and router
 *
 * Processing:
 *     Handles transmit, receive, and status interrupts for the UART.
 *     Based on the interrupt status, routes the interrupt to the
 *     respective callback to be handled by the user application using
 *     this driver.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: If a callback doesn't exist, the interrupt will be disabled.
 *
 **********************************************************************/
void uart_int_handler(UART_CFG_T *puartcfg)
{
  volatile UNS_32 tmp;

  puartcfg = &uartdat;

  /* Determine the interrupt source */
  tmp = puartcfg->regptr->iir_fcr & UART_IIR_INTSRC_MASK;
  switch (tmp)
  {
    case UART_IIR_INTSRC_RXLINE:
    default:
      /* RX line status interrupt, needs servicing */
      if (puartcfg->cbs.rxerrcb != NULL)
      {
        puartcfg->cbs.rxerrcb();
      }
      else
      {
        /* No callback, disable interrupt */
        puartcfg->regptr->dlm_ie &= ~UART_IE_RXLINE_STS;
      }
      break;

    case UART_IIR_INTSRC_THRE:
      /* Disable interrupt, write will re-enable it */
      if (puartcfg->cbs.txcb != NULL)
      {
        puartcfg->cbs.txcb();
      }
      break;

    case UART_IIR_INTSRC_RDA:
    case UART_IIR_INTSRC_CTI:
      /* Receive interrupt, needs servicing */
      if (puartcfg->cbs.rxcb != NULL)
      {
        puartcfg->cbs.rxcb();
      }
      else
      {
        /* No callback, disable interrupt */
        puartcfg->regptr->dlm_ie &= ~UART_IE_RDA;
      }
      break;
  }
}

/***********************************************************************
 *
 * Function: uart_flush_fifos
 *
 * Purpose: Flushes one or both of the UART FIFOs
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     pregs     : Pointer to a UART register base
 *     flushword : Masked flush value
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void uart_flush_fifos(UART_REGS_T *pregs,
                      UNS_32 flushword)
{
  pregs->iir_fcr |= (flushword & (UART_FCR_TXFIFO_FLUSH |
                                  UART_FCR_RXFIFO_FLUSH));
}

/***********************************************************************
 *
 * Function: uart_set_divisors
 *
 * Purpose: Determines best dividers to get a target clock rate
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     puartcfg   : Pointer to UART configuration data
 *     baudrate   : Desired UART baud rate
 *
 * Outputs:
 *	  baudrate : Sets the estimated buadrate value in puartcfg structre.
 *
 * Returns: Error status.
 *
 * Notes: None
 *
 **********************************************************************/
STATUS uart_set_divisors(UART_CFG_T *puartcfg, UNS_32 baudrate)
{
  STATUS errorStatus = _ERROR;

  UNS_32 uClk;
  UNS_32 calcBaudrate = 0;
  UNS_32 temp = 0;

  UNS_32 mulFracDiv, dividerAddFracDiv;
  UNS_32 diviser = 0 ;
  UNS_32 mulFracDivOptimal = 1;
  UNS_32 dividerAddOptimal = 0;
  UNS_32 diviserOptimal = 0;

  UNS_32 relativeError = 0;
  UNS_32 relativeOptimalError = 100000;

  /* get UART block clock */
  uClk = cgu_get_clk_freq(CGU_SB_UART_U_CLK_ID);

  uClk = uClk >> 4; /* div by 16 */
  /* In the Uart IP block, baud rate is calculated using FDR and DLL-DLM registers
   * The formula is :
   * BaudRate= uClk * (mulFracDiv/(mulFracDiv+dividerAddFracDiv) / (16 * (DLL)
   * It involves floating point calculations. That's the reason the formulae are adjusted with
   * Multiply and divide method.*/
  /* The value of mulFracDiv and dividerAddFracDiv should comply to the following expressions:
   * 0 < mulFracDiv <= 15, 0 <= dividerAddFracDiv <= 15 */
  for (mulFracDiv = 1 ; mulFracDiv <= 15 ;mulFracDiv++)
  {
    for (dividerAddFracDiv = 0 ; dividerAddFracDiv <= 15 ;dividerAddFracDiv++)
    {
      temp = (mulFracDiv * uClk) / ((mulFracDiv + dividerAddFracDiv));

      diviser = temp / baudrate;
      if ((temp % baudrate) > (baudrate / 2))
        diviser++;

      if (diviser > 2 && diviser < 65536)
      {
        calcBaudrate = temp / diviser;

        if (calcBaudrate <= baudrate)
          relativeError = baudrate - calcBaudrate;
        else
          relativeError = calcBaudrate - baudrate;

        if ((relativeError < relativeOptimalError))
        {
          mulFracDivOptimal = mulFracDiv ;
          dividerAddOptimal = dividerAddFracDiv;
          diviserOptimal = diviser;
          relativeOptimalError = relativeError;
          puartcfg->baudrate = calcBaudrate;
          if (relativeError == 0)
            break;
        }
      } /* End of if */
    } /* end of inner for loop */
    if (relativeError == 0)
      break;
  } /* end of outer for loop  */

  if (relativeOptimalError < (baudrate / 30))
  {
    /* Disable all UART interrupts */
    puartcfg->regptr->dlm_ie = 0;

    /* Set the `Divisor Latch Access Bit` and enable so the DLL/DLM access*/
    puartcfg->regptr->lcr |= UART_LCR_DIVLATCH_EN;
    /* Initialise the `Divisor latch LSB` and `Divisor latch MSB` registers */
    puartcfg->regptr->dll_fifo = UART_LOAD_DLL(diviserOptimal);
    puartcfg->regptr->dlm_ie = UART_LOAD_DLM(diviserOptimal);
    puartcfg->regptr->lcr &= ~ UART_LCR_DIVLATCH_EN;

    /* Initialise the Fractional Divider Register */
    puartcfg->regptr->fdr = UART_FDR_MUL_SET(mulFracDivOptimal) | UART_FDR_DIVADD_SET(dividerAddOptimal);

    errorStatus = _NO_ERROR;
  }
  return errorStatus;
}

/***********************************************************************
 *
 * Function: uart_setup_trans_mode
 *
 * Purpose: Sets up a UART data transfer mode
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     puartcfg   : Pointer to UART configuration data
 *     puartsetup : Pointer to a UART transfer mode setup structure
 *
 * Outputs: None
 *
 * Returns: _NO_ERROR if setup was ok, otherwise _ERROR
 *
 * Notes: None
 *
 **********************************************************************/
STATUS uart_setup_trans_mode(UART_CFG_T *puartcfg,
                             UART_CONTROL_T *puartsetup)
{
  UNS_32 tmp = 0;
  STATUS err = _NO_ERROR;

  /* Setup stop bits */
  switch (puartsetup->stopbits)
  {
    case 2:
      tmp |= UART_LCR_STOP2BITS;
    case 1:
      break;

    default:
      err = _ERROR;
      break;
  }

  /* Setup parity */
  switch (puartsetup->parity)
  {
    case UART_PAR_EVEN:
      tmp |= (UART_LCR_PARITY_EVEN | UART_LCR_PARITY_ENABLE);
      break;

    case UART_PAR_ODD:
      tmp |= (UART_LCR_PARITY_ODD | UART_LCR_PARITY_ENABLE);
      break;

    case UART_PAR_NONE:
      break;

    default:
      err = _ERROR;
      break;
  }

  /* Setup data bits */
  switch (puartsetup->databits)
  {
    case 5:
      tmp |= UART_LCR_WLEN_5BITS;
      break;

    case 6:
      tmp |= UART_LCR_WLEN_6BITS;
      break;

    case 7:
      tmp |= UART_LCR_WLEN_7BITS;
      break;

    case 8:
      tmp |= UART_LCR_WLEN_8BITS;
      break;

    default:
      err = _ERROR;
      break;
  }

  /* Find closest baud rate for desired clock frequency */
  err = uart_set_divisors(puartcfg, puartsetup->baud_rate);

  if (err == _NO_ERROR)
  {
    /* Set new UART settings */
    puartcfg->regptr->lcr = tmp;
  }

  return err;
}

/***********************************************************************
 * UART driver public functions
 **********************************************************************/

/***********************************************************************
 *
 * Function: uart_open
 *
 * Purpose: Open the UART
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     ipbase: Pointer to a UART peripheral block
 *     arg   : Pointer to UART setup structure or NULL
 *
 * Outputs: None
 *
 * Returns: The pointer to a UART config structure or NULL
 *
 * Notes: None
 *
 **********************************************************************/
INT_32 uart_open(void *ipbase,
                 INT_32 arg)
{
  UART_CONTROL_T uarttran;
  volatile UNS_32 tmp;
  INT_32 tptr = (INT_32) NULL;
  UART_CONTROL_T *pucfg = (UART_CONTROL_T *) arg;

  /* Has the UART been previously initialized? */
  if (uartdat.uart_init == FALSE)
  {
    /* UART is free */
    uartdat.uart_init = TRUE;
    uartdat.regptr = (UART_REGS_T *) ipbase;

    /* Enable UART system clock */
    cgu_clk_en_dis(CGU_SB_UART_APB_CLK_ID, TRUE);
    cgu_clk_en_dis(CGU_SB_UART_U_CLK_ID, TRUE);

    /* No callbacks by default */
    uartdat.cbs.rxcb = NULL;
    uartdat.cbs.txcb = NULL;
    uartdat.cbs.rxerrcb = NULL;

    /* Install UART interrupt handler */
    int_install_irq_handler(IRQ_UART, uart_int_handler);

    /* Setup default UART state for 115200N81 with FIFO mode */
    if (pucfg == NULL)
    {
      uarttran.baud_rate = 115200;
      uarttran.parity = UART_PAR_NONE;
      uarttran.stopbits = 1;
      uarttran.databits = 8;
      uart_setup_trans_mode(&uartdat, &uarttran);
    }
    else
    {
      uart_setup_trans_mode(&uartdat, pucfg);
    }

    /* Clear FIFOs, set FIFO level, and pending interrupts */
    uartdat.regptr->iir_fcr = (UART_FCR_RXFIFO_TL16 |
                               UART_FCR_FIFO_EN | UART_FCR_TXFIFO_FLUSH |
                               UART_FCR_RXFIFO_FLUSH);
    tmp = uartdat.regptr->iir_fcr;
    tmp = uartdat.regptr->lsr;

    /* Receive and RX line status interrupts enabled */
    uartdat.regptr->dlm_ie = (UART_IE_RXLINE_STS |
                              UART_IE_RDA | UART_IE_THRE);

    /* Return pointer to specific UART structure */
    tptr = (INT_32) & uartdat;
  }

  return tptr;
}

/***********************************************************************
 *
 * Function: uart_close
 *
 * Purpose: Close the UART
 *
 * Processing:
 *     If init is not TRUE, then return _ERROR to the caller as the
 *     device was not previously opened. Otherwise, disable the UART,
 *     set init to FALSE, and return _NO_ERROR to the caller.
 *
 * Parameters:
 *     devid: Pointer to UART config structure
 *
 * Outputs: None
 *
 * Returns: The status of the close operation
 *
 * Notes: None
 *
 **********************************************************************/
STATUS uart_close(INT_32 devid)
{
  UART_CFG_T *puart = (UART_CFG_T *) devid;
  STATUS status = _ERROR;

  /* Close and disable device if it was previously initialized */
  if (puart->uart_init == TRUE)
  {
    /* Disable interrupts */
    puart->regptr->dlm_ie = 0;

    uartdat.uart_init = FALSE;

    /* Turn off clocking */
    cgu_clk_en_dis(CGU_SB_UART_U_CLK_ID, FALSE);
    cgu_clk_en_dis(CGU_SB_UART_APB_CLK_ID, FALSE);
  }

  return status;
}

/***********************************************************************
 *
 * Function: uart_ioctl
 *
 * Purpose: UART configuration block
 *
 * Processing:
 *     This function is a large case block. Based on the passed function
 *     and option values, set or get the appropriate UART parameter.
 *
 * Parameters:
 *     devid: Pointer to UART config structure
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
STATUS uart_ioctl(INT_32 devid,
                  INT_32 cmd,
                  INT_32 arg)
{
  UART_CBS_T *pcbs;
  UART_CFG_T *puart = (UART_CFG_T *) devid;
  STATUS status = _ERROR;

  /* Close and disable device if it was previously initialized */
  if (puart->uart_init == TRUE)
  {
    status = _NO_ERROR;

    switch (cmd)
    {
      case UART_CLEAR_FIFOS:
        uart_flush_fifos(puart->regptr, (UNS_32) arg);
        break;

      case UART_SETUP_TRANSFER:
        uart_setup_trans_mode(puart, (UART_CONTROL_T *) arg);
        break;

      case UART_LOOPBACK_EN:
        if (arg != 0)
        {
          puart->regptr->mcr |= UART_MCR_LOOP;
        }
        else
        {
          puart->regptr->mcr &= ~UART_MCR_LOOP;
        }
        break;

      case UART_FORCE_BREAK:
        if (arg != 0)
        {
          puart->regptr->lcr |= UART_LCR_BREAK_EN;
        }
        else
        {
          puart->regptr->lcr &= ~UART_LCR_BREAK_EN;
        }
        break;

      case UART_INSTALL_CBS:
        pcbs = (UART_CBS_T *) arg;
        puart->cbs.rxcb = pcbs->rxcb;
        puart->cbs.txcb = pcbs->txcb;
        puart->cbs.rxerrcb = pcbs->rxerrcb;
        break;

      case UART_GET_STATUS:
        /* Return a UART status */
        switch (arg)
        {
          case UART_GET_IP_CLOCK:
            status = cgu_get_clk_freq(CGU_SB_UART_U_CLK_ID);
            break;

          case UART_GET_DERIVED_CLOCK:
            status = puart->baudrate;
            break;

          case UART_GET_LINE_STATUS:
            status = (STATUS) puart->regptr->lsr;
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

/***********************************************************************
 *
 * Function: uart_read
 *
 * Purpose: UART read function (stub only)
 *
 * Processing:
 *     Read the passed number of bytes in the passed buffer, or the
 *     amount of data that is available, whichever is less.
 *
 * Parameters:
 *     devid:     Pointer to UART descriptor
 *     buffer:    Pointer to data buffer to copy to
 *     max_bytes: Number of bytes to read
 *
 * Outputs: None
 *
 * Returns: Number of bytes actually read
 *
 * Notes: None
 *
 **********************************************************************/
INT_32 uart_read(INT_32 devid,
                 void *buffer,
                 INT_32 max_bytes)
{
  INT_32 bread = 0;
  UART_CFG_T *puart = (UART_CFG_T *) devid;
  UART_REGS_T *pregs = puart->regptr;
  UNS_8 *buff8 = (UNS_8 *) buffer;

  while ((max_bytes > 0) && ((pregs->lsr & UART_LSR_RDR) != 0))
  {
    *buff8 = (UNS_8) pregs->dll_fifo;
    buff8++;
    max_bytes--;
    bread++;
  }

  return bread;
}

/***********************************************************************
 *
 * Function: uart_write
 *
 * Purpose: Timer UART function
 *
 * Processing:
 *     Write the passed number of bytes in the passed buffer to the UART
 *     FIFO, or the amounf of data that the FIFO can handle.
 *
 * Parameters:
 *     devid:   Pointer to UART descriptor
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
INT_32 uart_write(INT_32 devid,
                  void *buffer,
                  INT_32 n_bytes)
{
  INT_32 bwrite = 0;
  UART_CFG_T *puart = (UART_CFG_T *) devid;
  UART_REGS_T *pregs = puart->regptr;
  UNS_8 *buff8 = (UNS_8 *) buffer;

  /* Only add data if the current FIFO level can be determined */
  if ((pregs->lsr & UART_LSR_TEMT) != 0)
  {
    /* Assuming TX fifo is 32 byte deep. */
    if (n_bytes > 32)
    {
      n_bytes = 32;
    }
    while (n_bytes > 0)
    {
      pregs->dll_fifo = (UNS_32) * buff8;
      buff8++;
      n_bytes--;
      bwrite++;
    }
  }

  return bwrite;
}
