/* cpuinfo.c

   written by Marc Singer
   15 Jan 2005

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
#include <service.h>
#include <arch-arm.h>
#include "hardware.h"

#if !defined (CONFIG_SMALL)

static void cpuinfo_report (void)
{
  unsigned long id;
  unsigned long cache;
  unsigned long ctrl;
  unsigned long cpsr;
  unsigned long ttbl;
  unsigned long domain;
  unsigned short chipid = RCPC_CHIPID;
  char* sz = NULL;
#if defined (BOOT_PBC)
  unsigned char bootmode = BOOT_PBC;
  char* szBootmode = NULL;
#endif

  switch (chipid>>4) {
  default   : sz = "lh?"; break;
  case 0x520: sz = "lh79520"; break;
  case 0x524: sz = "lh79524"; break;
  case 0x525: sz = "lh79525"; break;
  }

#if defined (BOOT_PBC)
  switch (bootmode) {
  case 0 ... 3:
  case 8:
  case 9:
    szBootmode = "NOR flash";
    break;
  case 4 ... 7:
  case 0xc:
  case 0xd:
    szBootmode = "NAND flash";
    break;
  case 0xe:
    szBootmode = "I2C";
    break;
  case 0xf:
    szBootmode = "UART";
    break;
  }
#endif

  __asm volatile ("mrc p15, 0, %0, c0, c0, 0" : "=r" (id));
  __asm volatile ("mrc p15, 0, %0, c0, c0, 1" : "=r" (cache));
  __asm volatile ("mrc p15, 0, %0, c1, c0, 0" : "=r" (ctrl));
  __asm volatile ("mrc p15, 0, %0, c2, c0, 0" : "=r" (ttbl));
  __asm volatile ("mrc p15, 0, %0, c3, c0, 0" : "=r" (domain));
  __asm volatile ("mrs %0, cpsr"	   : "=r" (cpsr));

  printf ("  cpu:      id 0x%08lx    ctrl 0x%08lx (%s)   cpsr 0x%08lx\n"
	  "          ttbl 0x%08lx  domain 0x%08lx  cache 0x%08lx\n",
	  id, ctrl, cp15_ctrl (ctrl), cpsr, ttbl, domain, cache);


  printf ("          chipid 0x%x, %s"
#if defined (BOOT_PBC)
	  "  bootmode 0x%x, %s"
#endif
	  "\n",
	  chipid, sz
#if defined (BOOT_PBC)
	  ,bootmode, szBootmode
#endif
	  );

#if defined (CPLD_REVISION)
  printf ("  cpld:   revision 0x%x\n", CPLD_REVISION);
#endif
}

static __service_7 struct service_d cpuinfo_service = {
  .report = cpuinfo_report,

};
#endif
