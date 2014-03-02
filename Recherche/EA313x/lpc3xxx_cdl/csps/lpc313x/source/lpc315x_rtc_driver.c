/***********************************************************************
 * $Id:: lpc315x_rtc_driver.c 6061 2011-01-04 06:03:30Z ing02124       $
 *
 * Project: LPC31xx RTC driver
 *
 * Description:
 *     This file contains driver support for the LPC31xx RTC.
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
#include "lpc315x_analog.h"
#include "lpc313x_i2c.h"
#include "lpc313x_mstr_i2c_driver.h"
#include "lpc315x_rtc_driver.h"

/***********************************************************************
 * RTC/I2C example definitions
 **********************************************************************/
#define ANALOG_DIE_I2C_ADDR		0x0C

typedef struct
{
  UNS_8 i2caddr;
} RTC_DAT_T;
static RTC_DAT_T drvdata;

static INT_32 i2chandle;

/***********************************************************************
 * RTC driver package data
***********************************************************************/

/* RTC driver data */
static RTC_CFG_T rtcdat;

/***********************************************************************
 *
 * Function: delay
 *
 * Purpose: generate a delay
 *
 * Processing:
 *     A local software counter counts up to the specified count.
 *
 * Parameters:
 *    cnt : number to be counted
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 **********************************************************************/
void rtc_delay(UNS_32 cnt)
{
  UNS_32 i = cnt;
  while (i != 0) i--;
  return;
}

/***********************************************************************
 *
 * Function: analogdie_reg_read
 *
 * Purpose: write data/status from RTC via I2C to the analog die
 *
 * Processing:
 *     See function.
 *
 * Parameters: devid and pointer to the buffer
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
UNS_32 analogdie_reg_read(UNS_32 reg_addr)
{
  UNS_32 data;
  UNS_32  rxdata[8], txdata[8];
  I2C_MTXRX_SETUP_T xfer;
  UNS_32 status;

  txdata[0] = drvdata.i2caddr | I2C_TXFF_START_CND | 0;
  txdata[1] = (reg_addr & 0xFF00) >> 8;
  txdata[2] = (reg_addr & 0x00FF) >> 0;
  txdata[3] = drvdata.i2caddr | I2C_TXFF_START_CND | 1;
  txdata[4] = 0;
  txdata[5] = 0;
  txdata[6] = 0;
  txdata[7] = I2C_TXFF_STOP_CND | 0;
  xfer.tx_data = txdata;
  xfer.rx_data = rxdata;
  xfer.tx_length = 8;
  xfer.clock_rate = 100000;
  xfer.cb = NULL;

  if (i2c_mstr_ioctl(i2chandle, I2C_MSTR_TRANSFER, (INT_32) &xfer) ==
      _NO_ERROR)
  {
    status = i2c_mstr_ioctl(i2chandle, I2C_MSTR_GET_STATUS, 0);
    while ((status & (I2C_MSTR_STATUS_ARBF |
                      I2C_MSTR_STATUS_NOACKF | I2C_MSTR_STATUS_DONE)) == 0)
    {
      status = i2c_mstr_ioctl(i2chandle,
                              I2C_MSTR_GET_STATUS, 0);
    }
  }

  data = ((rxdata[3] & 0xFF) | ((rxdata[2] & 0xFF) << 8) |
          ((rxdata[1] & 0xFF) << 16) | ((rxdata[0] & 0xFF) << 24));

  return data;
}

/***********************************************************************
 *
 * Function: analogdie_reg_write
 *
 * Purpose: write command to RTC via I2C to the analog die
 *
 * Processing:
 *     See function.
 *
 * Parameters: devid and pointer to the buffer
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void analogdie_reg_write(UNS_32 reg_addr, UNS_32 reg_value)
{
  UNS_32 rxdata[8], txdata[8];
  I2C_MTXRX_SETUP_T xfer;
  UNS_32 status;

  txdata[0] = drvdata.i2caddr | I2C_TXFF_START_CND | 0;
  txdata[1] = (reg_addr & 0xFF00) >> 8;
  txdata[2] = (reg_addr & 0x00FF) >> 0;
  txdata[3] = (reg_value & 0xFF000000) >> 24;
  txdata[4] = (reg_value & 0x00FF0000) >> 16;
  txdata[5] = (reg_value & 0x0000FF00) >> 8;
  txdata[6] = I2C_TXFF_STOP_CND | (reg_value & 0x000000FF) >> 0;
  xfer.tx_data = txdata;
  xfer.rx_data = rxdata;
  xfer.tx_length = 7;
  xfer.clock_rate = 100000;
  xfer.cb = NULL;

  if (i2c_mstr_ioctl(i2chandle, I2C_MSTR_TRANSFER, (INT_32) &xfer) ==
      _NO_ERROR)
  {
    status = i2c_mstr_ioctl(i2chandle, I2C_MSTR_GET_STATUS, 0);
    while ((status & (I2C_MSTR_STATUS_ARBF |
                      I2C_MSTR_STATUS_NOACKF | I2C_MSTR_STATUS_DONE)) == 0)
    {
      status = i2c_mstr_ioctl(i2chandle,
                              I2C_MSTR_GET_STATUS, 0);
    }
  }

}

/***********************************************************************
 *
 * Function: rtc_open
 *
 * Purpose: Open the RTC
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     ipbase: Pointer to a RTC peripheral block
 *     arg   : Not used
 *
 * Outputs: None
 *
 * Returns: The pointer to a RTC config structure or NULL
 *
 * Notes: No settings for the timer are changed with this call.
 *
 **********************************************************************/
