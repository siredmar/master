/* cmd-onenand.c

   written by Marc Singer
   30 Mar 2007

   Copyright (C) 2007 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   OneNAND driver and test routines.

   o Partial Writes.  We must infer from the OneNAND documentation
     that rewriting portions of a page require a read-modify-write
     cycle.  We don't really assume that we can rewrite a page from
     APEX, so the HW implementation details are somewhat irrelevent.

   o DataBuffer0/DataBuffer1.  In APEX, we don't really care about
     which data buffer we use to access the NAND flash.  The
     relocation code uses DataBuffer0 so that we can get more than 1K
     of the boot loader into contiguous memory before SDRAM is
     initialized.  However, we use DataBuffer1 here so that we can
     debug the relocator.

   o current_page.  This is an optimization for reading pages such
     that we only load a new page from the array when the user asks
     for a page different from the one currently loaded.  The OneNAND
     is fast enough that we probably never notice the time it takes to
     load a page.  However, it is a cheap optimization that is easy to
     verify for correctness.

   o Macros.  Normally, I wouldn't implement core functions as macros.
     In this case, I chose to use some macros to simplify the roles of
     multiple users of the OneNAND device.  This driver would be well
     served with a functional implementation, but the relocator cannot
     depend on those functions...unless we put them in the bootstap.
     OK...if we get to that point, we may locate some of this code
     among the bootstrap sections.

*/

#include <config.h>
#include <apex.h>
#include <service.h>
#include <linux/string.h>
#include <error.h>
#include <linux/kernel.h>
#include <driver.h>
#include <service.h>
#include <spinner.h>
#include <drv-onenand-base.h>

#include <asm/reg.h>

//#define TALK

#define I_DATABUFFER	1
#define DATABUFFER	ONENAND_DATARAM1

struct onenand_chip {
  unsigned short id[3];		/* Manufacturer, device, version */
  int technology;
  int page_size;
  int erase_size;
  int boot_size;
  int cBuffers;
  int unlocked;			/* Blocks have been unlocked */
};

struct onenand_chip chip;
int current_page;		/* Currently loaded page, -1 when undefined */

static char* describe_status (int status)
{
  static char sz[80];
  snprintf (sz, sizeof (sz), "(0x%x)", status);

#if !defined (CONFIG_SMALL)

#define _(b,s) if (status & (b))\
	snprintf (sz + strlen (sz), sizeof (sz) - strlen (sz), " %s", s)

  _(ONENAND_STATUS_TO, "TO");
  _(ONENAND_STATUS_PLANE2CURR, "PLANE2CURR");
  _(ONENAND_STATUS_PLANE2PREV, "PLANE2PREV");
  _(ONENAND_STATUS_PLANE1CURR, "PLANE1CURR");
  _(ONENAND_STATUS_PLANE1PREV, "PLANE1PREV");
  _(ONENAND_STATUS_OTPBL, "OTPBL");
  _(ONENAND_STATUS_OTPL, "OTPL");
  _(ONENAND_STATUS_RSTB, "RSTB");
  _(ONENAND_STATUS_SUSPEND, "SUSPEND");
  _(ONENAND_STATUS_ERROR, "ERROR");
  _(ONENAND_STATUS_ERASE, "ERASE");
  _(ONENAND_STATUS_PROG, "PROG");
  _(ONENAND_STATUS_LOAD, "LOAD");
  _(ONENAND_STATUS_LOCK, "LOCK");
  _(ONENAND_STATUS_ONGO, "ONGO");
#undef _

#endif

  return sz;
}

static void execute (int page, int command)
{
  ONENAND_PAGESETUP (page);
  ONENAND_BUFFSETUP (I_DATABUFFER, 0, 4);

#if defined (TALK)
  printf ("sa1 0x%x sa2 0x%x sa8 0x%x sb 0x%x sba 0x%x ",
	  ONENAND_SA_1, ONENAND_SA_2, ONENAND_SA_8, ONENAND_SB, ONENAND_SBA);
  printf ("NAND_INTR 0x%x  NAND_STATUS 0x%x\n",
	  ONENAND_INTR, ONENAND_STATUS);
#endif

  ONENAND_INTR = 0;
  ONENAND_CMD = command;

  while (ONENAND_IS_BUSY)
    ;
#if defined (TALK)
  printf ("OK\n");
#endif
}

static void onenand_unlock (void)
{
  if (chip.unlocked)
    return;

  ONENAND_SA_1 = DFS_FBA (0/PAGES_PER_BLOCK);
  ONENAND_SA_2 = DBS (0);
  ONENAND_SBA  = SBA (0);
  ONENAND_INTR = 0;
  ONENAND_CMD = ONENAND_CMD_UNLOCKALL;

  if (ONENAND_STATUS & ONENAND_STATUS_ERROR)
    printf ("unlock_all failed %s\n", describe_status (ONENAND_STATUS));

  chip.unlocked = 1;
}


