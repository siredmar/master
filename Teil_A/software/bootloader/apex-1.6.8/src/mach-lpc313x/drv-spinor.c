/* drv-spinor.c

written by Durgesh Pattamatta
19 Oct 2005

Copyright (C) 2008 NXP Semiconductors

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
#include <driver.h>
#include <service.h>
#include <linux/string.h>
#include <spinner.h>
#include <error.h>
#include <command.h>
#include <mach/hardware.h>

//#define TALK 1
#undef TALK 

#if defined (TALK)
#define PRINTF(f...)    printf (f)
#define ENTRY(l)    printf ("%s\n", __FUNCTION__)
#define DBG(l,f...)   do { if (TALK >= l) printf (f); } while (0)
#else
#define PRINTF(f...)    do {} while (0)
#define ENTRY(l)    do {} while (0)
#define DBG(l,f...)   do {} while (0)
#endif


/* SPI flash command defines */
#define CMD_RDID        0x9F        /* read device ID */
#define CMD_RDSR        0xD7        /* read status register */
#define CMD_FAST_READ   0x0B        /* read data (extra command setup time allows higher SPI clock) */
#define CMD_SE          0x7C        /* sector erase */
#define CMD_BE          0x50        /* block erase */
#define CMD_PE          0x81        /* page erase */
#define CMD_PP          0x82        /* page program */
#define CMD_DP          0xB9        /* deep power down */
#define CMD_RES         0xAB        /* release from deep power down */

#define MANFID_ATMEL    0x1F
#define DUMMY_BYTE		  0xFF

/* status register defines */
#define STATUS_RDY      (1 << 7)
#define STATUS_COMP     (1 << 6)
#define STATUS_PROTECT  (1 << 1)
#define STATUS_P512     (1 << 0)


typedef struct _SPINOR_INFO_T {
  u8 id[4];
  u16 total_pages;
  u16 page_size;
  u32 clock;
  u16 cs;
  u16 cs_pin;
  u32 cs_port;
} SPINOR_INFO_T;

/* forward declarations */

/*global variables */
static SPINOR_INFO_T* chip;
static const SPINOR_INFO_T chips[] = 
{
  /* Micron MT*/
#if defined (CONFIG_SPINOR_AT45DB)
  {
    .id = {0x1F, 0x27, 1, 0},
    .page_size = 512,
    .total_pages = 8192,
    .clock = 1000000,
    .cs = 0,
    .cs_pin = 4,
    .cs_port = IOCONF_SPI, 
  },
#endif
};


void spinor_cs_low(void)
{
  //GPIO_OUT_LOW(IOCONF_SPI, _BIT(4));
  GPIO_OUT_LOW(chip->cs_port, _BIT(chip->cs_pin));
}

void spinor_cs_high(void)
{
  udelay(10);
  //GPIO_OUT_HIGH(IOCONF_SPI, _BIT(4));
  GPIO_OUT_HIGH(chip->cs_port, _BIT(chip->cs_pin));
}
/*
Perform a single low level 8bit SPI read
*/
u32 spi_read_single8 (void)
{
  u32 temp;

  /* Flush all of the RxFifo */
  while ((SPI_STS_REG & SPI_ST_RX_EMPTY) == 0) {
    temp = SPI_FIFO_DATA_REG;
  }
  /* Write dummy word to generate clock cycles */
  SPI_FIFO_DATA_REG = 0;
  /* Please don't combine following two statements. They are intentionaly in sequence*/
  /* Wait until data read */
  while (SPI_STS_REG & SPI_ST_RX_EMPTY);
  /* Wait while spi activity*/
  while (SPI_STS_REG  & SPI_ST_BUSY); 
  /* Read the byte */
  temp = SPI_FIFO_DATA_REG;

  return temp;
}

