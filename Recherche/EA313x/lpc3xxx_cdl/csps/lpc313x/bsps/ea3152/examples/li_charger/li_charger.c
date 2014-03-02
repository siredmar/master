/***********************************************************************
 * $Id:: li_charger.c 6161 2011-01-12 06:25:31Z ing02124               $
 *
 * Project: Lion Charger example
 *
 * Description:
 *     A simple Lion Charger example.
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
#include "lpc313x_uart_driver.h"
#include "lpc313x_cgu_driver.h"
#include "lpc315x_psu_driver.h"
#include "lpc313x_intc_driver.h"
#include "lpc313x_intc.h"
#include "lpc315x_analog.h"

extern void analog_reg_read(UNS_32 reg_addr, UNS_32 * pReg_value);
extern void analog_reg_write(UNS_32 reg_addr, UNS_32 reg_value);


static INT_32 uartdev;
char buff[512];

void uart_string_write(char *pbuff)
{
  UNS_32 len = strlen(pbuff);
  UNS_32 written;

  while (len > 0)
  {
    written = uart_write(uartdev, pbuff, len);
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
  UNS_32 analog;
  UNS_32 reg_value;

  /* Disable interrupts in ARM core */
  disable_irq_fiq();

  /* Set virtual address of MMU table */
  cp15_set_vmmu_addr((void *)ISROM_MMU_TTB_BASE);

  /* Initialize interrupt system */
  int_initialize(0xFFFFFFFF);

  enable_irq();

  uartdev = uart_open((void *) UART_BASE, 0);

  if (uartdev != 0)
  {
    uart_string_write("LIC demo \r\n");
  }

  /* Power up LI block */
  analog_reg_read(0x2, &reg_value);

  reg_value &= ~CGU_RW_PD_LIC_CLK;

  analog_reg_write(0x2, reg_value);

  /* Detect USB VBUS */
  analog_reg_read(0x1, &reg_value);

  if (reg_value & DCDCLIC_RO_DCDC_CTRL_USB_DETECT)
  {

    uart_string_write("enabling the charger \r\n");
    analog_reg_read(0x0, &analog);
    analog |= (1 << 12);
    analog_reg_write(0x0, analog);
  }

  do
  {
    analog_reg_read(0x1, &analog);

    if (analog & 0x10)
      uart_string_write("Charging in trickle charge mode \r\n");

    else if (analog & 0x8)
      uart_string_write("Charging in Fast charge mode \r\n");

    else if (analog & 0x4)
      uart_string_write("Charging in Constant voltage charge mode \r\n");

  }
  while (!(analog & 0x20)); // Battery Full

  uart_string_write("Battery full ! \r\n");

  return 1;
}
