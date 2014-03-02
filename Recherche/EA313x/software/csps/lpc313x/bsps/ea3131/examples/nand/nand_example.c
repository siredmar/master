/***********************************************************************
 * $Id:: nand_example.c 1529 2009-01-06 02:08:40Z pdurgesh             $
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
#include "lpc_nandflash_params.h"
#include "ea3131_board.h"
#include "lpc313x_nandc.h"
#include "lpc313x_sysreg.h"
#include "lpc313x_uart_driver.h"
#include "lpc313x_cgu_driver.h"

/* UART device handles */
static char sendstr[] =
  "Press any key to have it read in from the terminal and\n\r"
  "returned back to the terminal. Press ESC to exit.\n\r";

static void nand_test(char* buff, INT_32 uartdev);

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
  INT_32 uartdev;
  UART_CONTROL_T ucntl;
  char buff[512];

  /* Disable interrupts in ARM core */
  disable_irq_fiq();

  /* Setup miscellaneous board functions */
  ea3131_board_init();
  /* enable clock_out */
  cgu_clk_en_dis(CGU_SB_CLOCK_OUT_ID, 1);

  /* Set virtual address of MMU table */
  cp15_set_vmmu_addr((void *)ISROM_MMU_TTB_BASE);

  /* Setup UART for 115.2K, 8 data bits, no parity, 1 stop bit */
  ucntl.baud_rate = 115200;
  ucntl.parity = UART_PAR_NONE;
  ucntl.databits = 8;
  ucntl.stopbits = 1;
  uartdev = uart_open((void *) UART_BASE, (INT_32) & ucntl);
  if (uartdev != 0)
  {
    sprintf(buff, "LPC3131 nand test\r\n");
    uart_string_write(uartdev, buff);

    nand_test(buff, uartdev);

    uart_close(uartdev);
  }

  return 1;
}

static void nand_test(char* buff, INT_32 uartdev)
{
  UNS_32 tmp, data_read, id = 0;
  /* enable NAND clocks */
  cgu_clk_en_dis(CGU_SB_NANDFLASH_S0_CLK_ID, TRUE);
  cgu_clk_en_dis(CGU_SB_NANDFLASH_ECC_CLK_ID, TRUE);
  cgu_clk_en_dis(CGU_SB_NANDFLASH_NAND_CLK_ID, TRUE);
  cgu_clk_en_dis(CGU_SB_NANDFLASH_PCLK_ID, TRUE);
  /* needed for LPC315x series only */
  cgu_clk_en_dis(CGU_SB_NANDFLASH_AES_CLK_ID, TRUE);
  /* reset NAND controller */
  cgu_soft_reset_module(NANDFLASH_NAND_RESET_N_SOFT);
  cgu_soft_reset_module(NANDFLASH_ECC_RESET_N_SOFT);
  /* need for LPC315x series only */
  cgu_soft_reset_module(NANDFLASH_AES_RESET_N_SOFT);

  /* check NAND mux signals */
  SYS_REGS->mux_nand_mci_sel = 0;

  /* select bank0*/
  NAND_CTRL->set_ce = NAND_SETCE_WP | NAND_SETCE_CV(0);
  NAND_CTRL->config = NAND_CFG_EC | NAND_CFG_DC;
  /* set 8 bit device */
  NAND_CTRL->config = NAND_CFG_EC | NAND_CFG_DC;
  // clear status
  tmp = NAND_CTRL->check_sts;
  /* reset nand */
  NAND_CTRL->set_cmd = NAND_CMD_RESET;
  // wait for device ready
  while (!((tmp = NAND_CTRL->check_sts) & NAND_CHK_STS_RB1_EDGE));

  NAND_CTRL->set_cmd = NAND_CMD_READID;
  NAND_CTRL->set_addr = 0;
  for (tmp = 0; tmp<4 ;tmp++)
  {
    data_read = NAND_CTRL->read_data;
    id = (id << 8) |  (data_read & 0x000000ff);
  }

  sprintf(buff, "nand id:0x%08x\r\n", id);
  while (buff[0] != 27)
  {
    if ((tmp = uart_read(uartdev, &buff[0], 16)) > 0)
    {
      break;
    }
    uart_string_write(uartdev, buff);
  }



}

