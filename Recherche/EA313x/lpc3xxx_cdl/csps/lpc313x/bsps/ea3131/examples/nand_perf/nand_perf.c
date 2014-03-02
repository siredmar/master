/***********************************************************************
 * $Id:: nand_perf.c 6148 2011-01-11 13:55:39Z ing02124                $
 *
 * Project: NAND driver performance example
 *
 * Description:
 *     A simple NAND driver performance measurement example.
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
#include "lpc_nandflash_params.h"
#include "ea3131_board.h"
#include "lpc313x_nand_driver.h"
#include "lpc313x_sysreg.h"
#include "lpc313x_uart_driver.h"
#include "lpc313x_cgu_driver.h"
#include "lpc313x_timer_driver.h"
#include "lpc313x_intc_driver.h"
#include "lpc31xx_vmem_driver.h"

#define BLK_NUM (10)
#define NUM_ITERATIONS  			(50)
#define NUM_ERASE_ITERATIONS 	(100)
#define TIMER_RELOAD_VAL (0xFFFFFFFF)

static DMA_BUFFER UNS_8 buf[LARGE_BLOCK_PAGE_SIZE];
static char buff[512];
static INT_32 uartdev;
static NAND_DEV_INFO_T	info;
volatile static UNS_32	start_time, end_time;
volatile static UNS_32	read[NUM_ITERATIONS], write[NUM_ITERATIONS];
volatile static UNS_32	erase[NUM_ERASE_ITERATIONS];

static void nand_perf_test(void);

static void uart_string_write(INT_32 devid, char *pbuff)
{
  UNS_32 len = strlen(pbuff);
  UNS_32 written;

  while (len > 0)
  {
    written = uart_write(devid, pbuff, len);
    len -= written;
    pbuff += written;
  }
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
  UART_CONTROL_T ucntl;

  /* Disable interrupts in ARM core */
  disable_irq_fiq();

  /* Setup miscellaneous board functions */
  ea3131_board_init();

  /* enable clock_out */
  cgu_clk_en_dis(CGU_SB_CLOCK_OUT_ID, 1);

  /* enable Timer 3 clock_out */
  cgu_clk_en_dis(CGU_SB_TIMER2_PCLK_ID, 1);

  /* Reset */
  cgu_soft_reset_module(TIMER2_PNRES_SOFT);

  /* Set virtual address of MMU table */
  cp15_set_vmmu_addr((void *)ISROM_MMU_TTB_BASE);

  /* Initialize interrupt system */
  int_initialize(0xFFFFFFFF);

  /* Enable IRQ */
  enable_irq();

  /* Setup UART for 115.2K, 8 data bits, no parity, 1 stop bit */
  ucntl.baud_rate = 115200;
  ucntl.parity = UART_PAR_NONE;
  ucntl.databits = 8;
  ucntl.stopbits = 1;
  uartdev = uart_open((void *) UART_BASE, (INT_32) & ucntl);
  if (uartdev != 0)
  {
    sprintf(buff, "-------------------------------------\r\n");
    uart_string_write(uartdev, buff);
    sprintf(buff, "LPC3131 nand performance measurement \r\n");
    uart_string_write(uartdev, buff);
    sprintf(buff, "-------------------------------------\r\n");
    uart_string_write(uartdev, buff);

    nand_perf_test();

    sprintf(buff, "End: LPC3131 nand performance measurement \r\n");
    uart_string_write(uartdev, buff);
    sprintf(buff, "----------------------------------------\r\n");
    uart_string_write(uartdev, buff);

    uart_close(uartdev);
  }

  return 1;
}

/***********************************************************************
 *
 * Function: timer_start
 *
 * Purpose: Timer start function
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *					None
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 **********************************************************************/
static void timer_start(void)
{
  /* Reset Timer 3, Reload to maximum Value */
  TIMER_CNTR2->control &= ~TM_CTRL_ENABLE;
  TIMER_CNTR2->clear = 0xFFFFFFFF;
  TIMER_CNTR2->load = TIMER_RELOAD_VAL;

  /* Enable the timer in free running mode*/
  TIMER_CNTR2->control = TM_CTRL_ENABLE;
}

/***********************************************************************
 *
 * Function: find_min
 *
 * Purpose: Find minimum of values, return minimum value
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *					data1: data value
 *					data2: data value
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 **********************************************************************/
static UNS_32 find_min(UNS_32 data1, UNS_32 data2)
{
  return ((data1 > data2) ? data2 : data1);
}

/***********************************************************************
 *
 * Function: find_max
 *
 * Purpose: Find maximum of values, return maximum value
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *					data1: data value
 *					data2: data value
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 **********************************************************************/
static UNS_32 find_max(UNS_32 data1, UNS_32 data2)
{
  return ((data1 > data2) ? data1 : data2);
}

