/* command-history.c

   written by Marc Singer
   31 Mar 2006

   Copyright (C) 2006 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   Ring-buffer command history.  Commands are stored in the buffer as
   a null-terminated string followed by a 16 bit length.  The string
   is padded to an even number of bytes, including the terminating
   null so that the length is aligned.  There are two pointers, one
   for the location where the next command will be saved and another
   for the current history scroll.  Both start at beginning of the
   buffer and both are moved to the first byte after the length of a
   command as it is added.  Scrollback is handled by finding the
   beginning of the last stored command and then returning that
   string.  Scroll-forward uses the null-terminated attribute of the
   strings.  It doesn't do any checking when adding strings to the
   buffer.  It only checks when it scrolls backward to make sure that
   the historical command doesn't span the next command pointer.

*/

#include <stdarg.h>
#include <driver.h>
#include <linux/string.h>
#include <linux/ctype.h>

#include <apex.h>
#include <command.h>

#include <debug_ll.h>

#define CB_HISTORY	(4*1024)

static char rgbHistory[CB_HISTORY];
int ibHistoryNext;
int ibHistoryScroll;

void history_add (const char* sz)
{
  size_t cb = strlen (sz) + 1;	/* Real storage of string */
  size_t cbStore = ((cb + 1)&~1) + 2;
  size_t available;

  if (cb == 1)
    return;			/* Skip empty strings */
  if (cbStore + 2 >= CB_HISTORY)
    return;			/* Can't store this one */

  while (cb) {
    available = CB_HISTORY - ibHistoryNext;
    if (available > cb)
      available = cb;
    memcpy (rgbHistory + ibHistoryNext, sz, available);
    cb -= available;
    sz += available;
    ibHistoryNext = (ibHistoryNext + available)%CB_HISTORY;
  }
  /* *** We could just force the copy loop to copy an even number of
     bytes, but that assumes that there is a byte after the null.  Bad
     form. */
  ibHistoryNext = ((ibHistoryNext + 1)&~1)%CB_HISTORY;

  *(unsigned short*) &rgbHistory[ibHistoryNext] = cbStore;

  ibHistoryNext = (ibHistoryNext + 2)%CB_HISTORY;
  ibHistoryScroll = ibHistoryNext;
  //  printf ("h %d %d\n", cbStore, ibHistoryNext);
}

int history_retrieve (int backward, char* rgb, size_t cbMax)
{
  int ib = ibHistoryScroll;
  int ibStart;
  size_t available;

  if (backward) {
    size_t cbStore;
    ib = (ib + CB_HISTORY - 2)%CB_HISTORY;
    //    printf ("ib %d\n", ib);
    if (ib == ibHistoryNext)
      return 0;			/* overlap */
    cbStore = *(unsigned short*) &rgbHistory[ib];
    //    printf ("cbStore %d\n", cbStore);
    if (cbStore == 0)
      return 0;			/* No more history */
    ibStart = (ib + CB_HISTORY - cbStore + 2)%CB_HISTORY;
    //    printf ("ibStart %d\n", ibStart);
    if (ibStart == ibHistoryNext)
      return 0;			/* overlap */
    if (ib > ibStart && ib > ibHistoryNext && ibStart < ibHistoryNext)
      return 0;			/* overlap */
    if (ib < ibStart && (ibHistoryNext < ib || ibHistoryNext > ibStart))
      return 0;			/* overlap */
    if (cbStore - 2 > cbMax)
      return 0;			/* *** buffer undersized */
    ibHistoryScroll = ibStart;
    while (cbStore) {
      available = CB_HISTORY - ibStart;
      if (available > cbStore)
	available = cbStore;
      memcpy (rgb, rgbHistory + ibStart, available);
      cbStore -= available;
      ibStart = (ibStart + available)%CB_HISTORY;
      rgb += available;
    }
  }
  else {
    size_t cb;
	/* forward */
    if (ib == ibHistoryNext)
      return 0;			/* No forward history */

	/* skip entry we're pointing at */
    available = CB_HISTORY - ib;
    cb = strnlen (rgbHistory + ib, available);
    if (cb == available)
      cb += strlen (rgbHistory);
    cb = ((cb + 1 + 1)&~1) + 2;
    ib = (ib + cb)%CB_HISTORY;
    if (ib == ibHistoryNext)
      return 0;			/* Still no forward history */
    ibHistoryScroll = ib;

    //    printf ("#");
    available = CB_HISTORY - ib;
    cb = strnlen (rgbHistory + ib, available);
    memcpy (rgb, rgbHistory + ib, cb);
    if (cb == available)
      cb += strlen (rgbHistory);
    cb = (cb + 1 + 1)&~1;
    if (cb > cbMax)
      return 0;			/* *** buffer undersized */
    //    ibHistoryScroll = (ib + cb + 2)%CB_HISTORY;
    if (available > cb)
      available = cb;
    memcpy (rgb, rgbHistory + ib, available);
    rgb += available;
    cb -= available;
    if (cb)
      memcpy (rgb, rgbHistory, cb);
  }

  //  printf ("{%d %d}\n", ibHistoryNext, ibHistoryScroll);
  return 1;
}

static __command struct command_d c_help_commandhistory = {
  .command = "command-history",
  .description = "help on using the command history",
  COMMAND_HELP(
"command-history\n"
"  Previously entered commands are available for reuse and editing.\n"
"  Use ^P or the UP arrow (when ANSI key interpretation is enabled)\n"
"  to scroll backward through the command history.  Use ^N or the DOWN\n"
"  arrow to to scroll forward.\n"
  )
};
