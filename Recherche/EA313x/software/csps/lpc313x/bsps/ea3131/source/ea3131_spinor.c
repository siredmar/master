/***********************************************************************
 * $Id:: ea3131_spinor.c 2382 2009-10-27 22:12:38Z pdurgesh            $
 *
 * Project: EA3131 board functions
 *
 * Description:
 *     This file contains driver support for various EA3131 board
 *     functions.
 *
 ***********************************************************************
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * NXP Semiconductors assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. NXP Semiconductors
 * reserves the right to make changes in the software without
 * notification. NXP Semiconductors also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
 **********************************************************************/

#include "ea3131_board.h"
#include "lpc313x_spi_driver.h"
#include "lpc313x_timer_driver.h"
#include "lpc313x_ioconf_driver.h"

#define MANFID_ATMEL    0x1F


#define CMD_RDID        0x9F        /* read device ID */
#define CMD_RDSR        0xD7        /* read status register */
#define CMD_FAST_READ   0x0B        /* read data (extra command setup time allows higher SPI clock) */

#define CMD_SE          0x7C        /* sector erase */
#define CMD_BE          0x50        /* block erase */
#define CMD_PE          0x81        /* page erase */

#define CMD_DP          0xB9        /* deep power down */
#define CMD_RES         0xAB        /* release from deep power down */

#define DUMMY_BYTE		  0xFF

/* status register defines */
#define STATUS_RDY      (1 << 7)
#define STATUS_COMP     (1 << 6)
#define STATUS_PROTECT  (1 << 1)
#define STATUS_P512     (1 << 0)


#define PAGE_528       (528)
#define PAGE_512       (512)
#define BLOCK_SIZE(pg_sz)      (16 * (pg_sz))
#define SECTOR_SIZE(pg_sz)     (32 * (pg_sz))


/****************************************************************************
****************************************************************************/

typedef struct
{
  INT_32 spi_id;
  UNS_32 spi_slave_id;
  UNS_32 page_size;
  UNS_8  manufacturer_id;
  UNS_8  device_type;
  UNS_8  device_capacity;
  UNS_8 _pad;

} SPINOR_INFO_T;

static SPINOR_INFO_T g_spinor;


/***********************************************************************
 * Private functions
 **********************************************************************/
void spinor_cs_low(void)
{
  gpio_set_outpin_low(IOCONF_SPI, 4);
}

void spinor_cs_high(void)
{
  gpio_set_outpin_high(IOCONF_SPI, 4);
}

/***********************************************************************
 *
 * Function: spinor_xfer
 *
 * Purpose: Transfer data to/from the serial EEPROM
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     out   : Output data
 *     in    : Input data
 *     bytes : Number of bytes to send/receive
 *
 * Outputs: None
 *
 * Returns: TRUE if the byte was transferred
 *
 * Notes: Do not use this function to transfer more than 8 bytes!
 *
 **********************************************************************/
static void spinor_xfer(UNS_8 *out,
                        UNS_8 *in,
                        int bytes)
{
  INT_32 rbytes = 0;
  volatile UNS_32 status;

  /* flush the FFOS */
  spi_ioctl(g_spinor.spi_id, SPI_FLUSH_RX_FIFO, 0);
  /* write the commands */
  spi_write(g_spinor.spi_id, out, bytes);
  /* wait till SPI is not busy */
  do
  {
    status = spi_ioctl(g_spinor.spi_id, SPI_GET_STATUS, SPI_FIFO_ST);
  }
  while ((status & SPI_ST_TX_EMPTY) != SPI_ST_TX_EMPTY);

  while (rbytes < bytes)
  {
    rbytes += spi_read(g_spinor.spi_id, &in [rbytes], 1);
  }
}


/***********************************************************************
 *
 * Function: spinor_read_device_id
 *
 * Purpose: Read device ids
 *
 * Processing:
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
static void spinor_read_device_id(UNS_8 *device_id)
{
  UNS_8 datao [4];

  datao[0] = CMD_RDID;
  datao[1] = DUMMY_BYTE;
  datao[2] = DUMMY_BYTE;
  datao[3] = DUMMY_BYTE;

  /* 3 bytes: manufacturer id, device type, device capacity */
  spinor_cs_low();
  spinor_xfer(datao, device_id, 4);
  spinor_cs_high();
}

