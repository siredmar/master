/* init-kev79524.c

   written by Marc Singer
   28 Oct 2004

   Copyright (C) 2004 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   Hardware initializations for the KEV79524/79525.  Some
   initializations may be left to drivers, such as the serial
   interface and timer.

*/

#include <config.h>
#include <asm/bootstrap.h>
#include <service.h>
#include <sdramboot.h>

#include "hardware.h"

#include <debug_ll.h>

//#define USE_SLOW

	// SDRAM
//#define SDRAM_CFG_SETUP ((1<<14)|(1<<12)|(4<<9)|(0<<7))
#if defined (CONFIG_MACH_KEV79524)
# define SDRAM_CFG_SETUP	((1<<14)|(1<<12)|(3<<9)|(1<<7)) /*32LP;16Mx16*/
# define SDRAM_MODE_SHIFT	11
# define SDRAM_BURST		2 /* 2^x where x is this value */
#endif
#if defined (CONFIG_MACH_KEV79525)
/* Two banks of 16Mx16 */
# define SDRAM_CFG_SETUP	 ((0<<14)|(1<<12)|(3<<9)|(1<<7))
# define SDRAM_MODE_SHIFT	10
# define SDRAM_BURST		3 /* 2^x where x is this value */
#endif
#define SDRAM_CFG		(SDRAM_CFG_SETUP | (1<<19))

#define SDRAM_RAS		3

#if defined (USE_SLOW)
# define SDRAM_CAS		3
#else
# define SDRAM_CAS		2
#endif

#define EMC_RASCAS_V	((SDRAM_RAS<<0)|(SDRAM_CAS<<8))
#define SDRAM_CHIP_MODE\
	(((SDRAM_BURST<<0)|(SDRAM_CAS<<4))<<SDRAM_MODE_SHIFT)


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

void __section (.bootstrap) usleep (unsigned long us)
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


/* initialize_bootstrap

   performs vital SDRAM initialization as well as some other memory
   controller initializations.  It will perform no work if we are
   already running from SDRAM.

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

void __naked __section (.bootstrap) initialize_bootstrap (void)
{
  unsigned long lr;
  __asm volatile ("mov %0, lr" : "=r" (lr));

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

  PUTC_LL('A');
#endif

	/* Setup HCLK, FCLK and peripheral clocks */
  RCPC_CTRL       |= RCPC_CTRL_UNLOCK;

  RCPC_AHBCLKCTRL = RCPC_AHBCLKCTRL_V;
  RCPC_PCLKCTRL0  &= ~(1<<9);	/* RTC enable */
