/* drv-mmc.c

   written by Marc Singer
   19 Oct 2005

   Copyright (C) 2005 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   NOTES
   -----

   o Detection should be continuous.
     o If the card doesn't respond to the known address, we can
       perform acquire again.
     o This can be done when the report is generated as well.
     o Perhaps we acquire every time?
     o The only way to know we have the right card is to randomize the
       ID we set in MMC cards or...we have to acquire every time we
       start a transaction.
   o IO should really be quite easy, check for boundaries and read
     blocks as we do in the CF driver.
   o Enable SD mode when available.  There are two pieces, the
     controller and the card.  It looks like the WIDE bit is the one
     we need.  Not clear what SDIO_EN does.
   o Check for the best speed.  Some cards can handle 25MHz.  Can the
     core?  The documentation states that 20MHz is the maximum.  In
     that case, we should use a predivisor of 5 and we can vary the
     rate from 20MHz to 312KHz.

   -----------
   DESCRIPTION
   -----------

   Clocking
   --------

   The specification limits the clock frequency during identification
   to 400KHz.  During I/O, the limit is 20MHz due to the CPU core

   Responses Byte Ordering
   -----------------------

   On old hardware, rev -01 of the LH7A400, the response FIFO
   half-word is byte swapped.

      15     8 7      0
     +--------+--------+
     | BYTE 0 | BYTE 1 |	// Old hardware, MSB default
     +--------+--------+

   The correct ordering puts the first byte in the low order bits.

      15     8 7      0
     +--------+--------+
     | BYTE 1 | BYTE 0 |	// Correct layout, LSB default
     +--------+--------+

   Block Caching
   -------------

   The driver caches one block from the card and copied data from the
   cached block to the caller's buffer.  This is done because the
   callers aren't expected to be efficient about reading large blocks
   of data.  This is a convenience for the upper layers as the command
   routines are best when they can handle data in the manner most
   efficient to their structure.

   Data FIFO Byte Ordering
   -----------------------

   The data FIFO is reading out in MSB order even though the
   BIG_ENDIAN_BIT isn't set in the CMD_CON register.

   Card Acquisition
   ----------------

   The present state of the card acqusition logic is weak.  We acquire
   a card when the system initializes.  If the card goes offline, we
   have no way to detect and correct it.  What should happen is the
   read code ought to detect that the card has changed, probably
   because the select fails, and perform the acquire at that time.

   SINGLE BLOCK WRITE
   ------------------

   18.1.10.4 Single Block Write
       For the Single Block Write command, the MMC registers are programmed:

   1.  The clock is stopped.
   2.  Set the block size in the BLK_LEN register.
   3.  Set the NOB register to 0x1.
   4.  Get card status.
   5.  Command card into transfer state.
   6.  Program the command code into the CMD register.
   7.  The most significant byte arguments are programmed in ARGUMENT.
   8.  The CMDCON parameters are programmed: RESPONSE_FORMAT
       programmed to 0x01, specifying Format R1.  DATA_EN is set,
       indicating the command includes a data transfer.  The Write
       field (WRITE) is set, specifying a Write.  The Stream field
       (STREAM) is cleared, specifying a block transfer.  BUSY is
       cleared, indicating no Busy signal is expected after the
       command.  Unless an Initialization sequence is required,
       INITIALIZE is cleared.
   9.  The clock is started.
   10. Software checks the STATUS register. FIFO Empty field
       (STATUS:FIFO_EMPTY) to identify whether the FIFO is empty.
   11. When the FIFO is empty, software writes the FIFO and starts the clock.
   12. Check STATUS:CRCWRITE. The SD/MMC will not accept Write data if
       received with a CRC error. If a CRC error occurs, software must
       re-write the data to the card.
   13. Send STOP_TRANS command.
   14. Software polls the STATUS register Data Transfer Done field
       (STATUS:TRANDONE) until the field is set.
   15. After TRANDONE is set, software checks various fields in the
       STATUS register to identify whether the data transfer finished
       successfully.
   16. To address a different card, software can send a Chip Select
       command to the card by sending a basic No Data Command and
       Response transaction. To address the same card, software must
       wait for the STATUS register Program Done field (STATUS:DONE)
       to be set, to ensure the Busy is finished.


   o Some data.

     2GiB
     cid
     00000000: 02 54 4d 53 44 30 32 47  38 a6 7d 54 8e 00 86 3f  .TMSD02G 8.}T...?
     csd
     00000000: 00 2e 00 32 5b 5a 83 a9  ff ff ff 80 16 80 00 3f  ...2[Z.. .......?

     Eye-fi (2GiB)
     cid
     00000000: 00 11 22 45 59 45 46 49  00 00 00 00 00 33 44 3f  .."EYEFI .....3D?
     csd
     00000000: 00 36 00 32 53 5a 83 bd  f6 db ff 87 16 80 00 3f  .6.2SZ.. .......?

     1GiB
     cid
     00000000: 03 53 44 53 55 30 31 47  80 01 c4 46 f7 00 7a 3f  .SDSU01G ...F..z?
     csd
     00000000: 00 26 00 32 5f 59 83 c8  be fb cf ff 92 40 40 3f  .&.2_Y.. .....@@?


*/

