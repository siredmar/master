/* cpuinfo.c

   written by Marc Singer
   25 Nov 2006

   Copyright (C) 2006 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#include <config.h>
#include <apex.h>
#include <service.h>
#include <arch-arm.h>
#include "hardware.h"

#define PIN_BOARD_ID1		MX31_PIN_GPIO1_4
#define PIN_BOARD_ID2		MX31_PIN_GPIO1_6

#if !defined (CONFIG_SMALL)

static void cpuinfo_report (void)
{
  unsigned long id;
  unsigned long cache;
  unsigned long ctrl;
  unsigned long cpsr;
  unsigned long ttbl;
  unsigned long domain;
//  unsigned long csc = (CSC_PWRSR>>CSC_PWRSR_CHIPID_SHIFT);
  unsigned long test;
  char* sz = "iMX31";

//  switch (csc & 0xf0) {
//  default  : sz = "lh?";     break;
//  case 0x00: sz = "lh7a400"; break;
//  case 0x20: sz = "lh7a404"; break;
//  }

  __asm volatile ("mrc p15, 0, %0, c0, c0, 0" : "=r" (id));
  __asm volatile ("mrc p15, 0, %0, c0, c0, 1" : "=r" (cache));
  __asm volatile ("mrc p15, 0, %0, c1, c0, 0" : "=r" (ctrl));
  __asm volatile ("mrc p15, 0, %0, c2, c0, 0" : "=r" (ttbl));
  __asm volatile ("mrc p15, 0, %0, c3, c0, 0" : "=r" (domain));
  __asm volatile ("mrc p15, 0, %0, c15, c0, 0" : "=r" (test));
  __asm volatile ("mrs %0, cpsr"	   : "=r" (cpsr));
  printf ("  cpu:      id 0x%08lx   cache 0x%08lx    cpsr 0x%08lx\n"
	  "          ttbl 0x%08lx  domain 0x%08lx  chipid %s\n"
          "          ctrl 0x%08lx (%s)\n"
	  "          cp15test 0x%04lx\n",
	  id, cache, cpsr,
	  ttbl, domain, sz,
          ctrl, cp15_ctrl (ctrl),
          test & 0xffff);

#if defined (CPLD_VERSION)
  {
    unsigned char v = CPLD_VERSION;
    printf ("  cpld:   version 0x%x, PCB %c\n", v, 'A' + ((v >> 8) & 0xff));
    printf ("          stat1 0x%x  stat2 0x%x\n", CPLD_STATUS1, CPLD_STATUS2);
  }
#endif

#if defined (CONFIG_MACH_EXBIBLIO_ROSENCRANTZ)
  printf ("  board:  %s\n",
	  IS_ROSENCRANTZ_FF ? "form-factor" : "non-form-factor");
#endif

}

static __service_7 struct service_d cpuinfo_service = {
  .report = cpuinfo_report,

};

#endif
