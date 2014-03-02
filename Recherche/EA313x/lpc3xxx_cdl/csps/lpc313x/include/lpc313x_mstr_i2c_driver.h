/***********************************************************************
 * $Id:: lpc313x_mstr_i2c_driver.h 3557 2010-05-20 00:31:30Z usb10131  $
 *
 * Project: LPC313x alternate I2C driver (master only)
 *
 * Description:
 *     This file contains an alternate I2C driver for the LPC31XX using
 *     master only mode. The driver allows multiple opens and handle
 *     large transfer sizes.
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

#ifndef LPC313X_MSTR_I2C_DRIVER_H
#define LPC313X_MSTR_I2C_DRIVER_H

#include "lpc313x_i2c.h"

#ifdef __cplusplus
extern "C"
{
#endif

/***********************************************************************
 * Master only I2C interrupt handler
 **********************************************************************/

/* This handler needs to be installed by the user application */
void i2c_mstr_interrupt(void);

/***********************************************************************
 * I2C device configuration commands (IOCTL commands and arguments)
 **********************************************************************/

/* Master I2C device commands (IOCTL commands) */
typedef enum
{
  I2C_MSTR_TRANSFER,
  I2C_MSTR_GET_STATUS
} I2C_MSTR_IOCTL_CMD_T;

/* Possible I2C status values signalled to the app */
#define I2C_MSTR_STATUS_BUSY   _BIT(0) /* Transfer in progress */
#define I2C_MSTR_STATUS_ARBF   _BIT(8) /* Data busy, arbitration */
#define I2C_MSTR_STATUS_NOACKF _BIT(9) /* No ACK */
#define I2C_MSTR_STATUS_DONE   _BIT(10) /* Successful transfer */

/* Callback function prototype used for I2C callback status to the
   application */
typedef void (*I2C_MSTR_CB)(UNS_32 status);

/* Structure defining the I2C transfer. It is up to the application
   to define how the transfer will occur */
typedef struct
{
  UNS_32* tx_data;
  UNS_32 tx_length;
  UNS_32* rx_data;
  UNS_32 clock_rate; /* Clock rate for this setup */
  I2C_MSTR_CB cb; /* Function called by driver when transfer is done */
} I2C_MTXRX_SETUP_T;

/***********************************************************************
 * I2C driver API functions
 **********************************************************************/

/* Open a I2C */
INT_32 i2c_mstr_open(void *ipbase, INT_32 arg);

/* Close the I2C */
STATUS i2c_mstr_close(INT_32 devid);

/* I2C configuration block */
STATUS i2c_mstr_ioctl(INT_32 devid,
                      INT_32 cmd,
                      INT_32 arg);

/* I2C read function (stub only) */
INT_32 i2c_mstr_read(INT_32 devid,
                     void *buffer,
                     INT_32 max_bytes);

/* I2C write function (stub only) */
INT_32 i2c_mstr_write(INT_32 devid,
                      void *buffer,
                      INT_32 n_bytes);

/* Interrupt handlers to be installed for the application when using
   I2C channels 0 or 1 */
void i2c0_mstr_int_hanlder(void);
void i2c1_mstr_int_hanlder(void);

#ifdef __cplusplus
}
#endif

#endif /* LPC313X_MSTR_I2C_DRIVER_H */