/***********************************************************************
 *
 * Function: nand_print_stats
 *
 * Purpose: NAND Print Statistics function
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *					data: Performance data buffer
 *					erase: Erase operation flag
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 **********************************************************************/
static void nand_print_stats(UNS_32 *data, BOOL_32 erase)
{
  UNS_32 min = 0xFFFFFFFF, max = 0x0;
  UNS_64 freq, sum = 0;
  UNS_32 ave = 0;
  UNS_32 iter;
  UNS_32 perf = 0;

  if (erase == TRUE)
  {
    for (iter = 0 ; iter < NUM_ERASE_ITERATIONS; iter++)
    {
      min = find_min(min, data[iter]);
      max = find_max(max, data[iter]);
      sum += (UNS_64)data[iter];
    }
  }
  else
  {
    for (iter = 0 ; iter < NUM_ITERATIONS; iter++)
    {
      min = find_min(min, data[iter]);
      max = find_max(max, data[iter]);
      sum += (UNS_64)data[iter];
    }
  }

  /* Get Timer Freq in HZ */
  freq = (UNS_64)cgu_get_clk_freq(CGU_SB_TIMER2_PCLK_ID);

  /* Find Min & Maximum Time in usec */
  min = ((((UNS_64)min) * 1000000)) / freq;
  max = ((((UNS_64)max) * 1000000)) / freq;

  if (erase == FALSE)
  {
    ave = sum / ((UNS_64)NUM_ITERATIONS);
    ave = (((UNS_64)ave) * 1000000) / freq;
    perf = (info.page_size * info.pages_per_block) / ave;
    sprintf(buff, "No. of Iterations: %u \r\n", NUM_ITERATIONS);
    uart_string_write(uartdev, buff);
  }
  else
  {
    ave = sum / ((UNS_64)NUM_ERASE_ITERATIONS);
    ave = (((UNS_64)ave) * 1000000) / freq;
    sprintf(buff, "No. of Iterations: %u \r\n", NUM_ERASE_ITERATIONS);
    uart_string_write(uartdev, buff);
  }

  sprintf(buff, "Minimum Time : %u usec \r\n", (unsigned int)min);
  uart_string_write(uartdev, buff);
  sprintf(buff, "Maximum Time : %u usec \r\n", (unsigned int)max);
  uart_string_write(uartdev, buff);
  sprintf(buff, "Average Time : %u usec \r\n", (unsigned int)ave);
  uart_string_write(uartdev, buff);

  if (erase == FALSE)
  {
    sprintf(buff, "Average Performance : %u MB/sec \r\n",
            (unsigned int)perf);
    uart_string_write(uartdev, buff);
  }
}

/***********************************************************************
 *
 * Function: nand_write_test
 *
 * Purpose: NAND Write test function
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *					dev: Device Descriptor
 *					blk: Block number to be written into
 *					dma: Use DMA flag
 *
 * Outputs: None
 *
 * Returns: TRUE for success, FALSE for failure
 *
 * Notes: None
 *
 **********************************************************************/
static STATUS nand_write_test(INT_32 dev, UNS_32 blk,
                              BOOL_32 dma)
{
  NAND_BB_CHECK_T bb_inf;
  UNS_32 m, j, k, blk_num = blk;
  NAND_BLOCKPAGE_T blkpage;
  STATUS status = _NO_ERROR;
  volatile BOOL_32 good;

  /* NAND Write */
  blkpage.dma = dma;
  for (k = 0; k < NUM_ITERATIONS; k++)
  {
    /* Fill up block buffer */
    for (m = 0; m < info.page_size; m++)
    {
      buf[m] = (UNS_8)(k + 1);
    }

    timer_start();

    /* Skip bad blocks */
    good = FALSE;
    while (good == FALSE)
    {
      bb_inf.blk_num = blk_num;
      status = nand_ioctl(dev, NAND_CHECK_BB, (INT_32) & bb_inf);
      if (status == _ERROR)
      {
        return _ERROR;
      }

      if (bb_inf.blk_bad == TRUE)
      {
        sprintf(buff, "Bad Block @ %u \r\n", (unsigned int)blk_num);
        uart_string_write(uartdev, buff);
        blk_num++;
        good = FALSE;
      }
      else
      {
        good = TRUE;
      }
    }

    blkpage.block_num = blk_num;
    blkpage.buffer = buf;
    if (dma == TRUE)
      blkpage.oobbuf = NULL;
    else
      blkpage.oobbuf = buf + info.page_size;

    start_time = TIMER_CNTR2->value;

    for (j = 0; j < info.pages_per_block; j++)
    {
      blkpage.page_num = j;
      status = nand_ioctl(dev, NAND_WRITE_PAGE, (INT_32) & blkpage);
      if (status == _ERROR)
      {
        return status;
      }
    }

    end_time = TIMER_CNTR2->value;

    if (start_time > end_time)
    {
      write[k] = start_time - end_time;
    }

    blk_num++;
  }

  return status;
}

