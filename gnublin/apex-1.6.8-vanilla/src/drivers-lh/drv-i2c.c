/* drv-i2c.c

   written by Marc Singer
   25 May 2006

   Copyright (C) 2006 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   Write Delay
   -----------

   After any write, the chip is unavailable for 5-10ms (depending on
   the part).

   Page Write
   ----------

   Page aligned writes may be performed.  The page is 128 bytes long.
   Only bytes within a single page may be written in one go.


*/

#include <config.h>
#include <driver.h>
#include <service.h>
#include <linux/string.h>
#include <apex.h>
#include <command.h>
#include <mach/hardware.h>
#include <console.h>
#include <spinner.h>

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

#define US_WRITE_DELAY		(10*1000)
#define PAGE_SIZE		(128)
#define WRITE_SIZE		(1)
#define READ_SIZE		(8)

#if 0

#define BMI_SWIDR		__REG(BMI_PHYS + 0x00)
#define BMI_SWICR		__REG(BMI_PHYS + 0x04)
#define BMI_SWISR		__REG(BMI_PHYS + 0x08)
#define BMI_SWIRISR		__REG(BMI_PHYS + 0x0c)
#define BMI_SWIEOI		__REG(BMI_PHYS + 0x0c)
#define BMI_SWIIDR		__REG(BMI_PHYS + 0x10)
#define BMI_SWIIER		__REG(BMI_PHYS + 0x14)
#define BMI_SWITR		__REG(BMI_PHYS + 0x18)
#define BMI_SWIBR		__REG(BMI_PHYS + 0x1c)

#define BMI_SWICR_SP_INVERT	(1<<16)
#define BMI_SWICR_T_RST		(1<<15)
#define BMI_SWICR_RDSS_SHIFT	(9)
#define BMI_SWICR_RDSS_MASK	(0x3f)
#define BMI_SWICR_WDSS_SHIFT	(3)
#define BMI_SWICR_WDSS_MASK	(0x3f)
#define BMI_SWICR_DINV		(1<<2)
#define BMI_SWICR_GBS		(1<<1)
#define BMI_SWICR_SWIEN		(1<<0)

#define BMI_SWISR_COL		(1<<11)	/* Collision */
#define BMI_SWISR_TEF		(1<<10)	/* Transmit Error Flag */
#define BMI_SWISR_RXB		(1<<9)	/* Receive Busy */
#define BMI_SWISR_BRFB		(1<<8)	/* Break Recovered Flag */
#define BMI_SWISR_BRS		(1<<7)	/* Break Recovery Status */
#define BMI_SWISR_BKS		(1<<6)	/* Break Status */
#define BMI_SWISR_RXF		(1<<5)	/* Received Flag */
#define BMI_SWISR_TXF		(1<<4)	/* Transmitted Flag */
#define BMI_SWISR_PBS		(1<<3)	/* Stop Bit Status */
#define BMI_SWISR_DBS		(1<<2)	/* Data Bit Status */
#define BMI_SWISR_SBS		(1<<1)	/* Start Bit Status */
#define BMI_SWISR_TXB		(1<<0)	/* Transmit Busy */

#define BMI_SWI_BRI		(1<<2) /* Break Recovered Interrupt */
#define BMI_SWI_WRI		(1<<1) /* Word Received Interrupt */
#define BMI_SWI_WTI		(1<<0) /* Word Transmitted Interrupt */

#define BMI_BTG_FAST_V		(1)     /*   1 *135.6 ns/bit, 7.3278 MHz*/
#define BMI_BTG_SLOW_V		(0x1df) /* 479 *135.6 ns/bit, 15.4KHz */

#endif

#define BMI_SBIDR		__REG(BMI_PHYS + 0x40)
#define BMI_SBICR		__REG(BMI_PHYS + 0x44)
#define BMI_SBICOUNT		__REG(BMI_PHYS + 0x48)
#define BMI_SBISR		__REG(BMI_PHYS + 0x4c)
#define BMI_SBRISR		__REG(BMI_PHYS + 0x50)
#define BMI_SBRIEOI		__REG(BMI_PHYS + 0x50)
#define BMI_SBRIISR		__REG(BMI_PHYS + 0x54)
#define BMI_SBRIIER		__REG(BMI_PHYS + 0x58)

