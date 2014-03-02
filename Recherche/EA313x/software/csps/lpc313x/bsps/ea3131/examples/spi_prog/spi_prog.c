/***********************************************************************
 * $Id:: spi_prog.c 1810 2009-04-21 18:14:23Z sscaglia                 $
 *
 * Project: LCD interface example
 *
 * Description:
 *     A simple LCD interface example.
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

#include <stdio.h>
#include <string.h>
#include "lpc_types.h"
#include "lpc_irq_fiq.h"
#include "lpc_arm922t_cp15_driver.h"
#include "ea3131_board.h"
#include "lpc313x_lcdc.h"
#include "lpc313x_sysreg.h"
#include "lpc313x_ioconf_driver.h"
#include "lpc313x_cgu_driver.h"
#include "lpc313x_timer_driver.h"
#include "lpc313x_spi_driver.h"
#include "lpc313x_uart_driver.h"
#include "lpc313x_intc_driver.h"
#include "lpc313x_crc_driver.h"

#define offsetof(s,m)   (int)&(((s *)0)->m)
#define COMPILE_TIME_ASSERT(expr)   char constraint[expr]

/* 1 sec timeout for the download end detection. */
#define UART_DOWNLOAD_TIMEOUT_END_US    1000000
/* 1min timeout for the download start. */
#define UART_DOWNLOAD_TIMEOUT_START_US   60000000

/* Maximum image size to be programmed */
#define MAX_PRG_SIZE        (80 * 1024)
#define DOWNLOAD_BUF_ADDR   (0x11028000 + (24 * 1024))

static UNS_8 g_image_buf[128 * 1024];

/* Timer expiration flag */
static volatile INT_32 g_timer_expired;
INT_32 g_timer_dev;
INT_32 g_uart_dev;

typedef struct
{
  UNS_32  vector;
  UNS_32  magic;
  UNS_32  image_crc;
  UNS_32  resrv0[4];
  UNS_32  image_type;
  UNS_32  image_length;
  UNS_32  release_id;
  UNS_32  build_time;
  UNS_32  reserved[16];
  UNS_32  header_crc;
  UNS_32  resrv1[4];
} IMAGE_HEADER_T;

/***********************************************************************
 *
 * Function: uart_string_write
 *
 * Purpose: Output given C string to UART port.
 *
 * Processing:
 *     See function.
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
static void uart_string_write(char *pbuff)
{
  UNS_32 len = strlen(pbuff);
  UNS_32 written;

  while (len > 0)
  {
    written = uart_write(g_uart_dev, pbuff, len);
    len -= written;
    pbuff += written;
  }
}


/***********************************************************************
 *
 * Function: timer0_user_interrupt
 *
 * Purpose: Timer 0 interrupt handler
 *
 * Processing:
 *     See function.
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
void timer0_user_interrupt(void)
{
  /* Clear latched timer interrupt */
  timer_ioctl(g_timer_dev, TMR_CLEAR_INTS, 1);

  /* Set timer exp flag */
  g_timer_expired = 1;
}

#if 1
/***********************************************************************
 *
 * Function: flash_verify
 *
 * Purpose: Output given C string to UART port.
 *
 * Processing:
 *     See function.
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
STATUS flash_verify(UNS_8* file_buf, INT_32 filesize, INT_32 page_size)
{
  UNS_8 data[528];
  UNS_32 offset;

  offset = 0;
  while (offset < filesize)
  {
    spinor_read(data, offset, page_size);
    if (memcmp(file_buf, data, page_size))
      return _ERROR;

    offset += page_size;
    file_buf += page_size;
    uart_string_write("#");
  };
  return _NO_ERROR;
}

/***********************************************************************
 *
 * Function: flash_program
 *
 * Purpose: Output given C string to UART port.
 *
 * Processing:
 *     See function.
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
INT_32 flash_program(UNS_8* file_buf, INT_32 filesize)
{
  INT_32 bytes_wr = 0;
  UNS_32 offset = 0;

  while (filesize > 0)
  {
    /* assuming 528 byte page size */
    bytes_wr = spinor_write_page(file_buf, offset);
    if (bytes_wr == 0)
      break;

    filesize -= bytes_wr;
    file_buf += bytes_wr;
    offset += bytes_wr;
    uart_string_write("#");

  }

  return (filesize > 0) ? _ERROR : _NO_ERROR;
}

#endif

