/* cmd-copy.c

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

#include <config.h>
#include <linux/string.h>
#include <linux/types.h>
#include <linux/ctype.h>
#include <apex.h>
#include <command.h>
#include <driver.h>
#include <error.h>
#include <spinner.h>
#include "region-copy.h"

int cmd_copy (int argc, const char** argv)
{
  struct descriptor_d din;
  struct descriptor_d dout;
  unsigned flags = regionCopySpinner;
#if defined (USE_COPY_VERIFY)
  struct descriptor_d din_v;
  struct descriptor_d dout_v;
#endif

  int result = 0;

  for (; argc > 1 && *argv[1] == '-'; --argc, ++argv) {
    switch (argv[1][1]) {
    case 'v':
      flags |= regionCopyVerify;
      break;
    case 's':
      flags |= regionCopySwap;
      break;
    default:
      return ERROR_PARAM;
    }
  }

  if (argc != 3)
    return ERROR_PARAM;

  if (   (result = parse_descriptor (argv[1], &din))
      || (result = open_descriptor (&din))) {
    printf ("Unable to open '%s'\n", argv[1]);
    goto fail_early;
  }

  if (   (result = parse_descriptor (argv[2], &dout))
      || (result = open_descriptor (&dout))) {
    printf ("Unable to open '%s'\n", argv[2]);
    goto fail;
  }

  if (!din.driver->read || !dout.driver->write) {
    result = ERROR_UNSUPPORTED;
    goto fail;
  }

  if (!dout.length)
    dout.length = DRIVER_LENGTH_MAX;

  result = region_copy (&dout, &din, flags);

  if (result > 0)
    printf ("\r%d bytes transferred\n", result);

 fail:
  close_descriptor (&din);
  close_descriptor (&dout);
 fail_early:

  return result <= 0 ? result : 0;
}

	/* Work-around for gcc-2.95 */
#if defined (USE_DIAG)
# define _USE_COPY_VERIFY(s) s
#else
# define _USE_COPY_VERIFY(s)
#endif


static __command struct command_d c_copy = {
  .command = "copy",
  .func = cmd_copy,
  COMMAND_DESCRIPTION ("copy data between devices")
  COMMAND_HELP(
"copy"
_USE_COPY_VERIFY(
" [-v]"
)
" [-s] SRC DST\n"
"  Copy data from SRC region to DST region.\n"
_USE_COPY_VERIFY(
"  Adding the -v performs redundant reads of the data to verify that\n"
"  what is read from SRC is correctly written to DST\n"
)
"  Adding the -s performs full word byte swap.  This is necessary when\n"
"  copying data stored in the opposite endian orientation from that which\n"
"  APEX is running.  This option requires that the length be an even\n"
"  multiple of words.\n"
"  The length of the DST region is ignored.\n\n"
"  e.g.  copy mem:0x20200000+0x4500 nor:0\n"
"        copy -s nor:0+1m 0x100000\n"
  )
};
