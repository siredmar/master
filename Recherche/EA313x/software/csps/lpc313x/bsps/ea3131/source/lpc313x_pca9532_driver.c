/***********************************************************************
 * $Id:: lpc313x_pca9532_driver.c 2530 2009-11-17 00:39:08Z pdurgesh       $
 *
 * Project: LPC3xxx PCA9532 driver
 *
 * Description:
 *     This file contains driver support for the LPC3xxx PCA9532.
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
#include "lpc313x_pca9532_driver.h"

#define PCA9532_ADDR0	0x60
#define DATA_LENGTH		16

/* I2C driver data for I2C0 and device handle */
static INT_32 i2cdev0;

/***********************************************************************
 * PCA9532 driver public functions
 **********************************************************************/
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
void delay(UNS_32 cnt)
{
  UNS_32 i = cnt;
  while (i != 0) i--;
  return;
}

/***********************************************************************
 *
 * Function: pca9532_open
 *
 * Purpose: Open the pca9532
 *
 * Processing:
 *     If the passed register base is valid and the pca9532
 *     driver isn't already initialized, then setup the pca9532
 *     into a default initialized state that is disabled. Return
 *     a pointer to the driver's data structure or NULL if driver
 *     initialization failed.
 *
 * Parameters:
 *     ipbase: Pointer to an I2C peripheral block
 *     arg   : Not used
 *
 * Outputs: None
 *
 * Returns: The pointer to an I2C config structure or NULL
 *
 * Notes: None
 *
 **********************************************************************/
INT_32 pca9532_open(void *ipbase, INT_32 arg)
{
  static I2C_SETUP_T i2c_setup[2];

  /* open I2C0 and I2C1 */
  i2cdev0 = i2c_open((void *)ipbase, 0);

  /* formally assign a 7-bit slave address 0x60 to I2C0    */
  /* I2C0 clock is 100 kHz, 50% duty cycle, high pin drive */
  /* This is a master only test, for both TX and RX.
  Slave address is used to generated packet header on the bus,
  not used for slave address register setting. */
  i2c_setup[0].addr_mode = ADDR7BIT;
  i2c_setup[0].rate_option = I2C_RATE_RELATIVE;
  i2c_setup[0].rate       = 100000;
  i2c_setup[0].low_phase  = 50;
  i2c_setup[0].high_phase = 50;
  i2c_ioctl(i2cdev0, I2C_SETUP, (INT_32)&i2c_setup[0]);
  return (i2cdev0);
}

/***********************************************************************
 *
 * Function: pca9532_close
 *
 * Purpose: Close the pca9532
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
STATUS pca9532_close(INT_32 devid)
{
  STATUS status = _ERROR;

  status = i2c_close(devid);
  return (status);
}

/***********************************************************************
 *
 * Function: pca9532_ioctl
 *
 * Purpose: pca9532 configuration block
 *
 * Processing:
 *     This function is a large case block. Based on the passed function
 *     and option values, set or get the appropriate timer parameter.
 *
 * Parameters:
 *     devid: Pointer to pca9532 config structure
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
STATUS pca9532_ioctl(INT_32 devid, INT_32 cmd, INT_32 arg)
{
  UNS_8  rx_data[DATA_LENGTH], tx_data[DATA_LENGTH];
  INT_32 status = _ERROR;
  UNS_8* temp_ptr;
  static I2C_MTX_SETUP_T   i2c_mtx_setup;
  static I2C_MTXRX_SETUP_T i2c_mtxrx_setup;

  if (((I2C_CFG_T *) devid)->regptr == I2C0)
  {
    if (((I2C_CFG_T *)devid)->init == TRUE)
    {
      status = _NO_ERROR;
      switch (cmd)
      {
        case PCA9532_SET_LED:
          /* Turn on LED 8~11 */
          tx_data[0] = arg >> 0x08;
          tx_data[1] = arg & 0xFF;
          i2c_mtx_setup.addr_mode = ADDR7BIT;
          i2c_mtx_setup.sl_addr = PCA9532_ADDR0;
          i2c_mtx_setup.tx_data = &tx_data[0];
          i2c_mtx_setup.tx_length = 2;
          i2c_mtx_setup.retransmissions_max = 1;
          i2c_ioctl(i2cdev0, I2C_MASTER_TX, (INT_32)&i2c_mtx_setup);
          while ((i2c_mtx_setup.status & I2C_SETUP_STATUS_DONE) == 0);
          delay(1000000);
          break;

        case PCA9532_GET_LED:
          temp_ptr = (UNS_8*)arg;
          tx_data[0] = *temp_ptr & 0xFF;
          i2c_mtxrx_setup.addr_mode = ADDR7BIT;
          i2c_mtxrx_setup.sl_addr = PCA9532_ADDR0;
          i2c_mtxrx_setup.tx_data = &tx_data[0];
          i2c_mtxrx_setup.tx_length = 1;
          i2c_mtxrx_setup.rx_data = &rx_data[0];
          i2c_mtxrx_setup.rx_length = 1;
          i2c_mtxrx_setup.retransmissions_max = 1;
          i2c_ioctl(i2cdev0, I2C_MASTER_TXRX, (INT_32)&i2c_mtxrx_setup);
          while ((i2c_mtxrx_setup.status & I2C_SETUP_STATUS_DONE) == 0);
          *temp_ptr = rx_data[0];
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
 * Function: pca9532_read
 *
 * Purpose: PCA9532 read function (stub only)
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
INT_32 pca9532_read(INT_32 devid,
                    void *buffer,
                    INT_32 max_bytes)
{
  return 0;
}

/***********************************************************************
 *
 * Function: pca9532_write
 *
 * Purpose: PCA9532 write function (stub only)
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
INT_32 pca9532_write(INT_32 devid,
                     void *buffer,
                     INT_32 n_bytes)
{
  return 0;
}
