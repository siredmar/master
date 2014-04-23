/* init-lpd79520.c

   written by Marc Singer
   14 Nov 2004

   Copyright (C) 2004 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   Hardware initializations for the LPD79520.  Some initializations
   may be left to drivers, such as the serial interface and timer.

   CompactFlash Timing

     I've adjusted the timing on the ISA/CF region of memory.  The
     read and write cycles use a 180ns delay.  This timing timing is
     well within the CF spec of 150ns/165ns for write/read minimum
     hold times.

*/

/*
	SDRAM configuration according to JEDEC spec

	WAIT 200us
	NOP->SDRAM
	WAIT 200us
	PRECHARGE_ALL->SDRAM
	MODE_REGISTER_SET->SDRAM_EXTENDED_MODE
	MODE_REGISTER_SET->SDRAM
	WAIT 200 cycles
	PRECHARGE_ALL->SDRAM
	AUTO_REFRESH->SDRAM *2
	MODE_REGISTER_SET->SDRAM

	SDRAM configuration according to u-boot code

	NOP->SDRAM
	WAIT 200us
	PRECHARGE->SDRAM
	AUTO_REFRESH->SDRAM *2
	LOAD_MODE->SDRAM
	AUTO_REFRESH->SDRAM *8
	NORMAL->SDRAM

	SDRAM initialization according to Sharp LH79520 documentation

	WAIT 200ps
	PRECHARGE_ALL->SDRAM
	AUTO_REFRESH->SDRAM *2   [lh79520 has a auto-refresh mode]
	LOAD_MODE->SDRAM (Program the SDRAM device's LOAD MODE register)

*/

#include <config.h>
#include <asm/bootstrap.h>
#include <service.h>
#include <sdramboot.h>

#include "hardware.h"
#include <debug_ll.h>

//#define USE_LONG_USLEEP

//#define USE_SLOW

#if defined (USE_SLOW)
//RAS3 CAS3
# define SDRC_CONFIG_V\
	(1<<24)|(3<<22)|(3<<20)|(0<<19)|(1<<18)|(0<<17)|(1<<7)|(1<<3)
#else
//RAS2 CAS2
# define SDRC_CONFIG_V\
	(1<<24)|(2<<22)|(2<<20)|(0<<19)|(1<<18)|(0<<17)|(1<<7)|(1<<3)
#endif

#if defined (USE_SLOW)
# define SDRAM_CHIP_MODE	(0x32<<12)	// CAS3 BURST4
#else
# define SDRAM_CHIP_MODE	(0x22<<12)	// CAS2 BURST4
#endif

#define SDRC_COMMAND_NORMAL	((0<<0)|(1<<3) |(1<<2))
#define SDRC_COMMAND_PRECHARGE	(1<<0)
#define SDRC_COMMAND_MODE	(2<<0)
#define	SDRC_COMMAND_NOP	(3<<0)

#define SDRC_CONFIG1_BUSY	(1<<5)

/* usleep

   this function accepts a count of microseconds and will wait at
   least that long (though probably longer) before returning.  The
   timer clock must be activated by the initialization code before
   using usleep.

   Even though the timer is limited in range, the function can loop
   until the full count has been exhausted making the longest sleep is
   2^31.  The USE_LONG_USLEEP macro must be defined for this
   functionality as it's unecessary for the core operation of the
   timer.

   Note that this function not static.  It is available to the rest of
   the application as is.

   The timer interval is (309657600/6)/256 which is about 4.96us.  We
   approximate this by dividing by four which means we are about 5%
   longer than requested.

   CPU timers count down toward zero.

*/

#define US_LOOP_MAX	131000

void __naked __section (.bootstrap) usleep (unsigned long us)
{
  unsigned long lr;
  __asm volatile ("mov %0, lr" : "=r" (lr));

#if defined (USE_LONG_USLEEP)
 {
   unsigned long us_total;
   us_total = us;
   while (us_total) {
     if (us_total > US_LOOP_MAX)
       us = US_LOOP_MAX;
     else
       us = us_total;
     us_total -= us;
#endif
     {
       unsigned long c = (us + 3)/4;	/* Timer counts down */
       __asm volatile ("str %2, [%1, #8]\n\t"	/* Stop timer */
		       "str %0, [%1, #0]\n\t"
		       "str %3, [%1, #8]\n\t"
		    "0: ldr %0, [%1, #4]\n\t"
		       "tst %0, %4\n\t"
		       "beq 0b\n\t"
		       : "+r" (c)
		       : "r" (TIMER0_PHYS),
			 "r" (0),
			 "r" (TIMER_ENABLE | TIMER_SCALE_256),
			 "I" (0x8000)
		       : "cc"
		       );
     }
#if defined (USE_LONG_USLEEP)
   }
 }
#endif

  __asm volatile ("mov pc, %0" : : "r" (lr));
}


/* initialize_bootstrap

   performs vital SDRAM initialization as well as some other memory
   controller initializations.  It will perform no work if we are
   already running from SDRAM.  It will

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
   *** long as the loader never runs from high BCR0 memory.

*/