/*
Perform a single low level 8bit SPI write
*/
u32 spi_write_single8 (u32 value)
{
  /* Write the word to be sent*/
  SPI_FIFO_DATA_REG = value;
  /* Please don't combine following two statements. They are intentionaly in sequence*/
  /* Wait until data transmitted*/
  while ((SPI_STS_REG & SPI_ST_TX_EMPTY) == 0);
  /* Wait while spi activity*/
  while (SPI_STS_REG  & SPI_ST_BUSY); 
  return 0;
}
static u32 spi_read_fifosize (u8 *dst)
{
  u32 temp;
  int count; 

  SPI_SLV_SET2_REG(chip->cs) = SPI_SLV2_WD_SZ(16-1);

  /* Flush all of the RxFifo */
  while ((SPI_STS_REG & SPI_ST_RX_EMPTY) == 0) {
    temp = SPI_FIFO_DATA_REG;
  }

  /* Write dummy word SPI_FIFO_DEPTH times to generate clock cycles for 
  SPI_FIFO_DEPTH 16-b words */
  temp = 0;
  for (count = 0; count < SPI_FIFO_DEPTH; count++ ) {
    SPI_FIFO_DATA_REG = temp;
  }

  /* Wait until data read*/
  while ((SPI_STS_REG & SPI_ST_TX_EMPTY) == 0);
  /* Wait while spi activity*/
  while (SPI_STS_REG  & SPI_ST_BUSY); 

  /* Read the data */
  for (count = 0; count < SPI_FIFO_DEPTH; count++ )
  {
    /* attention: following is valid under assumption that we always have msb first. 
    If that changes, following must change.
    */
    temp = SPI_FIFO_DATA_REG;
    dst[(count << 1) + 0] = (u8) ((temp >> 8) & 0xFF);   // even bytes are input first, so they are high part of the words
    dst[(count << 1) + 1] = (u8) ((temp >> 0) & 0xFF);   // odd bytes are input second, so they are low part of the words
  }

  SPI_SLV_SET2_REG(chip->cs) = SPI_SLV2_WD_SZ(8-1);

  return 0;
}

/*
Read 'count' bytes from SPI into 'dst' buffer.
*/
u32 spi_read_multi (u8 *dst, u32 count)
{
  int i = 0;

  while ((count-i) >= (SPI_FIFO_DEPTH<<1))
  {
    spi_read_fifosize (&dst[i]);
    i+= (SPI_FIFO_DEPTH<<1);
  }

  /* Crude and simple read of remaining bytes...*/

  for (; i < count; i++)
    dst[i] = (u8) spi_read_single8();

  return 0;
}

static u32 spi_write_fifosize (u8 *src)
{
  u32 temp;
  int count; 

  SPI_SLV_SET2_REG(chip->cs) = SPI_SLV2_WD_SZ(16-1);

  /* Write data in form of SPI_FIFO_DEPTH 16-b words  */
  for (count = 0; count < SPI_FIFO_DEPTH; count++ ) {
    /* attention: following is valid under assumption that we always have msb first. 
    If that changes, following must change */
    temp = src[count<<1];      /* even bytes go first */
    temp <<= 8;                /* so they are high part of 16-b word */
    temp |= src[(count<<1)+1]; /* then odd bytes */

    SPI_FIFO_DATA_REG = temp;
  }

  /* Wait until data read*/
  while ((SPI_STS_REG & SPI_ST_TX_EMPTY) == 0);
  /* Wait while spi activity*/
  while (SPI_STS_REG  & SPI_ST_BUSY); 

  SPI_SLV_SET2_REG(chip->cs) = SPI_SLV2_WD_SZ(8-1);

  return 0;
}


/*
Write 'count' bytes from 'src' buffer to SPI.
*/
u32 spi_write_multi (u8 *src, u32 count)
{
  int i = 0;

  while ((count - i) >= (SPI_FIFO_DEPTH << 1)) {
    spi_write_fifosize (&src[i]);
    i += (SPI_FIFO_DEPTH << 1);
  }

  /* Crude and simple write of remaining bytes */

  for (; i < count; i++)
    spi_write_single8 (src[i]);

  return 0;
}


static int spinor_read_device_id (u8 *device_id)
{
  spinor_cs_low();
  spi_write_single8 (CMD_RDID);
  spi_read_multi (device_id, 3);  /* 3 bytes: manufacturer id, device type, device capacity */
  spinor_cs_high();
  return 0;
}