#include <config.h>
#include <driver.h>
#include <service.h>
#include <linux/string.h>
#include <apex.h>
#include <command.h>
#include <mach/hardware.h>
#include <console.h>
#include <error.h>

#include <debug_ll.h>

#include <mmc.h>

//#define TALK 1

#if defined (USE_MMC_BOOTSTRAP)
# undef TALK
#endif

#include <talk.h>

//#define USE_BIGENDIAN_RESPONSE /* Old hardware */
#define USE_SD			/* Allow SD cards */
#define USE_WIDE		/* Allow WIDE mode */
//#define USE_SLOW_CLOCK	/* Slow the transfer clock to 12MHz */
//#define USE_WAYSLOW_CLOCK	/* Slow the transfer clock to 400KHz */
/* *** While multiblock read kinda works, it seems to put the
   interface in an odd state.  Since we don't care about this now,
   we're disabling the feature until we have time to look at it.  */
//#define USE_MULTIBLOCK_READ	/* Use multiblock read implementation */

#if defined (CONFIG_RELOCATE_COMPANION_EVT1)
# define USE_MMC_BOOTSTRAP	/* Allow MMC driver to be used in bootstrap */
# undef USE_MULTIBLOCK_READ	/* ...just in case */
#endif

#if defined (COMPANION)
# define GPIO_WP		PH1
# define USE_WIDE
#endif

#if defined (TROUNCER)
# define GPIO_CARD_DETECT	PF6
# define GPIO_WP		PC1
# define USE_WIDE
#endif

#if defined USE_MMC_BOOTSTRAP
# define SECTION __section (.bootstrap)
# define SECTIOND __section (.bss_bootstrap)
#else
# define SECTION
# define SECTIOND
#endif

#define C_SIZE		csd_short(62, 12)
#define C_SIZE_MULT	csd_byte(47, 3)
#define NSAC		csd_byte(104, 8)
#define READ_BL_LEN	csd_byte(80, 4)
#define SECTOR_SIZE	csd_byte(39, 7)
#define TAAC		csd_byte(112, 8)
#define TRAN_SPEED	csd_byte(96, 8)


extern char* strcat (char*, const char*);

#if defined (USE_MULTIBLOCK_READ)
# define CB_CACHE	(2*MMC_SECTOR_SIZE)
#else
# define CB_CACHE	(MMC_SECTOR_SIZE)
#endif

struct mmc_info SECTIOND mmc;
unsigned char SECTIOND mmc_rgb[CB_CACHE];

#define MS_TIMEOUT 1000

#if defined (USE_MMC_BOOTSTRAP)

static void SECTION _memcpy (void* dest, const void* src, size_t cb)
{
  while (cb--)
    *(unsigned char*) dest++ = *(unsigned char*) src++;
}
#define memcpy _memcpy

static void SECTION _memset (void* pv, int v, size_t cb)
{
  while (cb--)
    *(unsigned char*) pv++ = v;
}
#undef memset
#define memset _memset

#endif

static inline void mmc_clear (void) {
  memset (&mmc, 0, sizeof (mmc));
  mmc.ib = -1; }

static void mmc_report (void);

static inline unsigned long response_ocr (void) {
  return (((unsigned long) mmc.response[1]) << 24)
    | (((unsigned long) mmc.response[2]) << 16)
    | (((unsigned long) mmc.response[3]) << 8)
    | (((unsigned long) mmc.response[4]) << 0);
}

/** return eight or fewer bits from the CSD register where the bits
    are present in one or two bytes read from the register. */

static uint8_t csd_byte (int bit, int length)
{
  uint16_t v = (   (mmc.csd[15 - bit/8    ]
                 | (mmc.csd[15 - bit/8 - 1] <<  8)) >> (bit%8)) & ((1<<length) - 1);
  return v;
}

static uint16_t csd_short (int bit, int length)
{
  uint32_t v = (   (mmc.csd[15 - bit/8    ]
                 | (mmc.csd[15 - bit/8 - 1] <<  8)
                 | (mmc.csd[15 - bit/8 - 2] << 16)) >> (bit%8)) & ((1<<length) - 1);
  return v;
}

