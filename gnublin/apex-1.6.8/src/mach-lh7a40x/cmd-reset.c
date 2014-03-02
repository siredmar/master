/* cmd-reset.c

   written by Marc Singer
   27 Dec 2005

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
#include <command.h>
#include <service.h>

#include "hardware.h"

#define WDT_CTL		__REG (WDT_PHYS + 0x00)
#define WDT_RST		__REG (WDT_PHYS + 0x04)
#define WDT_STATUS	__REG (WDT_PHYS + 0x08)
#define WDT_COUNT0	__REG (WDT_PHYS + 0x0c)
#define WDT_COUNT1	__REG (WDT_PHYS + 0x10)
#define WDT_COUNT2	__REG (WDT_PHYS + 0x14)
#define WDT_COUNT3	__REG (WDT_PHYS + 0x18)

#define WDT_CTL_TOC_16	(0x0<<4)
#define WDT_CTL_TOC_32	(0xf<<4)
#define WDT_CTL_FRZ	(1<<3)
#define WDT_CTL_IF	(1<<1)
#define WDT_CTL_EN	(1<<0)
#define WDT_RST_V	(0x1984)


static void cmd_reset (int argc, const char** argv)
{
  release_services ();		/* Primarily to prep NOR flash */

  WDT_CTL  = WDT_CTL_TOC_16;
  WDT_RST  = WDT_RST_V;
  WDT_CTL |= WDT_CTL_EN;

  while (1)
    ;
}

static __command struct command_d c_reset = {
  .command = "reset",
  .func = (command_func_t) cmd_reset,
  COMMAND_DESCRIPTION ("reset target")
  COMMAND_HELP(
"reset\n"
"  Reset the system.\n"
"  This will perform a (nearly) immediate, hard reset of the CPU.\n"
  )
};
