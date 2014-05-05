/* drv-i2c.c

   written by Marc Singer
   22 Dec 2008

   Copyright (C) 2008 Marc Singer

   -----------
   DESCRIPTION
   -----------

   MX3 I2C driver.

   It only supports bus #1 for the time being, but it won't be hard to
   make it more flexible.

*/

#include <config.h>
#include <apex.h>
#include <driver.h>
#include <service.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <spinner.h>
#include <asm/reg.h>
#include <error.h>

//#define TALK 1
#include <talk.h>

#include "hardware.h"

#define I2C_IFDR_V		(0x3f)			/* Divisor of 2048 */
#define ADDR_NO_STOP		(1<<14)
#define ADDR_RESTART		(1<<13)
#define ADDR_CONTINUE		(1<<12)

const char* describe_i2sr (int v)
{
  static char sz[80];
  snprintf (sz, sizeof (sz), "(0x%02x)", v);

#define _(b,s)\
  if (v & (b)) snprintf (sz + strlen (sz), sizeof (sz) - strlen (sz),\
			 " %s", " " s);

  _(I2C_I2SR_ICF,  "ICF");
  _(I2C_I2SR_IAAS, "IAAS");
  _(I2C_I2SR_IBB,  "IBB");
  _(I2C_I2SR_IAL,  "IAL");
  _(I2C_I2SR_SRW,  "SRW");
  _(I2C_I2SR_IIF,  "IIF");
  _(I2C_I2SR_RXAK, "RXAK");

#undef _

  return sz;
}

const char* describe_i2cr (int v)
{
  static char sz[80];
  snprintf (sz, sizeof (sz), "(0x%02x)", v);

#define _(b,s)\
  if (v & (b)) snprintf (sz + strlen (sz), sizeof (sz) - strlen (sz),\
			 " %s", " " s);

  _(I2C_I2CR_IEN,   "IEN");
  _(I2C_I2CR_MSTA,  "MSTA");
  _(I2C_I2CR_MTX,   "MTX");
  _(I2C_I2CR_TXAK,  "TXAK");
  _(I2C_I2CR_RSATA, "RSATA");

#undef _

  return sz;
}


/* i2c_stop

   halts transmission on the I2C bus.  The kernel version will wait
   for 16 attempts of 3us for the BB to clear.  The return value is 0
   for success.

*/

static int i2c_stop (void)
{
//  unsigned long time;
  ENTRY (0);

//  printf ("--\n");
  I2C_I2CR &= ~(I2C_I2CR_MSTA | I2C_I2CR_MTX
		| I2C_I2CR_TXAK
		);
  I2C_I2SR = 0;

  return 0;
#if 0
  time = timer_read ();
  while ((I2C_I2SR & I2C_I2SR_IBB)	/* Wait for STOP condition */
	 && timer_delta (time, timer_read ()) < 10)
    usleep (3);

  return (I2C_I2SR & I2C_I2SR_IBB) != 0;
#endif
}

/* i2c_wait_for_interrupt

   waits for an interrupt and returns the status.

*/

static int i2c_wait_for_interrupt (void)
{
  unsigned long time;
  ENTRY (0);
  time = timer_read ();
  while (!(I2C_I2SR & I2C_I2SR_IIF)	/* Wait for interrupt */
	 && timer_delta (time, timer_read ()) < 1000*2)
    ;
  if (!(I2C_I2SR & I2C_I2SR_IIF))
    printf ("timeout waiting for interrupt\n");

  I2C_I2SR = 0;				/* Only IIF can be cleared */
  return I2C_I2SR;
}

/* i2c_start

   sends the slave the frame START and target address.  Return value
   is 0 on success.

*/

