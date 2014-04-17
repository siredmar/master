/* cmd-info.c

   written by Marc Singer
   10 June 2005

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
#include <linux/string.h>
#include <linux/types.h>
#include <linux/ctype.h>
#include <apex.h>
#include <command.h>
#include <driver.h>
#include <error.h>
#include <spinner.h>


#if !defined (CONFIG_SMALL)
//# define USE_COPY_VERIFY	/* Define to include verify feature */
#endif

int cmd_info (int argc, const char** argv)
{
  struct descriptor_d din;

  int result = 0;

  if (argc != 2)
    return ERROR_PARAM;

  if ((result = parse_descriptor (argv[1], &din))) {
    printf ("Unable to open '%s'\n", argv[1]);
    goto fail_early;
  }

  if (!din.driver->info) {
    result = ERROR_UNSUPPORTED;
    goto fail;
  }

  din.driver->info (&din);

 fail:
  close_descriptor (&din);
 fail_early:

  return result;
}

static __command struct command_d c_info = {
  .command = "info",
  .func = cmd_info,
  COMMAND_DESCRIPTION ("display information about a region descriptor")
  COMMAND_HELP(
"info"
" SRC\n"
"  Display information about the region.  This command either describes\n"
"  the device controlling the region descriptor, or the descriptor itself.\n"
"  If the region is a filesystem directory, the command lists the files\n"
"  in the directory.\n"
"  e.g.  info jffs2:/etc\n"
  )
};
