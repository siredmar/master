/***********************************************************************
 * $Id:: lpc315x_otp_driver.h 745 2008-05-13 19:59:29Z pdurgesh      $
 *
 * Project: LPC315x OTP driver
 *
 * Description:
 *     This file contains driver support for the LPC315x OTP.
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

#ifndef LPC315X_OTP_DRIVER_H
#define LPC315X_OTP_DRIVER_H

#include "lpc315x_otp.h"

#ifdef __cplusplus
extern "C"
{
#endif

/***********************************************************************
 * OTP device configuration commands (IOCTL commands and arguments)
 **********************************************************************/

/* OTP device commands (IOCTL commands) */
typedef enum
{
  OTP_SET_WRITE_PROTECT,
  OTP_SET_READ_PROTECT,
  OTP_GET_JTAG_SEC_LEVEL,
  OTP_GET_BANDGAP_ADJ,
  OTP_GET_AES_KEY,
  OTP_GET_NOAD_STATUS,
  OTP_REREAD_FUSES
} OTP_IOCTL_CMD_T;

typedef struct
{
  PFV set_read_voltage;
  PFV set_write_voltage;
} OTP_OPEN_ARG_T;

typedef struct
{
  UNS_32 data[4];
} OTP_AES_KEY_T;

typedef struct
{
  UNS_32 data[4];
} OTP_UNIQUE_ID_T;

/***********************************************************************
 * OTP driver API functions
 **********************************************************************/

/* Open the OTP */
INT_32 otp_open(void *ipbase, INT_32 arg);

/* Close the OTP */
STATUS otp_close(INT_32 devid);

/* OTP configuration block */
STATUS otp_ioctl(INT_32 devid,
                 INT_32 cmd,
                 INT_32 arg);

/* OTP read function - read all 16 data registers */
INT_32 otp_read(INT_32 devid,
                void *buffer,
                INT_32 max_bytes);

/* OTP write function - writes all 16 data registers */
INT_32 otp_write(INT_32 devid,
                 void *buffer,
                 INT_32 n_bytes);

/***********************************************************************
 * Other OTP driver functions
 **********************************************************************/


#ifdef __cplusplus
}
#endif

#endif /* LPC315X_OTP_DRIVER_H */
