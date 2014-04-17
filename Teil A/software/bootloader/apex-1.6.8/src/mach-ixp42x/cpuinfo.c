/* cpuinfo.c

   written by Marc Singer
   3 Feb 2005

   Copyright (C) 2005 Marc Singer

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
#include <linux/string.h>
#include "hardware.h"
#include <service.h>
#include <arch-arm.h>

#if !defined (CONFIG_SMALL)

static void cpuinfo_report (void)
{
  unsigned long id;
  unsigned long cache;
  unsigned long ctrl;
  unsigned long ttbl;
  unsigned long domain;
  unsigned long cpsr;

  __asm volatile ("mrc p15, 0, %0, c0, c0": "=r" (id));
  __asm volatile ("mrc p15, 0, %0, c0, c0, 1" : "=r" (cache));
  __asm volatile ("mrc p15, 0, %0, c1, c0": "=r" (ctrl));
  __asm volatile ("mrc p15, 0, %0, c2, c0, 0" : "=r" (ttbl));
  __asm volatile ("mrc p15, 0, %0, c3, c0, 0" : "=r" (domain));
  __asm volatile ("mrs %0, cpsr": "=r" (cpsr));
  printf ("  cpu:      id 0x%08lx  ctrl   0x%lx (%s)  cpsr 0x%lx\n"
	  "          ttbl 0x%08lx  domain 0x%08lx  cache 0x%08lx\n",
	  id, ctrl, cp15_ctrl (ctrl), cpsr, ttbl, domain, cache);
}

static __service_7 struct service_d cpuinfo_service = {
  .report = cpuinfo_report,
};

#endif
