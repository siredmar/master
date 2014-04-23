/* serial.c

   written by Marc Singer
   1 Nov 2004

   Copyright (C) 2004 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   This serial driver is nearly identitical for the lh79520 and the
   lh79524.  Since the only real difference is in the setup, the code
   is shared.

   *** FIXME: the device selection should be part of the platform
   *** header and not the CPU header.  The choice of the console
   *** device is made with the board layout.

*/

#include <config.h>
#include <driver.h>
#include <service.h>
#include "hardware.h"

#define UART_DR			__REG(UART + 0x00)
#define UART_IBRD		__REG(UART + 0x24)
#define UART_FBRD		__REG(UART + 0x28)
#define UART_LCR_H		__REG(UART + 0x2c)
#define UART_CR			__REG(UART + 0x30)
#define UART_FR			__REG(UART + 0x18)
#define UART_IMSC		__REG(UART + 0x38)
#define UART_ICR		__REG(UART + 0x44)

#define UART_FR_TXFE		(1<<7)
#define UART_FR_RXFF		(1<<6)
#define UART_FR_TXFF		(1<<5)
#define UART_FR_RXFE		(1<<4)
#define UART_FR_BUSY		(1<<3)
#define UART_DR_PE		(1<<9)
#define UART_DR_OE		(1<<11)
#define UART_DR_FE		(1<<8)
#define UART_CR_EN		(1<<0)
#define UART_CR_TXE		(1<<8)
#define UART_CR_RXE		(1<<9)
#define UART_LCR_WLEN8		(3<<5)
#define UART_LCR_FEN		(1<<4)
#define UART_DR_DATAMASK	(0xff)

extern struct driver_d* console_driver;

static struct driver_d lh7952x_serial_driver;

void lh7952x_serial_init (void)
{
  u32 baudrate = 115200;
  u32 divisor_i = 0;
  u32 divisor_f = 0;

  /* *** FIXME: the only reason there are two of these is because the
     *** name of the constants changed.  Also, the UARTMUX stuff.
     *** Clean it up. */

#if defined (CONFIG_ARCH_LH79520)

  /* Enable ALL uarts since we don't know which we're using */
  IOCON_UARTMUX = 0xf;
  RCPC_CTRL |= RCPC_CTRL_UNLOCK;
  RCPC_PERIPHCLKCTRL &=
    ~(RCPC_PERIPHCLK_U0 | RCPC_PERIPHCLK_U1 | RCPC_PERIPHCLK_U2);
  RCPC_CTRL &= ~RCPC_CTRL_UNLOCK;

  switch (baudrate) {
  case 115200:
    divisor_i = 8; divisor_f = 0; break;

  default:
    return;
  }

#endif

#if defined (CONFIG_ARCH_LH79524) || defined (CONFIG_ARCH_LH79525)

  /* Enable ALL uarts since we don't know which we're using */
  RCPC_CTRL |= RCPC_CTRL_UNLOCK;
  RCPC_PCLKCTRL0 &=
    ~(RCPC_PCLKCTRL0_U0 | RCPC_PCLKCTRL0_U1 | RCPC_PCLKCTRL0_U2);
  RCPC_CTRL &= ~RCPC_CTRL_UNLOCK;

  switch (baudrate) {
  case 115200:
    divisor_i = 6; divisor_f = 8; break;

  default:
    return;
  }
#endif


//  while (UART_FR & UART_FR_BUSY)
//    ;

  UART_CR = UART_CR_EN; /* Enable UART without drivers */

  UART_IBRD = divisor_i;
  UART_FBRD = divisor_f;

  UART_LCR_H = UART_LCR_FEN | UART_LCR_WLEN8;

  UART_IMSC = 0x00; /* Mask interrupts */
  UART_ICR  = 0xff; /* Clear interrupts */

  UART_CR = UART_CR_EN | UART_CR_TXE | UART_CR_RXE;

  if (console_driver == 0)
    console_driver = &lh7952x_serial_driver;
}

void lh7952x_serial_release (void)
{
	/* flush serial output */
  while (!(UART_FR & UART_FR_TXFE))
    ;
}

ssize_t lh7952x_serial_poll (struct descriptor_d* d, size_t cb)
{
  return cb ? ((UART_FR & UART_FR_RXFE) ? 0 : 1) : 0;
}

ssize_t lh7952x_serial_read (struct descriptor_d* d, void* pv, size_t cb)
{
  ssize_t cRead = 0;
  unsigned char* pb;
  for (pb = (unsigned char*) pv; cb--; ++pb) {
    u32 v;

    while (UART_FR & UART_FR_RXFE)
      ;				/* block until character is available */

    v = UART_DR;
    if (v & (UART_DR_FE | UART_DR_PE))
      return -1;		/* -ESERIAL */
    *pb = v & UART_DR_DATAMASK;
    ++cRead;
  }

  return cRead;
}

ssize_t lh7952x_serial_write (struct descriptor_d* d,
			      const void* pv, size_t cb)
{
  ssize_t cWrote = 0;
  const unsigned char* pb = pv;

  while (!(UART_FR & UART_FR_TXFE))
    ;

  for (pb = (unsigned char*) pv; cb--; ++pb) {

    while (UART_FR & UART_FR_TXFF)
      ;

    UART_DR = *pb;

    ++cWrote;
  }

	/* Wait for completion */
  /* *** This should not be necessary.  However, without it, the
     *** driver sometimes fails to send all of the output.  There is a
	 secondary benefit to this check in that it guarantees that
	 output is visible before execution continues. */
  while (!(UART_FR & UART_FR_TXFE))
    ;

  return cWrote;
}

static __driver_0 struct driver_d lh7952x_serial_driver = {
  .name = "serial-lh7952x",
  .description = "lh7952x serial driver",
  .flags = DRIVER_SERIAL | DRIVER_CONSOLE,
  .read = lh7952x_serial_read,
  .write = lh7952x_serial_write,
  .poll = lh7952x_serial_poll,
};

static __service_3 struct service_d lh7952x_serial_service = {
  .init = lh7952x_serial_init,
  .release = lh7952x_serial_release,
};