//  RCPC_PCLKCTRL0  = RCPC_PCLKCTRL0_V;
  RCPC_PCLKCTRL1  = RCPC_PCLKCTRL1_V;

  RCPC_PCLKSEL0   = RCPC_PCLKSEL0_V;
  RCPC_CORECONFIG = RCPC_CORECONFIG_FASTBUS;
  RCPC_SYSPLLCNTL = RCPC_SYSPLLCNTL_V;
  RCPC_SYSCLKPRE  = RCPC_SYSCLKPRE_V;
  RCPC_CPUCLKPRE  = RCPC_CPUCLKPRE_V;
  RCPC_CORECONFIG = RCPC_CORECONFIG_V;

  RCPC_CTRL       &= ~RCPC_CTRL_UNLOCK;

	/* Setup IO pin multiplexers */
  IOCON_MUXCTL5  = IOCON_MUXCTL5_V;	/* UART */
  IOCON_MUXCTL6  = IOCON_MUXCTL6_V;	/* UART */
  IOCON_MUXCTL10 = IOCON_MUXCTL10_V;	/* D */
  IOCON_MUXCTL11 = IOCON_MUXCTL11_V;	/* D */
  IOCON_MUXCTL12 = IOCON_MUXCTL12_V;	/* D */
  IOCON_MUXCTL19 = IOCON_MUXCTL19_V;	/* D */
  IOCON_MUXCTL20 = IOCON_MUXCTL20_V;	/* D */

	/* NOR flash, 16 bit */
  EMC_SCONFIG1    = 0x81;
  EMC_SWAITWEN1   = 1;
  EMC_SWAITOEN1   = 1;
  EMC_SWAITRD1    = 6;
  EMC_SWAITPAGE1  = 2;
  EMC_SWAITWR1    = 6;
  EMC_STURN1      = 1;

	/* CPLD, 16 bit */
  EMC_SCONFIG3    = 0x81;

  PUTC_LL('x');

  __asm volatile ("tst %0, #0xf0000000\n\t"
		  "beq 1f\n\t"
		  "cmp %0, %1\n\t"
#if defined (CONFIG_SDRAMBOOT_REPORT)
		  "movls r0, #1\n\t"
		  "strls r0, [%2]\n\t"
#endif
		  "movls r0, #0\n\t"
		  "movls pc, %0\n\t"
		  "1:" :: "r" (lr), "I" (SDRAM_BANK1_PHYS)
#if defined (CONFIG_SDRAMBOOT_REPORT)
		  , "r" (&fSDRAMBoot)
#endif
		  : "cc");

  PUTC_LL('r');

	/* SDRAM */
  EMC_READCONFIG  = EMC_READCONFIG_CMDDELAY;
  EMC_DYNCFG0     = SDRAM_CFG_SETUP;
  EMC_DYNRASCAS0  = EMC_RASCAS_V;
  EMC_DYNCFG1     = SDRAM_CFG_SETUP;
  EMC_DYNRASCAS1  = EMC_RASCAS_V;

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
  EMC_DYNMREF     = NS_TO_HCLK(7812)/16;
  EMC_DYNMCTRL    = (1<<1)|(1<<0)|(1<<7); /* MODE command */

  __REG (SDRAM_BANK0_PHYS + SDRAM_CHIP_MODE);
  __REG (SDRAM_BANK1_PHYS + SDRAM_CHIP_MODE);

  EMC_DYNMCTRL    = (1<<1)|(1<<0)|(0<<7); /* NORMAL */
  EMC_DYNCFG0     = SDRAM_CFG;
  EMC_DYNCFG1     = SDRAM_CFG;

  PUTC_LL('j');

#if defined (CONFIG_SDRAMBOOT_REPORT)
  barrier ();
  fSDRAMBoot = 0;
#endif

  __asm volatile ("mov r0, #-1\t\n"
		  "mov pc, %0" : : "r" (lr));
}


/* target_init

   performs the rest of the hardware initialization that didn't have
   to be performed during the bootstrap phase.

*/

static void target_init (void)
{
  /* *** FIXME: I'm not sure why I need these on the KEV.  It ought to
     *** default to these modes when not using NAND. */
  IOCON_MUXCTL7  = IOCON_MUXCTL7_V;   /* A23,A22 */
  IOCON_RESCTL7  = IOCON_RESCTL7_V;   /* pull-down */
  IOCON_MUXCTL14 = IOCON_MUXCTL14_V;  /* nCS0 normalize */

#if 0
	/* CompactFlash, 16 bit */
  EMC_SCONFIG2    = 0x81;
  EMC_SWAITWEN2   = 2;
  EMC_SWAITOEN2   = 2;
  EMC_SWAITRD2    = 6;
  EMC_SWAITPAGE2  = 2;
  EMC_SWAITWR2    = 6;
  EMC_STURN2      = 1;
#endif

	/* CPLD, 16 bit */
  EMC_SWAITWEN3   = 2;
  EMC_SWAITOEN3   = 2;
  EMC_SWAITRD3    = 5;
  EMC_SWAITPAGE3  = 2;
  EMC_SWAITWR3    = 5;
  EMC_STURN3      = 2;

  BOOT_CS1OV    &= ~(1<<0);
}

static __service_0 struct service_d lh79524_target_service = {
  .init    = target_init,
};
