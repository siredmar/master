/* serial-sc16c652.c

   written by Marc Singer
   16 December 2006

   Copyright (C) 2006 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   Serial driver for the external DUART on the MX31ADS development
   board.

*/

#include <driver.h>
#include <service.h>
#include <apex.h>		/* printf, only */
#include "hardware.h"

#include "sc16c652.h"

//#include <debug_ll.h>

extern struct driver_d* console_driver;

static struct driver_d sc16c652_serial_driver;

void sc16c652_serial_init (void)
{
  u32 baudrate = 115200;
  u32 divisor = SC_UART_CLK/(baudrate*16);

  u8 lcr = SC_UART_LCR_WLEN_8;

  SC_UART_LCR = lcr | SC_UART_LCR_DLE;
  SC_UART_DLL = divisor & 0xff;
  SC_UART_DLM = ((divisor >> 8) & 0xff);
  SC_UART_LCR = lcr;

  SC_UART_IER = 0;			/* Mask all interrupts */
  SC_UART_FCR = SC_UART_FCR_FEN | SC_UART_FCR_RX_RESET | SC_UART_FCR_TX_RESET;
  SC_UART_LSR = 0;

  if (console_driver == 0)
    console_driver = &sc16c652_serial_driver;

//  sc16c652_serial_driver.write (NULL, "serial console initialized\r\n", 28);
//  printf ("Really, it is\n");
}

void sc16c652_serial_release (void)
{
	/* flush serial output */
  while (!(SC_UART_LSR & SC_UART_LSR_TXIDLE))
    ;
}

ssize_t sc16c652_serial_poll (struct descriptor_d* d, size_t cb)
{
  return cb
    ? ((SC_UART_LSR & SC_UART_LSR_RDR) ? 1 : 0)
    : 0;
}

ssize_t sc16c652_serial_read (struct descriptor_d* d, void* pv, size_t cb)
{
  ssize_t cRead = 0;
  unsigned char* pb;
  for (pb = (unsigned char*) pv; cb--; ++pb) {
    u8 v;
    u8 lsr;
    while (!(SC_UART_LSR & SC_UART_LSR_RDR))
      ;				/* block until character is available */

    lsr = SC_UART_LSR;
    v = SC_UART_HR;
    if (lsr & (SC_UART_LSR_OVR | SC_UART_LSR_PE | SC_UART_LSR_FE))
      return -1;		/* -ESERIAL */
    *pb = v;
    ++cRead;
  }

  return cRead;
}

ssize_t sc16c652_serial_write (struct descriptor_d* d,
			      const void* pv, size_t cb)
{
  ssize_t cWrote = 0;
  const unsigned char* pb = pv;
  for (pb = (unsigned char*) pv; cb--; ++pb) {

    while (!(SC_UART_LSR & SC_UART_LSR_TXE))
      ;

    SC_UART_HR = *pb;

    ++cWrote;
  }

#if 1
	/* flush serial output */
  while (!(SC_UART_LSR & SC_UART_LSR_TXIDLE))
    ;
#endif

  return cWrote;
}

static __driver_0 struct driver_d sc16c652_serial_driver = {
  .name = "serial-sc16c652",
  .description = "sc16c652 serial driver",
  .flags = DRIVER_SERIAL | DRIVER_CONSOLE,
  .read = sc16c652_serial_read,
  .write = sc16c652_serial_write,
  .poll = sc16c652_serial_poll,
};

static __service_3 struct service_d sc16c652_serial_service = {
  .init = sc16c652_serial_init,
  .release = sc16c652_serial_release,
};