#if defined (TALK)
static const char* report_status (unsigned long l)
{
  static char sz[256];

  sprintf (sz, "[%04lx", l);
  if (l & MMC_STATUS_ENDRESP)
    strcat (sz, " ENDRESP");
  if (l & MMC_STATUS_DONE)
    strcat (sz, " DONE");
  if (l & MMC_STATUS_TRANDONE)
    strcat (sz, " TRANDONE");
  if (l & MMC_STATUS_CLK_DIS)
    strcat (sz, " CLK_DIS");
  if (l & MMC_STATUS_FIFO_FULL)
    strcat (sz, " FIFO_FULL");
  if (l & MMC_STATUS_FIFO_EMPTY)
    strcat (sz, " FIFO_EMPTY");
  if (l & MMC_STATUS_CRC)
    strcat (sz, " CRCRESP");
  if (l & MMC_STATUS_CRCREAD)
    strcat (sz, " CRCREAD");
  if (l & MMC_STATUS_CRCWRITE)
    strcat (sz, " CRCWRITE");
  if (l & MMC_STATUS_TORES)
    strcat (sz, " TORES");
  if (l & MMC_STATUS_TOREAD)
    strcat (sz, " TOREAD");
  if (l & MMC_STATUS_TIMED_OUT)
    strcat (sz, " TIMEDOUT");
  strcat (sz, "]");
  return sz;
}
#endif

static void SECTION start_clock (void)
{
  int c;

  if (!(MMC_STATUS & MMC_STATUS_CLK_DIS)) {
    DBG (2, "%s: clock already running\n", __FUNCTION__);
    return;
  }

  MMC_CLKC = MMC_CLKC_START_CLK;

  for (c = 0xffff; c--; )
    if (!(MMC_STATUS & MMC_STATUS_CLK_DIS))
     break;

  if (c == 0)
    DBG (2, "%s: clock failed to start %s\n",
	 __FUNCTION__, report_status (MMC_STATUS));

  /* *** FIXME: may be good to implement a timeout check. */

//  while (MMC_STATUS & MMC_STATUS_CLK_DIS)
//    ;
}

static void SECTION stop_clock (void)
{
  if (MMC_STATUS & MMC_STATUS_CLK_DIS)
    return;

  MMC_CLKC = MMC_CLKC_STOP_CLK;

  /* *** FIXME: may be helpful to implement a timeout check.
     Interestingly, the Sharp implementation of this function doesn't
     have a timeout. */
  while (!(MMC_STATUS & MMC_STATUS_CLK_DIS))
    udelay (1);
}


/* clear_all

   clears the FIFOs, response and data, and the interrupt status.
*/

static void SECTION clear_all (void)
{
  unsigned long prediv = MMC_PREDIV;
  int i;

  MMC_PREDIV |= MMC_PREDIV_APB_RD_EN;
  while (!(MMC_STATUS & MMC_STATUS_FIFO_EMPTY))
    MMC_DATA_FIFO;

  MMC_PREDIV = prediv;

  for (i = 16; i--; )
    MMC_RES_FIFO;

  /* Clear interrupt status */
//  MMC_EOI = (1<<5)|(1<<2)|(1<<1)|(1<<0);

}

static void SECTION set_clock (int speed)
{
  if (speed < 1024*1024) {
    /* 312KHz */
    MASK_AND_SET (MMC_PREDIV,
		  MMC_PREDIV_MMC_PREDIV_MASK<<MMC_PREDIV_MMC_PREDIV_SHIFT,
		  5<<MMC_PREDIV_MMC_PREDIV_SHIFT);
    MMC_RATE = 6;
  }

  if (speed >= 1024*1024) {
    /* 20MHz */
    MASK_AND_SET (MMC_PREDIV,
		  MMC_PREDIV_MMC_PREDIV_MASK<<MMC_PREDIV_MMC_PREDIV_SHIFT,
		  5<<MMC_PREDIV_MMC_PREDIV_SHIFT);
    MMC_RATE = 0;

#if defined (USE_SLOW_CLOCK)
    /* 12MHz */
    MASK_AND_SET (MMC_PREDIV,
		  MMC_PREDIV_MMC_PREDIV_MASK<<MMC_PREDIV_MMC_PREDIV_SHIFT,
		  8<<MMC_PREDIV_MMC_PREDIV_SHIFT);
    MMC_RATE = 0;
#endif
#if defined (USE_WAYSLOW_CLOCK)
    /* 195KHz */
    MASK_AND_SET (MMC_PREDIV,
		  MMC_PREDIV_MMC_PREDIV_MASK<<MMC_PREDIV_MMC_PREDIV_SHIFT,
		  8<<MMC_PREDIV_MMC_PREDIV_SHIFT);
    MMC_RATE = 6;
#endif
  }
}