INT_32 rtc_open(void *ipbase, INT_32 arg)
{
  if (rtcdat.init != TRUE)
  {
    drvdata.i2caddr = ANALOG_DIE_I2C_ADDR << 1;

    rtcdat.init = TRUE;
    i2chandle = i2c_mstr_open((void *) I2C1, 0);
    if (i2chandle == 0)
      return _ERROR;


  }
  return (0);
}

/***********************************************************************
 *
 * Function: rtc_close
 *
 * Purpose: Close the RTC
 *
 * Processing:
 *     If init is not TRUE, then return _ERROR to the caller as the
 *     device was not previously opened. Otherwise, set init to FALSE
 *     and return _NO_ERROR to the caller.
 *
 * Parameters:
 *     devid: Pointer to RTC config structure
 *
 * Outputs: None
 *
 * Returns: The status of the close operation
 *
 * Notes: None
 *
 **********************************************************************/
STATUS rtc_close(INT_32 devid)
{
  rtcdat.init = FALSE;
  return (i2c_mstr_close(devid));
}

/***********************************************************************
 *
 * Function: rtc_ioctl
 *
 * Purpose: RTC configuration block
 *
 * Processing:
 *     This function is a large case block. Based on the passed function
 *     and option values, set or get the appropriate RTC parameter.
 *
 * Parameters:
 *     devid: Pointer to RTC config structure
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
STATUS rtc_ioctl(INT_32 devid, INT_32 cmd, RTC_SETUP_T *arg)
{
  RTC_SETUP_T *param;
  INT_32 rtc_status;

  INT_32 status = _NO_ERROR;

  if (rtcdat.init == TRUE)
  {
    param  = (RTC_SETUP_T *)arg;
    switch (cmd)
    {
      case RTC_SETUP_TIME:
        rtc_status = analogdie_reg_read(AD_REG_RTC_STATUS);
        if ((rtc_status & RTC_TIME_UNSET)
            && !(rtc_status & RTC_TIME_PENDING))
        {
          analogdie_reg_write(AD_REG_RTC_TIME, param->tx_data);
          /* UNSET should be cleared after data is written to RTC_TIME.
          However, it doesn't seem to be happening. reg_read() for
          debugging. */
          rtc_status = analogdie_reg_read(AD_REG_RTC_STATUS);
        }
        else if (rtc_status & RTC_TIME_PENDING)
        {
          status = _ERROR;
        }
        break;
      case RTC_SETUP_ALARM:
        analogdie_reg_write(AD_REG_RTC_ALARM_TIME, param->tx_data);
        break;
      case RTC_ENA_SET:
        analogdie_reg_write(AD_REG_RTC_SET_ENA_STAT, param->tx_data);
        break;
      case RTC_DIS_CLR:
        analogdie_reg_write(AD_REG_RTC_CLR_ENA_STAT, param->tx_data);
        break;
      case RTC_GET_STATUS:
        param->rx_data = analogdie_reg_read(AD_REG_RTC_STATUS);
        break;
      case RTC_GET_TIME:
        param->rx_data = analogdie_reg_read(AD_REG_RTC_TIME);
        break;
      case RTC_GET_MOD_ID:
        /* This is the first to call, some house keeping work needs to
        be finished first before reading the MOD ID register. */
        while (1)
        {
          rtc_status = analogdie_reg_read(AD_REG_RTC_STATUS);
          if (rtc_status & RTC_LS_ENA)
          {
            if (!(rtc_status & RTC_TIME_PENDING))
            {
              break;	/* From while ( 1 ) */
            }
          }
        }
        param->rx_data = analogdie_reg_read(AD_REG_MOD_ID);
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
 * Function: rtc_read
 *
 * Purpose: RTC read function (stub only)
 *
 * Processing:
 *     Return 0 to the caller.
 *
 * Parameters:
 *     devid:     Pointer to RTC descriptor
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
INT_32 rtc_read(INT_32 devid, void *buffer, INT_32 max_bytes)
{
  return 0;
}

/***********************************************************************
 *
 * Function: rtc_write
 *
 * Purpose: Timer RTC function (stub only)
 *
 * Processing:
 *     Return 0 to the caller.
 *
 * Parameters:
 *     devid:   Pointer to RTC descriptor
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
INT_32 rtc_write(INT_32 devid, void *buffer, INT_32 n_bytes)
{
  return 0;
}
