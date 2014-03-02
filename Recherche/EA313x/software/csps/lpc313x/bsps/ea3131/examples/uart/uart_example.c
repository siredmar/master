/***********************************************************************
 * $Id:: uart_example.c 1659 2009-02-06 17:46:20Z pdurgesh             $
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
#include "lpc313x_wdt.h"
#include "lpc313x_uart_driver.h"
#include "lpc313x_cgu_driver.h"

/* UART device handles */
static char sendstr[] =
  "Press any key to have it read in from the terminal and\n\r"
  "returned back to the terminal. Press ESC to exit.\n\r";

static void wdt_test(char* buff, INT_32 uartdev);

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
  UNS_8 ch[16];
  UNS_32 bytes = 0;
  char buff[512];
  CGU_CLOCK_ID_T i;

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
    sprintf(buff, "LPC3131 clock rates1\r\n");
    uart_string_write(uartdev, buff);
    for (i = CGU_SYS_FIRST; i < (CGU_SYSCLK_O_LAST + 1); i++)
    {
      sprintf(buff, "clock[%d]: %d\r\n", i, cgu_get_clk_freq(i));
      uart_string_write(uartdev, buff);
    }

    /* Loop returning the data from the terminal */
    uart_string_write(uartdev, sendstr);
    ch[0] = 0;
    while (ch[0] != 27)
    {
      if ((bytes = uart_read(uartdev, &ch[0], 16)) > 0)
      {
        uart_write(uartdev, &ch[0], bytes);
      }
    }

    wdt_test(buff, uartdev);

    uart_close(uartdev);
  }

  return 1;
}

static void wdt_test(char* buff, INT_32 uartdev)
{
  volatile UNS_32 i = 0;

  /* enable WDT clock */
  cgu_clk_en_dis(CGU_SB_WDOG_PCLK_ID, 1);

  WDT->tcr = 0;
  WDT->mcr = WDT_MCR_STOP_MR1 | WDT_MCR_INT_MR1;

  sprintf(buff, "Wdog Reset enable\r\n");
  uart_string_write(uartdev, buff);

  while (i++ < 0x1000);

  /*  If TC and MR1 are equal a reset is generated. */
  WDT->pr  = 0x00000002;
  WDT->tc  = 0x00000FF0;
  WDT->mr0 = 0x0000F000;
  WDT->mr1 = 0x00001000;
  WDT->emr = WDT_EMR_CTRL1(0x3);
  // start the counter running
  WDT->tcr = WDT_TCR_CNT_EN;
  i = 0;
  do
  {
    i++;
    sprintf(buff, "Waiting... %d\n", 0x1000 - WDT->tc);
    uart_string_write(uartdev, buff);
  }
  while (i < 0x1000);
}