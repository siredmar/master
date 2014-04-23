/* cmd-image.c

   written by Marc Singer
   3 Aug 2008

   Copyright (C) 2008 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   o Dispatch to image handlers.  This code is a simple dispatcher for
     the various image handles.  There are only two, so it is a bit of
     over-engineering, but the fact that the two handlers can be
     included independently makes some sort of dispatch desirable.

*/

#undef TALK
//#define TALK 2

#include <config.h>
#include <apex.h>
#include <command.h>
#include <error.h>
#include <driver.h>
#include <linux/kernel.h>
#include "cmd-image.h"
#include <talk.h>

#include <debug_ll.h>

int cmd_image (int argc, const char** argv)
{
  struct descriptor_d d;
  int result = 0;
  int op = 's';
  struct image_info info;
  char rgb[16];                 /* Storage for bytes to determine image type */
  pfn_handle_image pfn = NULL;


  memset (&info, 0, sizeof (info));
  info.initrd_relocation = ~0;

  if (argc < 2)
    return ERROR_PARAM;

  while (argc > 2) {

    if (argv[1][0] == '-') {
      switch (argv[1][1]) {
      case 'r':
        if (argc < 3)
          return ERROR_PARAM;
        info.initrd_relocation = simple_strtoul (argv[2], NULL, 0);
        argc -= 2;
        argv += 2;
        break;
      default:
        return ERROR_PARAM;
      }
      continue;
    }

#if defined (CONFIG_CMD_IMAGE_SHOW)
    if      (PARTIAL_MATCH (argv[1], "s", "how") == 0)
      op = 's';
    else
#endif
    if      (PARTIAL_MATCH (argv[1], "c", "heck") == 0)
      op = 'c';
    else if (PARTIAL_MATCH (argv[1], "l", "oad") == 0)
      op = 'l';
    else
      ERROR_RETURN (ERROR_PARAM, "unrecognized OP");
    --argc;
    ++argv;
  }

  if ((result = parse_descriptor (argv[1], &d))) {
    printf ("Unable to parse '%s'\n", argv[1]);
    goto fail;
  }

  info.fRegionCanExpand = d.length == 0;

  /* *** Originally, this call wasn't made.  Trouble is that we cannot
     *** use a tftp descriptor without it.  So, now we check for zero
     *** length before opening and then we open.  */
  if ((result = open_descriptor (&d))) {
    printf ("Unable to open '%s'\n", argv[1]);
    goto fail;
  }

  if (info.fRegionCanExpand && d.length - d.index < sizeof (rgb))
    d.length = d.index + sizeof (rgb);

	/* Read some bytes so we can determine the image type */
  DBG (1,"start %ld length %ld index %d\n", d.start, d.length, d.index);
  result = d.driver->read (&d, rgb, sizeof (rgb));
  if (result != sizeof (rgb)) {
    DBG (1,"result %d != %d\n", result, sizeof (rgb));
    result = ERROR_RESULT (ERROR_IOFAILURE, "image read error");
    goto fail;
  }

	/* Detect image type */
#if defined (CONFIG_CMD_IMAGE_APEX)
  if (pfn == NULL)
    pfn = is_apex_image (rgb, sizeof (rgb));
#endif
#if defined (CONFIG_CMD_IMAGE_UBOOT)
  if (pfn == NULL)
    pfn = is_uboot_image (rgb, sizeof (rgb));
#endif

  if (pfn)
    result = pfn (op, &d, &info);
  else
    result = ERROR_RESULT (ERROR_UNRECOGNIZED, "unrecognized image");

 fail:
  close_descriptor (&d);
  return result;
}

static __command struct command_d c_image = {
  .command = "image",
  .func = cmd_image,
  COMMAND_DESCRIPTION ("image handling")
  COMMAND_HELP(
"image [OP|OPTIONS] REGION\n"
"  Operation on an image.\n"
"  Without an OP, the image command verifies the integrity of the image.\n"
"  One and only one OP may be specified:\n"
"    load     - load image payload or payloads into memory\n"
#if defined (CONFIG_CMD_IMAGE_SHOW)
"    show     - show the image metadata\n"
#endif
"    check    - check the integrity of the image including payload CRCs\n"
"  Options:\n"
"    -r ADDR  - Relocate ramdisk to ADDR on load.  Important for uImages.\n"
"  In some cases, the region length may be omitted and the command\n"
"  will infer the length from the image header.\n"
"  e.g.  image check 0xc1000000\n"
#if defined (CONFIG_CMD_IMAGE_SHOW)
"        image show  0xc0000000\n"
#endif
"        image load  0xc0000000\n"
  )
};
