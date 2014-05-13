/* timer.c

   written by Durgesh Pattamatta
   1 Nov 2004

   Copyright (C) 2008 NXP Semiconductors

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------
   This implementation assumes the TIMER1 clock is sourced by 12MHz FFAST_IN
   clock in CGU. 
   

*/

#include <service.h>
#include "lpc313x.h"
#include "lpc313x_cgu.h"

#define TICKS_PER_MSEC      (XTAL_CLOCK/(256 * 1000))

static void lpc313x_timer_init (void)
{
  /* enable Timer0 clocks */
  cgu_clk_en_dis(CGU_SB_TIMER0_PCLK_ID, 1);
	
  __REG (TIMER0_PHYS + TIMER_CONTROL) = TM_CTRL_PS256 | TM_CTRL_ENABLE;
  __REG (TIMER0_PHYS + TIMER_LOAD) = 0xffffffff;
  __REG (TIMER1_PHYS + TIMER_CONTROL) = 0;
  __REG (TIMER2_PHYS + TIMER_CONTROL) = 0;
  __REG (TIMER3_PHYS + TIMER_CONTROL) = 0;
}

static void lpc313x_timer_release (void)
{
  __REG (TIMER0_PHYS + TIMER_CONTROL) = 0;
}

unsigned long timer_read (void)
{
  return (0xFFFFFFFF - __REG (TIMER0_PHYS + TIMER_VALUE));
}

/* timer_delta

   returns the difference in time in milliseconds.

 */

unsigned long timer_delta (unsigned long start, unsigned long end)
{
  return (end - start)/TICKS_PER_MSEC;
}

static __service_2 struct service_d lpc313x_timer_service = {
  .init    = lpc313x_timer_init,
  .release = lpc313x_timer_release,
};
