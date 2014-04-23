/* cmd-parse.c

   written by Marc Singer
   21 Dec 2008

   Copyright (C) 2008 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#include <config.h>
#include <linux/types.h>
#include <apex.h>
#include <error.h>
#include <command.h>
#include <driver.h>
#include <environment.h>
#include <alias.h>
#include <describe.h>

int cmd_parse (int argc, const char** argv)
{
  struct descriptor_d d;
  int result;

  if (argc != 2)
    return ERROR_PARAM;

  result = parse_descriptor (argv[1], &d);
  if (result)
    return result;

  printf ("driver:   '%s' -> 0x%p '%s'\n", d.driver_name, d.driver,
          d.driver ? d.driver->name : "undefined");
  printf ("start:    0x%08x %s\n", (size_t) d.start, describe_size ((size_t) d.start));
  printf ("length:   0x%08x %s\n", (size_t) d.length, describe_size ((size_t) d.length));
  printf ("index:    0x%08x %s\n", (size_t) d.index, describe_size ((size_t) d.index));
  printf ("width:    %d\n", (size_t) d.width);
  if (d.c) {
    int i;
    printf ("path:     [iRoot %d c %d] ", d.iRoot, d.c);
    if (d.iRoot)
      printf ("/");
    for (i = 0; i < d.c; ++i)
      printf ("/%s", d.pb[i]);
    printf ("\n");
  }

  return 0;
}

static __command struct command_d c_parse = {
  .command = "parse",
  .func = cmd_parse,
  COMMAND_DESCRIPTION ("parse region and show result")
  COMMAND_HELP(
"parse REGION\n"
"  Parse REGION and show the result\n"
)
};
