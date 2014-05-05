/* cmd-standby.c

   written by Marc Singer
   12 July 2006

   Copyright (C) 2006 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   Put CPU into stanby mode.  This function tests the standby
   functionality.

*/

#include <config.h>
#include <apex.h>
#include <command.h>
#include <service.h>

#include "hardware.h"

#define CSC_STANDBY	__REG (CSC_PHYS + 0x0c)	/* Enter STANDBY mode */


static void cmd_standby (int argc, const char** argv)
{
  printf ("Entering STANDBY.\n\n");

  release_services ();		/* Primarily to prep NOR flash */

  CSC_PWRCNT &= ~(1<<1);	/* Allow wakeup to bring us out of sleep */
  //  CSC_PWRCNT |=   1<<1;
  CSC_STANDBY;
  __asm__ volatile ("nop\n\t"
		    "nop\n\t"
		    "nop\n\t"
		    "nop\n\t"
		    "nop\n\t"
		    "nop\n\t"
		    "nop\n\t"
		    "nop\n\t"
		    "nop\n\t"
		    "nop\n\t" );

  init_services ();

  printf ("\nEntering RUN.\n");
}

static __command struct command_d c_standby = {
  .command = "standby",
  .func = (command_func_t) cmd_standby,
  COMMAND_DESCRIPTION ("enter STANDBY mode")
  COMMAND_HELP(
"standby\n"
"  Put system in STANDBY mode.\n"
"  This will put the CPU in a low-power sleep mode.  Any interrupt\n"
"  will bring the CPU into RUN mode from STANDBY.\n"
  )
};