static int spinor_simple_command (u8 command_byte)
{
  spinor_cs_low();
  spi_write_single8 (command_byte);
  /*
  Note that the above write _must_ have completed (ie sent to
  SPI nor device and not stuck in a write fifo somewhere) before
  CS is de-asserted. 
  */
  spinor_cs_high();
  return 0;
}

static int spinor_enter_deep_power_down (void)
{
  return spinor_simple_command (CMD_DP);
  return 0;
}

static int spinor_exit_deep_power_down (void)
{
  spinor_simple_command (CMD_RES);
  /* max tEDPD in Atmel AT45DB161D-SU datasheet is given as 35 usec */
  udelay (50);                    
  return 0;
}



static int spinor_read_status_register (u32 *status)
{
  spinor_cs_low();
  spi_write_single8 (CMD_RDSR);
  *status = spi_read_single8();
  spinor_cs_high();
  return 0;
}

static int spinor_poll_is_busy (void)
{
  u32 status;

  spinor_cs_low();
  spi_write_single8 (CMD_RDSR);

  while (1) {
    udelay (10);
    status = spi_read_single8();
    if (status & STATUS_RDY)
      break;
  }

  spinor_cs_high();

  return 0;
}

static int spinor_page_erase (u32 offset)
{
  spinor_cs_low();
  spi_write_single8 (CMD_PE);
  spi_write_single8 ((offset >> 16) & 0xFF);  /* 24bit address upper byte */
  spi_write_single8 ((offset >>  8) & 0xFF);  /* 24bit address middle byte */
  spi_write_single8 ((offset >>  0) & 0xFF);  /* 24bit address least significant byte */
  /*
  Note that the above write _must_ have completed (ie sent to
  SPI nor device and not stuck in a write fifo somewhere) before
  CS is de-asserted. We rely on the SPI driver to take of this !!
  */
  spinor_cs_high();

  spinor_poll_is_busy();
  return 0;
}

static int spinor_page_program (u32 offset, u8 *src)
{
  spinor_cs_low();
  spi_write_single8 (CMD_PP);
  spi_write_single8 ((offset >> 16) & 0xFF);  /* 24bit address upper byte */
  spi_write_single8 ((offset >>  8) & 0xFF);  /* 24bit address middle byte */
  spi_write_single8 ((offset >>  0) & 0xFF);  /* 24bit address least significant byte */
  spi_write_multi (src, chip->page_size);
  /*
  Note that the above write _must_ have completed (ie sent to
  SPI nor device and not stuck in a write fifo somewhere) before
  CS is de-asserted. We rely on the SPI driver to take of this !!
  */
  spinor_cs_high();

  spinor_poll_is_busy();
  return 0;
}

static int spi_get_clock_div(u32 target_clock,
                                u32 *pDivSet)
{
  u32 div, spi_clk, ps, div1;
  int retvalue = -1;

  /* The SPI clock is derived from the (main system PLL / 2),
     so compute the best divider from that clock */
  spi_clk = cgu_get_clk_freq(CGU_SB_SPI_CLK_ID);

  /* Find closest divider to get at or under the target frequency.
     Use smallest prescaler possible and rely on the divider to get
     the closest target frequency */
  div = (spi_clk + target_clock / 2) / target_clock;

  if ((div < SPI_MAX_DIVIDER) && (div > SPI_MIN_DIVIDER))
  {
    ps = (((div - 1) / 512) + 1) * 2;
    div1 = ((((div + ps / 2) / ps) - 1));

    /* write the divider settings */
    *pDivSet = SPI_SLV1_CLK_PS(ps) | SPI_SLV1_CLK_DIV1((div1));

    retvalue = 0;
  }
  DBG(1, "SPI clocks - base:%d req:%d div:0x%08x \n", 
         spi_clk, target_clock, *pDivSet);

  return retvalue;
}


