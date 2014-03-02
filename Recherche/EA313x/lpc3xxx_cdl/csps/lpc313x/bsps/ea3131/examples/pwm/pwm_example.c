/***********************************************************************
 * $Id:: pwm_example.c 1519 2008-12-17 23:16:59Z pdurgesh              $
 *
 * Project: NXP LPC313x PWM example
 *
 * Description:
 *     This file contains a PWM code example
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
#include "lpc_irq_fiq.h"
#include "lpc_arm922t_cp15_driver.h"
#include "lpc313x_chip.h"
#include "ea3131_board.h"
#include "lpc313x_intc_driver.h"
#include "lpc313x_cgu_driver.h"
#include "lpc313x_pwm_driver.h"


/* PWM device handles */
static INT_32 pwmdev;

/***********************************************************************
 *
 * Function: delay
 *
 * Purpose: generate a delay
 *
 * Processing:
 *     A local software counter counts up to the specified count.
 *
 * Parameters:
 *    cnt : number to be counted
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 **********************************************************************/
void delay(UNS_32 cnt)
{
  UNS_32 i = cnt;
  while (i != 0) i--;
  return;
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
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void c_entry(void)
{
  /* Disable interrupts in ARM core */
  disable_irq_fiq();

  /* Setup miscellaneous board functions */
  ea3131_board_init();

  /* Set virtual address of MMU table */
  cp15_set_vmmu_addr((void *)ISROM_MMU_TTB_BASE);

  /* Initialize interrupt system */
  int_initialize(0xFFFFFFFF);

  /* Enable IRQ interrupts in the ARM core */
  enable_irq();

  /* Open PWM */
  pwmdev = pwm_open((void *)PWM, 0);

  if (pwmdev != (INT_32)NULL)
  {
    /* generate 10% duty cycle output and wait for a while */
    pwm_ioctl(pwmdev, PWM_DUTY_R, 10);  //set duty cycle
    delay(100000);                      //wait for a while

    /* generate 75% duty cycle output and wait for a while */
    pwm_ioctl(pwmdev, PWM_DUTY_R, 75);  //set duty cycle
    delay(100000);                      //wait for a while

    /* Close PWM */
    pwm_close(pwmdev);
  }

  return;
}
