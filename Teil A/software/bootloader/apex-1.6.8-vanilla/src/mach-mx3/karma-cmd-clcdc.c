/* karma-cmd-clcdc.c

   written by Marc Singer
   21 August 2007

   Copyright (C) 2006 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   Initialization of the NOKIA wide (640x200) LCD panel.  It appears
   to have the same (or compatible) LCD controller as is in OSD panel
   used on Companion...as below.

   o Vdd valid
   o < 1ms -> Vddi valid
   o > 100ns -> XRES goes high
   o > 5ms -> RGB valid
   o > 20ms -> SPI initialization

   The Toshiba controller chip describes other paramters.

   o Reset release time is 1ms.

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

#define PHYS_CSPI	PHYS_CSPI1

#define CSPI_RX		__REG(PHYS_CSPI + 0x00)
#define CSPI_TX		__REG(PHYS_CSPI + 0x04)
#define CSPI_CON	__REG(PHYS_CSPI + 0x08)
#define CSPI_INT	__REG(PHYS_CSPI + 0x0c)
#define CSPI_DMA	__REG(PHYS_CSPI + 0x10)
#define CSPI_STAT	__REG(PHYS_CSPI + 0x14)
#define CSPI_PERIOD	__REG(PHYS_CSPI + 0x18)

#define PIN_NLCD_RESET	MX31_PIN_ATA_DIOW

static void write_fifo_9 (const char* rgb, int cb)
{
  int cmddata = 0;		/* Cmd/Data bit */

  PRINTF ("%s: 0x%x (%d)\n", __FUNCTION__, *rgb, cb);

  while (cb--) {
//    while (!(SSP_SR & SSP_SR_TNF))	/* Wait for room in FIFO */
//      ;
    PRINTF ("(0x%x) ", *rgb | (cmddata << 8));
    CSPI_TX = *rgb++ | (cmddata << 8);
    cmddata = 1;
  }
}

static void spi_read_flush (void)
{
  ENTRY (0);

  while (CSPI_STAT & (1<<3))
    CSPI_RX;
}

static void spi_write (const char* rgb, int cb)
{
  ENTRY (0);

  write_fifo_9 (rgb, cb);

  CSPI_STAT |= 1<<8;            /* Clear completion flag */

  CSPI_CON |= (1<<2);           /* Initiate transfer */

  while (!(CSPI_STAT & (1<<8)))
    ;

  spi_read_flush ();
}

void karma_clcdc_setup (void)
{
  ENTRY (0);

  IOMUX_PIN_CONFIG_FUNC (MX31_PIN_CSPI1_SS0);
  IOMUX_PIN_CONFIG_FUNC (MX31_PIN_CSPI1_SCLK);
  IOMUX_PIN_CONFIG_FUNC (MX31_PIN_CSPI1_MOSI);

#if 0
  MX31_PIN_CSPI1_SS2	  = __N (31, 3,  43, 0),
  MX31_PIN_CSPI1_SCLK	  = __N (31, 2,  42, 2),
  MX31_PIN_CSPI1_SPI_RDY  = __N (31, 1,  42, 1),
  MX31_PIN_CSPI2_MOSI	  = __N (31, 0,  42, 0),
  MX31_PIN_CSPI1_MOSI	  = __N (32, 3,  44, 1),
  MX31_PIN_CSPI1_MISO	  = __N (32, 2,  44, 0),
  MX31_PIN_CSPI1_SS0	  = __N (32, 1,  43, 2),
  MX31_PIN_CSPI1_SS1	  = __N (32, 0,  43, 1),
#endif

  /* The panel must receive a reset pulse after power comes on. */
  IOMUX_PIN_CONFIG_GPIO	 (PIN_NLCD_RESET);
  GPIO_PIN_CONFIG_OUTPUT (PIN_NLCD_RESET);
  GPIO_PIN_SET		 (PIN_NLCD_RESET);
  mdelay (2);
  GPIO_PIN_CLEAR	 (PIN_NLCD_RESET);
  mdelay (2);
  GPIO_PIN_SET		 (PIN_NLCD_RESET);

  CSPI_CON = 0
    | (0<24)                    /* CHIP SELECT, CS0 */
//    | (7<<16)                   /* DATA RATE, divide by 512 (128KHz) */
//    | (3<<16)                   /* DATA RATE, divide by 32 (2MHz) */
    | (4<<16)                   /* DATA RATE, divide by 64 (1MHz) */
    | ((9-1)<<8)                /* BIT COUNT, 9 bits per transfer */
    | (0<<7)                    /* SSPOL, active low */
//    | (1<<6)                    /* SSCTL, SS frames each burst */
    | (0<<6)                    /* SSCTL, SS always low */
    | (0<<5)                    /* PHA, phase 0 operation */
    | (0<<4)                    /* POL, SPI clock polarity (default state) */
    | (0<<3)                    /* SMC, start transfer on XCH */
    | (1<<1)                    /* MODE, master */
    | (1<<0)                    /* EN, enable CSPI */
    ;

	/* Perform main setup of the LCD panel controller */
//  mdelay (1);		/* Paranoid */
#if 0
  spi_write ("\xb0\x02",     2);  /* Blanking period: Use DE */
  spi_write ("\xb4\x01",     2);  /* Display mode */
  spi_write ("\x36\x08",     2);  /* Memory access control: BGR mode */
  spi_write ("\xb7\x03",     2);  /* DCCK & DCEV timing setup */
  spi_write ("\xbe\x38",     2);  /* ASW signal slew rate adjustment */
  spi_write ("\xc0\x08\x08", 3);  /* CKV1,2 timing control */
  spi_write ("\xc2\x18\x18", 3);  /* OEV timing control */
  spi_write ("\xc4\x30\x30", 3);  /* ASW timing control */
  spi_write ("\xc5\x0c",     2);  /* ASW timing control */
  spi_write ("\xed\x04",     2);  /* Valid display lines: 256 */
  spi_write ("\x26\x04",     2);  /* Gamma set */
  spi_write ("\xba\x45",     2);  /* Booster operation setup */
  spi_write ("\xd6\x77\x35", 3);  /* gamma 3 (2) fine tuning */
  spi_write ("\xd7\x01",     2);  /* Gamma 3 (1) fine tuning */
  spi_write ("\xd8\x00",     2);  /* Gamma 3 inclination adjustment */
  spi_write ("\xd9\x00",     2);  /* Gamma 3 blue offset adjustment */
#endif
}

void karma_clcdc_wake (void)
{
  ENTRY (0);

  karma_clcdc_setup ();

  spi_write ("\x11", 1);
  /* 6 frames (2.7ms), one frame ~450us */
//  mdelay (200);			/* was 10 */
  spi_write ("\x26\x04", 2);
  spi_write ("\x29", 1);
  spi_write ("\x36\x00", 2);
}

void karma_clcdc_sleep (void)
{
  ENTRY (0);

  spi_write ("\x28", 1);
  spi_write ("\x10", 1);
//  mdelay (1);
}

static int cmd_clcdc (int argc, const char** argv)
{
  karma_clcdc_wake ();

  return 0;
}

static __command struct command_d c_clcdc = {
  .command = "clcdc",
  .func = (command_func_t) cmd_clcdc,
  COMMAND_DESCRIPTION ("enable Nokia panel")
  COMMAND_HELP(
"clcdc\n"
"  Enable the Nokia panel.\n"
  )
};
