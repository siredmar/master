/* drv-cpld-spi.c

   written by Marc Singer
   17 Nov 2004

   Copyright (C) 2004 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   SPI driver for CPLD connected devices.  The LPD79524 has only a
   pair of AT93C46 EEPROMS and the audio codec.  The LPD79520 has one
   EEPROM, a touch screen controller, and the audio codec.

*/

#include <config.h>
#include <apex.h>
#include <driver.h>
#include "hardware.h"

#if defined (CONFIG_MACH_LPD79520)
# define DRIVER_ARCH	"lpd79520"
#endif

#if defined (CONFIG_MACH_LPD79524)
#define DRIVER_ARCH	"lpd79524"
#endif

/* *** FIXME: these timing values are substantially larger than the
   *** chip requires. We may implement an nsleep () function. */
#define T_SKH	1		/* Clock time high (us) */
#define T_SKL	1		/* Clock time low (us) */
#define T_CS	1		/* Minimum chip select low time (us)  */
#define T_CSS	1		/* Minimum chip select setup time (us)  */
#define T_DIS	1		/* Data setup time (us) */

	 /* EEPROM SPI bits */
#define P_START		(1<<9)
#define P_WRITE		(1<<7)
#define P_READ		(2<<7)
#define P_ERASE		(3<<7)
#define P_EWDS		(0<<7)
#define P_WRAL		(0<<7)
#define P_ERAL		(0<<7)
#define P_EWEN		(0<<7)
#define P_A_EWDS	(0<<5)
#define P_A_WRAL	(1<<5)
#define P_A_ERAL	(2<<5)
#define P_A_EWEN	(3<<5)

#define CB_EEPROM_MAX	(128)

static void enable_cs (int chip_select)
{
  CPLD_SPI |=  chip_select;
  usleep (T_CSS);
}

static void disable_cs (int chip_select)
{
  CPLD_SPI &= ~chip_select;
  usleep (T_CS);
}

static void pulse_clock (void)
{
  CPLD_SPI |=  CPLD_SPI_SCLK;
  usleep (T_SKH);
  CPLD_SPI &= ~CPLD_SPI_SCLK;
  usleep (T_SKL);
}


/* execute_spi_command

   sends a spi command to a device.  It first sends cwrite bits from
   v.  If cread is greater than zero it will read cread bits
   (discarding the leading 0 bit) and return them.  If cread is less
   than zero it will check for completetion status and return 0 on
   success or -1 on timeout.  If cread is zero it does nothing other
   than sending the command.

*/

static int execute_spi_command (int chip_select,
				int v, int cwrite, int cread)
{
  unsigned long l = 0;

  enable_cs (chip_select);

  v <<= CPLD_SPI_TX_SHIFT;	/* Correction for the position of SPI_TX bit */
  while (cwrite--) {
    CPLD_SPI = (CPLD_SPI & ~CPLD_SPI_TX) | ((v >> cwrite) & CPLD_SPI_TX);
    usleep (T_DIS);
    pulse_clock ();
  }

  if (cread < 0) {
    unsigned long time;
    disable_cs (chip_select);
    time = timer_read ();
    enable_cs (chip_select);

    l = -1;
    do {
      if (CPLD_SPI & CPLD_SPI_RX) {
	l = 0;
	break;
      }
    } while (timer_delta (time, timer_read ()) < 10*1000);
  }
  else
	/* We pulse the clock before the data to skip the leading zero. */
    while (cread-- > 0) {
      pulse_clock ();
      l = (l<<1)
	| (((CPLD_SPI & CPLD_SPI_RX) >> CPLD_SPI_RX_SHIFT) & 0x1);
    }

  disable_cs (chip_select);
  return l;
}

static int spi_open (struct descriptor_d* d)
{
  if (d->start > CB_EEPROM_MAX)
    d->start = CB_EEPROM_MAX;
  if (d->start + d->length > CB_EEPROM_MAX)
    d->length = CB_EEPROM_MAX - d->start;

  return 0;
}

static unsigned long fixup (struct descriptor_d* d, size_t* pcb)
{
  unsigned long index = d->start + d->index;

  if (d->index + *pcb > d->length)
    *pcb = d->length - d->index;
  d->index += *pcb;
  return index;
}

static ssize_t spi_read (struct descriptor_d* d, void* pv, size_t cb)
{
  int chip_select = (d->driver->flags >> DRIVER_PRIVATE_SHIFT)
    &DRIVER_PRIVATE_MASK;
  unsigned long index = fixup (d, &cb);
  int i;

  for (i = 0; i < cb; ++i)
    ((unsigned char*) pv)[i]
      = execute_spi_command (chip_select,
			     P_START|P_READ|(i + index), 10, 8);

  return cb;
}

static ssize_t spi_write (struct descriptor_d* d, const void* pv, size_t cb)
{
  int chip_select = (d->driver->flags >> DRIVER_PRIVATE_SHIFT)
    &DRIVER_PRIVATE_MASK;
  unsigned long index = fixup (d, &cb);
  int i;

  execute_spi_command (chip_select, P_START|P_EWEN|P_A_EWEN, 10, 0);

  for (i = 0; i < cb; ++i)
    execute_spi_command (chip_select,
			 ((P_START|P_WRITE|(i + index))<<8)
			 |((unsigned char*) pv)[i], 18, -1);

  execute_spi_command (chip_select, P_START|P_EWDS|P_A_EWDS, 10, 0);

  return cb;
}

static void spi_erase (struct descriptor_d* d, size_t cb)
{
  int chip_select = (d->driver->flags >> DRIVER_PRIVATE_SHIFT)
    &DRIVER_PRIVATE_MASK;
  unsigned long index = fixup (d, &cb);
  int i;

  execute_spi_command (chip_select, P_START|P_EWEN|P_A_EWEN, 10, 0);

  for (i = 0; i < cb; ++i)
    execute_spi_command (chip_select,
			 P_START|P_ERASE|(i + index), 10, -1);

  execute_spi_command (chip_select, P_START|P_EWDS|P_A_EWDS, 10, 0);
}

static __driver_1 struct driver_d spi_eeprom_driver = {
  .name = "eeprom-" DRIVER_ARCH,
  .description = "configuration EEPROM driver",
  .flags = DRIVER_PRIVATE (CPLD_SPI_CS_EEPROM),
  .open = spi_open,
  .close = close_helper,
  .read = spi_read,
  .write = spi_write,
  .erase = spi_erase,
  .seek = seek_helper,
};

#if defined (CONFIG_MACH_LPD79524)
static __driver_1 struct driver_d spi_mac_driver = {
  .name = "mac-" DRIVER_ARCH,
  .description = "mac EEPROM driver",
  .flags = DRIVER_PRIVATE (CPLD_SPI_CS_MAC),
  .open = spi_open,
  .close = close_helper,
  .read = spi_read,
  .write = spi_write,
  .erase = spi_erase,
  .seek = seek_helper,
};
#endif
