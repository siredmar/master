/* cmd-erase.c

   written by Marc Singer
   5 Nov 2004

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
#include <linux/ctype.h>
#include <apex.h>
#include <command.h>
#include <driver.h>
#include <error.h>


int cmd_erase (int argc, const char** argv)
{
  struct descriptor_d d;
  int result;

  if (argc != 2)
    return ERROR_PARAM;

  if ((result = parse_descriptor (argv[1], &d))) {
    printf ("Unable to open '%s'\n", argv[1]);
    return result;
  }

  if (!d.driver->erase)
    return ERROR_UNSUPPORTED;

  if (!d.length)
    d.length = 1;

  if (d.driver->open (&d)) {
    d.driver->close (&d);
    return ERROR_OPEN;
  }

  d.driver->erase (&d, d.length);

  d.driver->close (&d);

  return 0;
}

static __command struct command_d c_erase = {
  .command = "erase",
  .func = cmd_erase,
  COMMAND_DESCRIPTION ("erase device region")
  COMMAND_HELP(
"erase DST\n"
"  Erases the DST region.  The default length is 1 which will\n"
"  erase a single flash block.  APEX will report an error if the\n"
"  DST driver does not support an erase function.\n"
"  e.g.  erase nor:0           # Erase first flash block\n"
  )
};