void __naked __section (.bootstrap) initialize_bootstrap (void)
{
  unsigned long lr;
  __asm volatile ("mov %0, lr" : "=r" (lr));

#if defined (CONFIG_DEBUG_LL)
  RCPC_CTRL      |= RCPC_CTRL_UNLOCK;
  RCPC_PERIPHCLKCTRL &= ~RCPC_PERIPHCLK_U1;
  RCPC_CTRL &= ~RCPC_CTRL_UNLOCK;
  IOCON_UARTMUX |= 0xc;		/* Activate U1 MUX */

  UART_CR = UART_CR_EN; /* Enable UART without drivers */

  UART_IBRD = 8;
  UART_FBRD = 0;
  UART_LCR_H = UART_LCR_FEN | UART_LCR_WLEN8;
  UART_IMSC = 0x00; /* Mask interrupts */
  UART_ICR  = 0xff; /* Clear interrupts */
  UART_CR = UART_CR_EN | UART_CR_TXE | UART_CR_RXE;

  PUTC_LL('A');
#endif

	/* Setup HCLK, FCLK and peripheral clocks */
  RCPC_CTRL |= RCPC_CTRL_UNLOCK;
  RCPC_CPUCLKPRESCALE = RCPC_CPUCLKPRESCALE_V;
  RCPC_HCLKCLKPRESCALE = RCPC_HCLKCLKPRESCALE_V;
  RCPC_CORECLKCONFIG = RCPC_CORECLKCONFIG_V;
  RCPC_AHBCLKCTRL &= ~RCPC_AHBCLK_SDC;
  /* Enable timer clock so that we can handle SDRAM setup  */
  RCPC_PERIPHCLKCTRL &= ~RCPC_PERIPHCLK_T01;

  RCPC_CTRL &= ~RCPC_CTRL_UNLOCK;

  SDRC_REFTIMER = SDRC_REFTIMER_V; /* Do this early */

  /* Stop watchdog? */

  /* *** 0x17ef was good enough for JTAG */
  IOCON_MEMMUX = 0x3fff; /* 32 bit, SDRAM, all SRAM */

  /* NOR flash */
  if (CPLD_REVISION < 0x0021)
    SMC_BCR0 = 0x100020ef;	/* BLE used for write-enable; old boards */
  else
    SMC_BCR0 = 0x100024ef;	/* WE used for write-enable */


//  SMC_BCR4 = 0x10007580; /* CompactFlash */
//  SMC_BCR5 = 0x100034c0; /* CPLD */

  SMC_BCR4 = (1<<28)|(1<<25)|(1<<24)|( 8<<11)|(1<<10)|(8<<5)|(0<<0); /* CF */
  SMC_BCR5 = (1<<28)|(1<<25)|(1<<24)|( 4<<11)|(1<<10)|(4<<5)|(4<<0); /* CPLD */

  __asm volatile ("tst %0, #0xf0000000\n\t"
		  "beq 1f\n\t"
		  "cmp %0, %1\n\t"
#if defined (CONFIG_SDRAMBOOT_REPORT)
		  "movls r0, #1\n\t"
		  "strls r0, [%2]\n\t"
#endif
		  "movls r0, #0\n\t"
		  "movls pc, %0\n\t"
		"1:" :: "r" (lr), "i" (SDRAM_BANK1_PHYS)
#if defined (CONFIG_SDRAMBOOT_REPORT)
		  , "r" (&fSDRAMBoot)
#endif
		  : "r0");


  /* SDRAM Initialization

     I spent some time exploring what could be removed from the
     initialization sequence.  Below is the result.  It appears that
     the LH79520 handles all of the AUTO_REFRESH cycles.  I never
     tried breaking it by clearing the AP bit in the CFG0 register.
     Note that there are fewer delays, one and only one NOP, and there
     is no precharge after setting the mode.
  */

  SDRC_CONFIG1 = SDRC_COMMAND_NOP;
  usleep (200);
  SDRC_CONFIG1 = SDRC_COMMAND_PRECHARGE;
  usleep (200);
  SDRC_CONFIG1 = SDRC_COMMAND_MODE;

  __REG (SDRAM_BANK0_PHYS + SDRAM_CHIP_MODE);
  __REG (SDRAM_BANK1_PHYS + SDRAM_CHIP_MODE);

		/* Wait for controller to idle
		   -- See AppNote LH79520 SoC SDRAM connection and Usage */
  while (SDRC_CONFIG1 & SDRC_CONFIG1_BUSY)
    ;

  SDRC_CONFIG0 = SDRC_CONFIG_V;
  SDRC_CONFIG1 = SDRC_COMMAND_NORMAL;

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

#if 0
static void target_init (void)
{
  unsigned long lr;
  __asm volatile ("mov %0, lr" : "=r" (lr));
  __asm volatile ("mov pc, %0" : : "r" (lr));
}
#endif

static void target_release (void)
{
  /* Flash is enabled for the kernel */
  CPLD_FLASH |=  CPLD_FLASH_FL_VPEN;
}

static __service_0 struct service_d lh79520_target_service = {
  //  .init    = target_init,
  .release = target_release,
};
