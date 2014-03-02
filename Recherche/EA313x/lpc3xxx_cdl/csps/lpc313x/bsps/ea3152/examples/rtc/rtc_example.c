/***********************************************************************
 * $Id:: rtc_example.c 6162 2011-01-12 06:30:33Z ing02124              $
 *
 * Project: NXP LPC315x RTC example
 *
 * Description:
 *     This file contains a RTC code example to communicate with the
 *     RTC module on the analog die(LPC315x), From NXP Semiconductor.
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
#include "lpc_arm922t_cp15_driver.h"
#include "lpc_irq_fiq.h"
#include "lpc313x_chip.h"
#include "lpc313x_intc_driver.h"
#include "lpc313x_cgu_driver.h"
#include "lpc315x_analog.h"
#include "lpc313x_mstr_i2c_driver.h"
#include "lpc313x_evt_driver.h"
#include "lpc315x_rtc_driver.h"
#include "lpc313x_uart_driver.h"


/* UART Handle */
static INT_32 g_uartdev;
static char buff[512];

/* RTC device handles */
static INT_32 rtcdev;
volatile static UNS_32 alarm_count = 0;

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
 * Function: evt_user_interrupt
 *
 * Purpose: Handling interrupt from event router
 *
 * Processing:
 *     Process an event router interrupt
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
void evt2_user_interrupt(void)
{
  if (evtr_int_pending(EVT_AD_NINT_I))
  {
    alarm_count++;
    /* clear RTC on the event router if it's a latched interrupt. */
    evtr_int_clear(EVT_AD_NINT_I);
  }

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
  UNS_32 day, time, set_time;
  UNS_32 alarm_day, alarm_time;
  volatile UNS_32 current_day, current_time;
  volatile UNS_32 reg_value = 0, prv_val = 0;
  static RTC_SETUP_T rtc_setup;
  static EVENT_SETUP_T evt_setup;
  STATUS status;

  /* Without ref. time table, the 32-bit RTC time will overflow.
  This value is TBD. */
  RTC_TIME_T ref_time_table = { 0, 0, 0, 1, 1, 1970 };
  RTC_TIME_T time_table = { 0, 0, 0, 1, 1, 2011 };
  RTC_TIME_T alarm_table = { 0, 1, 0, 1, 1, 2011 };
  UNS_8 month_day_table[12] = { 31, 28, 31, 30, 31, 30,
                                31, 31, 30, 31, 30, 31
                              };

  /* Disable interrupts in ARM core */
  disable_irq_fiq();

  /* Set virtual address of MMU table */
  cp15_set_vmmu_addr((void *)ISROM_MMU_TTB_BASE);

  /* Enable SYSCLK_O */
  cgu_clk_en_dis(CGU_SB_SYSCLK_O_ID, TRUE);

  /* Enable I2C1 clock */
  cgu_clk_en_dis(CGU_SB_I2C1_PCLK_ID, TRUE);

  /* Initialise Event Router Interrupts */
  evtr_int_initialize();

  /* Initialize interrupt system */
  int_initialize(0xFFFFFFFF);

  /* install default I2C1 interrupt handler */
  int_install_irq_handler(IRQ_I2C1, (PFV)i2c1_mstr_int_hanlder);

  /* Enable IRQ interrupts in the ARM core */
  enable_irq();
  int_enable(IRQ_I2C1);

  /* Open UART */
  g_uartdev = uart_open((void *) UART_BASE, 0);
  if (g_uartdev == 0)
  {
    /* Return if failure */
    return;
  }

  uart_string_write("--------------------------- \r\n");
  uart_string_write("RTC Example Application: \r\n");
  uart_string_write("--------------------------- \r\n");

  /* Open RTC instance */
  if (rtc_open((void *)I2C1, 0))
  {
    uart_string_write("Unable to open rtc driver \r\n");
    return;
  }

  /* Clear Alarm interrupts, if already set */
  analogdie_reg_write(AD_REG_RTC_CLR_ENA_STAT,
                      (RTC_ALARM | RTC_AD_INT_EN));

  /* Setup event router */
  evt_setup.pin = EVT_AD_NINT_I;
  evt_setup.sense = RISING_EDGE;
  /* Event router to INTOUT 2 */
  evt_setup.vector = EVT_VEC_ROUTER2;
  evt_setup.func_ptr = (PFV)evt2_user_interrupt;
  evtr_int_install_handler((EVENT_SETUP_T *)&evt_setup);
  evtr_int_clear(EVT_AD_NINT_I);
  evtr_int_enable(EVT_AD_NINT_I);

  /* Setup RTC time first. */
  time = ((time_table.rtc_hour - ref_time_table.rtc_hour) * 3600) +
         ((time_table.rtc_minute - ref_time_table.rtc_minute) * 60) +
         (time_table.rtc_second - ref_time_table.rtc_second);
  if (((time_table.rtc_year % 4) == 0) &&
      ((time_table.rtc_year % 100) != 0) &&
      ((time_table.rtc_year % 400) == 0))
  {
    /* this is a leap year. 29 days in Feb, 366 days/year */
    month_day_table[1] = 29;
    day = (time_table.rtc_year - ref_time_table.rtc_year) * 366 +
          (month_day_table[time_table.rtc_month-1] -
           month_day_table[ref_time_table.rtc_month-1]) +
          (time_table.rtc_day - ref_time_table.rtc_day);
  }
  else
  {
    day = (time_table.rtc_year - ref_time_table.rtc_year) * 365 +
          (month_day_table[time_table.rtc_month-1] -
           month_day_table[ref_time_table.rtc_month-1]) +
          (time_table.rtc_day - ref_time_table.rtc_day);
  }

  uart_string_write("Current time set in RTC:  \r\n");
  sprintf(buff, "Year/Month/Day  Hours:Minutes:Seconds Format \r\n");
  uart_string_write(buff);
  sprintf(buff, "%4d/%2d/%2d  %2d:%2d:%2d \r\n",
          (int)time_table.rtc_year, (int)time_table.rtc_month,
          (int)time_table.rtc_day, (int)time_table.rtc_hour,
          (int)time_table.rtc_minute, (int)time_table.rtc_second);
  uart_string_write(buff);
  set_time = (86400 * day) + time;
  sprintf(buff, "Current time (in secs): %d \r\n", (int)set_time);
  uart_string_write(buff);
  uart_string_write("--------------------------- \r\n");

  /* Set time */
  rtc_setup.tx_data = set_time;
  rtc_setup.rx_data = 0;
  status = rtc_ioctl(rtcdev, RTC_SETUP_TIME, (RTC_SETUP_T *) & rtc_setup);
  if (status == _ERROR)
  {
    uart_string_write("RTC: Setup Time Failed \r\n");
    rtc_close(rtcdev);
    return;
  }

  /* Set the TIME_UNSET bit. This is for debugging purpose,
  otherwise, the RTC time will go on forever. */
  rtc_setup.tx_data = RTC_TIME_UNSET;
  rtc_setup.rx_data = 0;
  status  = rtc_ioctl(rtcdev, RTC_ENA_SET, (RTC_SETUP_T *) & rtc_setup);
  if (status == _ERROR)
  {
    uart_string_write("RTC: Enable Set Failed \r\n");
    rtc_close(rtcdev);
    return;
  }

  /* Read RTC status */
  rtc_setup.tx_data = 0x0;	/* dummy data */
  rtc_setup.rx_data = 0;	/* Status will be updated here. */
  status = rtc_ioctl(rtcdev, RTC_GET_STATUS, (RTC_SETUP_T *) & rtc_setup);
  if (status == _ERROR)
  {
    uart_string_write("RTC: Get Status Failed \r\n");
    rtc_close(rtcdev);
    return;
  }

  /* Read Time */
  rtc_setup.tx_data = 0x0;	/* dummy data */
  rtc_setup.rx_data = 0;	/* RTC timer value will be updated here. */
  status = rtc_ioctl(rtcdev, RTC_GET_TIME, (RTC_SETUP_T *) & rtc_setup);
  if (status == _ERROR)
  {
    uart_string_write("RTC: Get time Failed \r\n");
    rtc_close(rtcdev);
    return;
  }

  /* Current time */
  current_time = rtc_setup.rx_data % 86400;
  current_day = (UNS_32)(rtc_setup.rx_data / 86400);
  alarm_time = ((alarm_table.rtc_hour - ref_time_table.rtc_hour) * 3600) +
               ((alarm_table.rtc_minute - ref_time_table.rtc_minute) * 60) +
               (alarm_table.rtc_second - ref_time_table.rtc_second);
  if (((alarm_table.rtc_year % 4) == 0) &&
      ((alarm_table.rtc_year % 100) != 0) &&
      ((alarm_table.rtc_year % 400) == 0))
  {
    /* this is a leap year. 29 days in Feb, 366 days/year */
    month_day_table[1] = 29;
    alarm_day = (alarm_table.rtc_year - ref_time_table.rtc_year) * 366 +
                (month_day_table[alarm_table.rtc_month-1] -
                 month_day_table[ref_time_table.rtc_month-1]) +
                (alarm_table.rtc_day - ref_time_table.rtc_day);
  }
  else
  {
    alarm_day = (alarm_table.rtc_year - ref_time_table.rtc_year) * 365 +
                (month_day_table[alarm_table.rtc_month-1] -
                 month_day_table[ref_time_table.rtc_month-1]) +
                (alarm_table.rtc_day - ref_time_table.rtc_day);
  }

  uart_string_write("Alarm time set in RTC:  \r\n");
  sprintf(buff, "%4d/%2d/%2d  %2d:%2d:%2d \r\n",
          (int)alarm_table.rtc_year, (int)alarm_table.rtc_month,
          (int)alarm_table.rtc_day, (int)alarm_table.rtc_hour,
          (int)alarm_table.rtc_minute, (int)alarm_table.rtc_second);
  uart_string_write(buff);

  rtc_setup.tx_data = (86400 * alarm_day) + alarm_time;
  sprintf(buff, "Alarm time (in seconds): %d \r\n", (int)rtc_setup.tx_data);
  uart_string_write(buff);
  uart_string_write("--------------------------- \r\n");
  rtc_setup.rx_data = 0;
  status = rtc_ioctl(rtcdev, RTC_SETUP_ALARM, (RTC_SETUP_T *) & rtc_setup);
  if (status == _ERROR)
  {
    uart_string_write("RTC: Setup Alarm Failed \r\n");
    rtc_close(rtcdev);
    return;
  }

  rtc_setup.tx_data = 0x0;	/* dummy data */
  rtc_setup.rx_data = 0;	/* RTC timer value will be updated here. */
  status = rtc_ioctl(rtcdev, RTC_GET_TIME, (RTC_SETUP_T *) & rtc_setup);
  if (status == _ERROR)
  {
    uart_string_write("RTC: Get time Failed \r\n");
    rtc_close(rtcdev);
    return;
  }

  /* Enable Alarm interrupt */
  rtc_setup.tx_data = RTC_AD_INT_EN;
  rtc_setup.rx_data = 0;
  status = rtc_ioctl(rtcdev, RTC_ENA_SET, (RTC_SETUP_T *) & rtc_setup);
  if (status == _ERROR)
  {
    uart_string_write("RTC: Get time Failed \r\n");
    rtc_close(rtcdev);
    return;
  }

  /* Don't exit until RTC ALARM is hit in the event router. */
  uart_string_write("Wait for alarm.........\r\n");
  while (1)
  {
    if (alarm_count)
    {
      /* Once the RTC alarm interrupt occurs, the RTL_ALARM will be
      set. Clear RTC ALARM status, then exit. */
      uart_string_write("Alarm interrupt triggered \r\n");
      analogdie_reg_write(AD_REG_RTC_CLR_ENA_STAT, RTC_ALARM);
      break;
    }
    else
    {
      reg_value = analogdie_reg_read(AD_REG_RTC_TIME);
      if (prv_val != reg_value)
      {
        prv_val = reg_value;
        sprintf(buff, "Current Time (sec): %d \r\n", (int)prv_val);
        uart_string_write(buff);
      }
      rtc_delay(100000);
    }
  }

  rtc_close(rtcdev);

  uart_string_write("--------------------------- \r\n");
  uart_string_write("RTC Example Application: Done \r\n");
  uart_string_write("--------------------------- \r\n");

  return;
}
