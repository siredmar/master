/* cmd-sleep.c

   written by Marc Singer
   30 May 2007

   Copyright (C) 2007 Marc Singer

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
#include <error.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <asm/mmu.h>
#include <asm/cp15.h>

#include "hardware.h"


static int cmd_sleep (int argc, const char** argv)
{
  unsigned long ccmr;
  int mode;
  int well_bias = 0;

  if (argc < 2)
    return ERROR_PARAM;

  mode = simple_strtoul (argv[1], NULL, 10);

  switch (mode) {
  default:
  case 0:
    printf ("Entering wait mode\n");
    mode = 0;
    break;
  case 1:
    printf ("Entering doze mode\n");
    mode = 1;
    break;
  case 2:
    printf ("Entering state retention mode\n");
    mode = 2;
    well_bias = 1;
    break;
  case 3:
    printf ("Entering deep sleep mode\n");
    mode = 3;
    well_bias = 1;
    break;
  }

  ccmr = CCM_CCMR;

  MASK_AND_SET (ccmr, 3<<14, mode<<14); /* Set the mode we'll enter */
  MASK_AND_SET (ccmr, 1<<27, well_bias<<27); /* Set the mode we'll enter */
  ccmr |= 1<<28;		/* VSTBY */

  printf ("CCM_CCMR 0x%08lx\n", ccmr);
  CCM_CCMR = ccmr;

  CCM_WIMR0 = 0;
  CCM_CGR0 = 0xffffffff;
  CCM_CGR1 = 0xffffffff;
  CCM_CGR2 = 0xffffffff;

		/* Flush serial device */
  release_services ();

  /* We may want to flush caches before waiting for interrupts.  It
     depends on whether the wait mode retains the state of the
     cache. */

  WAIT_FOR_INTERRUPT;

  /* We will get here only if we implement some form of wake-up.  */

  return 0;
}

static __command struct command_d c_sleep = {
  .command = "sleep",
  .func = (command_func_t) cmd_sleep,
  COMMAND_DESCRIPTION ("sleep test ")
  COMMAND_HELP(
"sleep N\n"
"  Put CPU into a sleep mode.\n"
"  The mode, N, is one of the following:\n"
"    0 - enter wait mode\n"
"    1 - enter doze mode\n"
"    2 - enter state retention mode\n"
"    3 - enter deep sleep mode\n"
  )
};