/***********************************************************************
 *
 * Function: spinor_write_cfg_reg
 *
 * Purpose: Write configuration register of the device
 *
 * Processing:
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
static void spinor_write_cfg_reg(void)
{
  UNS_8 datai [4], datao [4];

  /* 4 bytes magic sequence to program the device to 512 page device */
  datao[0] = 0x3D;
  datao[1] = 0x2A;
  datao[2] = 0x80;
  datao[3] = 0xA6;

  spinor_cs_low();
  spinor_xfer(datao, datai, 4);
  spinor_cs_high();
}
/***********************************************************************
 *
 * Function: spinor_simple_command
 *
 * Purpose: Send simple commands to device.
 *
 * Processing:
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
static void spinor_simple_command(UNS_8 command_byte)
{
  UNS_8 dummy;
  /* should we wait for command to txfer */
  spinor_cs_low();
  spinor_xfer(&command_byte, &dummy, 1);
  spinor_cs_high();
}

/***********************************************************************
 *
 * Function: spinor_enter_deep_power_down
 *
 * Purpose: Put the device in deep power down mode.
 *
 * Processing:
 *     Does nothing
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
static void spinor_enter_deep_power_down(void)
{
  spinor_simple_command(CMD_DP);
}

/***********************************************************************
 *
 * Function: spinor_exit_deep_power_down
 *
 * Purpose: Exit the device from deep power down mode.
 *
 * Processing:
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
static void spinor_exit_deep_power_down(void)
{
  spinor_simple_command(CMD_RES);
  /* max tEDPD in Atmel AT45DB161D-SU datasheet is given as 35 usec */
  timer_wait_us(NULL, 50);
}

/***********************************************************************
 *
 * Function: spinor_read_status_register
 *
 * Purpose: Read status register 
 *
 * Processing:
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
static void spinor_read_status_register(UNS_32 *status)
{
  UNS_8 datai [2], datao [2];

  datao[0] = CMD_RDSR;
  datao[1] = DUMMY_BYTE;
  spinor_cs_low();
  spinor_xfer(datao, datai, 2);
  spinor_cs_high();
  *status = datai[1];
}

/***********************************************************************
 *
 * Function: spinor_poll_is_busy
 *
 * Purpose: Check if device is busy
 *
 * Processing:
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
static void spinor_poll_is_busy(void)
{
  UNS_32 status;

  do
  {
    timer_wait_us(NULL, 10);
    spinor_read_status_register(&status);
  }
  while ((status & STATUS_RDY) == 0);
}

/***********************************************************************
 *
 * Function: spinor_sector_erase
 *
 * Purpose: Erase a sector
 *
 * Processing:
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
static void spinor_sector_erase(UNS_32 offset)
{
  UNS_8 datai [4], datao [4];

  /* Hopefully sectors are not protected */

  datao[0] = CMD_SE;
  datao[1] = (offset >> 16) & 0xFF; /* 24bit address upper byte */
  datao[2] = (offset >>  8) & 0xFF; /* 24bit address middle byte */
  datao[3] = (offset >>  0) & 0xFF; /* 24bit address least significant byte */

  spinor_cs_low();
  spinor_xfer(datao, datai, 4);
  spinor_cs_high();

  spinor_poll_is_busy();
}

/***********************************************************************
 *
 * Function: spinor_page_program
 *
 * Purpose: Program a page
 *
 * Processing:
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
static void spinor_page_program(UNS_32 offset, UNS_8 *src, UNS_32 bytecount)
{
  UNS_8 datai [64], datao [4];
  UNS_32 count, status;

  /* assuming sectors are not protected. */

  datao[0] = 0x82;
  datao[1] = (offset >> 16) & 0xFF; /* 24bit address upper byte */
  datao[2] = (offset >>  8) & 0xFF; /* 24bit address middle byte */
  datao[3] = (offset >>  0) & 0xFF; /* 24bit address least significant byte */
  spinor_cs_low();

  spinor_xfer(datao, datai, 4);

  while (bytecount > 0)
  {
    count = spi_write(g_spinor.spi_id, src, bytecount);
    bytecount -= count;
    src += count;
  }
  /* wait for Tx FIFO to empty */
  do
  {
    status = spi_ioctl(g_spinor.spi_id, SPI_GET_STATUS, SPI_FIFO_ST);
  }
  while ((status & SPI_ST_TX_EMPTY) != SPI_ST_TX_EMPTY);


  /* flush RX FIFO */
  spi_ioctl(g_spinor.spi_id, SPI_FLUSH_RX_FIFO, 0);
  spinor_cs_high();

  spinor_poll_is_busy();
}



