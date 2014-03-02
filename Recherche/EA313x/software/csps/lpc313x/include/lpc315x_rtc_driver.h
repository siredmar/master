/***********************************************************************
 * $Id:: lpc315x_rtc_driver.h 1388 2008-12-01 19:20:06Z pdurgesh       $
 *
 * Project: LPC315x RTC driver
 *
 * Description:
 *     This file contains driver support for the LPC315x RTC.
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

#ifndef LPC315X_RTC_DRIVER_H
#define LPC315X_RTC_DRIVER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "lpc313x_i2c.h"
#include "lpc315x_rtc.h"

/* RTC device configuration structure type */
typedef struct
{
  I2C_REGS_T *regptr;
  BOOL_32 init;
} RTC_CFG_T;

/* structure containing RTC channel access */
typedef struct
{
  UNS_32		tx_data;
  UNS_32		rx_data;
  UNS_32		status;
} RTC_SETUP_T;

/* structure containing year, month, day, and time */
typedef struct
{
  UNS_32		rtc_second;
  UNS_32		rtc_minute;
  UNS_32		rtc_hour;
  UNS_32		rtc_day;
  UNS_32		rtc_month;
  UNS_32		rtc_year;
} RTC_TIME_T;

/***********************************************************************
 * RTC configuration commands (IOCTL commands and arguments)
 **********************************************************************/

/* RTC device commands (IOCTL commands) */
typedef enum
{
  RTC_SETUP_TIME,
  RTC_SETUP_ALARM,
  RTC_ENA_SET,
  RTC_DIS_CLR,
  RTC_GET_TIME,
  RTC_GET_STATUS,
  RTC_GET_MOD_ID
} RTC_IOCTL_CMD_T;

/***********************************************************************
 * RTC driver API functions
 **********************************************************************/

/* Open and close functions will NOT alter RTC functions */

/* Open the RTC */
INT_32 rtc_open(void *ipbase, INT_32 arg);

/* Close the RTC */
STATUS rtc_close(INT_32 devid);

/* RTC configuration block */
STATUS rtc_ioctl(INT_32 devid, INT_32 cmd, RTC_SETUP_T *arg);

/* RTC read function (stub only) */
INT_32 rtc_read(INT_32 devid, void *buffer, INT_32 max_bytes);

/* RTC write function (stub only) */
INT_32 rtc_write(INT_32 devid, void *buffer, INT_32 n_bytes);

void rtc_delay(UNS_32 cnt);
UNS_32 analog_reg_read(INT_32 devid, UNS_32 reg_addr);
void analog_reg_write(INT_32 devid, UNS_32 reg_addr, UNS_32 reg_value);

#ifdef __cplusplus
}
#endif

#endif /* LPC315X_RTC_DRIVER_H */
