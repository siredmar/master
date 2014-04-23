/* atag-marvell.c

   written by Marc Singer
   8 Nov 2008

   Copyright (C) 2008 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   o From the Marvell loader:

apex> du -4 0x100+256
00000100: 00000005 54410001  00000000 00000000  ......AT ........
00000110: 00000000 00000004  54410002 08000000  ........ ..AT....
00000120: 00000000 00000004  54410002 00000000  ........ ..AT....
00000130: ffffffff 00000004  54410002 00000000  ........ ..AT....
00000140: ffffffff 00000004  54410002 00000000  ........ ..AT....
00000150: ffffffff 00000008  54410009 736e6f63  ........ ..ATcons
00000160: 3d656c6f 53797474  31312c30 30303235  ole=ttyS 0,115200
00000170: aaaaaa00 00000009  41000403 02030b23  ........ ...A#...
00000180: 09ef21ab 09ef21ab  00000003 00000000  .!...!.. ........
00000190: 83e40a00 aaaa71a5  00000000 00000000  .....q.. ........
000001a0: aaaaaaaa aaaaaaaa  aaaaaaaa aaaaaaaa  ........ ........



*/

#include <atag.h>
#include <linux/string.h>
#include <config.h>
#include <apex.h>
//#include <lookup.h>
#include "hardware.h"

#define ATAG_MV_UBOOT		0x41000403

struct tag_mv_uboot {
  u32 uboot_version;
  u32 tclk;
  u32 sysclk;
  u32 isUsbHost;
  u32 overEthAddr;
  char macAddr[6];
};

struct tag* atag_mv_uboot (struct tag* p)
{
  struct tag_mv_uboot* tag = (struct tag_mv_uboot*)(&p->u);
  p->hdr.tag = ATAG_MV_UBOOT;
  p->hdr.size = tag_size (tag_mv_uboot);

  memset (tag, 0, sizeof (*tag));
  tag->uboot_version = 0x02030b23;		/* Stock kernel requires this */
  tag->tclk = get_tclk ();
  tag->sysclk = get_tclk ();    		/* CPU Bus clock */

# if !defined (CONFIG_SMALL)
  printf ("ATAG_MV_UBOOT: tclk %d  sysclk %d\n", tag->tclk, tag->sysclk);
# endif

  return tag_next (p);
}

static __atag_6 struct atag_d _atag_mv_uboot = { atag_mv_uboot };