/***********************************************************************
 * Public functions
 **********************************************************************/

/***********************************************************************
 *
 * Function: spinor_init
 *
 * Purpose: Initializes SPI interface and SPI_NOR device
 *
 * Processing:
 *     Programs OTP in ATMEL device to set device 512 byte page mode
 *  instead of 528 byte page mode.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
STATUS spinor_init(INT_32 spi_id)
{
  UNS_8 device_id[4];
  SPI_SLAVE_CONFIG_T cfg;
  UNS_32 temp;

  /* de-select the device by pulling the CS high. */
  gpio_set_outpin_high(IOCONF_SPI, 4);

  g_spinor.spi_id = spi_id;
  /* on VAL3150 SPI NOR is connected to chip select 0 */
  g_spinor.spi_slave_id = SPI_NOR_SLAVE_ID;

  /* configure the slave setting with NOR flash timings */
  cfg.sid = (SPI_SLAVE_ID_T)SPI_NOR_SLAVE_ID;
  cfg.databits = 8;
  cfg.words = SPI_FIFO_DEPTH;
  cfg.mode = SPI_MODE0;
  cfg.cs_high = 0;
  cfg.pp_delay = 0;
  cfg.inter_delay = 0;
  cfg.clk = 1000000;
  spi_ioctl(g_spinor.spi_id, SPI_SLAVE_CONFIG, (INT_32)&cfg);

  spi_ioctl(g_spinor.spi_id, SPI_SET_ACTIVE_SLAVE, SPI_NOR_SLAVE_ID);
  /* make sure SPI module is enabled */
  spi_ioctl(g_spinor.spi_id, SPI_ENABLE, 1);

  spinor_exit_deep_power_down();
  spinor_read_device_id(device_id);

  g_spinor.manufacturer_id = device_id[1];
  g_spinor.device_capacity = device_id[2];
  g_spinor.device_type = device_id[3];

  /* return if it is not ATMEL device */
  if (g_spinor.manufacturer_id !=  MANFID_ATMEL)
    return _ERROR;


  do
  {
    spinor_read_status_register(&temp);
  }
  while ((temp & STATUS_RDY) == 0);


  /* Check if the device is configured as 512 page device.
     If not program it to be 512 page device.
  */
  if ((temp & STATUS_P512) == 0)
  {
    g_spinor.page_size = PAGE_528;
    /* program device to 512 mode. needs a power cycle after this.*/
    spinor_write_cfg_reg();
    return _ERROR;
  }
  else
  {
    g_spinor.page_size = PAGE_512;
  }

  spinor_enter_deep_power_down();

  return g_spinor.page_size;
}


/***********************************************************************
 *
 * Function: spinor_read
 *
 * Purpose: Read data from SPI-NOR device
 *
 * Processing:
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
STATUS spinor_read(UNS_8 *dst, UNS_32 offset, UNS_32 bytecount)
{
  UNS_8 datai [5], datao [5];
  INT_32 count, rbytes;
  volatile UNS_32 status = 0;

  if (g_spinor.spi_id == 0)
    return _ERROR;

  spi_ioctl(g_spinor.spi_id, SPI_SET_ACTIVE_SLAVE, SPI_NOR_SLAVE_ID);
  spinor_exit_deep_power_down();

  datao[0] = (CMD_FAST_READ);          /* FAST_READ is safe at all supported SPI speeds... */
  datao[1] = ((offset >> 16) & 0xFF);  /* 24bit address upper byte */
  datao[2] = ((offset >>  8) & 0xFF);  /* 24bit address middle byte */
  datao[3] = ((offset >>  0) & 0xFF);  /* 24bit address least significant byte */
  datao[4] = (0);                      /* dummy byte */

  spinor_cs_low();
  gpio_set_outpin_low(IOCONF_SPI, 4);
  spinor_xfer(datao, datai, 5);

  /* wait until both FIFOs are empty */
  status = 0;
  while ((status & (SPI_ST_RX_EMPTY | SPI_ST_TX_EMPTY)) !=
         (SPI_ST_RX_EMPTY | SPI_ST_TX_EMPTY))
  {
    status = spi_ioctl(g_spinor.spi_id, SPI_GET_STATUS, SPI_FIFO_ST);
  }

  while (bytecount > 0)
  {
    count = spi_write(g_spinor.spi_id, dst, bytecount);
    status = 0;
    while ((status & SPI_ST_TX_EMPTY) != SPI_ST_TX_EMPTY)
    {
      status = spi_ioctl(g_spinor.spi_id, SPI_GET_STATUS, SPI_FIFO_ST);
    }

    rbytes = 0;
    while (count > 0)
    {
      rbytes = spi_read(g_spinor.spi_id, dst, count);
      count -= rbytes;
      dst += rbytes;
      bytecount -= rbytes;
    }
  }
  spinor_cs_high();

  gpio_set_outpin_high(IOCONF_SPI, 4);

  spinor_enter_deep_power_down();

  return _NO_ERROR;
}


