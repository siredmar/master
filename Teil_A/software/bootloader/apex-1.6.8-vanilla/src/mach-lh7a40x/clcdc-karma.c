/* clcdc-karma.c

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

   Initialization of the NOKIA wide LCD panel.  It appears to have the
   same (or compatible) LCD controller as is in OSD panel used on
   Companion...as below.

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
#include "hardware.h"

#include <debug_ll.h>

//#define TALK

#if defined TALK
# define PRINTF(v...)	printf (v)
#else
# define PRINTF(v...)	do {} while (0)
#endif

#define ENTRY(l) PRINTF ("%s\n", __FUNCTION__)

//#define USE_FAST_CLOCK
#define USE_SPI

#define SSP_CR0		__REG(SSP_PHYS + 0x00)
#define SSP_CR1		__REG(SSP_PHYS + 0x04)
#define SSP_IIR		__REG(SSP_PHYS + 0x08)
#define SSP_ROEOI	__REG(SSP_PHYS + 0x08)
#define SSP_DR		__REG(SSP_PHYS + 0x0c)
#define SSP_CPR		__REG(SSP_PHYS + 0x10)
#define SSP_SR		__REG(SSP_PHYS + 0x14)

#define SSP_CR0_SCR_SHIFT (8)
#define SSP_CR0_SSE	  (1<<7)
#define SSP_CR0_FRF_SHIFT (4)
#define SSP_CR0_FRF_SPI	  (0<<SSP_CR0_FRF_SHIFT)
#define SSP_CR0_DSS_SHIFT (0)
#define SSP_CR0_DSS(b)	  ((b)-1)

#define SSP_SR_RFF	(1<<8)	/* Receive FIFO full */
#define SSP_SR_TFE	(1<<7)	/* Transmit FIFO full */
#define SSP_SR_ROR	(1<<6)	/* Receive overrun */
#define SSP_SR_RHF	(1<<5)	/* Receive FIFO half full */
#define SSP_SR_THE	(1<<4)	/* Transmit FIFO half empty */
#define SSP_SR_BSY	(1<<3)	/* Receive or transmit active */
#define SSP_SR_RNE	(1<<2)	/* Receive FIFO not empty */
#define SSP_SR_TNF	(1<<1)	/* Transmit FIFO not full */


static inline void cs_enable (void)
{
  GPIO_PGD &= ~(1<<5);		/* Enable nLCD_SPI_GPIO_CS */
}

static inline void cs_disable (void)
{
  GPIO_PGD |= (1<<5);		/* Disable nLCD_SPI_GPIO_CS */
}

#if 0
static void write_fifo (const char* rgb, int cb)
{
  int cmddata = 0;		/* Cmd/Data bit */
  int bits = 0;			/* Count of bits in accumulator */
  unsigned long accumulator = 0;

  for (; cb; --cb) {
    accumulator = (accumulator << 1) | cmddata;
    cmddata = 1;		/* Reset of bytes are data */
    ++bits;
//    printf ("%s: 0x%02x\n", __FUNCTION__, *rgb);
    accumulator = (accumulator << 8) | *rgb++;
    bits += 8;

    if (bits >= 16) {
      printf ("%s: 0x%04lx %d\n", __FUNCTION__,
	      (accumulator >> (bits - 16)) & 0xffff, 16);
      SSP_DR = (accumulator >> (bits - 16)) & 0xffff;
      bits -= 16;
    }
  }
  if (bits) {
    printf ("%s: 0x%04lx %d (%x)\n", __FUNCTION__,
	    (accumulator << (16 - bits))
	    & (((1<<bits) - 1) << (16 - bits)), bits,
	    (((1<<bits) - 1) << (16 - bits)));
    SSP_DR = (accumulator << (16 - bits))
      & (((1<<bits) - 1) << (16 - bits));
  }
}
#endif

static void write_fifo_9 (const char* rgb, int cb)
{
  int cmddata = 0;		/* Cmd/Data bit */

  PRINTF ("%s: 0x%x\n", __FUNCTION__, *rgb);

  while (cb--) {
    while (!(SSP_SR & SSP_SR_TNF))	/* Wait for room in FIFO */
      ;
//    printf ("(0x%x) ", *rgb | (cmddata << 8));
    SSP_DR = *rgb++ | (cmddata << 8);
    cmddata = 1;
  }
}

static void spi_read_flush (void)
{
  while (SSP_SR & SSP_SR_RNE)
    SSP_DR;
}

static void spi_write (const char* rgb, int cb)
{
#if defined (USE_SPI)
  cs_enable ();
//  mdelay (1);

  write_fifo_9 (rgb, cb);

  while (SSP_SR & SSP_SR_BSY)
    ;

//  mdelay (1);

  cs_disable ();
//  mdelay (1);
  spi_read_flush ();
#endif
}

void karma_clcdc_setup (void)
{
  ENTRY (0);

  /* The panel must received a reset pulse after power comes on. */

  GPIO_PCD  &= ~(1<<4);		/* Bring reset low */
  GPIO_PCD  |=  (1<<3);		/* Bring power on */
  GPIO_PCDD &= ~((1<<3)|(1<<4)); /* Drive as outputs */

  mdelay (2);
  GPIO_PCD  &= ~(1<<4);		/* Bring reset low */
  mdelay (2);
  GPIO_PCD  |=  (1<<4);		/* Bring reset high */

  cs_disable ();
  GPIO_PGDD &= ~(1<<5);		/* Force to output */

#if defined (USE_FAST_CLOCK)
  SSP_CPR = 2;			/* /2 for 7.3728 MHz master clock*/
#else
//  SSP_CPR = 74;			/* /74 for 99KHz clock*/
  SSP_CPR = 118;
#endif

  SSP_CR1 = 0
    | (1<<6)			/* FEN: FIFO enable  */
    | (0<<4)			/* SPH: Framing high for one SSPCLK period */
    | (0<<3)			/* SPO */
    ;

  SSP_CR0 = 0
    | (0<<4)			/* SPI frame format  */
    | SSP_CR0_SSE
    | (0<<8)			/* SCR == 0  */
//    | SSP_CR0_DSS (16)		/* 16 bit frame format */
    | SSP_CR0_DSS (9)		/* 9 bit frame format */
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

  spi_write ("\x11", 1);
  /* 6 frames (2.7ms), one frame ~450us */
  mdelay (200);			/* was 10 */
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