static int spinor_open (struct descriptor_d* d)
{
  u32 status;
  if (!chip)
    return -1;

#if defined (CONFIG_SPINOR_AT45DB)
  /* check status if 528 byte device program it to be a 512 byte device */
  spinor_read_status_register(&status);
  if ((status & STATUS_P512) == 0) {
    /* program device to 512 mode. needs a power cycle after this.*/
    spinor_cs_low();
    spi_write_single8 (0x3D);
    spi_write_single8 (0x2A);
    spi_write_single8 (0x80);
    spi_write_single8 (0xA6);
    spinor_cs_high();
    printf (" Power cycle the board for 512 byte page size switch to be affective. \n");
    chip = NULL;
  }
#endif

  return 0;
}

static ssize_t spinor_read (struct descriptor_d* d, void* pv, size_t cb)
{
  ssize_t cbRead = 0;
  int index = (d->start + d->index);

  if (!chip)
    return cbRead;

  if ((d->index + cb) > d->length)
    cb = d->length - d->index;

  spinor_exit_deep_power_down();

  /* do read setup */
  spinor_cs_low();
  spi_write_single8 (CMD_FAST_READ);          /* FAST_READ is safe at all supported SPI speeds... */
  spi_write_single8 ((index >> 16) & 0xFF);  /* 24bit address upper byte */
  spi_write_single8 ((index >>  8) & 0xFF);  /* 24bit address middle byte */
  spi_write_single8 ((index >>  0) & 0xFF);  /* 24bit address least significant byte */
  spi_write_single8 (0);                      /* dummy byte */
  spi_read_multi (pv, cb);            /* data */
  spinor_cs_high();
  d->index += cb;

  spinor_enter_deep_power_down();

  return cb;
}

static ssize_t spinor_write (struct descriptor_d* d, const void* pv, size_t cb)
{
  int cbWrote = 0;

  if (!chip)
    return cbWrote;

  /* this driver always writes on page (512) boundaries only */
  if ((d->start + d->index) & (chip->page_size - 1)) {
    printf("spinor: unaligned write 0x%08x FAILED!!\n", d->start + d->index);
    return 0;
  }
  if ((d->index + cb) > d->length)
    cb = d->length - d->index;

  spinor_exit_deep_power_down();
  while (cb) {
    int available = chip->page_size;

    if (available > cb)
      available = cb;

    DBG(2, "spinor write page:0x%08x pv:0x%08x\n", 
        (d->start + d->index)/chip->page_size, (u32)(pv + cbWrote));

    spinor_page_program(d->start + d->index, (u8*)((u32)pv + cbWrote));
    d->index += available;
    cb -= available;
    cbWrote += available;
  }
  spinor_enter_deep_power_down();

  return cbWrote;
}


static void spinor_erase (struct descriptor_d* d, size_t cb)
{
  if (!chip)
    return;

  /* this driver always writes on page (512) boundaries only */
  if ((d->start + d->index) & (chip->page_size - 1)) {
    printf("spinor: unaligned write 0x%08x FAILED!!\n", d->start + d->index);
    return;
  }
  if ((d->index + cb) > d->length)
    cb = d->length - d->index;

  spinor_exit_deep_power_down();
  while (cb) {
    int available = chip->page_size;

    if (available > cb)
      available = cb;

    DBG(1, "spinor page erase:0x%08x \n",
      (d->start + d->index)/chip->page_size);
    spinor_page_erase(d->start + d->index);
    d->index += available;
    cb -= available;
  }
  spinor_enter_deep_power_down();
}

#if !defined (CONFIG_SMALL)
static void spinor_report (void)
{
  u32 status;

  if (!chip)
    return;

  spinor_read_status_register(&status);

  printf ("  spinor:   %d MiB total, %d B page %s%s%s%s\n",
    (chip->page_size * chip->total_pages)/(1024*1024), 
    chip->page_size,
    (status & STATUS_RDY) ? " RDY" : "BSY",
    (status & STATUS_COMP) ? " FAIL" : "",
    (status & STATUS_PROTECT) ? " R/O" : " R/W",
    (status & STATUS_P512) ? " 512" : " 528"
    );
}
#endif