/* pull_response

   retrieves a command response.  The length is the length of the
   expected response, in bits.

*/

static void SECTION pull_response (int length)
{
  int i;
//  int c = 16;
  int c = (length + 7)/8;

  DBG (3, "%s: %d-%d  ", __FUNCTION__, length, c);

  for (i = 0; i < c; ) {
    unsigned short s = MMC_RES_FIFO;

#if defined (USE_BIGENDIAN_RESPONSE)
    mmc.response[i++] = (s >> 8) & 0xff;
    if (i < c)
      mmc.response[i++] = s & 0xff;
#else
    mmc.response[i++] = s & 0xff;
    if (i < c)
      mmc.response[i++] = (s >> 8) & 0xff;
#endif
    DBG (3, " %02x %02x", mmc.response[i-2], mmc.response[i-1]);
  }

  DBG (3, "\n");
}

static void SECTION pull_data (void* pv)
{
  int i;
  //  ENTRY (3);
  //  DBG (3, "%s: %p %p\n", __FUNCTION__, pv, mmc_rgb);
  for (i = 0; i < MMC_SECTOR_SIZE/2; ++i) {
    unsigned short v = MMC_DATA_FIFO;
    *(unsigned char*) pv++ = (v >> 8) & 0xff;
    *(unsigned char*) pv++ = v & 0xff;
  }
}

static void SECTION push_data (const void* pv, size_t cb)
{
  int i;
  size_t remainder = MMC_SECTOR_SIZE - cb;
  //  ENTRY (3);
  //  DBG (3, "%s: %p %p\n", __FUNCTION__, pv, mmc_rgb);
  for (i = (cb + 1)/2; i--; ) {
    unsigned short v = (((unsigned char*)pv)[0] << 8)
      | ((unsigned char*)pv)[1];
    MMC_DATA_FIFO = v;
    pv += 2;
  }
  for (i = remainder/2; i--; ) {
    MMC_DATA_FIFO = 0;
    pv += 2;
  }
}

static unsigned long SECTION wait_for_completion (short bits)
{
  unsigned short status = 0;
#if defined (TALK) && TALK > 0
  unsigned short status_last = 0;
#endif
#if !defined (USE_MMC_BOOTSTRAP)
  unsigned long time_start = timer_read ();
#else
  int timeout_count = 100000;
#endif
  int timed_out = 0;

  DBG (3, " (%x) ", bits);
  do {
    udelay (1);
    status = MMC_STATUS;
#if defined (TALK) && TALK > 0
    if (status != status_last)
      DBG (3, " %04x", status);
    status_last = status;
#endif

#if !defined (USE_MMC_BOOTSTRAP)
    if (timer_delta (time_start, timer_read ()) >= MS_TIMEOUT) {
      printf ("\nbailing at timeout status 0x%x bits 0x%x\n", status, bits);
      timed_out = 1;
    }
#else
    if (timeout_count-- <= 0)
      timed_out = 1;
#endif
  } while ((status
	    & ( bits
//  MMC_STATUS_ENDRESP
//	       | MMC_STATUS_DONE
//	       | MMC_STATUS_TRANDONE
//	       | MMC_STATUS_TORES
//	       | MMC_STATUS_TOREAD
//	       | MMC_STATUS_CRC
//	       | MMC_STATUS_CRCREAD
		 ))
	   == 0 && !timed_out);
  DBG (3, " => %s %lx\n", report_status (status), MMC_INT_STATUS);
  stop_clock ();

  if (timed_out)
    DBG (1, " timedout => %s %lx\n", report_status (status), MMC_INT_STATUS);

  return status | (timed_out ? MMC_STATUS_TIMED_OUT : 0);
}

#if defined (TALK) && ! defined (USE_MMC_BOOTSTRAP)
static void talk_command (void)
{
  DBG (2, "cmd 0x%02lx (%03ld) arg 0x%08lx  cmdcon 0x%04lx"
       "  rate 0x%02lx/%04ld",
       MMC_CMD, MMC_CMD, MMC_ARGUMENT, MMC_CMDCON, MMC_PREDIV, MMC_RATE);
}
#else
# define talk_command(v)
#endif

