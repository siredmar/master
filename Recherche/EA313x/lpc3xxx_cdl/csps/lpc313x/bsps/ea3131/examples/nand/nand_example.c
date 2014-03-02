/***********************************************************************
 * $Id:: nand_example.c 6148 2011-01-11 13:55:39Z ing02124             $
 *
 * Project: NAND driver example
 *
 * Description:
 *     A simple NAND driver example.
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
#include "lpc313x_intc_driver.h"
#include "lpc31xx_vmem_driver.h"

#define BLK_NUM (20)
#define MARK_BB_NUM (50)

static DMA_BUFFER UNS_8	buf[2112];
static DMA_BUFFER UNS_8	buf_r[2112];
static char buff[512];
static INT_32 uartdev;
static NAND_DEV_INFO_T	info;
volatile static UNS_32 num_bbs = 0, bb_num = 0;

static void nand_test(void);

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
    sprintf(buff, "---------------------\r\n");
    uart_string_write(uartdev, buff);
    sprintf(buff, "LPC3131 nand test\r\n");
    uart_string_write(uartdev, buff);
    sprintf(buff, "---------------------\r\n");
    uart_string_write(uartdev, buff);

    nand_test();

    sprintf(buff, "End: LPC3131 nand test\r\n");
    uart_string_write(uartdev, buff);
    sprintf(buff, "---------------------\r\n");
    uart_string_write(uartdev, buff);

    uart_close(uartdev);
  }

  return 1;
}

/***********************************************************************
 *
 * Function: nand_print_bbs
 *
 * Purpose: Print Bad Blocks function
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *						dev: Device Descriptor
 *
 * Outputs: None
 *
 * Returns: TRUE for success, FALSE for failure
 *
 * Notes: None
 *
 **********************************************************************/
static STATUS nand_print_bbs(INT_32 dev)
{
  UNS_32 blk_num;
  NAND_BB_CHECK_T bb_inf;
  STATUS status = _NO_ERROR;

  /* Print all the Bad Blocks */
  for (blk_num = 0; blk_num < info.nbr_of_blocks; blk_num++)
  {
    /* NAND Check bad block */
    bb_inf.blk_num = blk_num;
    status = nand_ioctl(dev, NAND_CHECK_BB, (INT_32) & bb_inf);
    if (status == _ERROR)
    {
      sprintf(buff, "NAND Check bad block failed for %d \r\n", (int)blk_num);
      uart_string_write(uartdev, buff);
      return _ERROR;
    }

    if (bb_inf.blk_bad == TRUE)
    {
      sprintf(buff, "Bad Block @ %d \r\n", (int)blk_num);
      uart_string_write(uartdev, buff);
      num_bbs++;
      bb_num = blk_num;
    }
  }

  return status;
}

/***********************************************************************
 *
 * Function: nand_erase_test
 *
 * Purpose: NAND Erase test function
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *					dev: Device Descriptor
 *					blk: Block number to be erased
 *
 * Outputs: None
 *
 * Returns: TRUE for success, FALSE for failure
 *
 * Notes: None
 *
 **********************************************************************/
static STATUS nand_erase_test(INT_32 dev,
                              UNS_32 blk)
{
  UNS_32 i;
  NAND_BLOCKPAGE_T blkpage;
  STATUS status = _NO_ERROR;

  /* NAND Erase */
  status = nand_ioctl(dev, NAND_ERASE_BLOCK, (INT_32)blk);
  if (status == _ERROR)
  {
    return status;
  }

  /* NAND Read */
  memset(buf_r, 0x00, info.page_size + info.oob_size);
  blkpage.block_num = blk;
  blkpage.dma = FALSE;
  blkpage.page_num = 0;
  blkpage.buffer = buf_r;
  blkpage.oobbuf = buf_r + info.page_size;
  status = nand_ioctl(dev, NAND_READ_PAGE, (INT_32) & blkpage);
  /* Ignore status, since empty page can cause ECC failures */
  /* Compare Data */
  for (i = 0; i < info.page_size; i++)
  {
    if (buf_r[i] != 0xFF)
    {
      sprintf(buff, "NAND erase data  mismatch: %d 0x%x 0x%x \r\n",
              (int)i, (unsigned int)buf_r[i], 0xFF);
      uart_string_write(uartdev, buff);
      return _ERROR;
    }
  }

  return _NO_ERROR;
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
  NAND_BLOCKPAGE_T blkpage;
  STATUS status = _NO_ERROR;

  /* NAND Write */
  blkpage.dma = dma;
  blkpage.block_num = blk;
  blkpage.page_num = 0;
  blkpage.buffer = buf;
  if (dma == TRUE)
    blkpage.oobbuf = NULL;
  else
    blkpage.oobbuf = buf + info.page_size;
  status = nand_ioctl(dev, NAND_WRITE_PAGE, (INT_32) & blkpage);

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
  NAND_BLOCKPAGE_T blkpage;
  STATUS status = _NO_ERROR;

  /* NAND Read */
  blkpage.dma = dma;
  blkpage.block_num = blk;
  blkpage.page_num = 0;
  blkpage.buffer = buf_r;
  if (dma == TRUE)
    blkpage.oobbuf = NULL;
  else
    blkpage.oobbuf = buf_r + info.page_size;
  status = nand_ioctl(dev, NAND_READ_PAGE, (INT_32) & blkpage);

  return status;
}

