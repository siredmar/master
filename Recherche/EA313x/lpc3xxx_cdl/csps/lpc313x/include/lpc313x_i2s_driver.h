/***********************************************************************
 * $Id:: lpc313x_i2s_driver.h 2554 2009-11-24 23:06:41Z pdurgesh       $
 *
 * Project: LPC3xxx standard UART driver
 *
 * Description:
 *     This file contains driver support for the LPC313X standard UART.
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

#ifndef LPC313X_I2S_DRIVER_H
#define LPC313X_I2S_DRIVER_H

#include "lpc313x_i2s.h"
#include "lpc313x_intc_driver.h"


#ifdef __cplusplus
extern "C"
{
#endif


/* defines for I2S modules      */
#define I2S_TX0			((I2S_REGS_T  *)(I2STX0_BASE))
#define I2S_TX1			((I2S_REGS_T  *)(I2STX1_BASE))
#define I2S_RX0			((I2S_REGS_T  *)(I2SRX0_BASE))
#define I2S_RX1			((I2S_REGS_T  *)(I2SRX1_BASE))

/* defines for I2S_FORMATS */
#define I2S								3
#define LSB_16							4
#define LSB_18							5
#define LSB_20							6
#define LSB_24							7

/* defines for I2S RX Master Slave Modes */
#define I2S_RX_SLAVE					0
#define I2S_RX_MASTER					1

/*  defines por I2S NSOF edge detection  */
#define I2S_NSOF_POS_NEG_EDGE			0
#define I2S_NSOF_POS_ONLY_EDGE			1

/* defines for Interrupt request sources for I2STX  */
#define  I2STX_FIFO_RIGHT_UNDERRUN		_BIT(0)
#define  I2STX_FIFO_LEFT_UNDERRUN		_BIT(1)
#define  I2STX_FIFO_RIGHT_OVERRUN		_BIT(2)
#define  I2STX_FIFO_LEFT_OVERRUN		_BIT(3)
#define  I2STX_FIFO_LEFT_FULL			_BIT(4)
#define  I2STX_FIFO_LEFT_HALF_EMPTY		_BIT(5)
#define  I2STX_FIFO_LEFT_EMPTY			_BIT(6)
#define  I2STX_FIFO_RIGHT_FULL			_BIT(7)
#define  I2STX_FIFO_RIGHT_HALF_EMPTY	_BIT(8)
#define  I2STX_FIFO_RIGHT_EMPTY			_BIT(9)


/* defines for Interrupt request sources for I2SRX  */
#define  I2SRX_FIFO_RIGHT_UNDERRUN		_BIT(0)
#define  I2SRX_FIFO_LEFT_UNDERRUN		_BIT(1)
#define  I2SRX_FIFO_RIGHT_OVERRUN		_BIT(2)
#define  I2SRX_FIFO_LEFT_OVERRUN		_BIT(3)
#define  I2SRX_FIFO_LEFT_FULL			_BIT(4)
#define  I2SRX_FIFO_LEFT_HALF_FULL		_BIT(5)
#define  I2SRX_FIFO_LEFT_NOT_EMPTY		_BIT(6)
#define  I2SRX_FIFO_RIGHT_FULL			_BIT(7)
#define  I2SRX_FIFO_RIGHT_HALF_FULL		_BIT(8)
#define  I2SRX_FIFO_RIGHT_EMPTY			_BIT(9)


/* I2S device configuration structure type */
typedef struct
{
  BOOL_32 init;
  INT_32  i2snum;
  INTERRUPT_SOURCE_T source;
  I2S_REGS_T *regptr;
} I2S_CFG_T;


/***********************************************************************
 * I2S device configuration commands (IOCTL commands and arguments)
 **********************************************************************/


/* I2S device commands (IOCTL commands) */
typedef enum
{
  /*  used for I2S Format setup; use I2S_FORMATS defines  */
  I2S_FORMAT_SETTINGS,
  /*  used for I2S_RX Master - Slave modes; use I2S RX Master Slaves defines  */
  I2S_RX_MASTER_SLAVE_MODE,
  /*  used for I2S NSOF Counter edge settings; use NSOF Edge deifnes  */
  I2S_NSOF_EDGE_DETECTION,
  /*  used for mask all interrupts; no argument is used  */
  I2S_INT_MASK_ALL,
  /* used for mask interrupts; use defines for interrupts ORed as an argument  */
  I2S_INT_MASK,
  /*  used for unmask all interrupts; no argument is used  */
  I2S_INT_UNMASK_ALL,
  /* used for unmask interrupts; use defines for interrupts ORed as an argument  */
  I2S_INT_UNMASK,
  /* used for get I2S status; use arg as value of type I2S_IOCTL_CMD_T  */
  I2S_GET_STATUS,

} I2S_IOCTL_CMD_T;


/* I2S device arguments for I2S_GET_STATUS command (IOCTL arguments) */
typedef enum
{
  /*  returns the INT_STATUS register  */
  I2S_GET_INT_STATUS,
  /*  returns the NSOF counter value  */
  I2S_GET_NSOF_COUNTER,

} I2S_IOCTL_STS_T;


/***********************************************************************
 * I2S driver API functions
 **********************************************************************/
/* Open the I2S module */
INT_32 i2s_open(I2S_REGS_T *ipbase,
                PFV handler_ptr);

/* Close the I2S module */
STATUS i2s_close(INT_32 devid);

/* I2S configuration block */
STATUS i2s_ioctl(INT_32 devid,
                 INT_32 cmd,
                 INT_32 arg);

#ifdef __cplusplus
}
#endif

#endif /* LPC313X_I2S_DRIVER_H */
