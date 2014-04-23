/* memory.h
   
   written by Durgesh Pattamatta
   10 Oct 2008

   Copyright (C) 2008 NXP Semiconductors

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

/* ----- Includes */

#include <config.h>
#include <linux/types.h>
#include <apex.h>
#include <command.h>
#include <driver.h>
#include <error.h>
#include <spinner.h>
#include <mach/hardware.h>

/* ----- Constants */
#define crc32table  ((const u32*)0x12015CBC)

void crc32_initialise(u32 *crc)
{
  *crc = 0xFFFFFFFF;
}

void crc32_generate(u32 *pcrc, u8 *data, int length)
{
  u32 crc = *pcrc;

  while (length--)
  {
    crc = crc32table[(crc ^ *data++) & 0xFF] ^(crc >> 8);
  }

  *pcrc = crc;
}
void crc32_complete(u32 *pcrc)
{
  *pcrc ^= 0xFFFFFFFF;
}

u32 crc32_compute(u8 *data, int length)
{
  u32 crc ;

  crc32_initialise(&crc);
  crc32_generate(&crc, data, length);
  crc32_complete(&crc);
  return crc;
}

int cmd_crc (int argc, const char** argv)
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

  crc = crc32_compute ((u8*)d.start, cb);

  printf ("\rcrc32 0x%lx (%lu) over %d (0x%x) bytes\n", crc, crc, cb, cb);

 fail:
  close_descriptor (&d);

  return result;
}

static __command struct command_d c_crc = {
  .command = "crc",
  .func = cmd_crc,
  COMMAND_DESCRIPTION ("compute crc32 checksum using lpc313x tables")
  COMMAND_HELP(
"checksum REGION\n"
"  Calculate a CRC32 checksum over REGION.\n"
  )

};
