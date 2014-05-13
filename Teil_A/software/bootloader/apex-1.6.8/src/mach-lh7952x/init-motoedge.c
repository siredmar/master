/* init-motoedge.c

   written by Marc Singer
   9 Feb 2007

   Copyright (C) 2007 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   Hardware initializations for the LPD79524.  Some initializations
   may be left to drivers, such as the serial interface and timer.

   SDRAM Refresh

     The datasheet for the SDRAM states that the memory requires 8192
     refresh cycles very 64ms.  Which is a refresh cycle (AUTOREFRESH
     command) every 64ms/8192 or 7.812us.  I assume that this is what
     the SDRAM controller does when the refresh timer expires.

*/

#include <config.h>
#include <asm/bootstrap.h>
#include <service.h>
#include <sdramboot.h>

#include "hardware.h"

#include <debug_ll.h>

//#define USE_SLOW

	// SDRAM
#if defined (CONFIG_SDRAM_FORCE16BIT)
# define SDRAM_CFG_SETUP	((0<<14)|(1<<12)|(2<<9)|(1<<7)) /*16LP;8Mx16*/
# define SDRAM_MODE_SHIFT	10
# define SDRAM_BURST		3
#else
# define SDRAM_CFG_SETUP	((1<<14)|(1<<12)|(3<<9)|(1<<7)) /*32LP;16Mx16*/
# define SDRAM_MODE_SHIFT	11
# define SDRAM_BURST		2
#endif
#define SDRAM_CFG		(SDRAM_CFG_SETUP | (1<<19))

#define SDRAM_RAS		3

#if defined (USE_SLOW)
# define SDRAM_CAS		3
#else
# define SDRAM_CAS		2
#endif

#define SDRAM_RASCAS		((SDRAM_RAS<<0)|(SDRAM_CAS<<8))
#define SDRAM_CHIP_MODE\
	(((SDRAM_BURST<<0)|(SDRAM_CAS<<4))<<SDRAM_MODE_SHIFT)


void __naked __section (.bootstrap.pre) bootstrap_pre (void)
{
#if defined (CONFIG_DEBUG_LL)
  RCPC_CTRL      |= RCPC_CTRL_UNLOCK;
  RCPC_PCLKCTRL0 &= ~RCPC_PCLKCTRL0_U0;
  RCPC_PCLKSEL0  &= ~(1<<0);
  RCPC_CTRL &= ~RCPC_CTRL_UNLOCK;

  MASK_AND_SET (IOCON_MUXCTL5, (3<<10)|(3<<8), (2<<10)|(2<<8));
  MASK_AND_SET (IOCON_MUXCTL6, (3<<2)|(3<<0), (2<<2)|(2<<0));

  UART_CR = UART_CR_EN; /* Enable UART without drivers */

  UART_IBRD = 6;
  UART_FBRD = 8;
  UART_LCR_H = UART_LCR_FEN | UART_LCR_WLEN8;
  UART_IMSC = 0x00; /* Mask interrupts */
  UART_ICR  = 0xff; /* Clear interrupts */
  UART_CR = UART_CR_EN | UART_CR_TXE | UART_CR_RXE;

  PUTC ('A');
#endif

	/* Setup HCLK, FCLK and peripheral clocks */
  RCPC_CTRL      |= RCPC_CTRL_UNLOCK;

  RCPC_AHBCLKCTRL = RCPC_AHBCLKCTRL_V;
//  RCPC_PCLKCTRL0  = RCPC_PCLKCTRL0_V;
  RCPC_PCLKCTRL0  &= ~(1<<9);	/* RTC enable */
  RCPC_PCLKCTRL1  = RCPC_PCLKCTRL1_V;
  RCPC_PCLKSEL0   = RCPC_PCLKSEL0_V;

  RCPC_CORECONFIG = RCPC_CORECONFIG_FASTBUS; /* Necessary to change clock */
  RCPC_SYSPLLCNTL = RCPC_SYSPLLCNTL_V;
  RCPC_SYSCLKPRE  = RCPC_SYSCLKPRE_V;
  RCPC_CPUCLKPRE  = RCPC_CPUCLKPRE_V;
  RCPC_CORECONFIG = RCPC_CORECONFIG_V;

  RCPC_CTRL      &= ~RCPC_CTRL_UNLOCK;

	/* Setup IO pin multiplexers */
  IOCON_MUXCTL5  = IOCON_MUXCTL5_V;	/* UART */
  IOCON_MUXCTL6  = IOCON_MUXCTL6_V;	/* UART */
  IOCON_MUXCTL10 = IOCON_MUXCTL10_V;	/* D */
  IOCON_MUXCTL11 = IOCON_MUXCTL11_V;	/* D */
  IOCON_MUXCTL12 = IOCON_MUXCTL12_V;	/* D */
  IOCON_MUXCTL19 = IOCON_MUXCTL19_V;	/* D */
  IOCON_MUXCTL20 = IOCON_MUXCTL20_V;	/* D */

	/* NAND flash, 8 bit */
  EMC_SCONFIG0    = 0x80;
  EMC_SWAITWEN0   = 1;
  EMC_SWAITOEN0   = 1;
  EMC_SWAITRD0    = 2;
  EMC_SWAITPAGE0  = 2;
  EMC_SWAITWR0	  = 2;
  EMC_STURN0      = 2;


	/* NOR flash, 16 bit */
  EMC_SCONFIG1    = 0x81;
  EMC_SWAITWEN1   = 1;
  EMC_SWAITOEN1   = 1;
  EMC_SWAITRD1    = 6;
  EMC_SWAITPAGE1  = 2;
  EMC_SWAITWR1    = 6;
  EMC_STURN1      = 1;

  PUTC_LL('x');
  __asm volatile ("b bootstrap_pre_exit");
}

