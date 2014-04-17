/* serial.c

   written by Marc Singer
   1 Nov 2004

   with modifications by David Anders
   06 Nov 2005

   Copyright (C) 2004 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

    basic serial driver for the s3c2410

*/

#include <config.h>
#include <driver.h>
#include <service.h>
#include "hardware.h"
#include <debug_ll.h>

#define S3C2410_UART_BASE		0x50000000
#define S3C2410_UART0_BASE		(S3C2410_UART_BASE + 0x00 )
#define UART_BASE			__REG(S3C2410_UART0_BASE + 0x00)
#define UART0_UFCON			__REG(S3C2410_UART0_BASE + 0x08)
#define UART0_UMCON			__REG(S3C2410_UART0_BASE + 0x0C)
#define UART0_ULCON			__REG(S3C2410_UART0_BASE + 0x00)
#define UART0_UCON			__REG(S3C2410_UART0_BASE + 0x04)
#define UART0_UBRDIV			__REG(S3C2410_UART0_BASE + 0x28)
#define UART0_OUTRSTAT			__REG(S3C2410_UART0_BASE + 0x10)
#define UART0_UTXHL			__REG(S3C2410_UART0_BASE + 0x20)
#define UART0_URXHL			__REG(S3C2410_UART0_BASE + 0x24)

#define UART0_OUTRSTAT_TXEMPTY	(1<<2)
#define UART0_OUTRSTAT_RXREADY	(1<<0)


extern struct driver_d *console_driver;

static struct driver_d s3c2410_serial_driver;

void s3c2410_serial_init(void)
{
	int nop_count;
#if defined (CONFIG_DEBUG_LL)
	PUTC_LL('U');
#endif
	GPIO_CTRL_BASE_H_CON = 0x0016faaa;
	GPIO_CTRL_BASE_H_UP = 0x000007ff;
	UART0_CTRL_UFCON = 0x00;
	UART0_CTRL_UMCON = 0x00;
	UART0_CTRL_ULCON = 0x03;
	UART0_CTRL_UCON = 0x245;
	UART0_CTRL_UBRDIV = UART_BRD;

	nop_count = 0;
	while (nop_count < 100)
		nop_count++;

	if (console_driver == 0)
		console_driver = &s3c2410_serial_driver;
}

ssize_t s3c2410_serial_poll(struct descriptor_d *d, size_t cb)
{
	return cb ? ((UART0_OUTRSTAT & UART0_OUTRSTAT_RXREADY) ? 1 : 0) :
	    0;
}

ssize_t s3c2410_serial_read(struct descriptor_d * d, void *pv, size_t cb)
{
	ssize_t cRead = 0;
	unsigned char *pb;
	for (pb = (unsigned char *) pv; cb--; ++pb) {
		u32 v;

				/* block until character is available */
		while ((UART0_OUTRSTAT & UART0_OUTRSTAT_RXREADY) == 0);

		v = UART0_URXHL;
		*pb = v;
		++cRead;
	}

	return cRead;
}

ssize_t s3c2410_serial_write(struct descriptor_d * d,
			     const void *pv, size_t cb)
{
	ssize_t cWrote = 0;
	const unsigned char *pb = pv;
	for (pb = (unsigned char *) pv; cb--; ++pb) {

		while ((UART0_OUTRSTAT & UART0_OUTRSTAT_TXEMPTY) == 0);

		UART0_UTXHL = *pb;

		++cWrote;
	}
	return cWrote;
}

static __driver_0 struct driver_d s3c2410_serial_driver = {
	.name = "serial-s3c2410",
	.description = "s3c2410 serial driver",
	.flags = DRIVER_SERIAL | DRIVER_CONSOLE,
	.read = s3c2410_serial_read,
	.write = s3c2410_serial_write,
	.poll = s3c2410_serial_poll,
};

static __service_3 struct service_d s3c2410_serial_service = {
	.init = s3c2410_serial_init,
};
