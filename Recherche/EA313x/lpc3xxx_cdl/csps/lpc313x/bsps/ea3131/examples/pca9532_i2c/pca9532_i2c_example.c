/***********************************************************************
 * $Id:: i2c_example.c 1390 2008-12-02 22:05:03Z tangdz                $
 *
 * Project: NXP LPC313x I2C example
 *
 * Description:
 *     This file contains a I2C code example to communicate with a
 *     external Serial EEPROM, PCF8594, From NXP Semiconductor.
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

#include "lpc_types.h"
#include "lpc_arm922t_cp15_driver.h"
#include "lpc_irq_fiq.h"
#include "lpc313x_chip.h"
#include "ea3131_board.h"
#include "lpc313x_intc_driver.h"
#include "lpc313x_cgu_driver.h"
#include "lpc313x_i2c_driver.h"
#include "lpc313x_pca9532_driver.h"

/***********************************************************************
 * I2C example definitions
 **********************************************************************/

/* I2C device handles */
static INT_32 i2cdev0;

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
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void c_entry(void)
{
  UNS_32 i;

  /* Disable interrupts in ARM core */
  disable_irq_fiq();

  /* Setup miscellaneous board functions */
  ea3131_board_init();

  /* Set virtual address of MMU table */
  cp15_set_vmmu_addr((void *)ISROM_MMU_TTB_BASE);

  /* Enable I2C system clock */
  cgu_clk_en_dis(CGU_SB_I2C0_PCLK_ID, TRUE);

  /* Initialize interrupt system */
  int_initialize(0xFFFFFFFF);

  /* install default I2C0 & I2C1 interrupt handlers */
  int_install_irq_handler(IRQ_I2C0, (PFV)i2c0_user_interrupt);

  /* Enable IRQ interrupts in the ARM core */
  enable_irq();

  /* Open I2C0 */
  i2cdev0 = pca9532_open((void *)I2C0, 0);

  /* The MSB of the argument(3rd parameter) is the command,
  the LSB of the argument is the data. */
  for (i = 0; i < 0x10; i++)
  {
    pca9532_ioctl(i2cdev0, PCA9532_SET_LED, 0x0855);
    pca9532_ioctl(i2cdev0, PCA9532_SET_LED, 0x0955);
    pca9532_ioctl(i2cdev0, PCA9532_SET_LED, 0x0800);
    pca9532_ioctl(i2cdev0, PCA9532_SET_LED, 0x0900);
    pca9532_ioctl(i2cdev0, PCA9532_SET_LED, 0x08AA);
    pca9532_ioctl(i2cdev0, PCA9532_SET_LED, 0x09AA);
    pca9532_ioctl(i2cdev0, PCA9532_SET_LED, 0x0800);
    pca9532_ioctl(i2cdev0, PCA9532_SET_LED, 0x0900);
  }

  pca9532_close(i2cdev0);

  return;
}
