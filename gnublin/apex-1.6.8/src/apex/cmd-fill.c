/* cmd-fill.c

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

   For the time being, this function can only fill with bytes.

*/

#include <config.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <apex.h>
#include <command.h>
#include <driver.h>
#include <error.h>
#include <spinner.h>


int cmd_fill (int argc, const char** argv)
{
  struct descriptor_d dout;
  char __aligned rgb[4];
  int result = 0;
  static int width;

  /* Parse arguments */
  while (argc > 1 && *argv[1] == '-') {
    switch (argv[1][1]) {
    case '1':
    case '2':
    case '4':
      width = argv[1][1] - '0';
      break;
    default:
      return ERROR_PARAM;
      break;
    }
    --argc;
    ++argv;
  }

  if (argc != 3)
    return ERROR_PARAM;

  *(unsigned long*) rgb = simple_strtoul (argv[1], NULL, 0);

  if (   (result = parse_descriptor (argv[2], &dout))
      || (result = open_descriptor (&dout))) {
    printf ("Unable to open '%s'\n", argv[2]);
    goto fail_early;
  }

  if (!dout.driver->write) {
    result = ERROR_UNSUPPORTED;
    goto fail;
  }

  if (width == 0)
    width = 1;
  dout.width = width;

  if (!dout.length)
    dout.length = width;

  {
    //    char rgb[512];
    ssize_t cb = dout.length;
    int step = (dout.driver->flags >> DRIVER_WRITEPROGRESS_SHIFT)
      &DRIVER_WRITEPROGRESS_MASK;
    if (step)
      step += 10;

    for (; cb > 0; cb -= width) {
      size_t cbWrote;
      SPINNER_STEP;
      cbWrote = dout.driver->write (&dout, rgb, width);
      if (!cbWrote) {
	result = ERROR_FAILURE;
	goto fail;
      }
    }
  }

 fail:
  close_descriptor (&dout);
 fail_early:

  return result;
}

static __command struct command_d c_fill = {
  .command = "fill",
  .func = cmd_fill,
  COMMAND_DESCRIPTION ("fill a region with a byte")
  COMMAND_HELP(
"fill [-1|-2|-4] VALUE DST\n"
"  Fills the DST region with the value VALUE.\n"
"  The default DST region length is the fill width.  The default\n"
"  fill width is 1 byte.  The -# options change the fill width.\n"
"  The width option is sticky for this command--once set it will\n"
"  remain in effect until changed with another fill command.\n"
"  This command cannot be used to erase flash.\n\n"
"  e.g.  fill 0xe5 0x100           # Write one 0xe5 byte\n"
"        fill -4 0xe5 0x100+256    # Write 64 0x000000e5's\n"
"                                  #  & set default fill width to 4\n"
  )
};