static int i2c_start (char address, int reading)
{
  int retries = 16;
  int restart = (address & ADDR_RESTART) != 0;
  int sr;

  ENTRY (0);

  {
    unsigned long time = timer_read ();
    while (timer_delta (time, timer_read ()) < 10)
      if (!(I2C_I2SR & I2C_I2SR_IBB))
	break;
    if (I2C_I2SR & I2C_I2SR_IBB) {
      printf ("start aborted, unable to acquire bus sr %s cr %s\n",
	      describe_i2sr (I2C_I2SR), describe_i2cr (I2C_I2CR));
      return 1;			/* Unable to acquire, bus still busy */
    }
  }

  address &= 0x7f;
  PRINTF ("%s: address 0x%x\n", __FUNCTION__, address);

  I2C_I2CR |= I2C_I2CR_MSTA	/* Acquire bus */
    | (restart ? I2C_I2CR_RSATA : 0); /* Assert restart */

  do {
    int sr = I2C_I2SR;
    if (sr & I2C_I2SR_IBB)	/* Arbitration won */
      break;
    usleep (3);
  } while (retries-- > 0);
  if (!(I2C_I2SR & I2C_I2SR_IBB))
    return 1;			/* Failed to acquire */
  I2C_I2SR = 0;
  I2C_I2CR |= I2C_I2CR_MTX;	/* Prepare for transmit */
//  printf (" --> 0x%02x\n", (address << 1) | (reading ? 1 : 0));
  I2C_I2DR = (address << 1) | (reading ? 1 : 0);

  sr = i2c_wait_for_interrupt ();
  I2C_I2CR &= ~I2C_I2CR_RSATA;	/* Clear repeat start */

				/* Address must be ACK'd */
  return (sr & I2C_I2SR_RXAK) ? 1 : 0;
}


static int i2c_open (struct descriptor_d* d)
{
  return 0;
}

static ssize_t i2c_read (struct descriptor_d* d, void* pv, size_t cb)
{
  return ERROR_UNSUPPORTED;
}

static ssize_t i2c_write (struct descriptor_d* d, const void* pv, size_t cb)
{
  int address;
  int i;
  const char* rgb = (const char*) pv;

  ENTRY (0);

  address = simple_strtoul (d->pb[d->iRoot], NULL, 0);

//  int continuing = (address & ADDR_CONTINUE) != 0;
//  int restart    = (address & ADDR_RESTART) != 0;


//  printf ("%s\n", __FUNCTION__);
//  dumpw (rgb, cb, 0, 0);

#if 0
  if (!continuing && !restart && (I2C_I2SR & I2C_I2SR_IBB)) {
    printf ("%s error: i2c bus busy 0x%x\n", __FUNCTION__, address);
    i2c_stop ();
    return ERROR_IOFAILURE;
  }
#endif

  if (i2c_start (address, 0)) {
    printf ("stopping on failed start\n");
    if (i2c_stop ())
      printf ("%s error on stop on failed start sr %s cr %s\n",
	      __FUNCTION__, describe_i2sr (I2C_I2SR),
	      describe_i2cr (I2C_I2CR));
    return ERROR_IOFAILURE;
  }

  I2C_I2CR |= I2C_I2CR_MTX;
  for (i = 0; i < cb; ++i) {
    PRINTF (" ==> 0x%02x\n", *rgb);
    I2C_I2DR = *rgb++;
    if (i2c_wait_for_interrupt () & I2C_I2SR_RXAK) {
      printf ("stopping on failed interrupt\n");
      i2c_stop ();
      return ERROR_IOFAILURE;
    }
  }

  PRINTF ("normal stop\n");
  if (!(address & ADDR_NO_STOP) && i2c_stop ())
    printf ("%s error on stop sr %s cr %s\n",
	    __FUNCTION__, describe_i2sr (I2C_I2SR), describe_i2cr (I2C_I2CR));

  PRINTF ("OK\n");
  return cb;
}

static void i2c_init (void)
{
  ENTRY (0);

  IOMUX_PIN_CONFIG_FUNC  (MX31_PIN_I2C_CLK);
  IOMUX_PIN_CONFIG_FUNC  (MX31_PIN_I2C_DAT);

  I2C2_I2CR = 0;		/* Disable other I2C controllers */
  I2C3_I2CR = 0;

  I2C_IFDR = I2C_IFDR_V;
  I2C_IADR = 0;			/* Our address is zero  */

  I2C_I2SR = 0;
  i2c_stop ();
  I2C_I2CR = I2C_I2CR_IEN;	/* Enable I2C */
}

static void i2c_release (void)
{
  i2c_stop ();
}

static void i2c_report (void)
{
  printf ("  i2c:    enabled\n");
}


static __driver_4 struct driver_d i2c_driver = {
  .name = "i2c-mx3",
  .description = "I2C driver for Freescale MX3x",
  .flags = DRIVER_DESCRIP_FS,
  .open = i2c_open,
  .close = close_helper,
  .read = i2c_read,
  .write = i2c_write,
  .seek = seek_helper,
};

static __service_6 struct service_d i2c_service = {
  .init = i2c_init,
  .release = i2c_release,
#if !defined (CONFIG_SMALL)
  .report = i2c_report,
#endif
};