/* onenand_init

   probes the OneNAND flash device.

*/

static void onenand_init (void)
{
  chip.id[0] = ONENAND_MAN_ID;
  chip.id[1] = ONENAND_DEV_ID;
  chip.id[2] = ONENAND_VER_ID;
  chip.technology = ONENAND_TECH_INFO;
  chip.page_size = ONENAND_DATA_SIZE;
  chip.erase_size = chip.page_size*PAGES_PER_BLOCK;
  chip.boot_size = ONENAND_BOOT_SIZE;
  chip.cBuffers = ONENAND_BUFF_CNT;

  current_page = -1;
}

static int onenand_open (struct descriptor_d* d)
{
  if (!chip.id[0])
    return -1;

  /* Perform bounds check */

  return 0;
}

static ssize_t onenand_read (struct descriptor_d* d, void* pv, size_t cb)
{
  ssize_t cbRead = 0;

  if (!chip.id[0])
    return cbRead;

  if (d->index + cb > d->length)
    cb = d->length - d->index;

  while (cb) {
    unsigned long page  = (d->start + d->index)/chip.page_size;
    int index = (d->start + d->index)%chip.page_size;
    int available = chip.page_size - index;

    if (available > cb)
      available = cb;

    d->index += available;
    cb -= available;
    cbRead += available;

    if (page != current_page) {
      execute (page, ONENAND_CMD_LOAD);
      current_page = page;
    }

    if (ONENAND_STATUS & ONENAND_STATUS_ERROR)
      printf ("read failed %s\n", describe_status (ONENAND_STATUS));

    memcpy (pv, (const char*) DATABUFFER + index, available);
    pv += available;
  }

  return cbRead;
}

static ssize_t onenand_write (struct descriptor_d* d,
			      const void* pv, size_t cb)
{
  int cbWrote = 0;

  if (!chip.id[0])
    return cbWrote;

  current_page = -1;

  if (d->index + cb > d->length)
    cb = d->length - d->index;

  SPINNER_STEP;

  onenand_unlock ();

  while (cb) {
    unsigned long page  = (d->start + d->index)/chip.page_size;
    unsigned long index = (d->start + d->index)%chip.page_size;
    int available = chip.page_size - index;

    if (available > cb)
      available = cb;

    if (available != chip.page_size)
      execute (page, ONENAND_CMD_LOAD);		/* Prepare for partial write */
    memcpy ((char*) DATABUFFER + index, pv, available);
    execute (page, ONENAND_CMD_PROGRAM);

    SPINNER_STEP;

    if (ONENAND_STATUS & ONENAND_STATUS_ERROR) {
      printf ("Write failed at page %ld %s\n", page,
	      describe_status (ONENAND_STATUS));
      goto exit;
    }

    d->index += available;
    cb -= available;
    cbWrote += available;
    pv += available;

  }

 exit:

  return cbWrote;
}

static void onenand_erase (struct descriptor_d* d, size_t cb)
{
  if (!chip.id[0])
    return;

  current_page = -1;

  onenand_unlock ();

  if (d->index + cb > d->length)
    cb = d->length - d->index;

  SPINNER_STEP;

  do {
    unsigned long page = (d->start + d->index)/chip.page_size;
    unsigned long available
      = chip.erase_size - ((d->start + d->index) & (chip.erase_size - 1));

    execute (page, ONENAND_CMD_BLOCKERASE);

    SPINNER_STEP;

    if (ONENAND_STATUS & ONENAND_STATUS_ERROR) {
      printf ("Erase failed at page %ld %s\n", page,
	      describe_status (ONENAND_STATUS));
      goto exit;
    }

    if (available < cb) {
      cb -= available;
      d->index += available;
    }
    else {
      cb = 0;
      d->index = d->length;
    }
  } while (cb > 0);

 exit:
  ;
}

#if !defined (CONFIG_SMALL)

static void onenand_report (void)
{
  if (!chip.id[0])
    return;

  printf (" onenand: %d MiB total (%s), %d KiB erase, %d B page\n",
	  (1<<(((chip.id[1] >> 4) & 0xf)))*128/8,
	  (chip.id[1] & (1<<3)) ? "DDP" : "Single",
	  chip.erase_size/1024, chip.page_size);
}

#endif

static __driver_3 struct driver_d onenand_driver = {
  .name = "nand-onenand",
  .description = "OneNAND flash driver",
  .flags = DRIVER_WRITEPROGRESS(6),
  .open = onenand_open,
  .close = close_helper,
  .read = onenand_read,
  .write = onenand_write,
  .erase = onenand_erase,
  .seek = seek_helper,
};

static __service_6 struct service_d onenand_service = {
  .init = onenand_init,
#if !defined (CONFIG_SMALL)
  .report = onenand_report,
#endif
};