#define BMI_SBICR_PEF		(1<<25)
#define BMI_SBICR_BRF		(1<<24)
#define BMI_SBICR_TOC_SHIFT	(1<<12)
#define BMI_SBICR_TOC_MASK	(0xfff)
#define BMI_SBICR_RX_FDIS	(1<<11)
#define BMI_SBICR_TX_FDIS	(1<<10)
#define BMI_SBICR_DIVFACT_SHIFT	(2)
#define BMI_SBICR_DIVFACT_MASK	(0xff)
#define BMI_SBICR_FFLUSH	(1<<1)
#define BMI_SBICR_SBI_EN	(1<<0)

#define BMI_SBICOUNT_PRE_SHIFT	(0)
#define BMI_SBICOUNT_PRE_MASK	(0x1f)
#define BMI_SBICOUNT_REP_SHIFT	(5)
#define BMI_SBICOUNT_REP_MASK	(0x1f)
#define BMI_SBICOUNT_READ_SHIFT	(10)
#define BMI_SBICOUNT_READ_MASK	(0x1f)

#define BMI_SBISR_SBH		(1<<12)	/* Slave Bus Hold */
#define BMI_SBISR_CLT		(1<<11)	/* Clock Low Timeout */
#define BMI_SBISR_TXUE		(1<<10)	/* Transmit Underrun Error */
#define BMI_SBISR_RXOE		(1<<9) /* Receive Overrun Error */
#define BMI_SBISR_ACKFAIL	(1<<8) /* Acknowledge Fail */
#define BMI_SBISR_RNW		(1<<7) /* Read/nWrite */
#define BMI_SBISR_SLAVE		(1<<6) /* Slave mode */
#define BMI_SBISR_MASTER	(1<<5) /* Master Mode */
#define BMI_SBISR_TXBUSY	(1<<4) /* Transmit Busy */
#define BMI_SBISR_TXFF		(1<<3) /* Transmit FIFO Full */
#define BMI_SBISR_RXFF		(1<<2) /* Receive FIFO Full */
#define BMI_SBISR_TXFE		(1<<1) /* Transmit FIFO Empty */
#define BMI_SBISR_RXFE		(1<<0) /* Receive FIFO Empty */

#define BMI_SBI_CLTI		(1<<7)
#define BMI_SBI_STCI		(1<<6)
#define BMI_SBI_ALI		(1<<5)
#define BMI_SBI_AFI		(1<<4)
#define BMI_SBI_RXI		(1<<3)
#define BMI_SBI_TXI		(1<<2)
#define BMI_SBI_RTI		(1<<1)
#define BMI_SBI_MTCI		(1<<0)

static const char* report_status (unsigned long l)
{
  static char sz[256];

  sprintf (sz, "[%04lx", l);

  if (l & BMI_SBISR_SBH)
    strcat (sz, " SLAVEH");
  if (l & BMI_SBISR_CLT)
    strcat (sz, " CLT");
  if (l & BMI_SBISR_TXUE)
    strcat (sz, " TXUE");
  if (l & BMI_SBISR_RXOE)
    strcat (sz, " RXOE");
  if (l & BMI_SBISR_ACKFAIL)
    strcat (sz, " ACKFAIL");
  if (l & BMI_SBISR_RNW)
    strcat (sz, " RNW");
  if (l & BMI_SBISR_SLAVE)
    strcat (sz, " SLAVE");
  if (l & BMI_SBISR_MASTER)
    strcat (sz, " MASTER");
  if (l & BMI_SBISR_TXBUSY)
    strcat (sz, " TXBUSY");
  if (l & BMI_SBISR_TXFF)
    strcat (sz, " TXFF");
  if (l & BMI_SBISR_RXFF)
    strcat (sz, " RXFF");
  if (l & BMI_SBISR_TXFE)
    strcat (sz, " TXFE");
  if (l & BMI_SBISR_RXFE)
    strcat (sz, " RXFE");

  strcat (sz, "]");
  return sz;
}