static __driver_3 struct driver_d spinor_driver = {
  .name = "spinor",
  .description = "LPC SPI flash driver",
  .flags = DRIVER_WRITEPROGRESS(6),
  .open = spinor_open,
  .close = close_helper,
  .read = spinor_read,
  .write = spinor_write,
  .erase = spinor_erase,
  .seek = seek_helper,
};

/* spinor_init

probes the NAND flash device.

Note that the status check redundantly sends the Status command
when we are not using the CONFIG_NAND_LPD mode.  It's left in for
now.
*/
static void spinor_init (void)
{
  u32 clk_div = 0;
  u8 id[4] = {1, 2, 3, 4};

  /* Enable SPI clock */
  cgu_clk_en_dis(CGU_SB_SPI_PCLK_ID, 1);
  cgu_clk_en_dis(CGU_SB_SPI_PCLK_GATED_ID, 1);
  cgu_clk_en_dis(CGU_SB_SPI_CLK_ID, 1);
  cgu_clk_en_dis(CGU_SB_SPI_CLK_GATED_ID, 1);

  /* reset the SPI block */
  SPI_CONFIG_REG = SPI_CFG_SW_RESET;

  DBG(2, "spinor: 0x%08lx, 0x%08lx, 0x%08lx, 0x%08lx, 0x%08lx\n",
      SPI_CONFIG_REG, SPI_SLV_ENAB_REG, SPI_STS_REG, 
      SPI_SLV_SET1_REG(0), SPI_SLV_SET2_REG(0));

  /* configure SPI*/
  for (chip = (SPINOR_INFO_T*)&chips[0];
    chip < (chips + sizeof(chips)/sizeof (SPINOR_INFO_T)); ++chip) {
  
    SPI_CONFIG_REG &= ~SPI_CFG_ENABLE;
    spinor_cs_high();       /* Make nCS an output, initially high (ie de-asserted) */

    SPI_SLV_ENAB_REG = SPI_SLV_EN(chip->cs);
    SPI_CONFIG_REG |= SPI_CFG_UPDATE_EN;
    SPI_CONFIG_REG |= SPI_CFG_ENABLE;

    if (spi_get_clock_div(chip->clock, &clk_div)) {

      DBG(1, "SPI clock divider not found. cs:%d "
             "id:0x%02x 0x%02x 0x%02x 0x%02x \n", 
             chip->cs, chip->id[0], chip->id[1], chip->id[2], chip->id[3]);
      continue;
    }

    SPI_SLV_SET1_REG(chip->cs) = clk_div;
    SPI_SLV_SET2_REG(chip->cs) = SPI_SLV2_WD_SZ(8-1);

    DBG(2, "spinor: 0x%08lx, 0x%08lx, 0x%08lx, 0x%08lx, 0x%08lx\n",
      SPI_CONFIG_REG, SPI_SLV_ENAB_REG, SPI_STS_REG, 
      SPI_SLV_SET1_REG(0), SPI_SLV_SET2_REG(0));

    spinor_exit_deep_power_down();
    spinor_read_device_id(id);

    DBG(1, "spinor: ID 0x%02x/0x%02x/0x%02x/0x%02x stat:0x%08lx\n",
      id[0], id[1], id[2], id[3], SPI_STS_REG);

#ifdef _M
#undef _M
#endif
#define _M(i) (id[(i)] == chip->id[(i)])
    if (_M (0) && _M (1) && _M (2)) {
        DBG(1, "ID matchded\n");
        break;
    }
#undef _M
  }

  if (chip >= (chips + sizeof(chips)/sizeof (chips[0])))
    chip = NULL;

  if (chip)
    printf (" %d MiB total, %d B page"
      " (0x%02x/0x%02x/0x%02x/0x%02x)\n",
      (chip->total_pages * chip->page_size)/(1024*1024), 
      chip->page_size,
      id[0], id[1], id[2], id[3]);
  else
    printf (" unknown 0x%02x/0x%02x/0x%02x/0x%02x\n",
      id[0], id[1], id[2], id[3]);

}

static __service_6 struct service_d spinor_service = {
  .init = spinor_init,
#if !defined (CONFIG_SMALL)
  .report = spinor_report,
#endif
};

