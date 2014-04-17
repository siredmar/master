/* karma-cmd-accel.c

   written by Marc Singer
   29 Aug 2008

   Copyright (C) 2008 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#include <config.h>
#include <apex.h>
#include <asm/bootstrap.h>
#include <command.h>
#include "hardware.h"

#include <debug_ll.h>

//#define TALK 1

#if defined (TALK)
#define PRINTF(f...)		printf (f)
#define ENTRY(l)		printf ("%s\n", __FUNCTION__)
#define DBG(l,f...)		do { if (TALK >= l) printf (f); } while (0)
#else
#define PRINTF(f...)		do {} while (0)
#define ENTRY(l)		do {} while (0)
#define DBG(l,f...)		do {} while (0)
#endif

#define PHYS_CSPI	PHYS_CSPI2

#define CSPI_RX		__REG(PHYS_CSPI + 0x00)
#define CSPI_TX		__REG(PHYS_CSPI + 0x04)
#define CSPI_CON	__REG(PHYS_CSPI + 0x08)
#define CSPI_INT	__REG(PHYS_CSPI + 0x0c)
#define CSPI_DMA	__REG(PHYS_CSPI + 0x10)
#define CSPI_STAT	__REG(PHYS_CSPI + 0x14)
#define CSPI_PERIOD	__REG(PHYS_CSPI + 0x18)

#define ABS(i) ((i) < 0 ? -(i) : (i))

static void spi_read_flush (void)
{
  ENTRY (0);

  while (CSPI_STAT & (1<<3)){
    int value = CSPI_RX;
    printf ("purging 0x%x\n", value);
  }
}

static int spi_exchange (int read, int address, int data)
{
  int value = 0
    | ((read ? : 0)     << 15)
    | ((address & 0x7f) <<  8)
    | ((data    & 0xff) <<  0);
  ENTRY (0);

  CSPI_TX = value;

  CSPI_STAT |= 1<<8;            /* Clear completion flag */

  spi_read_flush ();

  CSPI_CON |= (1<<2);           /* Initiate transfer */

  while (!(CSPI_STAT & (1<<8)))
    ;

  return CSPI_RX & 0xff;
}

void accel_setup (void)
{
  ENTRY (0);

  IOMUX_PIN_CONFIG_FUNC (MX31_PIN_CSPI2_SS0);
  IOMUX_PIN_CONFIG_FUNC (MX31_PIN_CSPI2_SS1);
  IOMUX_PIN_CONFIG_FUNC (MX31_PIN_CSPI2_SCLK);
  IOMUX_PIN_CONFIG_FUNC (MX31_PIN_CSPI2_MOSI);
  IOMUX_PIN_CONFIG_FUNC (MX31_PIN_CSPI2_MISO);

  CSPI_CON = 0
    | (0<24)                    /* CHIP SELECT, CS0 */
    | (7<<16)                   /* DATA RATE, divide by 512 (128KHz) */
//    | (3<<16)                   /* DATA RATE, divide by 32 (2MHz) */
//    | (4<<16)                   /* DATA RATE, divide by 64 (1MHz) */
    | ((16-1)<<8)               /* BIT COUNT, 16 bits per transfer */
    | (0<<7)                    /* SSPOL, active low */
//    | (1<<6)                    /* SSCTL, SS frames each burst */
    | (0<<6)                    /* SSCTL, SS always low */
    | (1<<5)                    /* PHA, phase */
    | (1<<4)                    /* POL, SPI clock polarity (default state) */
    | (0<<3)                    /* SMC, start transfer on XCH */
    | (1<<1)                    /* MODE, master */
    | (1<<0)                    /* EN, enable CSPI */
    ;

//  spi_exchange (0, 0xa, 1<<1);     /* Soft reset */
//  usleep (100);                  /* Wait for reset */
  spi_exchange (0, 0xa, 4);     /* Self-test 1 */
}

#define ACCELERATION(l,m) ((int)(( ((int)((signed char)m))<<2)\
                                 | (((l) >> 6) & 0x3)))
#define ACC_INT(v,g)  ((ABS(v))*(g)/512)
#define ACC_FRAC(v,g) (((1000*(ABS(v))*(g) + 512/2)/512)%1000)

#define RAW(l,m) (((unsigned) ((m)<<2)) | (((unsigned) ((l)>>6)) & 0x3))

void accel_query (void)
{
  char rgb[0x3e];
  int i;
  int acc_x;
  int acc_y;
  int acc_z;
  static const int g_range = 4;
  //  static const int scalar = g_range*1000/512;

  ENTRY (0);

  for (i = 0; i < sizeof (rgb); ++i)
    rgb[i] = spi_exchange (1, i, 0);

  dumpw (rgb, sizeof (rgb), 0, 0);

  acc_x = ACCELERATION (rgb[2], rgb[3]);
  acc_y = ACCELERATION (rgb[4], rgb[5]);
  acc_z = ACCELERATION (rgb[6], rgb[7]);

  printf ("x%c %c%d.%03dg (%5d 0x%03x)  "
          "y%c %c%d.%03dg (%5d 0x%03x)  "
          "z%c %c%d.%03dg (%5d 0x%03x)\n",
          (rgb[2] & 1) ? '*' : ' ',
          acc_x < 0 ? '-' : ' ',
          ACC_INT (acc_x, g_range), ACC_FRAC (acc_x, g_range), acc_x,
          RAW (rgb[2], rgb[3]),
          (rgb[4] & 1) ? '*' : ' ',
          acc_y < 0 ? '-' : ' ',
          ACC_INT (acc_y, g_range), ACC_FRAC (acc_y, g_range), acc_y,
          RAW (rgb[4], rgb[5]),
          (rgb[6] & 1) ? '*' : ' ',
          acc_z < 0 ? '-' : ' ',
          ACC_INT (acc_z, g_range), ACC_FRAC (acc_z, g_range), acc_z,
          RAW (rgb[6], rgb[7])
          );
  printf ("temp %d C\n", (rgb[8]/2) - 30);
}

static int cmd_accel (int argc, const char** argv)
{
  accel_setup ();
  accel_query ();

  return 0;
}

static __command struct command_d c_clcdc = {
  .command = "accel",
  .func = (command_func_t) cmd_accel,
  COMMAND_DESCRIPTION ("query accelerometers")
  COMMAND_HELP(
"accel\n"
"  Query Boscm BM150 accelerometer.\n"
  )
};