/***********************************************************************
 *
 * Function: nand_markbb_test
 *
 * Purpose: NAND Mark Bad Block test function
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *					dev: Device Descriptor
 *					blk: Block number to be marked as bad
 *
 * Outputs: None
 *
 * Returns: TRUE for success, FALSE for failure
 *
 * Notes: None
 *
 **********************************************************************/
static STATUS nand_markbb_test(INT_32 dev, UNS_32 blk)
{
  STATUS status = _NO_ERROR;

  /* NAND Mark bad block */
  status = nand_ioctl(dev, NAND_MARK_BB, (INT_32)blk);

  return status;
}

/***********************************************************************
 *
 * Function: nand_test
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
static void nand_test(void)
{
  STATUS status;
  INT_32 dev;
  NAND_DEV_T cfg;
  UNS_8 ready;
  UNS_16 i, j;
  BOOL_32 dma_array[] = {FALSE, TRUE};
  UNS_32 blk_array[] = {BLK_NUM, (BLK_NUM + 1)};

  sprintf(buff, "---------------------\r\n");
  uart_string_write(uartdev, buff);
  sprintf(buff, "NAND open: ");
  uart_string_write(uartdev, buff);

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
  sprintf(buff, "Passed \r\n");
  uart_string_write(uartdev, buff);

  sprintf(buff, "---------------------\r\n");
  uart_string_write(uartdev, buff);
  sprintf(buff, "NAND Get Dev Info: ");
  uart_string_write(uartdev, buff);

  /* Read IDs */
  status = nand_ioctl(dev, NAND_GET_DEV_INFO, (INT_32) & info);
  if (status == _ERROR)
  {
    sprintf(buff, "NAND Get Dev Info failed:0x%x \r\n", (unsigned int)status);
    uart_string_write(uartdev, buff);
    nand_close(dev);
    return;
  }
  sprintf(buff, "Passed \r\n");
  uart_string_write(uartdev, buff);

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
  sprintf(buff, "---------------------\r\n");
  uart_string_write(uartdev, buff);

  sprintf(buff, "NAND Device Get Status: ");
  uart_string_write(uartdev, buff);
  status = nand_ioctl(dev, NAND_GET_STATUS, (INT_32) & ready);
  if (status == _ERROR)
  {
    sprintf(buff, "NAND Get status failed:0x%x \r\n", (unsigned int)status);
    uart_string_write(uartdev, buff);
    nand_close(dev);
    return;
  }
  sprintf(buff, "Passed \r\n");
  uart_string_write(uartdev, buff);

  /* print chip status */
  if (ready == NAND_READY)
  {
    sprintf(buff, "NAND status: Ready \r\n");
  }
  else
  {
    sprintf(buff, "NAND status: Busy \r\n");
  }
  uart_string_write(uartdev, buff);
  sprintf(buff, "---------------------\r\n");
  uart_string_write(uartdev, buff);


  /* NAND Erase, write & read operations */
  for (i = 0; i < 2; i++)
  {
    sprintf(buff, "---------------------------------------\r\n");
    uart_string_write(uartdev, buff);
    sprintf(buff, "NAND Erase of good block %d : ", (int)blk_array[i]);
    uart_string_write(uartdev, buff);
    status = nand_erase_test(dev, blk_array[i]);
    if (status == _ERROR)
    {
      nand_close(dev);
      return;
    }
    sprintf(buff, "Passed\r\n");
    uart_string_write(uartdev, buff);
    sprintf(buff, "----------------------------------------\r\n");
    uart_string_write(uartdev, buff);

    sprintf(buff, "NAND Write using ");
    uart_string_write(uartdev, buff);
    if (dma_array[i] == FALSE)
    {
      sprintf(buff, "MEMCPY: ");
      for (j = 0; j < (info.page_size + info.oob_size); j++)
      {
        buf[j] = (UNS_8)j;
        buf_r[j] = 0;
      }
    }
    else
    {
      sprintf(buff, "DMA Ext Enable: ");
      for (j = 0; j < (info.page_size + info.oob_size); j++)
      {
        buf[j] = (UNS_8)(256 - j);
        buf_r[j] = 0;
      }
    }
    uart_string_write(uartdev, buff);
    status = nand_write_test(dev, blk_array[i], dma_array[i]);
    if (status == _ERROR)
    {
      nand_close(dev);
      return;
    }

    sprintf(buff, "Passed \r\n");
    uart_string_write(uartdev, buff);
    sprintf(buff, "----------------------------------------\r\n");
    uart_string_write(uartdev, buff);

    sprintf(buff, "NAND Read using ");
    uart_string_write(uartdev, buff);

    if (dma_array[i] == FALSE)
    {
      sprintf(buff, "MEMCPY: ");
    }
    else
    {
      sprintf(buff, "DMA Ext Enable: ");
    }
    uart_string_write(uartdev, buff);
    status = nand_read_test(dev, blk_array[i], dma_array[i]);
    if (status == _ERROR)
    {
      nand_close(dev);
      return;
    }

    /* Compare data */
    for (j = 0; j < info.page_size; j++)
    {
      if (buf[j] != buf_r[j])
      {
        sprintf(buff, "NAND Data mismtach:%d 0x%x 0x%x \r\n",
                j, buf[j], buf_r[j]);
        uart_string_write(uartdev, buff);
        nand_close(dev);
        return;
      }
    }

    sprintf(buff, "Passed \r\n");
    uart_string_write(uartdev, buff);
    sprintf(buff, "----------------------------------------\r\n");
    uart_string_write(uartdev, buff);
  }

  sprintf(buff, "Print NAND bad blocks information: \r\n");
  uart_string_write(uartdev, buff);

  /* Check BB Tests */
  status = nand_print_bbs(dev);
  if (status == _ERROR)
  {
    nand_close(dev);
    return;
  }

  sprintf(buff, "----------------------------------------\r\n");
  uart_string_write(uartdev, buff);

  /* Mark Block as bad */
  sprintf(buff, "NAND Mark block %d as BAD: ", MARK_BB_NUM);
  uart_string_write(uartdev, buff);
  status = nand_markbb_test(dev, MARK_BB_NUM);
  if (status == _ERROR)
  {
    nand_close(dev);
    return;
  }
  sprintf(buff, "Passed \r\n");
  uart_string_write(uartdev, buff);

  nand_close(dev);

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

  /* if BAD blocks are present, Execute Bad block Tests */
  if (num_bbs != 0)
  {
    sprintf(buff, "---------------------------------------\r\n");
    uart_string_write(uartdev, buff);
    sprintf(buff, "NAND Erase of BAD block %d : ", (int)bb_num);
    uart_string_write(uartdev, buff);
    status = nand_erase_test(dev, bb_num);
    if (status == _NO_ERROR)
    {
      nand_close(dev);
      return;
    }
    sprintf(buff, "Passed \r\n");
    uart_string_write(uartdev, buff);
    sprintf(buff, "----------------------------------------\r\n");
    uart_string_write(uartdev, buff);

    sprintf(buff, "NAND Write into BAD block %d : ", (int)bb_num);
    uart_string_write(uartdev, buff);
    status = nand_write_test(dev, bb_num, TRUE);
    if (status == _NO_ERROR)
    {
      nand_close(dev);
      return;
    }

    sprintf(buff, "Passed \r\n");
    uart_string_write(uartdev, buff);
    sprintf(buff, "----------------------------------------\r\n");
    uart_string_write(uartdev, buff);

    sprintf(buff, "NAND Read from BAD block %d : ", (int)bb_num);
    uart_string_write(uartdev, buff);
    status = nand_read_test(dev, bb_num, FALSE);
    if (status == _NO_ERROR)
    {
      nand_close(dev);
      return;
    }

    sprintf(buff, "Passed \r\n");
    uart_string_write(uartdev, buff);
    sprintf(buff, "----------------------------------------\r\n");
    uart_string_write(uartdev, buff);
  }

  nand_close(dev);

  return;
}