static int SECTION execute_command (unsigned long cmd,
				    unsigned long arg,
				    unsigned short wait_status)
{
  int state = (cmd & CMD_BIT_APP) ? 99 : 0;
  int status = 0;

  if (!wait_status)
    wait_status = ((cmd >> CMD_SHIFT_RESP) & CMD_MASK_RESP)
		   ? MMC_STATUS_ENDRESP : MMC_STATUS_TRANDONE;

 top:
  stop_clock ();

  set_clock ((cmd & CMD_BIT_LS) ? 400*1024 : 20*1024*1024);

  if (status)
    DBG (3, "%s: state %d s 0x%x\n", __FUNCTION__, state, status);

  switch (state) {

  case 0:			/* Execute command */
    MMC_CMD = ((cmd & CMD_MASK_CMD) >> CMD_SHIFT_CMD);
    MMC_ARGUMENT = arg;
    MMC_CMDCON
      = ((cmd >> CMD_SHIFT_RESP) & CMD_MASK_RESP)
      | ((cmd & CMD_BIT_INIT)  ? MMC_CMDCON_INITIALIZE : 0)
      | ((cmd & CMD_BIT_BUSY)  ? MMC_CMDCON_BUSY       : 0)
      | ((cmd & CMD_BIT_DATA)  ? (MMC_CMDCON_DATA_EN
//				  | MMC_CMDCON_BIG_ENDIAN
#if defined (USE_WIDE)
				  | mmc.cmdcon_sd
#endif
				  ) : 0)
      | ((cmd & CMD_BIT_WRITE)  ? MMC_CMDCON_WRITE     : 0)
      | ((cmd & CMD_BIT_STREAM)	? MMC_CMDCON_STREAM    : 0)
      ;
    ++state;
    break;

  case 1:
    return status;

  case 99:			/* APP prefix */
    MMC_CMD = MMC_APP_CMD;
    MMC_ARGUMENT = mmc.rca<<16;
    MMC_CMDCON = 0
      | MMC_CMDCON_RESPONSE_R1	/* Response is status */
      | ((cmd & CMD_BIT_INIT) ? MMC_CMDCON_INITIALIZE : 0);
    state = 0;
    break;
  }

  clear_all ();
  talk_command ();
  start_clock ();
  status = wait_for_completion (wait_status);

  /* We return an error if there is a timeout, even if we've fetched a
     response */
  if (status & MMC_STATUS_TORES)
    return status;

  if (status & MMC_STATUS_ENDRESP)
    switch (MMC_CMDCON & 0x3) {
    case 0:
      break;
    case 1:
    case 3:
      pull_response (48);
      break;
    case 2:
      pull_response (136);
      break;
    }

  goto top;
}


/* mmc_acquire

   detects cards on the bus and initializes them for IO.  It can
   detect both SD and MMC card types.

   It will only detect a single card and the first one will be the one
   that is configured with an RCA and will be used by the driver.

*/

