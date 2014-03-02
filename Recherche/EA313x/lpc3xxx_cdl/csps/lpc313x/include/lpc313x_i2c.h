/***********************************************************************
* $Id:: lpc313x_i2c.h 1334 2008-11-20 03:10:10Z tangdz                $
*
* Project: LPC313X I2C Master Slave interface controller definitions
*
* Description:
*     This file contains the structure definitions and manifest
*     constants for the LPC313X chip family component:
*         I2C Master Slave interface Controller
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

#ifndef LPC313X_I2C_H
#define LPC313X_I2C_H

#include "lpc_types.h"
#include "lpc313x_chip.h"

#if defined(__cplusplus)
extern "C"
{
#endif

/***********************************************************************
* I2C Master Slave interface controller Register Structures
**********************************************************************/

/* I2C Master Slave interface Controller Module Register Structure */
typedef volatile struct
{
  /* LSB */
  volatile UNS_32 fifo; /* Receive FIFO Register - Read Only */
  /* Transmit FIFO Register - Write Only */
  volatile UNS_32 sts;  /* Status Register - Read Only */
  volatile UNS_32 ctl;  /* Control Register - Read/Write */
  volatile UNS_32 ckh;  /* Clock divider high  - Read/Write */
  volatile UNS_32 ckl;  /* Clock divider low - Read/Write */
  volatile UNS_32 adr;  /* I2C address (optional) - Read/Write */
  volatile const UNS_32 rfl;  /* Rx FIFO level (optional) - Read Only */
  volatile const UNS_32 tfl;  /* Tx FIFO level (optional) - Read Only */
  volatile const UNS_32 rxb;  /* Number of bytes received (opt.) - Read Only */
  volatile const UNS_32 txb;  /* Number of bytes transmitted (opt.) - Read Only */
  volatile UNS_32 txs;  /* Tx Slave FIFO register - Write Only */
  volatile const UNS_32 stfl; /* Tx Slave FIFO level - Read Only */
  /* MSB */
} I2C_REGS_T;

/***********************************************************************
 * I2C device contants
 **********************************************************************/
#define SPI_FIFO_DEPTH  64 /* 64 words (16bit) deep */


/***********************************************************************
 * I2C Tx FIFO register definitions
 **********************************************************************/
#define I2C_TXFF_STOP_CND   _BIT(9)
#define I2C_TXFF_START_CND  _BIT(8)
#define I2C_TXFF_DATA_MSK   0xFF

/***********************************************************************
 * I2C status register definitions
 **********************************************************************/
#define I2C_STS_TFES        _BIT(13)
#define I2C_STS_TFFS        _BIT(12)
#define I2C_STS_TFE         _BIT(11)
#define I2C_STS_TFF         _BIT(10)
#define I2C_STS_RFE         _BIT(9)
#define I2C_STS_RFF         _BIT(8)
#define I2C_STS_SDA         _BIT(7)
#define I2C_STS_SCL         _BIT(6)
#define I2C_STS_ACTIVE      _BIT(5)
#define I2C_STS_DRSI        _BIT(4)
#define I2C_STS_DRMI        _BIT(3)
#define I2C_STS_NAI         _BIT(2)
#define I2C_STS_AFI         _BIT(1)
#define I2C_STS_TDI         _BIT(0)

/***********************************************************************
 * I2C control register definitions
 **********************************************************************/
#define I2C_CTL_TFFSIE       _BIT(10)
// #define I2C_CTL_SEVEN        _BIT(9) /* Seven-bit slave address */
#define I2C_CTL_SOFT_RESET   _BIT(8)
#define I2C_CTL_TFFIE        _BIT(7)
#define I2C_CTL_DAIE         _BIT(6)
#define I2C_CTL_RFFIE        _BIT(5)
#define I2C_CTL_DRSIE        _BIT(4)
#define I2C_CTL_DRMIE        _BIT(3)
#define I2C_CTL_NAIE         _BIT(2)
#define I2C_CTL_AFIE         _BIT(1)
#define I2C_CTL_TDIE         _BIT(0)

/**********************************************************************
* i2c channel select
**********************************************************************/

/* Macro pointing to I2C controller registers */
#define I2C0  ((I2C_REGS_T  *)(I2C0_BASE))
#define I2C1  ((I2C_REGS_T  *)(I2C1_BASE))



#if defined(__cplusplus)
}
#endif

#endif /* LPC313X_I2C_H */
