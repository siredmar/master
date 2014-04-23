/* timer.c

   written by Marc Singer
   16 Dec 2006

   Copyright (C) 2006 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#include <service.h>
#include "hardware.h"

static void mx3x_timer_init (void)
{
  CCM_CGR0 |= CCM_CGR_RUN << CCM_CGR0_GPT_SH;	/* Enable GPT clock  */
  GPT_PR = 32 - 1;		/* 32000/32 -> 1.00 ms/cycle */
  GPT_CR = GPT_CR_EN | GPT_CR_CLKSRC_32K | GPT_CR_FREERUN;
}

static void mx3x_timer_release (void)
{
  GPT_CR = 0;
}

unsigned long timer_read (void)
{
  return GPT_CNT;
}


/* timer_delta

   returns the difference in time in milliseconds.

 */

unsigned long timer_delta (unsigned long start, unsigned long end)
{
  return end - start;
}

static __service_2 struct service_d mx3x_timer_service = {
  .init    = mx3x_timer_init,
  .release = mx3x_timer_release,
};