void SECTION mmc_acquire (void)
{
  int status;
  int tries = 0;
  unsigned long ocr = 0;
  unsigned long r;
  int state = 0;
  unsigned long command = 0;
  int cmdcon_sd = MMC_CMDCON_WIDE;

  mmc_clear ();

  stop_clock ();

//  PUTC_LL('I');
  status = execute_command (CMD_IDLE, 0, MMC_STATUS_ENDRESP);
//  PUTC_LL('i');

  while (state < 100) {
//    PUTHEX_LL (state);
//    PUTC_LL (':');

    /* It would be great if I could use a switch statement here,
       especially because the compile is very efficient about
       converting it to a table.  However, the absolute PCs in the
       table make that impossible when we use this code in the
       bootstrap.  So, we're left with a cascading sequence of if's.
       *** FIXME: can we use -fpic?
    */

//    switch (state) {

//    case 0:			/* Setup for SD */
    if (state == 0) {
      mmc.acquired = 0;
      command = CMD_SD_OP_COND;
      tries = 10;		/* *** We're not sure we need to wait
				   for the READY bit to be clear, but
				   it should be in any case. */
      ++state;
//      break;
    }

    else if (state == 10) {
//    case 10:			/* Setup for MMC */
      command = CMD_MMC_OP_COND;
      tries = 10;
      cmdcon_sd = 0;
      ++state;
//      break;
    }

    else if (state == 1 || state == 11) {
//    case 1:
//    case 11:
      status = execute_command (command, 0, 0);
      if (status & MMC_STATUS_TORES)
	state += 8;		/* Mode unavailable */
      else
	++state;
//      break;
    }

    else if (state == 2 || state == 12) {
//    case 2:			/* Initial OCR check  */
//    case 12:
      ocr = response_ocr ();
      if (ocr & OCR_ALL_READY)
	++state;
      else
	state += 2;
//      break;
    }

    else if (state == 3 || state == 13) {
//    case 3:			/* Initial wait for OCR clear */
//    case 13:
      while ((ocr & OCR_ALL_READY) && --tries > 0) {
	mdelay (MS_ACQUIRE_DELAY);
	status = execute_command (command, 0, 0);
	ocr = response_ocr ();
      }
      if (ocr & OCR_ALL_READY)
	state += 6;
      else
	++state;
//      break;
    }

    else if (state == 4 || state == 14) {
//    case 4:			/* Assign OCR */
//    case 14:
      tries = 200;
      ocr &= 0x00ff8000;	/* Mask for the bits we care about */
      do {
	mdelay (MS_ACQUIRE_DELAY);
	mmc.acquire_time += MS_ACQUIRE_DELAY;
	status = execute_command (command, ocr, 0);
	r = response_ocr ();
      } while (!(r & OCR_ALL_READY) && --tries > 0);
      if (r & OCR_ALL_READY)
	++state;
      else
	state += 5;
//      break;
    }

    else if (state == 5 || state == 15) {
//    case 5:			/* CID polling */
//    case 15:
      mmc.cmdcon_sd = cmdcon_sd;
      status = execute_command (CMD_ALL_SEND_CID, 0, 0);
      memcpy (mmc.cid, mmc.response + 1, 16);
      ++state;
//      break;
    }

    else if (state == 6) {
//    case 6:			/* RCA send */
      status = execute_command (CMD_SD_SEND_RCA, 0, 0);
      mmc.rca = (mmc.response[1] << 8) | mmc.response[2];
      ++state;
//      break;
    }

    else if (state == 16) {
//    case 16:			/* RCA assignment */
      mmc.rca = 1;
      status = execute_command (CMD_MMC_SET_RCA, mmc.rca << 16, 0);
      ++state;
//      break;
    }

    else if (state == 7 || state == 17) {
//    case 7:
//    case 17:
      status = execute_command (CMD_SEND_CSD, mmc.rca << 16, 0);
      memcpy (mmc.csd, mmc.response + 1, 16);
      mmc.acquired = 1;
      state = 100;
//      break;
    }

    else if (state == 9) {
//    case 9:
      ++state;			/* Continue with MMC */
//      break;
    }

    else if (state == 19 || state == 20) {
//    case 19:
//    case 20:
      state = 999;
//      break;			/* No cards */
    }
    else
      break;
  }

#if defined (TALK)
  printf ("cid\n");
  dump (mmc.cid, 16, 0);
  printf ("csd\n");
  dump (mmc.csd, 16, 0);
#endif

  if (mmc_card_acquired ()) {
    PUTC_LL ('A');
    mmc.c_size      = C_SIZE;
    mmc.c_size_mult = C_SIZE_MULT;
    mmc.read_bl_len = READ_BL_LEN;
    mmc.mult = 1<<(mmc.c_size_mult + 2);
    mmc.block_len = 1<<mmc.read_bl_len;
    mmc.blocknr = (mmc.c_size + 1)*mmc.mult;
    mmc.device_size = mmc.blocknr*mmc.block_len;
  }
}

void SECTION mmc_init (void)
{
//  ENTRY ();

#if defined (MACH_TROUNCER)
  GPIO_PFDD |= (1<<6);		/* Enable card detect interrupt pin */
#endif

  MMC_PREDIV = 0;
#if defined (ARCH_LH7A400)
  MMC_SPI = 0;
  MMC_BUF_PART_FULL = 0;
#endif

  DBG (2, "%s: enabling MMC\n", __FUNCTION__);

  MMC_PREDIV = MMC_PREDIV_MMC_EN | MMC_PREDIV_APB_RD_EN
    | (1<<MMC_PREDIV_MMC_PREDIV_SHIFT);
  MMC_NOB = 1;
  MMC_INT_MASK = 0;		/* Mask all interrupts */
  MMC_EOI = 0x27;		/* Clear all interrupts */

  mmc_clear ();

  mmc_acquire ();
}

static void mmc_report (void)
{
#if defined (MACH_TROUNCER)
  printf ("  mmc:    %s\n", (GPIO_PFD & (1<<6)) ? "no card" : "card present");
#endif
  printf ("  mmc:    %s card acquired",
	  mmc_card_acquired () ? (mmc.cmdcon_sd ? "sd" : "mmc") : "no");
  if (mmc_card_acquired ()) {
    printf (", rca 0x%x (%d ms)", mmc.rca, mmc.acquire_time);
    printf (", %ld.%02ld MiB\n",
	    mmc.device_size/(1024*1024),
	    (((mmc.device_size/1024)%1024)*100)/1024);
    printf ("          %d B sectors, %d sectors/block, C_SIZE %d, C_SIZE_MULT %d\n",
            1<<READ_BL_LEN, SECTOR_SIZE + 1,
            mmc.c_size, mmc.c_size_mult);
    printf ("          TAAC 0x%x  NSAC %d  TRAN_SPEED 0x%x\n",
            TAAC, NSAC, TRAN_SPEED);
//    dump (mmc.cid, 16, 0);
//    dump (mmc.csd, 16, 0);
//    printf ("\n");
  }
  else
    printf ("\n");
}