static void reset_io (void)
{
  ENTRY (1);

  BMI_SBICR |= BMI_SBICR_FFLUSH;
  udelay (10);
  BMI_SBICR &= ~BMI_SBICR_FFLUSH;

  BMI_SBRIEOI = 0xff;		/* Clear interrupts and status */

  /* Flush receive FIFO */
  while (!(BMI_SBISR & BMI_SBISR_RXFE))
    BMI_SBIDR;
}

static void flush_read_fifo (void)
{
  while (!(BMI_SBISR & BMI_SBISR_RXFE))
    BMI_SBIDR;
}

static int wait_for_idle (void)
{
  DBG (3, "idle_wait %s\n", report_status (BMI_SBISR));
  while (BMI_SBISR & (BMI_SBISR_TXBUSY))
    ;

  DBG (3, "  done %s\n", report_status (BMI_SBISR));
  return 0;
}

static int send_buffer (const void* pv, int cb)
{
  //  dump (rgb, cb, 0);

  DBG (2, "sending %d %s\n", cb, report_status (BMI_SBISR));
  while (cb--) {
    while (BMI_SBISR & BMI_SBISR_TXFF)
      ;
    BMI_SBIDR = *(unsigned char*) pv++;
  }

  DBG (2, "done %s\n", report_status (BMI_SBISR));

  return 0;
}

static void setup_count (int prefix, int repeat, int read)
{
  if (repeat + prefix < 31) {
    prefix += repeat;
    repeat = 0;
  }

  BMI_SBICOUNT =
      (prefix << BMI_SBICOUNT_PRE_SHIFT)
    | (repeat << BMI_SBICOUNT_REP_SHIFT)
    | (read   << BMI_SBICOUNT_READ_SHIFT);
  DBG (1, "%s: %d %d %d -> 0x%lx\n",
       __FUNCTION__, prefix, repeat, read, BMI_SBICOUNT);
}

static int start_cmd_read_current (void)
{
  char rgb[1] = { 0xa0 | 1 };

  wait_for_idle ();
  setup_count (1, 0, 1);
  send_buffer (rgb, sizeof (rgb));

  return 0;
}

static int start_cmd_page_read (void* pv, int cb)
{
  char rgb[1] = { 0xa0 | 1 };

  wait_for_idle ();
  setup_count (sizeof (rgb), 0, cb);
  send_buffer (rgb, sizeof (rgb));

  while (cb--) {
    while (BMI_SBISR & BMI_SBISR_RXFE)
      ;
    *(unsigned char*) pv++ = BMI_SBIDR & 0xff;
  }

  return 0;
}

static int start_cmd_seek (unsigned long address)
{
  char rgb[3] = { 0xa0, (address >> 8) & 0xff, (address >> 0) & 0xff };

  wait_for_idle ();
  setup_count (sizeof (rgb), 0, 0);
  send_buffer (rgb, sizeof (rgb));

  return 0;
}

static int start_cmd_write (unsigned long address, char value)
{
  char rgb[4] = { 0xa0, (address >> 8) & 0xff, (address >> 0) & 0xff, value };

  wait_for_idle ();
  setup_count (sizeof (rgb), 0, 0);
  send_buffer (rgb, sizeof (rgb));

  return 0;
}

static int start_cmd_page_write (unsigned long address,
				  const void* pv, int cb)
{
  char rgb[3] = { 0xa0, (address >> 8) & 0xff, (address >> 0) & 0xff };

  wait_for_idle ();
  setup_count (sizeof (rgb), cb, 0);
  send_buffer (rgb, sizeof (rgb));
  send_buffer (pv, cb);

  return 0;
}

static void i2c_init (void)
{
  /* 0x49 would be the fastest allowed */
  BMI_SBICR = BMI_SBICR_SBI_EN | (0x4a<<BMI_SBICR_DIVFACT_SHIFT);
}

