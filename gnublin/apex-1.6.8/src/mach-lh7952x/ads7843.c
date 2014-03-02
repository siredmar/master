/* ads7843.c

   written by Marc Singer
   3 Aug 2005

   Copyright (C) 2005 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#include <config.h>
#include <driver.h>
#include <service.h>
#include <linux/string.h>
#include <apex.h>
#include <command.h>
#include <mach/hardware.h>
#include <console.h>

#define TALK 1

#define ADS_CTRL_START	(1<<7)
#define ADS_CTRL_AD(a)	(((a) & 0x7)<<4)
#define ADS_CTRL_PD(p)	((p) & 0x3)

#define ADS_CTRL_PD_IRQ	     0
#define ADS_CTRL_PD_NOIRQ    1
#define ADS_CTRL_PD_PON	     3

#if defined (TALK)
#define PRINTF(f...)		printf (f)
#define ENTRY(l)		printf ("%s\n", __FUNCTION__)
#define DBG(l,f...)		do { if (TALK >= l) printf (f); } while (0)
#else
#define PRINTF(f...)		do {} while (0)
#define ENTRY(l)		do {} while (0)
#define DBG(l,f...)		do {} while (0)
#endif

#define US_SETTLING		(20)		/* Signal stablization */

#if 0
static void execute_spi_command (int v, int cwrite)
{
  PRINTF ("spi 0x%04x -> 0x%x\n", v & 0x1ff, (v >> CODEC_ADDR_SHIFT) & 0x7f);

  CPLD_SPID = (v >> 8) & 0xff;
  CPLD_SPIC = CPLD_SPIC_LOAD | CPLD_SPIC_CS_CODEC;
  while (!(CPLD_SPIC & CPLD_SPIC_LOADED))
    ;
  CPLD_SPIC = CPLD_SPIC_CS_CODEC;
  while (!(CPLD_SPIC & CPLD_SPIC_DONE))
    ;

  CPLD_SPID = v & 0xff;
  CPLD_SPIC = CPLD_SPIC_LOAD | CPLD_SPIC_CS_CODEC;
  while (!(CPLD_SPIC & CPLD_SPIC_LOADED))
    ;
  CPLD_SPIC = CPLD_SPIC_CS_CODEC;
  while (!(CPLD_SPIC & CPLD_SPIC_DONE))
    ;

  CPLD_SPIC = 0;
}
#endif

#define MS_TIMEOUT 2*1000

static void inline msleep (int ms)
{
  int l = timer_read ();
  while (timer_delta (l, timer_read ()) < ms)
    ;
}

static void reset_irq (void)
{
  CPLD_INT |= CPLD_INT_PIRQ;
  msleep (3);
  CPLD_INT &= ~CPLD_INT_PIRQ;
}

static void inline request (int a, int pd)
{
  CPLD_SPID = ADS_CTRL_START | ADS_CTRL_AD(a) | ADS_CTRL_PD(pd);
  CPLD_SPIC = CPLD_SPIC_CS_TOUCH | CPLD_SPIC_LOAD;
  while (!(CPLD_SPIC & CPLD_SPIC_LOADED))
    ;
  CPLD_SPIC = CPLD_SPIC_CS_TOUCH;
  while (!(CPLD_SPIC & CPLD_SPIC_DONE))
    ;
}

static int inline read (void)
{
  int v;

  CPLD_SPID = 0;
  CPLD_SPIC = CPLD_SPIC_CS_TOUCH | CPLD_SPIC_READ | CPLD_SPIC_START;
  while (!(CPLD_SPIC & CPLD_SPIC_LOADED))
    ;
  CPLD_SPIC = CPLD_SPIC_CS_TOUCH | CPLD_SPIC_READ;
  while (!(CPLD_SPIC & CPLD_SPIC_DONE))
    ;
  v = CPLD_SPID;

  CPLD_SPID = 0;
  CPLD_SPIC = CPLD_SPIC_CS_TOUCH | CPLD_SPIC_START | CPLD_SPIC_READ;
  while (!(CPLD_SPIC & CPLD_SPIC_LOADED))
    ;
  CPLD_SPIC = CPLD_SPIC_CS_TOUCH | CPLD_SPIC_READ;
  while (!(CPLD_SPIC & CPLD_SPIC_DONE))
    ;

  v = (v << 5) | (CPLD_SPID >> 3);

  return v;
}

static void inline disable (void)
{
  CPLD_SPIC = 0;
}

static int cmd_ads (int argc, const char** argv)
{
  int x;
  int y;
  int i;
  int j;
  int v;

  request (1, ADS_CTRL_PD_IRQ);
  read ();
  reset_irq ();

  do {
    if (!(CPLD_INT & CPLD_INT_NTOUCH)) {
      x = 0;
      y = 0;

      for (j = 2; j--; ) {
	request (1, ADS_CTRL_PD_PON);
	read ();
      }

      request (1, ADS_CTRL_PD_PON);
      x += read ();

      request (1, ADS_CTRL_PD_PON);
      x += read ();

      for (j = 2; j--; ) {
	request (5, ADS_CTRL_PD_PON);
	read ();
      }

      request (5, ADS_CTRL_PD_PON);
      y += read ();

      request (5, ADS_CTRL_PD_IRQ);
      y += read ();

      disable ();

      x /= 2;
      y /= 2;
      printf ("(%04x %04x)\n", x, y);
      reset_irq ();
    }
  } while (!console->poll (0, 1));
  {
    char ch;
    console->read (0, &ch, 1);
  }

  return 0;
}


static __command struct command_d c_ads = {
  .command = "ads",
  .description = "test ADS7843 touch controller",
  .func = cmd_ads,
};

static __service_7 struct service_d ads7843_service = {
//  .init = ads_init,
//  .release = ads_release,
};
