/* timer-lh79524.c

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

   Using a Timer Interrupt
   -----------------------

   The timers can be clocked, at the slowest, at HCLK/128 which is
   ~2.52us given the standard HCLK of 50803200 Hz.  The counters count
   up and can be programmed to automatically clear when they reach a
   set value.  The longest this can be is ~165ms which should be
   adequate.

   We can compute the timer limit by multiplying the HCLK base
   frequency by the desired interrupt interval, in this case 10ms
   (.01), and dividing that by the clock scalar of 128.

   In this mode, using an interrupt as a time base, accuracy is very
   good.  The ticks value is wrap in 2^32/(24*60*60*100) days where
   the TIMER_LIMIT is HCLK/(128*100).  The '100' value is the number
   of ticks per second.

*/

#include <config.h>
#include <service.h>
#include "hardware.h"
#include <asm/interrupts.h>

#if defined (CONFIG_INTERRUPTS)
# include <linux/compiler-gcc.h>
#endif

#include <debug_ll.h>

#define TIMER_CTRL	TIMER2_CTRL
#define TIMER_INTEN	TIMER2_INTEN
#define TIMER_STATUS	TIMER2_STATUS
#define TIMER_CNT	TIMER2_CNT
#define TIMER_CMP0	TIMER2_CMP0
#define TIMER_CMP1	TIMER2_CMP1
#define TIMER_CAPA	TIMER2_CAPA
#define TIMER_CAPB	TIMER2_CAPB
#define TIMER_IRQ	IRQ_TIMER2

#define UART_DR		__REG(UART + 0x00)

#define TIMER_LIMIT	(HCLK/(128*100))

#if defined (CONFIG_INTERRUPTS)

static unsigned long ticks;

static irq_return_t timer_isr (int irq)
{
  TIMER_STATUS = 0xf;		/* Clear interrupt */

  ++ticks;

//  if (ticks%100 == 0)
//    PUTC_LL ('.');

  return IRQ_HANDLED;
}
#endif

void lh79524_timer_init (void)
{
#if defined (CONFIG_INTERRUPTS)
 {
   interrupt_handlers[TIMER_IRQ] = timer_isr;
   barrier ();

   TIMER_CMP1 = TIMER_LIMIT;
   TIMER_CTRL
     = TIMER_CTRL_SCALE_128
     | TIMER_CTRL_CS
     | TIMER_CTRL_TC
     | TIMER_CTRL_CCL;
   TIMER_INTEN = TIMER_INTEN_CMP1;

   VIC_INTENABLE |= (1<<TIMER_IRQ);
 }
#else
  RCPC_CTRL      |= RCPC_CTRL_UNLOCK;
  RCPC_PCLKSEL0  |= 3<<7; /* 32KHz oscillator for RTC */
  RCPC_CTRL      &= ~RCPC_CTRL_UNLOCK;
  RTC_CR	  = RTC_CR_EN;
#endif
}

static void lh79524_timer_release (void)
{
#if defined (CONFIG_INTERRUPTS)
  VIC_INTENCLEAR |= (1<<TIMER_IRQ);
  TIMER_INTEN = 0;
  TIMER_CTRL = 0;
#else
  RTC_CR	 &= ~RTC_CR_EN;
  RCPC_CTRL      |=  RCPC_CTRL_UNLOCK;
  RCPC_PCLKSEL0  &= ~(3<<7); /* 1Hz RTC */
  RCPC_CTRL      &= ~RCPC_CTRL_UNLOCK;
#endif
}

unsigned long timer_read (void)
{
#if defined (CONFIG_INTERRUPTS)
  return ticks;
#else
  return RTC_DR;
#endif
}


/* timer_delta

   returns the difference in time in milliseconds.

 */

unsigned long timer_delta (unsigned long start, unsigned long end)
{
#if defined (CONFIG_INTERRUPTS)
  return (end - start)*10;
#else
  return (end - start)*1000/32768;
#endif
}

static __service_2 struct service_d lh79524_timer_service = {
  .init    = lh79524_timer_init,
  .release = lh79524_timer_release,
};
