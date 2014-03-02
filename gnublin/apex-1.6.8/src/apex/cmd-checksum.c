/* cmd-checksum.c

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

#include <config.h>
#include <linux/types.h>
#include <apex.h>
#include <command.h>
#include <driver.h>
#include <error.h>
#include <spinner.h>
#include "region-checksum.h"

extern unsigned long compute_crc32 (unsigned long crc, const void *pv, int cb);

int cmd_checksum (int argc, const char** argv)
{
  struct descriptor_d d;
  int result = 0;
  unsigned long crc = 0;
  ssize_t cb;

  if (argc < 2)
    return ERROR_PARAM;

  if (   (result = parse_descriptor (argv[1], &d))
      || (result = open_descriptor (&d))) {
    printf ("Unable to open '%s'\n", argv[1]);
    goto fail;
  }

  cb = d.length - d.index;

  result = region_checksum (0, &d, regionChecksumSpinner | regionChecksumLength,
                            &crc);
  if (result)
    goto fail;

  printf ("\rcrc32 0x%lx (%lu) over %d (0x%x) bytes\n", ~crc, ~crc, cb, cb);

 fail:
  close_descriptor (&d);

  return result;
}

static __command struct command_d c_checksum = {
  .command = "checksum",
  .func = cmd_checksum,
  COMMAND_DESCRIPTION ("compute crc32 checksum")
  COMMAND_HELP(
"checksum REGION\n"
"  Calculate a CRC32 checksum over REGION.\n"
"  The result conforms to the POSIX standard for the cksum command.\n"
  )

};
