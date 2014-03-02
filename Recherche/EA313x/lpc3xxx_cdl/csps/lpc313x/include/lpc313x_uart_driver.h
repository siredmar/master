/***********************************************************************
 * $Id:: lpc313x_uart_driver.h 1180 2008-09-11 19:32:40Z pdurgesh      $
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

#ifndef LPC313X_UART_DRIVER_H
#define LPC313X_UART_DRIVER_H

#include "lpc313x_uart.h"
#include "lpc_params.h"

#ifdef __cplusplus
extern "C"
{
#endif

/***********************************************************************
 * UART device configuration commands (IOCTL commands and arguments)
 **********************************************************************/

/* Structure containing callback functions for data FIFO management
   and error management */
typedef struct
{
  /* Pointer to RX FIFO callback, called when their is data to
     read in the RX FIFO */
  PFV rxcb;
  /* Pointer to TX FIFO callback, called when transmit data is
     needed in the FIFO */
  PFV txcb;
  /* Pointer to error/break handler callback, called when their is
     a frame, parity, RX FIFO overrun error, or a break */
  PFV rxerrcb;
} UART_CBS_T;

/* UART device commands (IOCTL commands) */
typedef enum
{
  /* Clear RX and/or TX UART FIFOs, use arg as a OR'ed combination
     of UART_FCR_TXFIFO_FLUSH and/or UART_FCR_RXFIFO_FLUSH to
     select which FIFOs to clear */
  UART_CLEAR_FIFOS,
  /* Setup general UART parameters, use arg as a pointer to a
     structure of type UART_CONTROL_T */
  UART_SETUP_TRANSFER,
  /* Enable or disable loopback mode for the UART, use arg as '0'
     to disable, or any other value to enable */
  UART_LOOPBACK_EN,
  /* Force a break condition on the TX line, use arg as '0' to clear
     the break, or any other value to set a break */
  UART_FORCE_BREAK,
  /* Install UART data handler callback functions, use arg as a
     pointer to type UART_CBS_T */
  UART_INSTALL_CBS,
  /* Get a UART status, use arg as value of type UART_IOCTL_STS_T */
  UART_GET_STATUS
} UART_IOCTL_CMD_T;

/* UART device arguments for UART_GET_STATUS command (IOCTL
   arguments) */
typedef enum
{
  /* Returns the clock rate driving the standard UARTS */
  UART_GET_IP_CLOCK,
  /* Returns the actual clock rate the UARTs will transfer data at,
     after division */
  UART_GET_DERIVED_CLOCK,
  /* Returns the current line status register value, this is useful
     for popping the FIFO one byte at a time and getting the status
     for each byte, or using polling operation */
  UART_GET_LINE_STATUS
} UART_IOCTL_STS_T;

/***********************************************************************
 * UART driver API functions
 **********************************************************************/

/* Open the UART */
INT_32 uart_open(void *ipbase,
                 INT_32 arg);

/* Close the UART */
STATUS uart_close(INT_32 devid);

/* UART configuration block */
STATUS uart_ioctl(INT_32 devid,
                  INT_32 cmd,
                  INT_32 arg);

/* UART read function */
INT_32 uart_read(INT_32 devid,
                 void *buffer,
                 INT_32 max_bytes);

/* UART write function */
INT_32 uart_write(INT_32 devid,
                  void *buffer,
                  INT_32 n_bytes);

#ifdef __cplusplus
}
#endif

#endif /* LPC313X_UART_DRIVER_H */
