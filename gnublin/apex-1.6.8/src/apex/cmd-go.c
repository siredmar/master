/* cmd-go.c

   written by Marc Singer
   3 Feb 2005

   Copyright (C) 2004 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#include <linux/types.h>
#include <linux/string.h>
#include <apex.h>
#include <command.h>
#include <error.h>
#include <linux/kernel.h>
#include <config.h>
#include <environment.h>
#include <service.h>

int cmd_go (int argc, const char** argv)
{
  unsigned long address;

  if (argc != 2)
    return ERROR_PARAM;


  address = simple_strtoul (argv[1], NULL, 0);

  printf ("Jumping to 0x%p...\n", (void*) address);

  //serial_flush_output();

  release_services ();

  __asm volatile ("mov pc, %0" :: "r" (address));

  return 0;
}

static __command struct command_d c_go = {
  .command = "go",
  .func = cmd_go,
  COMMAND_DESCRIPTION ("execute program at address")
  COMMAND_HELP(
"go ADDRESS\n"
"  Transfer execution to the program at ADDRESS.\n"
"  Unlike the boot command, go does not pass data to a Linux\n"
"  kernel.  Services and drivers will be disabled and the MMU\n"
"  will be off before jumping to ADDRESS.\n\n"
/* Cannot use this example until the ; works on the command line */
//"  e.g.  xreceive 0xb0000000 ; go 0xb0000000\n"
  )
};