static int mmc_open (struct descriptor_d* d)
{
  DBG (2,"%s: opened %d %d\n", __FUNCTION__, d->start, d->length);

  if (!mmc_card_acquired ())
    ERROR_RETURN (ERROR_IOFAILURE, "no card");

  return 0;
}


/* mmc_read

   performs the read of data from the SD/MMC card.  It handles
   unaligned, and sub-block length I/O.

*/

ssize_t SECTION mmc_read (struct descriptor_d* d, void* pv, size_t cb)
{
  ssize_t cbRead = 0;
  int status;

#if 0
  PUTHEX_LL (d->index);
  PUTC_LL ('/');
  PUTHEX_LL (d->start);
  PUTC_LL ('/');
  PUTHEX_LL (d->length);
  PUTC_LL ('/');
  PUTHEX_LL (cb);
  PUTC_LL ('\r');
  PUTC_LL ('\n');
#endif

  if (d->index + cb > d->length)
    cb = d->length - d->index;

  //  DBG (1, "%s: %ld %ld %d; %ld\n",
  //       __FUNCTION__, d->start + d->index, d->length, cb, mmc.ib);

#if 0
 {
   unsigned long sp;
   __asm volatile ("mov %0, sp\n\t"
		   : "=r" (sp));
   PUTHEX_LL (sp);
 }
#endif

  while (cb) {
    unsigned long index = d->start + d->index;
    int availableMax = CB_CACHE - (index & (CB_CACHE - 1));
    int available = cb;

#if 0
    PUTC_LL ('X');
    PUTHEX_LL (index);
    PUTC_LL ('/');
    PUTHEX_LL (cbRead);
    PUTC_LL ('\r');
    PUTC_LL ('\n');
#endif

    if (available > availableMax)
      available = availableMax;

    //    DBG (1, "%ld %ld\n", mmc.ib, index);

    if (mmc.ib == -1
	|| mmc.ib >= index  + CB_CACHE
	|| index  >= mmc.ib + CB_CACHE) {

      mmc.ib = index & ~(CB_CACHE - 1);

      DBG (1, "%s: read av %d  avM %d  ind %ld  cb %d\n", __FUNCTION__,
	   available, availableMax, index, cb);

      status = execute_command (CMD_DESELECT_CARD, 0,
				MMC_STATUS_ENDRESP | MMC_STATUS_TRANDONE);
      status = execute_command (CMD_SELECT_CARD, mmc.rca<<16, 0);

#if defined (USE_WIDE)
      if (mmc.cmdcon_sd)
	execute_command (CMD_SD_SET_WIDTH, 2, 0);	/* SD, 4 bit width */
#endif
      status = execute_command (CMD_SET_BLOCKLEN, MMC_SECTOR_SIZE, 0);
      MMC_BLK_LEN = MMC_SECTOR_SIZE;
      MMC_NOB = CB_CACHE/MMC_SECTOR_SIZE;

		/* Fill the sector cache */
      {
	int i = 0;
	for (i = 0; i < CB_CACHE/MMC_SECTOR_SIZE; ++i) {
	  if (i == 0) {
	    DBG (3, "%s: first sector\n", __FUNCTION__);
	    status = execute_command (
#if defined (USE_MULTIBLOCK_READ)
				 CMD_READ_MULTIPLE
#else
				 CMD_READ_SINGLE
#endif
				 , mmc.ib,
				 MMC_STATUS_TRANDONE
				 | MMC_STATUS_FIFO_FULL
				 | MMC_STATUS_TOREAD);
	  }
	  else {
	    DBG (3, "%s: next sector\n", __FUNCTION__);
	    start_clock ();
	    status = wait_for_completion (  MMC_STATUS_TRANDONE
					  | MMC_STATUS_FIFO_FULL
					  | MMC_STATUS_TOREAD);
	  }

	  if (status & MMC_STATUS_FIFO_FULL)
	    pull_data (mmc_rgb + i*MMC_SECTOR_SIZE);
	  else if (status & (MMC_STATUS_TOREAD | MMC_STATUS_TIMED_OUT)) {
	    printf ("\nstatus %x\n", status);
	    ERROR_RETURN (ERROR_TIMEOUT, "timeout on read");
	  }
#if defined (USE_MULTIBLOCK_READ)
	  status = execute_command (CMD_STOP, 0, MMC_STATUS_DONE);
#endif
	}
      }

//      status = wait_for_completion (MMC_STATUS_TRANDONE);
//      printf ("\nstatus %x\n", status);
//      if (status & MMC_STATUS_TIMED_OUT)
//	ERROR_RETURN (ERROR_TIMEOUT, "timeout on trandone");
    }

    memcpy (pv, mmc_rgb + (index & (CB_CACHE - 1)), available);
    d->index += available;
    cb -= available;
    cbRead += available;
    pv += available;
  }

  return cbRead;
}