/***********************************************************************
 *
 * Function: spinor_write
 *
 * Purpose: Very crude write support...
 *
 * Processing:
 *     Assumes all sectors are 64k bytes, does not verify address is 
 *  within bounds of device, etc, etc.
 *
 *  Currently, write support is meant for simple testing only... !!
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
STATUS spinor_write(UNS_8 *src, UNS_32 offset, UNS_32 bytecount)
{
  UNS_32 offset_end;


  if ((g_spinor.spi_id == 0) ||
      ((offset % g_spinor.page_size) != 0)) /* error if address is not sector aligned */
  {
    return _ERROR;
  }

  spi_ioctl(g_spinor.spi_id, SPI_SET_ACTIVE_SLAVE, SPI_NOR_SLAVE_ID);
  spinor_exit_deep_power_down();

  offset_end = (offset + bytecount);              /* end offset of entire write operation */

  while (offset < offset_end)
  {
    /* No need to erase since we use "Main Memory Page Program Through Buffer" in this driver.
    */
    spinor_page_program(offset, src, g_spinor.page_size);
    src += g_spinor.page_size;
    offset += g_spinor.page_size;
  }

  spinor_enter_deep_power_down();

  return _NO_ERROR;
}

/***********************************************************************
 *
 * Function: spinor_erase_from_beginning
 *
 * Purpose: Erase SPI-NOR device from beginning.
 *
 * Processing:
 *     Does nothing
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
STATUS spinor_erase_from_beginning(UNS_32 bytecount)
{
  UNS_32 offset = 0;

  if (g_spinor.spi_id == 0)
    return _ERROR;

  spi_ioctl(g_spinor.spi_id, SPI_SET_ACTIVE_SLAVE, SPI_NOR_SLAVE_ID);
  spinor_exit_deep_power_down();

  while (offset < bytecount)
  {
    spinor_sector_erase(offset);
    /* check if it is first sector */
    if (offset < SECTOR_SIZE(g_spinor.page_size))
    {
      offset += SECTOR_SIZE(g_spinor.page_size) / 2;
    }
    else
    {
      offset += SECTOR_SIZE(g_spinor.page_size);
    }
  }

  spinor_enter_deep_power_down();

  return _NO_ERROR;
}

/***********************************************************************
 *
 * Function: spinor_erase_sector
 *
 * Purpose: Erase a sector
 *
 * Processing:
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
STATUS spinor_erase_sector(UNS_32 sectorIndex)
{
  UNS_32 offset;

  if (g_spinor.spi_id == 0)
    return _ERROR;

  spi_ioctl(g_spinor.spi_id, SPI_SET_ACTIVE_SLAVE, SPI_NOR_SLAVE_ID);
  spinor_exit_deep_power_down();

  offset = sectorIndex * SECTOR_SIZE(g_spinor.page_size);

  spinor_sector_erase(offset);

  spinor_enter_deep_power_down();

  return _NO_ERROR;
}

/***********************************************************************
 *
 * Function: spinor_write_page
 *
 * Purpose: Write a page
 *
 * Processing:
 *     
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
INT_32 spinor_write_page(UNS_8 *src, UNS_32 offset)
{
  if ((g_spinor.spi_id == 0) ||
      ((offset % g_spinor.page_size) != 0)) /* error if address is not sector aligned */
  {
    return 0;
  }

  spi_ioctl(g_spinor.spi_id, SPI_SET_ACTIVE_SLAVE, SPI_NOR_SLAVE_ID);
  spinor_exit_deep_power_down();

  spinor_page_program(offset, src, g_spinor.page_size);

  spinor_enter_deep_power_down();

  return g_spinor.page_size;
}

