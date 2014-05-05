/* cpuinfo.c

   written by Marc Singer
   15 Sep 2008

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
#include <asm/cp15.h>

#if !defined (CONFIG_SMALL)

static void cpuinfo_report (void)
{
  unsigned long id	= CP15_ID;
  unsigned long cache	= CP15_CACHE_TYPE;
  unsigned long ctrl	= CP15_CTRL;
  unsigned long cpsr	= CPSR;
  unsigned long ttbl	= LOAD_TTB;
  unsigned long domain	= LOAD_DOMAIN;
  unsigned long test	= CP15_TEST;
//  unsigned long tcm	= CP15_TCM_TYPE;
//  unsigned long tlb	= CP15_TLB_TYPE;
//  unsigned long mpu	= CP15_MPU_TYPE;
  const char* sz = "orion5x";

  printf ("  cpu:      id 0x%08lx    cpsr 0x%08lx   ctrl 0x%08lx (%s)\n"
//          "           tcm 0x%08lx     tlb 0x%08lx    mpu 0x%08lx\n"
	  "          ttbl 0x%08lx  domain 0x%08lx  cache 0x%08lx\n"
	  "          chipid %s\n"
	  "          cp15test 0x%04lx\n",
	  id, ctrl, cpsr, cp15_ctrl (ctrl),
//          tcm, tlb, mpu,
	  ttbl, domain, cache, sz,
	  test & 0xffff);
  printf ("          tclk %d  fclk %d\n", get_tclk (), get_cpu_clk ());
}

static __service_7 struct service_d cpuinfo_service = {
  .report = cpuinfo_report,

};

#endif
