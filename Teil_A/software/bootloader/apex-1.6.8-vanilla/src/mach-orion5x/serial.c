/* serial.c

   written by Marc Singer
   14 Jan 2005

   Copyright (C) 2005 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   Serial driver for the orion5x UARTs.  These appear to be variants
   of the pl010 PrimeCell.  The code herein is cribbed from the APEX
   implementation of the ixp42x UART driver.

   o Need to be careful with the includes.  Don't include <debug_ll.h>
     before the uart register macros as it will interfere with them.
     Yes, I could also put them in a shared header, I won't do that
     for the sake of a debug feature.

*/

#include <config.h>
#include <driver.h>
#include <service.h>
#include "uart.h"

extern struct driver_d* console_driver;

static struct driver_d orion5x_serial_driver;

void orion5x_serial_init (void)
{
  static const uint32_t baudrate = 115200;
  uint32_t divisor;

  divisor = (get_tclk ()/baudrate)>>4;

  UART_LCR = UART_LCR_WLS_8 | UART_LCR_STB_1 | UART_LCR_DLAB;
  UART_DLL =  divisor       & 0xff;
  UART_DLH = (divisor >> 8) & 0xff;
  UART_LCR = UART_LCR_WLS_8 | UART_LCR_STB_1;
  UART_FCR = UART_FCR_FIFOEN;

  UART_MCR = UART_MCR_RTS;	/* Assert RTS in case someone uses it */

  if (console_driver == 0)
    console_driver = &orion5x_serial_driver;

//  console_driver->write (0, "Console initialized\r\n", 21);
}

ssize_t orion5x_serial_poll (struct descriptor_d* d, size_t cb)
{
  return cb ? ((UART_LSR & UART_LSR_RXD) ? 1 : 0) : 0;
}

ssize_t orion5x_serial_read (struct descriptor_d* d, void* pv, size_t cb)
{
  ssize_t cRead = 0;
  unsigned char* pb;
  for (pb = (unsigned char*) pv; cb--; ++pb) {
    u32 v;

    while (!(UART_LSR & UART_LSR_RXD))
      ;				/* block until character is available */

    v = UART_DR;
    if (UART_LSR & (UART_LSR_OE | UART_LSR_PE | UART_LSR_FE))
      return -1;		/* -ESERIAL */
    *pb = v;
    ++cRead;
  }

  return cRead;
}

ssize_t orion5x_serial_write (struct descriptor_d* d, const void* pv,
                              size_t cb)
{
  ssize_t cWrote = 0;
  const unsigned char* pb = pv;
  for (pb = (unsigned char*) pv; cb--; ++pb) {

    while (!(UART_LSR & UART_LSR_TXD))
      ;

    UART_DR = *pb;

    ++cWrote;
  }

	/* Wait for completion */
  while (!(UART_LSR & UART_LSR_TEMT))
    ;

  return cWrote;
}

static __driver_0 struct driver_d orion5x_serial_driver = {
  .name  = "serial-orion5x",
  .description = "orion5x serial driver",
  .flags = DRIVER_SERIAL | DRIVER_CONSOLE,
  .read  = orion5x_serial_read,
  .write = orion5x_serial_write,
  .poll  = orion5x_serial_poll,
};

static __service_3 struct service_d orion5x_serial_service = {
  .init = orion5x_serial_init,
};
