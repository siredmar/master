/* timer.c

   written by Marc Singer
   15 Sep 2008

   Copyright (C) 2008 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   We use timer 0 for the master system timer supported by
   timer_read() and timer_delta().  Timer 1 is used as the short delay
   timer for udelay().

   The timer cannot be configured to count a full cycle.  The largest
   LOAD value is ~0 which means that we count one fewer than a full
   cycle.  The only effect is that comparing a timer value before and
   after the wrap will yield a value that is one tick short.  Good
   thing we don't care.

*/

#include <service.h>
#include "hardware.h"

static void orion5x_timer_init (void)
{
  CPU_TIMER0_LOAD = ~0;
  CPU_TIMERS_CTRL |= CPU_TIMERS_CTRL_0_AUTO | CPU_TIMERS_CTRL_0_EN;
}

static void orion5x_timer_release (void)
{
  CPU_TIMERS_CTRL &= ~CPU_TIMERS_CTRL_0_EN;
}

unsigned long timer_read (void)
{
  return CPU_TIMER0;
}


/* timer_delta

   returns the difference in time in milliseconds.

 */

unsigned long timer_delta (unsigned long start, unsigned long end)
{
  return (start - end)/(get_tclk ()/1000);
}

static __service_2 struct service_d orion5x_timer_service = {
  .init    = orion5x_timer_init,
  .release = orion5x_timer_release,
};
