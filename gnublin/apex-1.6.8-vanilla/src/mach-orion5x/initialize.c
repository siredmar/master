/* initialize.c

   written by Marc Singer
   14 Sep 2008

   Copyright (C) 2008 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   Hardware initializations.  Some initialization may be left to
   drivers, such as the serial interface initialization.

*/

#include <config.h>
#include <asm/bootstrap.h>
#include <service.h>
#include <sdramboot.h>
#include <asm/cp15.h>

#include "hardware.h"
#include <debug_ll.h>

//#define NO_INIT

#define ESDCTL_CTL0_SDE		(1<<31)	/* Enable */
#define ESDCTL_CTL0_ROW13	(2<<24)	/* 13 rows */
#define ESDCTL_CTL0_COL9	(1<<20)	/* 9 columns */
#define ESDCTL_CTL0_COL10	(2<<20)	/* 10 columns */
#define ESDCTL_CTL0_DSIZ32	(2<<16)	/* 32 bit interface */
#define ESDCTL_CTL0_SREFR7_81	(3<<13)	/* 7.81 us refresh */
#define ESDCTL_CTL0_SREFR3_91	(4<<13)	/* 3.91 us refresh */
#define ESDCTL_CTL0_BL8		(1<<7)	/* burst length of 8 */

#define ESDCTL_CTL0_SREFR_V	ESDCTL_CTL0_SREFR7_81

#define ESDCTL_CTL0_V1 0\
	| ESDCTL_CTL0_SDE\
	| ESDCTL_CTL0_ROW13\
	| ESDCTL_CTL0_COL10\
	| ESDCTL_CTL0_DSIZ32\
	| ESDCTL_CTL0_SREFR_V\
	| ESDCTL_CTL0_BL8

#define ESDCTL_CTL0_V2 0\
	| ESDCTL_CTL0_SDE\
	| ESDCTL_CTL0_ROW13\
	| ESDCTL_CTL0_COL9\
	| ESDCTL_CTL0_DSIZ32\
	| ESDCTL_CTL0_SREFR_V\
	| ESDCTL_CTL0_BL8

#define ADDR_CHECK_0	(0x80000000)
#define ADDR_CHECK_1	(0x80000800)

//#define ESDCTL_CFG_V	(0x0075e73a)
#define ESDCTL_CFG_V	(0x00096728)	/* 1Gib   MT46H32M32LF-6 */
//#define ESDCTL_CFG_V	(0x0069e728)	/* 512Mib MT46H16M32LF-6 */

#define BA0(r,c,w)	((r) + (c) + (w)/16)
#define BA1(r,c,w)	((r) + (c) + (w)/16 + 1)

static unsigned long tclk;	// Rate of TCLK, determined for usleep

void __naked __section (.bootstrap.early) bootstrap_early (void)
{
	// Determine TCLK so that usleep has an easy time
  switch (MPP_SAMPLE_TCLK (MPP_SAMPLE)) {
  case MPP_SAMPLE_TCLK_133:
    tclk = 133*1000*1000;
  case MPP_SAMPLE_TCLK_150:
    tclk = 150*1000*1000;
  default:
  case MPP_SAMPLE_TCLK_166:
    tclk = 166*1000*1000;
  }
  __asm volatile ("b bootstrap_early_exit");
}

void __naked __section (.bootstrap.early.exit) bootstrap_early_exit (void)
{
}


/* usleep

   this function accepts a count of microseconds and will wait at
   least that long before returning.

   Note that this function is neither __naked nor static.  It is
   available to the rest of the application as is.

   The source of this timer is TIMER1 which counts at the rate of
   TCLK.  Normally, we'd call get_tclk (), but we may not have a stack
   to do that.  So, we must determine the base clock rate at
   initialization.

*/

void __section (.bootstrap.sdram.func) usleep (unsigned long us)
{
  CPU_TIMERS_CTRL &= ~(CPU_TIMERS_CTRL_1_EN | CPU_TIMERS_CTRL_1_AUTO);
  CPU_TIMER1_LOAD = us*(tclk/(1000*1000));
  CPU_TIMERS_CTRL |= CPU_TIMERS_CTRL_1_EN;

  while (CPU_TIMER1)
    ;
}


/* bootstrap_pre

   performs mandatory, pre-SDRAM initializations that were not
   performed in the bootstrap_early function, if one even existed.  In
   this case, we wait until now to setup the PLLs.

*/

void __naked __section (.bootstrap.pre) bootstrap_pre (void)
{
#if defined (CONFIG_STARTUP_UART)
  INITIALIZE_CONSOLE_UART;
  PUTC ('A');
#endif
  __asm volatile ("b bootstrap_pre_exit");
}

void __naked __section (.bootstrap.pre.exit) bootstrap_pre_exit (void)
{
}


/* bootstrap_sdram

   performs the SDRAM initialization.  This piece of code is skipped
   by the reset() code if it determines that APEX is already running
   in SDRAM.

*/

void __naked __section (.bootstrap.sdram) bootstrap_sdram (void)
{
  __asm volatile ("mov r0, #0");		/* SDRAM initialized */
  __asm volatile ("b bootstrap_sdram_exit");
}

void __naked __section (.bootstrap.sdram.exit) bootstrap_sdram_exit (void)
{
}


/* target_init

   performs the rest of the hardware initialization that didn't have
   to be performed during the bootstrap phase.

*/

#if 0
static void target_init (void)
{
}


static void target_release (void)
{
}
#endif

static __service_0 struct service_d mx3x_target_service = {
//  .init    = target_init,
//  .release = target_release,
};