void __naked __section (.bootstrap.pre.exit) bootstrap_pre_exit (void)
{
}


/* usleep

   this function accepts a count of microseconds and will wait at
   least that long before returning.  The timer clock must be
   activated by the initialization code before using usleep.

   When in C, this function was one instruction longer than the
   hand-coded assembler.  For some reason, the compiler would reload
   the TIMER1_PHYS at the top of the while loop.

   Note that this function is neither __naked nor static.  It is
   available to the rest of the application as is.

   Keep in mind that it has a limit of about 32ms.  Anything longer
   (and less than 16 bits) will return immediately.

   To be precise, the interval is HCLK/64 or 1.2597us.  In other
   words, it is about 25% longer than desired.  The code compensates
   by removing 25% of the requested delay.  The clever ARM instruction
   set makes this a single operation.

 */

void __section (.bootstrap.sdram.func) usleep (unsigned long us)
{
  unsigned long c = (unsigned long) 0x8000 - (us - us/4); /* Timer counts up */
  __asm volatile ("str %2, [%1, #0]\n\t"
		  "str %0, [%1, #0xc]\n\t"
		  "str %3, [%1, #0]\n\t"
	       "0: ldr %0, [%1, #0xc]\n\t"
		  "tst %0, %4\n\t"
		  "beq 0b\n\t"
		  : "+r" (c)
		  :  "r" (TIMER1_PHYS),
		     "r" (0),
		     "r" (TIMER_CTRL_CS | TIMER_CTRL_SCALE_64),
		     "I" (0x8000)
		  : "cc"
		  );
}


/* bootstrap_sdram

   performs SDRAM initialization.

   *** FIXME, comment out of date.

   The assembly output of this implementation of the initialization is
   more dense than the assembler version using a table of
   initializations.  This is primarily due to the compiler's ability
   to keep track of the register set offsets and value being output.

   The return value is true if SDRAM has been initialized and false if
   this initialization has already been performed.  Note that the
   non-SDRAM initializations are performed regardless of whether or
   not we're running in SDRAM.

   *** FIXME: The memory region 0x1000000-0x20000000 will be
   *** incorrectly detected as SDRAM with code below.  It is OK as
   *** long as the loader never runs from high nCS0 memory.

*/

void __naked __section (.bootstrap.sdram) bootstrap_sdram (void)
{
	/* SDRAM */
  EMC_READCONFIG  = EMC_READCONFIG_CMDDELAY;
  EMC_DYNCFG0     = SDRAM_CFG_SETUP;
  EMC_DYNRASCAS0  = SDRAM_RASCAS;
  EMC_DYNCFG1     = SDRAM_CFG_SETUP;
  EMC_DYNRASCAS1  = SDRAM_RASCAS;

  EMC_PRECHARGE   = NS_TO_HCLK(20);
  EMC_DYNM2PRE    = NS_TO_HCLK(60);
  EMC_REFEXIT     = NS_TO_HCLK(120);
  EMC_DOACTIVE    = NS_TO_HCLK(120);
  EMC_DIACTIVE    = NS_TO_HCLK(120);
  EMC_DWRT	  = NS_TO_HCLK(40);
  EMC_DYNACTCMD   = NS_TO_HCLK(120);
  EMC_DYNAUTO     = NS_TO_HCLK(120);
  EMC_DYNREFEXIT  = NS_TO_HCLK(120);
  EMC_DYNACTIVEAB = NS_TO_HCLK(40);
  EMC_DYNAMICTMRD = NS_TO_HCLK(40);

  EMC_DYNMCTRL    = ((1<<1)|(1<<0));
  usleep (200);
  EMC_DYNMCTRL    = (1<<1)|(1<<0)|(3<<7); /* NOP command */
  usleep (200);
  EMC_DYNMCTRL    = (1<<1)|(1<<0)|(2<<7); /* PRECHARGE ALL*/
  EMC_DYNMREF     = NS_TO_HCLK(100)/16 + 1;
  usleep (250);
  EMC_DYNMREF     = NS_TO_HCLK(7812)/16; /* See NOTES */
  EMC_DYNMCTRL    = (1<<1)|(1<<0)|(1<<7); /* MODE command */

  __REG (SDRAM_BANK0_PHYS + SDRAM_CHIP_MODE);
  __REG (SDRAM_BANK1_PHYS + SDRAM_CHIP_MODE);

  EMC_DYNMCTRL    = (1<<1)|(1<<0)|(0<<7); /* NORMAL */
  EMC_DYNCFG0     = SDRAM_CFG;
  EMC_DYNCFG1     = SDRAM_CFG;

  __asm volatile ("mov r0, #0");		/* SDRAM initialized */
  __asm volatile ("b boot_sdram_exit");
}

void __naked __section (.bootstrap.sdram.exit) boot_sdram_exit (void)
{
}


/* target_init

   performs the rest of the hardware initialization that didn't have
   to be performed during the bootstrap phase.

*/

static void target_init (void)
{
  PUTC_LL('T');

  IOCON_MUXCTL5  = 0xaaa0;	/* *** Doc? */

  PUTC_LL('c');

  BOOT_CS1OV &= ~(1<<0);

  PUTC_LL('d');
}

#if 0
static void target_release (void)
{
}
#endif

static __service_0 struct service_d lh79524_target_service = {
  .init    = target_init,
  //  .release = target_release,
};