STATUS image_verify(UNS_8* buf, INT_32 filesize)
{
  STATUS ret = _ERROR;
  IMAGE_HEADER_T* phdr = (IMAGE_HEADER_T*)buf;

  /* compute header CRC */
  if (phdr->header_crc == crc32_compute(buf,
                                        (sizeof(IMAGE_HEADER_T) - 5 * sizeof(UNS_32))))
  {
    buf += sizeof(IMAGE_HEADER_T);
    /* compute image CRC */
    if (phdr->image_crc == crc32_compute(buf,
                                         (phdr->image_length - sizeof(IMAGE_HEADER_T))))
    {
      ret = _NO_ERROR;
    }
    else
    {
      uart_string_write("Image CRC32 checksum failed!!!\r\n");
    }
  }
  else
  {
    uart_string_write("Image header CRC32 checksum failed!!!\r\n");
  }
  return ret;
}
/***********************************************************************
 *
 * Function: uart_download
 *
 * Purpose: Output given C string to UART port.
 *
 * Processing:
 *     See function.
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
static INT_32 uart_download(INT_32 devid, UNS_8 *pdest)
{
  TMR_CFG_ARG_T tmr_cfg;
  INT_32 bytes_remain = MAX_PRG_SIZE, bread = 0;


  /* Set timer to expire after 6 secs  */
  tmr_cfg.periodic = TRUE;
  /* Download the image until the maximal size is reached or a timeout on the byte reception happened. */
  tmr_cfg.usecs = UART_DOWNLOAD_TIMEOUT_START_US;
  tmr_cfg.enable = TRUE;
  timer_ioctl(g_timer_dev, TMR_SET_CFG, (INT_32)&tmr_cfg);
  g_timer_expired = 0;

  /* Enable timer interrupts in the interrupt controller */
  int_enable(IRQ_TIMER0);


  while (1)
  {
    while (bread == 0)
    {
      bread = uart_read(devid, pdest, bytes_remain);
      if (g_timer_expired)
      {
        break;
      }
    }
    if (g_timer_expired)
    {
      if (tmr_cfg.usecs == UART_DOWNLOAD_TIMEOUT_END_US)
      {
        uart_string_write("Download finished\r\n");
      }
      else
      {
        uart_string_write("Timeout!\r\n");
      }
      break;
    }

    pdest += bread;
    bytes_remain -= bread;
    bread = 0;

    if (bytes_remain < 1)
    {
      break;
    }
    /* disable timer */
    timer_ioctl(g_timer_dev, TMR_ENABLE, 0);
    /* Change time out to max inter character delay */
    tmr_cfg.usecs = UART_DOWNLOAD_TIMEOUT_END_US;
    timer_ioctl(g_timer_dev, TMR_SET_CFG, (INT_32)&tmr_cfg);
  }

  /* return the size of image downloaded */
  return (MAX_PRG_SIZE - bytes_remain);
}


/***********************************************************************
 *
 * Function: c_entry
 *
 * Purpose: Application entry point from the startup code
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Always returns 1
 *
 * Notes: None
 *
 **********************************************************************/
int c_entry(void)
{
  INT_32 spi_id;
  UART_CONTROL_T ucntl;
  INT_32 img_len = 0, page_size;
  UNS_8* pDestBuf = g_image_buf;

  /* Disable interrupts in ARM core */
  disable_irq_fiq();

  /* Setup miscellaneous board functions */
  ea3131_board_init();

  /* Set virtual address of MMU table */
  cp15_set_vmmu_addr((void *)ISROM_MMU_TTB_BASE);

  /* Initialize interrupt system */
  int_initialize(0xFFFFFFFF);

  /* Install timer interrupts handlers as a IRQ interrupts */
  int_install_irq_handler(IRQ_TIMER0, (PFV) timer0_user_interrupt);

  /* Enable IRQ interrupts in the ARM core */
  enable_irq();

  /* Open timers - this will enable the clocks for all timers when
     match control, match output, and capture control functions
     disabled. Default clock will be internal. */
  g_timer_dev = timer_open((void *)TIMER_CNTR0, 0);

  /* Setup UART for 115.2K, 8 data bits, no parity, 1 stop bit */
  ucntl.baud_rate = 115200;
  ucntl.parity = UART_PAR_NONE;
  ucntl.databits = 8;
  ucntl.stopbits = 1;
  g_uart_dev = uart_open((void *) UART_BASE, (INT_32) & ucntl);

  /* init SPI module */
  spi_id = spi_open((void *)SPI_BASE, 0);

  if (spi_id != 0)
  {
    /* init SPI NOR flash device */
    page_size = spinor_init(spi_id);
    if (_ERROR == page_size)
    {
//      uart_string_write("Error opening SPI NOR flash. \r\n");
//      return _ERROR;
//    }
//    else if ( 528 == page_size)
//    {
      /* tell user that SPI-flash was in 528 mode and the program
      converted it to 512 page mode. So he needs to power cycle the
      board. */
      uart_string_write( "*********************** \r\n"
        "Converted SPI NOR flash from 528byte page size mode " 
        "to 512 byte page mode. \r\n So please power cycle the board. \r\n"
        "After power-up try programming again. \r\n"
        "*********************** \r\n");
      return _ERROR;

    }

    uart_string_write("SPI programmer is ready for download> \r\n");

    /* download the data to ISRAM buffer */
    img_len = uart_download(g_uart_dev, pDestBuf);


    if (img_len)
    {
      if (image_verify(pDestBuf, img_len) == _NO_ERROR)
      {
        uart_string_write("Programming flash\r\n");
        /* dump SPI data */
        if (_NO_ERROR == flash_program(pDestBuf, img_len))
        {
          uart_string_write("Verifying flash\r\n");
          /* verify the image */
          if (_NO_ERROR == flash_verify(pDestBuf, img_len, page_size))
          {
            uart_string_write("Done!!!\r\n");
            return 1;
          }

          uart_string_write("Error data mismatch!!!\r\n");
        }
      }
      else
      {
        uart_string_write("Improper image file.\r\n");
        uart_string_write("Please use following command to generate proper file.\r\n");
        uart_string_write("lpc313xImgCreator.exe -pc -o ./ -i demo.bin\r\n");
        uart_string_write("And select demo.rom file for programming.\r\n");
      }
    }
  }

  return 1;
}