/* mmc_write

   performs the write of data from the SD/MMC card.

//   It handles unaligned, and sub-block length I/O.

*/

ssize_t SECTION mmc_write (struct descriptor_d* d, const void* pv, size_t cb)
{
  ssize_t cbWrote = 0;
  int status;

#if 1
  PUTHEX_LL (d->index);
  PUTC_LL ('/');
  PUTHEX_LL (d->start);
  PUTC_LL ('/');
  PUTHEX_LL (d->length);
  PUTC_LL ('/');
  PUTHEX_LL (cb);
  PUTC_LL ('\r');
  PUTC_LL ('\n');
#endif

  if (d->index + cb > d->length)
    cb = d->length - d->index;

  //  DBG (1, "%s: %ld %ld %d; %ld\n",
  //       __FUNCTION__, d->start + d->index, d->length, cb, mmc.ib);

 {
   unsigned long sp;
   __asm volatile ("mov %0, sp\n\t"
		   : "=r" (sp));
   PUTHEX_LL (sp);
 }

  while (cb) {
    unsigned long index = d->start + d->index;
    int availableMax = MMC_SECTOR_SIZE
      - (index & (MMC_SECTOR_SIZE - 1)); /* Must not overlap a block */
    int available = cb;

#if 0
    PUTC_LL ('X');
    PUTHEX_LL (index);
    PUTC_LL ('/');
    PUTHEX_LL (cbRead);
    PUTC_LL ('\r');
    PUTC_LL ('\n');
#endif

    if (available > availableMax)
      available = availableMax;

    //    DBG (1, "%ld %ld\n", mmc.ib, index);

    DBG (1, "%s: write av %d  avM %d  ind %ld  cb %d\n", __FUNCTION__,
	 available, availableMax, index, cb);

    status = execute_command (CMD_DESELECT_CARD, 0,
			      MMC_STATUS_ENDRESP | MMC_STATUS_TRANDONE);
    status = execute_command (CMD_SELECT_CARD, mmc.rca<<16, 0);

#if defined (USE_WIDE)
    if (mmc.cmdcon_sd)
      execute_command (CMD_SD_SET_WIDTH, 2, 0);	/* SD, 4 bit width */
#endif
    status = execute_command (CMD_SET_BLOCKLEN, available, 0);
    MMC_BLK_LEN = available;
    MMC_NOB = 1;

    status = execute_command (CMD_WRITE_SINGLE, index,
			      MMC_STATUS_TORES | MMC_STATUS_FIFO_EMPTY);
    DBG (2, "%s: sending waiting to write %x\n", __FUNCTION__, status);

//    status = wait_for_completion (MMC_STATUS_FIFO_EMPTY);
    push_data (pv, available);
    start_clock ();
    DBG (2, "%s: restarted %s\n", __FUNCTION__, report_status (MMC_STATUS));
//    status = execute_command (CMD_STOP, 0, MMC_STATUS_TRANDONE);
    status = wait_for_completion (MMC_STATUS_TRANDONE);
    DBG (2, "%s: end %s\n", __FUNCTION__, report_status (status));

    d->index += available;
    cb -= available;
    cbWrote += available;
    pv += available;
  }

  return cbWrote;
}

static __driver_5 struct driver_d mmc_driver = {
  .name		= "mmc-lh7a404",
  .description	= "MMC/SD card driver",
  .flags	= DRIVER_READPROGRESS(2),
  .open		= mmc_open,
  .close	= close_helper,
  .read		= mmc_read,
  .write	= mmc_write,
  .seek		= seek_helper,
};

static __service_6 struct service_d mmc_service = {
  .init		= mmc_init,
#if !defined (CONFIG_SMALL)
  .report	= mmc_report,
#endif
};

static int cmd_mmc (int argc, const char** argv)
{
#if defined (MACH_TROUNCER)
  printf ("%s: card %s\n", __FUNCTION__,
	  (GPIO_PFD & (1<<6)) ? "not inserted" : "inserted");
#endif

  mmc_init ();

  mmc_acquire ();		/* *** FIXME: the read and report code
				   should do this */

  return 0;
}

static __command struct command_d c_mmc = {
  .command = "mmc",
  .func = cmd_mmc,
  COMMAND_DESCRIPTION ("test MMC controller")
};
