/***********************************************************************
 * $Id:: timer_example.c 6152 2011-01-12 04:02:51Z ing02124            $
 *
 * Project: Timer driver example
 *
 * Description:
 *     A simple timer driver example.
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
#include "lpc313x_intc_driver.h"
#include "lpc313x_ioconf_driver.h"
#include "lpc313x_uart_driver.h"

#ifdef offsetof
#undef offsetof
#endif

#define offsetof(s,m)   (int)&(((s *)0)->m)
#define COMPILE_TIME_ASSERT(expr)   char constraint[expr]

COMPILE_TIME_ASSERT(offsetof(INTC_REGS_T, request[1]) == 0x404);
COMPILE_TIME_ASSERT(offsetof(INTC_REGS_T, request[29]) == 0x474);
COMPILE_TIME_ASSERT(offsetof(INTC_REGS_T, modId) == 0xFFC);

/* Timer device handles */
static INT_32 timer0dev;

/* Number of milliSeconds */
static volatile INT_32 msecs;


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
  static int state = 0;
  /* Clear latched timer interrupt */
  timer_ioctl(timer0dev, TMR_CLEAR_INTS, 1);

  /* Toggle mode0 LED */
  if (state)
    gpio_set_outpin_high(IOCONF_GPIO, 2);
  else
    gpio_set_outpin_low(IOCONF_GPIO, 2);

  state ^= 1;

  msecs += 100;
}


/***********************************************************************
 *
 * Function: uart_string_write
 *
 * Purpose: UART string output helper function
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
 ***********************************************************************/
static void uart_string_write(INT_32 devid, char *pbuff)
{
  UNS_32 len = strlen(pbuff);
  UNS_32 written;

  if (devid != 0)
  {
    while (len > 0)
    {
      written = uart_write(devid, pbuff, len);
      len -= written;
      pbuff += written;
    }
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
  char buff[512];

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

  /* Open timers - this will enable the clocks for the timers */
  timer0dev = timer_open((void *)TIMER_CNTR0, 0);

  /******************************************************************/
  /* Setup timer 0 for a 10Hz match rate                            */

  /* Set timer load register to 100mS (10Hz))  */
  timer_ioctl(timer0dev, TMR_SET_MSECS, 100);
  timer_ioctl(timer0dev, TMR_SET_PERIODIC_MODE, 0);
  /* Clear any latched timer 0 interrupts and enable match
   interrupt */
  timer_ioctl(timer0dev, TMR_CLEAR_INTS, 1);
  /******************************************************************/

  /******************************************************************/
  /* Setup UART for 115.2K, 8 data bits, no parity, 1 stop bit */
  uartdev = uart_open((void *) UART_BASE, 0);
  sprintf(buff, "LPC313x timer test\r\n");
  uart_string_write(uartdev, buff);

  /* Enable timers (starts counting) */
  msecs = 0;
  timer_ioctl(timer0dev, TMR_ENABLE, 1);

  /* Enable timer interrupts in the interrupt controller */
  int_enable(IRQ_TIMER0);

  /* Enable IRQ interrupts in the ARM core */
  enable_irq();

  /* Loop for 20 seconds and let interrupts toggle the LEDs */
  //while (msecs < (10 * 1000));
  while (1)
  {
    if ((msecs % 1000) == 0)
    {
      sprintf(buff, "msecs: %d\r\n", (int)msecs);
      uart_string_write(uartdev, buff);
    }
  }

  /* Disable timer interrupts in the interrupt controller */
  int_disable(IRQ_TIMER0);

  /* Disable interrupts in ARM core */
  disable_irq_fiq();

  /* Close timers */
  timer_close(timer0dev);

  return 1;
}
