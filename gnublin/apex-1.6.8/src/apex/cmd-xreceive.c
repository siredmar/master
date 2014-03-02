/* cmd-xreceive.c

   written by Marc Singer
   6 Nov 2004

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

extern int xmodem_receive (struct descriptor_d*);

int cmd_xreceive (int argc, const char** argv)
{
  struct descriptor_d d;
  int result;

  if (argc != 2)
    return ERROR_PARAM;

  if ((result = parse_descriptor (argv[1], &d))) {
    printf ("Unable to open '%s'\n", argv[1]);
    return result;
  }

  if (!d.length)
    d.length = DRIVER_LENGTH_MAX;

  if (d.driver->open (&d)) {
    d.driver->close (&d);
    return ERROR_OPEN;
  }

  {
    int cbRead = xmodem_receive (&d);
    printf ("%d bytes received\n", cbRead);
  }

  d.driver->close (&d);

  return 0;
}

static __command struct command_d c_xreceive = {
  .command = "xreceive",
  .func = cmd_xreceive,
  COMMAND_DESCRIPTION ("receive data over the serial line")
  COMMAND_HELP(
"xreceive REGION\n"
"  Receive binary data via the console using Xmodem protocol.\n"
"  Data is written directly to REGION.\n"
"  The length of REGION is ignored.\n\n"
"  e.g.  xreceive 0x2001000    # Receive and write to SDRAM\n"
"        xreceive nor:0        # Receive and write to NOR flash\n"
  )
};
