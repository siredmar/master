/* serial.c

   written by Marc Singer
   12 Nov 2004

   Copyright (C) 2004 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#include <driver.h>
#include <service.h>
#include <apex.h>		/* printf, only */
#include "lh7a40x.h"

#define UART_DATA		__REG(UART + 0x00)
#define UART_FCON		__REG(UART + 0x04)
#define UART_BRCON		__REG(UART + 0x08)
#define UART_CON		__REG(UART + 0x0c)
#define UART_STATUS		__REG(UART + 0x10)
#define UART_RAWISR		__REG(UART + 0x14)
#define UART_INTEN		__REG(UART + 0x18)
#define UART_ISR		__REG(UART + 0x1c)

//#include <debug_ll.h>

extern struct driver_d* console_driver;

static struct driver_d lh7a40x_serial_driver;

void lh7a40x_serial_init (void)
{
  u32 baudrate = 115200;
  u32 divisor = 0;

  switch (baudrate) {
  case 115200:
    divisor = 0x03; break;

  default:
    return;
  }

  UART_BRCON = divisor;
  UART_FCON = UART_FCON_FEN | UART_FCON_WLEN8;
  UART_INTEN = 0x00; /* Mask interrupts */
  UART_CON = UART_CON_ENABLE;

  if (console_driver == 0)
    console_driver = &lh7a40x_serial_driver;

//  lh7a40x_serial_driver.write (NULL, "serial console initialized\r\n", 28);
//  printf ("Really, it is\n");
}

void lh7a40x_serial_release (void)
{
	/* flush serial output */
  while (!(UART_STATUS & UART_STATUS_TXFE))
    ;
}

ssize_t lh7a40x_serial_poll (struct descriptor_d* d, size_t cb)
{
  return cb
    ? ((UART_STATUS & UART_STATUS_RXFE) ? 0 : 1)
    : 0;
}

ssize_t lh7a40x_serial_read (struct descriptor_d* d, void* pv, size_t cb)
{
  ssize_t cRead = 0;
  unsigned char* pb;
  for (pb = (unsigned char*) pv; cb--; ++pb) {
    u32 v;

    while (UART_STATUS & UART_STATUS_RXFE)
      ;				/* block until character is available */

    v = UART_DATA;
    if (v & (UART_DATA_FE | UART_DATA_PE))
      return -1;		/* -ESERIAL */
    *pb = v & UART_DATA_DATAMASK;
    ++cRead;
  }

  return cRead;
}

ssize_t lh7a40x_serial_write (struct descriptor_d* d, const void* pv, size_t cb)
{
  ssize_t cWrote = 0;
  const unsigned char* pb = pv;
  for (pb = (unsigned char*) pv; cb--; ++pb) {

    while (UART_STATUS & UART_STATUS_TXFF)
      ;

    UART_DATA = *pb;

    ++cWrote;
  }

#if 1
	/* flush serial output */
  while (!(UART_STATUS & UART_STATUS_TXFE))
    ;
#endif

  return cWrote;
}

static __driver_0 struct driver_d lh7a40x_serial_driver = {
  .name = "serial-lh7a40x",
  .description = "lh7a40x serial driver",
  .flags = DRIVER_SERIAL | DRIVER_CONSOLE,
  .read = lh7a40x_serial_read,
  .write = lh7a40x_serial_write,
  .poll = lh7a40x_serial_poll,
};

static __service_3 struct service_d lh7a40x_serial_service = {
  .init = lh7a40x_serial_init,
  .release = lh7a40x_serial_release,
};
