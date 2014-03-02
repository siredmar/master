/***********************************************************************
 * $Id:: lpc315x_rtc_driver.c 1388 2008-12-01 19:20:06Z pdurgesh       $
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
#include "lpc313x_i2c_driver.h"
#include "lpc315x_rtc_driver.h"

/***********************************************************************
 * RTC/I2C example definitions
 **********************************************************************/
#define ANALOG_DIE_I2C_ADDR		0x0C
#define DATA_LENGTH	16

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
 * Function: analog_reg_read
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
UNS_32 analog_reg_read(INT_32 devid, UNS_32 reg_addr)
{
  static I2C_MTXRX_SETUP_T i2c_mtxrx_setup;
  UNS_32 reg_value;
  UNS_8 tx_data[DATA_LENGTH], rx_data[DATA_LENGTH];

  /* high byte of the register address first, then low byte. */
  tx_data[0] = (UNS_8)((reg_addr >> 8) & 0xFF);
  tx_data[1] = (UNS_8)(reg_addr & 0xFF);
  i2c_mtxrx_setup.addr_mode = ADDR7BIT;
  i2c_mtxrx_setup.sl_addr = ANALOG_DIE_I2C_ADDR;
  i2c_mtxrx_setup.tx_data = &tx_data[0];
  i2c_mtxrx_setup.tx_length = 2;
  i2c_mtxrx_setup.rx_data = &rx_data[0];
  i2c_mtxrx_setup.rx_length = 4;
  i2c_mtxrx_setup.retransmissions_max = 1;
  i2c_ioctl(devid, I2C_MASTER_TXRX, (INT_32)&i2c_mtxrx_setup);

  while ((i2c_mtxrx_setup.status & I2C_SETUP_STATUS_DONE) == 0);
  reg_value  = ((rx_data[0] & 0xFF) << 24);
  reg_value |= ((rx_data[1] & 0xFF) << 16);
  reg_value |= ((rx_data[2] & 0xFF) << 8);
  reg_value |= (rx_data[3] & 0xFF);
  return(reg_value);
}

/***********************************************************************
 *
 * Function: analog_reg_write
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
void analog_reg_write(INT_32 devid, UNS_32 reg_addr, UNS_32 reg_value)
{
  UNS_8 tx_data[DATA_LENGTH];
  static I2C_MTX_SETUP_T   i2c_mtx_setup;

  /* high byte of the register address first, then low byte. */
  tx_data[0] = (UNS_8)((reg_addr >> 8) & 0xFF);
  tx_data[1] = (UNS_8)(reg_addr & 0xFF);
  /* 32 bit of data with MSB first. */
  tx_data[2] = (UNS_8)(reg_value >> 24);
  tx_data[3] = (UNS_8)((reg_value >> 16) & 0xFF);
  tx_data[4] = (UNS_8)((reg_value >> 8) & 0xFF);
  tx_data[5] = (UNS_8)(reg_value & 0xFF);
  i2c_mtx_setup.addr_mode = ADDR7BIT;
  i2c_mtx_setup.sl_addr = ANALOG_DIE_I2C_ADDR;
  i2c_mtx_setup.tx_data = &tx_data[0];
  i2c_mtx_setup.tx_length = 6;
  i2c_mtx_setup.retransmissions_max = 1;
  i2c_ioctl(devid, I2C_MASTER_TX, (INT_32)&i2c_mtx_setup);
  while ((i2c_mtx_setup.status & I2C_SETUP_STATUS_DONE) == 0);
  rtc_delay(1000000);
  return;
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
    rtcdat.init = TRUE;
    return (i2c_open(ipbase, arg));
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
  return (i2c_close(devid));
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
        rtc_status = analog_reg_read(devid, AD_REG_RTC_STATUS);
        if ((rtc_status & RTC_TIME_UNSET)
            && !(rtc_status & RTC_TIME_PENDING))
        {
          analog_reg_write(devid, AD_REG_RTC_TIME, param->tx_data);
          /* UNSET should be cleared after data is written to RTC_TIME.
          However, it doesn't seem to be happening. reg_read() for
          debugging. */
          rtc_status = analog_reg_read(devid, AD_REG_RTC_STATUS);
        }
        else if (rtc_status & RTC_TIME_PENDING)
        {
          status = _ERROR;
        }
        break;
      case RTC_SETUP_ALARM:
        analog_reg_write(devid, AD_REG_RTC_ALARM_TIME, param->tx_data);
        break;
      case RTC_ENA_SET:
        analog_reg_write(devid, AD_REG_RTC_SET_ENA_STAT, param->tx_data);
        break;
      case RTC_DIS_CLR:
        analog_reg_write(devid, AD_REG_RTC_CLR_ENA_STAT, param->tx_data);
        break;
      case RTC_GET_STATUS:
        param->rx_data = analog_reg_read(devid, AD_REG_RTC_STATUS);
        break;
      case RTC_GET_TIME:
        param->rx_data = analog_reg_read(devid, AD_REG_RTC_TIME);
        break;
      case RTC_GET_MOD_ID:
        /* This is the first to call, some house keeping work needs to
        be finished first before reading the MOD ID register. */
        while (1)
        {
          rtc_status = analog_reg_read(devid, AD_REG_RTC_STATUS);
          if (rtc_status & RTC_LS_ENA)
          {
            if (!(rtc_status & RTC_TIME_PENDING))
            {
              break;	/* From while ( 1 ) */
            }
          }
        }
        param->rx_data = analog_reg_read(devid, AD_REG_MOD_ID);
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
