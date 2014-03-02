/* mtdpartition.cc

   written by Marc Singer
   14 Jan 2007

   Copyright (C) 2007 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "mtdpartition.h"

char MTDPartition::g_rgb[MTDPartition::g_cbMax];
MTDPartition MTDPartition::g_rg[MTDPartition::g_cPartitionMax];


/* OSPartition::find

   scan the partitions look for the one that contains the given
   offset.  This is used to recover the partition from a raw NOR flash
   address.

*/

const MTDPartition MTDPartition::find (unsigned long addr)
{
  if (!g_rgb[0])
    init ();
  if (!g_rgb[0])
    return MTDPartition ();

  unsigned long base = 0;
  for (MTDPartition* p = g_rg; p->device; base += p->size, ++p) {
//    printf ("%6d %s\n", p->blocks, p->name);
    if (base + p->size > addr)
      return *p;
  }

  return MTDPartition ();
}

const MTDPartition MTDPartition::find (const char* sz)
{
  if (!g_rgb[0])
    init ();
  if (!g_rgb[0])
    return MTDPartition ();


  for (MTDPartition* p = g_rg; p->device; ++p) {
//    printf ("'%s' '%s'\n", p->name, sz);
    if (strcasecmp (p->name, sz) == 0)
      return *p;
  }

//  printf ("nothing\n");

  return MTDPartition ();
}


const MTDPartition MTDPartition::first (void)
{
  if (!g_rgb[0])
    init ();
  if (!g_rgb[0])
    return MTDPartition ();
  return g_rg[0];
}

const MTDPartition MTDPartition::next (void) const
{
  return (index + 1 < g_cPartitionMax && g_rg[index + 1].device)
    ? g_rg[index + 1]
    : MTDPartition ();
}

void MTDPartition::init (void)
{
  int fh = open ("/proc/mtd", O_RDONLY);
  if (fh == -1)
    return;
  int cb = read (fh, &g_rgb, sizeof (g_rgb) - 1);
  close (fh);
  g_rgb[cb] = 0;

#define SP "[[:space:]]+"
#define XD  "[[:xdigit:]]+"
#define RX "(mtd[0-9]+):" SP "(" XD ")" SP "(" XD ")"  SP "\"([^\"]+)\""

  regex_t rx;
  if (regcomp (&rx, RX, REG_EXTENDED | REG_NEWLINE))
    return;

#undef SP
#undef D
#undef RX

  regmatch_t m[5];
  char* pb = g_rgb;
  int c = 0;
  int base = 0;
  while (*pb) {
    int cbLine = strcspn (pb, "\n");
    char ch = pb[cbLine];
    pb[cbLine] = 0;
    if (regexec (&rx, pb, sizeof (m)/sizeof (*m), m, 0) == 0) {
      pb[m[1].rm_eo]    = 0;
      g_rg[c].device	= pb + m[1].rm_so;
      g_rg[c].size	= strtoul (pb + m[2].rm_so, NULL, 16);
      g_rg[c].erasesize = strtoul (pb + m[3].rm_so, NULL, 16);
      pb[m[4].rm_eo]    = 0;
      g_rg[c].name	= pb + m[4].rm_so;
      g_rg[c].base      = base;
      g_rg[c].index     = c;
      base             += g_rg[c].size;
      //      printf ("%s: 0x%x 0x%x '%s'\n",
//	      g_rg[c].device, g_rg[c].size, g_rg[c].erasesize, g_rg[c].name);
      ++c;
    }
    pb += cbLine + (ch ? 1 : 0);
  }
}
