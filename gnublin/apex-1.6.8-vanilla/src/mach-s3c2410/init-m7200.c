/* init-m7200.c

   written by David Anders
   20 Nov 2005

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

    main initialization point for the s3c2410 based M7200 board

*/

#include <config.h>
#include <asm/bootstrap.h>
#include <service.h>
#include <sdramboot.h>

#include "hardware.h"
#include <debug_ll.h>

/* initialize_bootstrap

   performs vital SDRAM initialization as well as some other memory
   controller initializations.  It will perform no work if we are
   already running from SDRAM.  It will

   The return value is true if SDRAM has been initialized and false if
   this initialization has already been performed.  Note that the
   non-SDRAM initializations are performed regardless of whether or
   not we're running in SDRAM.

*/

void __naked __section (.bootstrap) initialize_bootstrap(void)
{
	unsigned long lr, nop_count;
	__asm volatile ("mov %0, lr":"=r" (lr));

	WATCHDOG_CTRL = 0x00;
	INT_CTRL_MASK = 0xffffffff;
	INT_CTRL_SUBMASK = 0x7ff;
	CLK_CTRL_BASE = 0x00ffffff;
	CLK_CTRL_DIVN = 0x03;
	/* Enable Asynchronous Bus mode, NotFast and iA bits */
	{
		unsigned long l;
		__asm volatile ("mrc	p15, 0, %0, c1, c0, 0\n\t"
				"orr	%0, %0, #0xc0000000\n\t"
				"mcr	p15, 0, %0, c1, c0, 0":"=r" (l));
	}


	CLK_CTRL_MPLL = MPLLCON_USER;
	nop_count = 0;
	while (nop_count < 100)
		nop_count++;

#if defined (CONFIG_DEBUG_LL)
	GPIO_CTRL_BASE_H_CON = 0x0016faaa;
	GPIO_CTRL_BASE_H_UP = 0x000007ff;
	UART0_CTRL_UFCON = 0x00;
	UART0_CTRL_UMCON = 0x00;
	UART0_CTRL_ULCON = 0x03;
	UART0_CTRL_UCON = 0x245;
	UART0_CTRL_UBRDIV = UART_BRD;
	PUTC_LL('A');
	nop_count = 0;
	while (nop_count < 100)
		nop_count++;
#endif

	BWSCON = 0x22111122;
	BANKCON0 = 0x00000700;
	BANKCON1 = 0x00000700;
	BANKCON2 = 0x00000700;
	BANKCON3 = 0x00000700;
	BANKCON4 = 0x00000700;
	BANKCON5 = 0x00000700;

	__asm volatile ("cmp %0, %1\n\t"
#if defined (CONFIG_SDRAMBOOT_REPORT)
			"movhi r0, #1\n\t" "strhi r0, [%2]\n\t"
#endif
			"movhi r0, #0\n\t"
			"movhi pc, %0\n\t"
			"1:"::"r" (lr), "i"(SDRAM_BANK0_PHYS)
#if defined (CONFIG_SDRAMBOOT_REPORT)
			, "r"(&fSDRAMBoot)
#endif
			:"r0");

	BANKCON6 = BANKCON6_VAL;
	BANKCON7 = BANKCON7_VAL;
	REFRESH = REFRESH_VAL;
	BANKSIZE = BANKSIZE_VAL;
	MRSRB6 = MRSRB6_VAL;
	MRSRB7 = MRSRB7_VAL;

	nop_count = 0;
	while (nop_count < 100)
		nop_count++;


#if defined (CONFIG_SDRAMBOOT_REPORT)
	barrier();
	fSDRAMBoot = 0;
#endif

	__asm volatile ("mov r0, #-1\t\n" "mov pc, %0"::"r" (lr));

}
