/* timer-lh79520.c

   written by Marc Singer
   1 Nov 2004

   Copyright (C) 2004 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#include <service.h>
#include "lh79520.h"

static void lh79520_timer_init (void)
{
  RCPC_CTRL	     |=  RCPC_CTRL_UNLOCK;
  RCPC_PERIPHCLKSEL  &= ~RCPC_PERIPHCLKSEL_RTC_MASK;
  RCPC_PERIPHCLKSEL  |=  RCPC_PERIPHCLKSEL_RTC_32KHZ;
  RCPC_PERIPHCLKCTRL &= ~RCPC_PERIPHCLK_RTC;
  RCPC_CTRL	     &= ~RCPC_CTRL_UNLOCK;
}

static void lh79520_timer_release (void)
{
  RCPC_CTRL          |=  RCPC_CTRL_UNLOCK;
  RCPC_PERIPHCLKSEL  &= ~RCPC_PERIPHCLKSEL_RTC_MASK;
  RCPC_PERIPHCLKSEL  |=  RCPC_PERIPHCLKSEL_RTC_1HZ;
  RCPC_PERIPHCLKCTRL |=  RCPC_PERIPHCLK_RTC;
  RCPC_CTRL	     &= ~RCPC_CTRL_UNLOCK;
}

unsigned long timer_read (void)
{
  return RTC_DR;
}


/* timer_delta

   returns the difference in time in milliseconds.

 */

unsigned long timer_delta (unsigned long start, unsigned long end)
{
  return (end - start)*1000/32768;
}

static __service_2 struct service_d lh79520_timer_service = {
  .init = lh79520_timer_init,
  .release = lh79520_timer_release,
};