/***********************************************************************
 *
 * Function: nand_read_test
 *
 * Purpose: NAND Read test function
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *					dev: Device Descriptor
 *					blk: Block number to be read from
 *					dma: Use DMA flag
 *
 * Outputs: None
 *
 * Returns: TRUE for success, FALSE for failure
 *
 * Notes: None
 *
 **********************************************************************/
static STATUS nand_read_test(INT_32 dev, UNS_32 blk,
                             BOOL_32 dma)
{
  NAND_BB_CHECK_T bb_inf;
  UNS_32 m, k, j, blk_num = blk;
  NAND_BLOCKPAGE_T blkpage;
  STATUS status = _NO_ERROR;
  volatile BOOL_32 good;

  /* NAND Write */
  for (k = 0; k < NUM_ITERATIONS; k++)
  {
    timer_start();

    /* Skip bad blocks */
    good = FALSE;
    while (good == FALSE)
    {
      bb_inf.blk_num = blk_num;
      status = nand_ioctl(dev, NAND_CHECK_BB, (INT_32) & bb_inf);
      if (status == _ERROR)
      {
        return _ERROR;
      }

      if (bb_inf.blk_bad == TRUE)
      {
        sprintf(buff, "Bad Block @ %u \r\n", (unsigned int)blk_num);
        uart_string_write(uartdev, buff);
        blk_num++;
        good = FALSE;
      }
      else
      {
        good = TRUE;
      }
    }

    /* Reset read buffer */
    memset(buf, 0xFF, sizeof(buf));

    blkpage.dma = dma;
    blkpage.block_num = blk_num;
    blkpage.buffer = buf;
    if (dma == TRUE)
      blkpage.oobbuf = NULL;
    else
      blkpage.oobbuf = buf + info.page_size;

    start_time = TIMER_CNTR2->value;

    for (j = 0; j < info.pages_per_block; j++)
    {
      blkpage.page_num = j;
      status = nand_ioctl(dev, NAND_READ_PAGE, (INT_32) & blkpage);
      if (status == _ERROR)
      {
        return status;
      }
    }

    end_time = TIMER_CNTR2->value;

    if (start_time > end_time)
    {
      read[k] = start_time - end_time;
    }

    for (m = 0; m < info.page_size; m++)
    {
      if (buf[m] != (UNS_8)(k + 1))
      {
        sprintf(buff, "Data mismatch @ blk_num: %d index: %d W: 0x%x R: 0x%x \r\n",
                (int)blk_num, (int)m, (unsigned int)(k + 1), (unsigned int)buf[m]);
        uart_string_write(uartdev, buff);
        return _ERROR;
      }
    }

    blk_num++;
  }

  return status;
}

