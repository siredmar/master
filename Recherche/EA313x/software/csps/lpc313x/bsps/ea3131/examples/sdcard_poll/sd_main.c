/***********************************************************************
 * $Id:: sd_main.c 1757 2009-03-27 00:11:10Z pdurgesh                  $
 *
 * Project: UART driver example
 *
 * Description:
 *     A simple UART driver example.
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
#include "lpc313x_timer_driver.h"
#include "lpc313x_uart_driver.h"
#include "lpc313x_mci_driver.h"
#include "lpc313x_ioconf_driver.h"
#include "lpc313x_cgu_driver.h"
#include "fat.h"


#ifdef offsetof
#undef offsetof
#endif

#define offsetof(s,m)   (int)&(((s *)0)->m)
#define COMPILE_TIME_ASSERT(expr)   char constraint[expr]

COMPILE_TIME_ASSERT(offsetof(MCI_REGS_T, hcon) == 0x70);
COMPILE_TIME_ASSERT(offsetof(MCI_REGS_T, data) == 0x100);


char buff[512];
INT_32 g_mcidev;
INT_32 g_uartdev;

/***********************************************************************
 *
 * Function: uart_string_write
 *
 * Purpose: Uart output function
 *
 * Processing:
 *     Writes the buffer to UART port
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 **********************************************************************/
static void uart_string_write(char *pbuff)
{
  UNS_32 len = strlen(pbuff);
  UNS_32 written;

  if (g_uartdev == 0) 
    return;

  while (len > 0)
  {
    written = uart_write(g_uartdev, pbuff, len);
    len -= written;
    pbuff += written;
  }
}

/***********************************************************************
 *
 * Function: blkdev_init
 *
 * Purpose: SDMMC init
 *
 * Processing:
 *     Initialize the SDMMC card
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Always returns TRUE
 *
 * Notes: None
 *
 **********************************************************************/
BOOL_32 blkdev_init(void) 
{
  BOOL_32 ret = TRUE;
  MCI_CARD_INFO_T* psdcard;
  int i = 0;

  /* enable power to SD card slot by setting I2STX_DATA0 low. */
  gpio_set_outpin_low(IOCONF_MUX1, 5);
  /* wait atleast 10msec for card to stablize */
  timer_wait_ms(NULL, 10);

  /* open MCI device */
  g_mcidev = mci_open((void*)SD_MMC_BASE, NULL);
  psdcard = (MCI_CARD_INFO_T*)g_mcidev;

  /* check if card is acquired */
  if (psdcard->cid[0] != 0)
  {
    /* print card info */
    sprintf(buff, "%s card acquired\r\n", (psdcard->card_type & CARD_TYPE_SD) ? "sd" : "mmc");
    uart_string_write(buff);
    sprintf(buff, "rca 0x%x %d.%02d MiB\r\n", psdcard->rca,
            psdcard->device_size / (1024*1024),
            (((psdcard->device_size / 1024) % 1024)*100) / 1024);
    uart_string_write( buff);
    sprintf(buff, "cid: 0x%08x 0x%08x 0x%08x 0x%08x\r\n",
            psdcard->cid[0], psdcard->cid[1], psdcard->cid[2], psdcard->cid[3]);
    uart_string_write(buff);

    sprintf(buff, "csd: 0x%08x 0x%08x 0x%08x 0x%08x\r\n",
            psdcard->csd[0], psdcard->csd[1], psdcard->csd[2], psdcard->csd[3]);
    uart_string_write(buff);

    if ( psdcard->ext_csd[53] != 0)
    {
      uart_string_write("ext_csd:\r\n");
      i = 0;
      while (i < (MMC_SECTOR_SIZE/4)) 
      {
        sprintf(buff, "0x%08x 0x%08x 0x%08x 0x%08x\r\n",
                psdcard->ext_csd[i], psdcard->ext_csd[i+1], psdcard->ext_csd[i+2], psdcard->ext_csd[i+3]);
        uart_string_write(buff);
        i += 4;
      }
    }
  }
  else
  {
    uart_string_write("no card acquired\r\n");
    ret = FALSE;
  }
  return ret;
}

/***********************************************************************
 *
 * Function: blkdev_read
 *
 * Purpose: SDMMC sector read 
 *
 * Processing:
 *     SDMMC sector read 
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: 
 *
 * Notes: None
 *
 **********************************************************************/
BOOL_32 blkdev_read(void *buff, UNS_32 sector, UNS_32 sec_cnt) 
{
	if (mci_read_blocks(g_mcidev,
                       buff,
                       sector,
                       sector + sec_cnt) < 0)
	{
		return FALSE;
	}
	
	return TRUE;
}

/***********************************************************************
 *
 * Function: blkdev_deinit
 *
 * Purpose: SDMMC deinit
 *
 * Processing:
 *     De-initialize the SDMMC card
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Always returns TRUE
 *
 * Notes: None
 *
 **********************************************************************/
BOOL_32 blkdev_deinit(void) 
{
	if (g_mcidev != 0) 
	{
		mci_close(g_mcidev);
		g_mcidev = 0;
	}

	return TRUE;
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
	BOOL_32 lp;
  INT_32 sz;
  PFV entry;

  /* Disable interrupts in ARM core */
  disable_irq_fiq();

  /* Setup miscellaneous board functions */
  ea3131_board_init();

  /* Set virtual address of MMU table */
  cp15_set_vmmu_addr((void *)ISROM_MMU_TTB_BASE);

  /* Setup UART for 115.2K, 8 data bits, no parity, 1 stop bit */
  g_uartdev = uart_open((void *) UART_BASE, NULL);
  if (g_uartdev != 0)
  {
    sprintf(buff, "LPC315x SD card test\r\n");
    uart_string_write(buff);

    /* Initialize FAT interface first */
    if (fat_init(blkdev_init, blkdev_read, blkdev_deinit) == FALSE)
    {
    	uart_string_write("fat_init failed\r\n");
    }
    else
    {
    	/* Show items in BLKDEV root directory */
    	lp = fat_get_dir((UNS_8*)buff, TRUE);
    	while (lp == FALSE) 
    	{
    		uart_string_write(" ");
    		uart_string_write(buff);
    		uart_string_write("\r\n");

    		lp = fat_get_dir((UNS_8*)buff, FALSE);
    	}
      /* read firmware.bin file if present on drive*/
      sz = fat_file_read_nocache("FIRMWARE.BIN", 
        (UNS_8*)0x30008000, 0x800000);
      if (sz != 0)
      {
        entry = (PFV)0x30008000;
      	/* Execute new program */
      	cp15_dcache_flush();
      	cp15_invalidate_cache();
        cp15_write_buffer_flush();
        cp15_set_mmu(FALSE);
        entry();
      }
          
    
    	fat_deinit();
    }

    //uart_close(g_uartdev);
  }

  return 1;
}