static int i2c_open (struct descriptor_d* d)
{
  return 0;
}

static ssize_t i2c_read (struct descriptor_d* d, void* pv, size_t cb)
{
  ssize_t cbRead = 0;

  reset_io ();

  if (d->index + cb > d->length)
    cb = d->length - d->index;

  start_cmd_seek (d->start + d->index);

  while (cb > 0) {
    unsigned int available = READ_SIZE;

    if (available > cb)
      available = cb;

    wait_for_idle ();
    start_cmd_page_read (pv, available);

    d->index += available;
    pv += available;
    cb -= available;
    cbRead += available;
  }

  return cbRead;
}

static ssize_t i2c_write (struct descriptor_d* d, const void* pv, size_t cb)
{
  size_t cbWrote = 0;

  reset_io ();

  if (d->index + cb > d->length)
    cb = d->length - d->index;

  while (cb > 0) {
    unsigned long index = d->start + d->index;
    int available = PAGE_SIZE - (index & (PAGE_SIZE - 1));

    if (available > cb)
      available = cb;
    if (available > WRITE_SIZE)
      available = WRITE_SIZE;

    wait_for_idle ();
    if (available > 1)
      start_cmd_page_write (index, pv, available);
    else
      start_cmd_write (index, *(unsigned char*) pv);

    SPINNER_STEP;

    udelay (US_WRITE_DELAY);

    d->index += available;
    pv += available;
    cb -= available;
    cbWrote += available;
  }

  wait_for_idle ();

  return cbWrote;
}


static void i2c_erase (struct descriptor_d* d, size_t cb)
{
  char __aligned rgb[128];
  memset (rgb, 0xff, sizeof (rgb));

  while (cb) {
    int available = sizeof (rgb);
    if (available > cb)
      available = cb;

    if (i2c_write (d, rgb, available) != available)
      return;

    d->index += available;
    cb -= available;
  }
}


static void i2c_report (void)
{
  printf ("  i2c:    -\n");
}

static __driver_5 struct driver_d i2c_driver = {
  .name = "i2c-lh7a40x",
  .description = "I2C EEPROM driver",
  .flags = DRIVER_WRITEPROGRESS(1) | DRIVER_READPROGRESS(1),
  .open = i2c_open,
  .close = close_helper,
  .read = i2c_read,
  .write = i2c_write,
  .erase = i2c_erase,
  .seek = seek_helper,
};

static __service_6 struct service_d i2c_service = {
  .init = i2c_init,
#if !defined (CONFIG_SMALL)
  .report = i2c_report,
#endif
};

static int cmd_i2c (int argc, const char** argv)
{
  if (argc != 2)
    goto usage;

  switch (*argv[1]) {
  case 'r':
    reset_io ();

    flush_read_fifo ();

    printf ("status at top is 0x%lx\n", BMI_SBISR);

    start_cmd_read_current ();

    wait_for_idle ();
    printf ("status after idle is 0x%lx\n", BMI_SBISR);

    while (BMI_SBISR & BMI_SBISR_RXFE)
      ;

    printf ("status after waiting for receive is 0x%lx\n", BMI_SBISR);

    while (!(BMI_SBISR & BMI_SBISR_RXFE))
      printf ("  0x%02lx\n", BMI_SBIDR & 0xff);
    break;

  case 'w':
    reset_io ();

    start_cmd_write (0, 0xe5);
    wait_for_idle ();

    printf ("status after waiting for receive is 0x%lx\n", BMI_SBISR);
    break;

  case 's':
    reset_io ();

    wait_for_idle ();
    printf ("status after idle is 0x%lx\n", BMI_SBISR);
    start_cmd_seek (0);
    wait_for_idle ();
    printf ("status after idle is 0x%lx\n", BMI_SBISR);
    break;

  default:
  usage:
    printf ("usage: i2c r|w|s\n");
    break;
  }

  return 0;
}

static __command struct command_d c_i2c = {
  .command = "i2c",
  .func = cmd_i2c,
  COMMAND_DESCRIPTION ("test i2c interface")
};
