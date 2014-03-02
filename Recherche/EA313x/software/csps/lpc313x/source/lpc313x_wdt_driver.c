/***********************************************************************
 * $Id:: lpc313x_wdt_driver.c 1180 2008-09-11 19:32:40Z pdurgesh       $
 *
 * Project: LPC313x WDT driver
 *
 * Description:
 *     This file contains driver support for the LPC313x WDT.
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

#include "lpc313x_wdt_driver.h"
#include "lpc313x_cgu_driver.h"

/***********************************************************************
 * WDT driver package data
***********************************************************************/

/* WDT device configuration structure type */
typedef struct
{
  WDT_REGS_T *regptr;     /* Pointer to WDT registers */
  BOOL_32 init;           /* Device initialized flag */
} WDT_CFG_T;

/* WDT driver data */
static WDT_CFG_T wdtdat;

/***********************************************************************
 * WDT driver private functions
 **********************************************************************/

/***********************************************************************
 * WDT driver public functions
 **********************************************************************/

/***********************************************************************
 *
 * Function: wdt_open
 *
 * Purpose: Open the WDT
 *
 * Processing:
 *     If the passed register base is valid and the WDT
 *     driver isn't already initialized, then setup the WDT
 *     into a default initialized state that is disabled. Return
 *     a pointer to the driver's data structure or NULL if driver
 *     initialization failed.
 *
 * Parameters:
 *     ipbase: Pointer to a WDT peripheral block
 *     arg   : Not used
 *
 * Outputs: None
 *
 * Returns: The pointer to a WDT config structure or NULL
 *
 * Notes: None
 *
 **********************************************************************/
INT_32 wdt_open(void *ipbase, INT_32 arg)
{
  INT_32 tptr = (INT_32) NULL;

  /* has the selected WDT been previously initialized? */
  if (wdtdat.init == FALSE)
  {
    /* Device not initialized and it is usable, so set it to used */
    wdtdat.init = TRUE;

    /* Save address of register block */
    wdtdat.regptr = WDT;

    /* enable WDT clock */
    cgu_clk_en_dis(CGU_SB_WDOG_PCLK_ID, 1);

    /* Disable WDT timer and outputs */
    wdtdat.regptr->tcr = 0x00;
    wdtdat.regptr->emr = 0x00;

    tptr = (INT_32) & wdtdat;
  }

  return tptr;
}

/***********************************************************************
 *
 * Function: wdt_close
 *
 * Purpose: Close the WDT
 *
 * Processing:
 *     If init is not TRUE, then return _ERROR to the caller as the
 *     device was not previously opened. Otherwise, disable the
 *     WDT, set init to FALSE, and return _NO_ERROR to the caller.
 *
 * Parameters:
 *     devid: Pointer to WDT config structure
 *
 * Outputs: None
 *
 * Returns: The status of the close operation
 *
 * Notes: None
 *
 **********************************************************************/
STATUS wdt_close(INT_32 devid)
{
  STATUS status = _ERROR;

  if (wdtdat.init == TRUE)
  {
    /* Disable WDT */
    wdtdat.regptr->tcr = 0x00;
    wdtdat.regptr->emr = 0x00;

    /* Set timer as uninitialized */
    wdtdat.init = FALSE;

    /* Successful operation */
    status = _NO_ERROR;
  }

  return status;
}

/***********************************************************************
 *
 * Function: wdt_ioctl
 *
 * Purpose: WDT configuration block
 *
 * Processing:
 *     This function is a large case block. Based on the passed function
 *     and option values, set or get the appropriate timer parameter.
 *
 * Parameters:
 *     devid: Pointer to WDT config structure
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
STATUS wdt_ioctl(INT_32 devid,
                 INT_32 cmd,
                 INT_32 arg)
{
  INT_32 status = _ERROR;

  if (wdtdat.init == TRUE)
  {
    status = _NO_ERROR;

    switch (cmd)
    {
      case WDT_TIMER_CONTROL:
        switch (arg)
        {
          case WDT_TIMER_OFF:
            wdtdat.regptr->tcr = 0x00;
            break;

          case WDT_TIMER_STOP:
            wdtdat.regptr->tcr &= ~WDT_TCR_CNT_EN;
            break;

          case WDT_TIMER_RESET:
            wdtdat.regptr->tcr |= WDT_TCR_CNT_RESET;
            wdtdat.regptr->tcr &= ~WDT_TCR_CNT_RESET;
            break;

          case WDT_TIMER_RESTART:
            wdtdat.regptr->tcr &= ~WDT_TCR_CNT_EN;
            wdtdat.regptr->tcr |= WDT_TCR_CNT_RESET;
            wdtdat.regptr->tcr &= ~WDT_TCR_CNT_RESET;
            wdtdat.regptr->tcr |= WDT_TCR_CNT_EN;
            break;

          case WDT_TIMER_GO:
            wdtdat.regptr->tcr |= WDT_TCR_CNT_EN;
            break;

          default:
            status = LPC_BAD_PARAMS;
            break;
        }
        break;

      case WDT_COUNTER_READ:
        status = wdtdat.regptr->tc;
        break;

      case WDT_COUNTER_WRITE:
        wdtdat.regptr->tc = arg;
        break;

      case WDT_MATCH_WRITE:
        wdtdat.regptr->mr0 = arg;
        break;

      case WDT_SETUP:
        break;


      case WDT_INT_ENABLE:
        wdtdat.regptr->mcr |= WDT_MCR_INT_MR0;
        break;

      case WDT_INT_DISABLE:
        wdtdat.regptr->mcr &= ~WDT_MCR_INT_MR0;
        break;

      case WDT_INT_CLEAR:
        wdtdat.regptr->ir |= WDT_IR_MR0;
        break;

      default:
        break;
    }
  }

  return status;
}

/***********************************************************************
 *
 * Function: wdt_read
 *
 * Purpose: WDT read function (stub only)
 *
 * Processing:
 *     Return 0 to the caller.
 *
 * Parameters:
 *     devid:     Pointer to WDT descriptor
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
INT_32 wdt_read(INT_32 devid,
                void *buffer,
                INT_32 max_bytes)
{
  return 0;
}

/***********************************************************************
 *
 * Function: wdt_write
 *
 * Purpose: WDT write function (stub only)
 *
 * Processing:
 *     Return 0 to the caller.
 *
 * Parameters:
 *     devid:   Pointer to WDT descriptor
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
INT_32 wdt_write(INT_32 devid,
                 void *buffer,
                 INT_32 n_bytes)
{
  return 0;
}