/***********************************************************************
 *
 * Function: nand_perf_test
 *
 * Purpose: NAND Test function
 *
 * Processing:
 *     See function.
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
static void nand_perf_test(void)
{
  STATUS status;
  INT_32 dev;
  NAND_DEV_T cfg;
  UNS_32 i, k, j, blk_num;
  NAND_BB_CHECK_T bb_inf;
  BOOL_32 dma_array[] = {FALSE, TRUE};
  UNS_32 blk_array[] = {BLK_NUM, (BLK_NUM + 100)};
  volatile BOOL_32 good;

  /* Open NAND instance */
  cfg.cs = 0;
  cfg.interface_width = 8;
  dev = nand_open((void *)NAND_CTRL, (INT_32) & cfg);
  if (!dev)
  {
    sprintf(buff, "NAND open failed:0x%x \r\n", (unsigned int)dev);
    uart_string_write(uartdev, buff);
    return;
  }

  /* Read IDs */
  status = nand_ioctl(dev, NAND_GET_DEV_INFO, (INT_32) & info);
  if (status == _ERROR)
  {
    sprintf(buff, "NAND Get Dev Info failed:0x%x \r\n", (unsigned int)status);
    uart_string_write(uartdev, buff);
    nand_close(dev);
    return;
  }

  /* Print NAND Ids */
  sprintf(buff, "Nand Maf id:0x%02x\r\n", info.id[0]);
  uart_string_write(uartdev, buff);
  sprintf(buff, "Nand Dev id:0x%02x\r\n", info.id[1]);
  uart_string_write(uartdev, buff);
  sprintf(buff, "Nand Ext1 id:0x%02x\r\n", info.id[2]);
  uart_string_write(uartdev, buff);
  sprintf(buff, "Nand Ext2 id:0x%02x\r\n", info.id[3]);
  uart_string_write(uartdev, buff);
  sprintf(buff, "Nand page size:0x%04x\r\n", info.page_size);
  uart_string_write(uartdev, buff);
  sprintf(buff, "Nand OOB size:0x%04x\r\n", info.oob_size);
  uart_string_write(uartdev, buff);
  sprintf(buff, "Nand pages per block:0x%04x\r\n", info.pages_per_block);
  uart_string_write(uartdev, buff);
  sprintf(buff, "Nand No. of blocks:0x%04x\r\n", (unsigned int)info.nbr_of_blocks);
  uart_string_write(uartdev, buff);
  sprintf(buff, "------------------------------\r\n");
  uart_string_write(uartdev, buff);

  sprintf(buff, "NAND Erase Perf Measurements \r\n");
  uart_string_write(uartdev, buff);

  /* Measure Erase Time */
  for (j = 0; j < 2; j++)
  {
    blk_num = blk_array[j];
    for (k = 0; k < NUM_ITERATIONS; k++)
    {
      /* Start Timer */
      timer_start();

      good = FALSE;
      while (good == FALSE)
      {
        bb_inf.blk_num = blk_num;
        status = nand_ioctl(dev, NAND_CHECK_BB, (INT_32) & bb_inf);
        if (status == _ERROR)
        {
          nand_close(dev);
          return;
        }

        if (bb_inf.blk_bad == TRUE)
        {
          sprintf(buff, "Bad Block @ %u \r\n", (unsigned int)blk_num);
          uart_string_write(uartdev, buff);
          blk_num++;
          good = FALSE;
        }
        else
        {
          good = TRUE;
        }
      }

      /* Get Start Timer value */
      start_time = TIMER_CNTR2->value;

      /* Erase Good Block */
      status = nand_ioctl(dev, NAND_ERASE_BLOCK,
                          (INT_32) blk_num);

      /* Get End Timer value */
      end_time = TIMER_CNTR2->value;

      if (status == _ERROR)
      {
        sprintf(buff, "nand erase failed %u \r\n", (unsigned int)blk_num);
        uart_string_write(uartdev, buff);

        nand_close(dev);
        return;
      }

      if (start_time > end_time)
      {
        erase[k + (j * NUM_ITERATIONS)] = start_time - end_time;
      }

      /* Next Block */
      blk_num++;
    }
  }

  sprintf(buff, "Erase Time For 1 Block (in usec): \r\n");
  uart_string_write(uartdev, buff);

  /* Print Erase Timings */
  nand_print_stats((UNS_32 *)erase, TRUE);

  sprintf(buff, "----------------------------------------\r\n");
  uart_string_write(uartdev, buff);

  /* NAND write & read operations */
  for (i = 0; i < 2; i++)
  {
    sprintf(buff, "NAND Write using ");
    uart_string_write(uartdev, buff);

    if (dma_array[i] == FALSE)
    {
      sprintf(buff, "MEMCPY \r\n");
    }
    else
    {
      sprintf(buff, "DMA Ext Enable \r\n");
    }
    uart_string_write(uartdev, buff);
    sprintf(buff, "----------------------------------------\r\n");
    uart_string_write(uartdev, buff);

    status = nand_write_test(dev, blk_array[i], dma_array[i]);
    if (status == _ERROR)
    {
      nand_close(dev);
      return;
    }

    sprintf(buff, "Write Time For 1 Block (in usec): \r\n");
    uart_string_write(uartdev, buff);

    /* Print Write Timings */
    nand_print_stats((UNS_32 *)write, FALSE);

    sprintf(buff, "----------------------------------------\r\n");
    uart_string_write(uartdev, buff);

    sprintf(buff, "NAND Read using ");
    uart_string_write(uartdev, buff);

    if (dma_array[i] == FALSE)
    {
      sprintf(buff, "MEMCPY \r\n");
    }
    else
    {
      sprintf(buff, "DMA Ext Enable \r\n");
    }
    uart_string_write(uartdev, buff);
    status = nand_read_test(dev, blk_array[i], dma_array[i]);
    if (status == _ERROR)
    {
      nand_close(dev);
      return;
    }

    /* Print Read Timings */
    sprintf(buff, "Read Time For 1 Block (in usec): \r\n");
    uart_string_write(uartdev, buff);

    nand_print_stats((UNS_32 *)read, FALSE);

    sprintf(buff, "----------------------------------------\r\n");
    uart_string_write(uartdev, buff);
  }

  nand_close(dev);

  return;
}

