/* cmd-spi.c

   written by Marc Singer
   7 Jun 2007

   Copyright (C) 2007 Marc Singer

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
#include "hardware.h"

#include <linux/types.h>
#include <linux/string.h>
#include <command.h>
#include <error.h>
#include <linux/kernel.h>
#include <environment.h>
#include <service.h>
#include <lookup.h>
#include <error.h>

#include <debug_ll.h>

#define USE_FAST_CLOCK
//#define USE_DDS_HACK
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

static void write_fifo_short (const unsigned short* rgs, int c)
{
  while (c--) {
    while (!(SSP_SR & SSP_SR_TNF))	/* Wait for room in FIFO */
      ;
//    printf ("(0x%x) ", *rgb | (cmddata << 8));
    SSP_DR = *rgs++;
  }
}

static void spi_read_flush (void)
{
  while (SSP_SR & SSP_SR_RNE)
    SSP_DR;
}

#if 0
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

#endif

static void spi_write (const short* rgs, int c)
{
#if defined (USE_SPI)
  cs_enable ();
//  mdelay (1);

  write_fifo_short (rgs, c);

  while (SSP_SR & SSP_SR_BSY)
    ;

//  mdelay (1);

  cs_disable ();
//  mdelay (1);
  spi_read_flush ();
#endif
}


static void spi_init (int speed)
{
#if defined (USE_FAST_CLOCK)
    SSP_CPR = speed;			/* /2 for 7.3728 MHz master clock*/
#else
//  SSP_CPR = 74;			/* /74 for 99KHz clock*/
    SSP_CPR = 118;
#endif

    printf ("SSP_CPR 0x%lx\n", SSP_CPR);

    SSP_CR1 = 0
      | (1<<6)			/* FEN: FIFO enable  */
      | (0<<4)			/* SPH: Framing high for one SSPCLK period */
      | (1<<3)			/* SPO */
      ;

    SSP_CR0 = 0
      | (0<<4)			/* SPI frame format  */
      | SSP_CR0_SSE
      | (0<<8)			/* SCR == 0  */
//    | SSP_CR0_DSS (16)		/* 16 bit frame format */
      | SSP_CR0_DSS (9)		/* 9 bit frame format */
      ;
}

static int cmd_spi (int argc, const char** argv)
{
  if (argc < 2)
    return ERROR_PARAM;

  if (strcmp (argv[1], "i") == 0) {
    int speed = 2;

    if (argc == 3)
      speed = simple_strtoul (argv[2], NULL, 10);

    printf ("spi init speed %d\n", speed);
    cs_disable ();
    GPIO_PGDD &= ~(1<<5);		/* Force to output */

    spi_init (speed);

    printf ("SSP_CPR 0x%lx\n", SSP_CPR);
    printf ("SSP_CR0 0x%lx\n", SSP_CR0);
    printf ("SSP_CR1 0x%lx\n", SSP_CR1);

  }
  if (strcmp (argv[1], "o") == 0)
    SSP_CR0 = 0
      | (0<<4)			/* SPI frame format  */
//      | SSP_CR0_SSE
      | (0<<8)			/* SCR == 0  */
//    | SSP_CR0_DSS (16)		/* 16 bit frame format */
      | SSP_CR0_DSS (9)		/* 9 bit frame format */
      ;
  if (strcmp (argv[1], "r") == 0) {
    printf ("spi read\n");
  }
  if (strcmp (argv[1], "w") == 0) {
    unsigned long v;
    printf ("spi w\n");
    if (argc != 3)
      return ERROR_PARAM;
    if (argv[2][0] == '0' && (argv[2][1] == 'x' || argv[2][1] == 'X'))
      v = simple_strtoul (argv[2]+2, NULL, 16);
    else
      v = simple_strtoul (argv[2], NULL, 10);
    printf ("spi write 0x%lx (%ld)\n", v, v);
    spi_write ((unsigned short*) &v, 1);
  }

  if (strcmp (argv[1], "wake") == 0) {
    unsigned short s;
    printf ("wake\n");
    spi_init (2);
    s = 0x11;
    spi_write (&s, 1);
  /* 6 frames (2.7ms), one frame ~450us */
    mdelay (200);

    s = 0x26;
    spi_write (&s, 1);
    s = 0x104;
    spi_write (&s, 1);

    s = 0x29;
    spi_write (&s, 1);

    s = 0x36;
    spi_write (&s, 1);
    s = 0x100;
    spi_write (&s, 1);

    SSP_CR0 = 0;
  }

  return 0;
}


static __command struct command_d c_spi = {
  .command = "spi",
  .func = (command_func_t) cmd_spi,
  COMMAND_DESCRIPTION ("issue spi transaction")
  COMMAND_HELP(
"spi r|w [VALUE]\n"
"  Issue an SPI command.\n"
"  If the direction is w, the VALUE must be defined.\n"
"  i	- initialize SPI interface\n"
"  w VALUE	- write VALUE which is a 9 bit hexadecimal number\n"
  )
};
