/* cmd-flashusage.c

   written by Marc Singer
   4 Jan 2009

   Copyright (C) 2009 Marc Singer

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
#include <describe.h>


static int cmd_flashusage (int argc, const char** argv)
{
  struct descriptor_d d;
  int result;
  driver_size_t start;
  driver_ssize_t cb;
  int block = 0;

  if (argc != 2)
    return ERROR_PARAM;

  if ((result = parse_descriptor (argv[1], &d))) {
    printf ("Unable to open '%s'\n", argv[1]);
    return result;
  }

  if (!d.driver->query)
    return ERROR_UNSUPPORTED;

  start = d.start;
  d.start = 0;              /* Always scan from beginning of device */
  if (!d.length) {
    unsigned long size;
    descriptor_query (&d, QUERY_SIZE, &size);
    d.length = size - d.start;
  }
  else
    d.length += start;

  if (d.driver->open (&d)) {
    d.driver->close (&d);
    return ERROR_OPEN;
  }

  for (cb = d.length; cb > 0; ++block) {
    char rgb[256];
    bool in_use;
    unsigned long eraseblocksize;
    descriptor_query (&d, QUERY_ERASEBLOCKSIZE, &eraseblocksize);
    result = d.driver->read (&d, rgb, sizeof (rgb));
    if (result != sizeof (rgb)) {
      d.driver->close (&d);
      return result;
    }
    {
      int i;
      in_use = false;
      for (i = 0; i < sizeof (rgb); ++i)
        if (rgb[i] != 0xff) {
          in_use = true;
          break;
        }
    }
    cb -= eraseblocksize - sizeof (rgb);
    if (d.index - sizeof (rgb) + eraseblocksize > start)
      printf ("block #%2d %s (%s:0x%08x) %s\n",
              block,
              in_use ? "in-use" : "empty ",
              d.driver->name,
              (unsigned) d.index - sizeof (rgb),
              describe_size (eraseblocksize)
              );
    d.driver->seek (&d, eraseblocksize - sizeof (rgb), SEEK_CUR);
  }

  return 0;
}

static __command struct command_d c_flashusage = {
  .command = "flashusage",
  .func = cmd_flashusage,
  COMMAND_DESCRIPTION ("show flash device eraseblock usage")
  COMMAND_HELP(
"flashusage REGION\n"
"  Shows which erase blocks are in use in a NOR flash device.  The query\n"
"  is bounded by the region if the region has a start address or a length.\n"
  )
};
