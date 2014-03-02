/***********************************************************************
 * $Id:: pca9532_driver.h 3304 2010-04-20 21:04:01Z usb10131               $
 *
 * Project: LPC313x PCa9532 driver
 *
 * Description:
 *     This file contains driver support for the LPC313x PCA9532.
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

#ifndef PCA9532_DRIVER_H
#define PCA9532_DRIVER_H

#include "lpc313x_i2c.h"

#ifdef __cplusplus
extern "C"
{
#endif

/***********************************************************************
 * PCA9532 device configuration commands (IOCTL commands and arguments)
 **********************************************************************/

/* PCA9532 device commands (IOCTL commands) */
typedef enum
{
  PCA9532_SET_LED,               /* PCa9532 setup LED */
  PCA9532_GET_LED,               /* PCA9532 get LED status */
} PCA9532_IOCTL_CMD_T;

/***********************************************************************
 * I2C driver API functions
 **********************************************************************/

/* Open a PCA9532 */
INT_32 pca9532_open(void *ipbase, INT_32 arg);

/* Close the PCA9532 */
STATUS pca9532_close(INT_32 devid);

/* I2C configuration block */
STATUS pca9532_ioctl(INT_32 devid,
                     INT_32 cmd,
                     INT_32 arg);

/* I2C read function (stub only) */
INT_32 pca9532_read(INT_32 devid,
                    void *buffer,
                    INT_32 max_bytes);

/* I2C write function (stub only) */
INT_32 pca9532_write(INT_32 devid,
                     void *buffer,
                     INT_32 n_bytes);

#ifdef __cplusplus
}
#endif

#endif /* PCA9532_DRIVER_H */
