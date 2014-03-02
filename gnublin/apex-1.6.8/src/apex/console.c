/* console.c

   written by Marc Singer
   1 Nov 2004

   Copyright (C) 2004-2008 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#include <config.h>
#include <stdarg.h>
#include <driver.h>
#include <linux/string.h>
#include <linux/ctype.h>

#include <apex.h>
#include <command.h>
#include <console.h>

#include <debug_ll.h>

#if defined (CONFIG_COMMAND_EDITING)
# define EDITING(a) a
#else
# define EDITING(a)
#endif

struct driver_d* console_driver;

/* Ring buffer for polled bytes */
static char __xbss(console) rgbBuffer[128];
static size_t cbBuffer;

ssize_t console_read (struct descriptor_d* d, void* pv, size_t cb)
{
  int available;
  ssize_t cbRead = 0;

  if (console_driver == 0)
    return 0;

  available = cbBuffer;
  if (available) {
    if (available > cb)
      available = cb;
    memcpy (pv, rgbBuffer, available);
    cbRead = available;
    if (cbBuffer > available)
      memmove (rgbBuffer, rgbBuffer + available, cbBuffer - available);
    cbBuffer -= available;
    pv += available;
    cb -= available;
  }
  /* *** FIXME: do we need to be concerned about the read function
     *** returning a negative number? */
  if (cb)
    return cbRead + console_driver->read (d, pv, cb);
  return cbRead;
}

ssize_t console_write (struct descriptor_d* d, const void* pv, size_t cb)
{
//  PUTC_LL ('#');

  if (console_driver == 0)
    return 0;
  return console_driver->write (d, pv, cb);
}

ssize_t console_poll (struct descriptor_d* d, size_t cb)
{
  if (console_driver == 0)
    return 0;

  if (cb && cbBuffer) {
    if (cb > cbBuffer)
      cb = cbBuffer;
    return cb;
  }

	/* Special break character polling */
  if (cb == 0) {
    while (cbBuffer < sizeof rgbBuffer
	   && console_driver->poll (d, 1)) {
      char ch;
      console_driver->read (d, &ch, 1);
      if (ch == 0x3)		/* *** FIXME: ^C check */
	return 1;
      rgbBuffer[cbBuffer++] = ch;
    }
    return 0;
  }

  return console_driver->poll (d, cb);
}

static __driver_0 struct driver_d _console = {
  .name = "console",
  .description = "console driver",
  .flags = DRIVER_SERIAL | DRIVER_CONSOLE,
  .read = console_read,
  .write = console_write,
  .poll = console_poll,
};

struct driver_d* __rodata console = &_console;

int puts (const char* fmt)
{
  return console->write (0, fmt, strlen (fmt));
}

int putchar (int ch)
{
  char rgb[1];
  rgb[0] = ch;
  return console->write (0, rgb, 1);
}

void backspaces (int c)
{
  while (c-- > 0)
    console->write (0, "\b", 1);
}

int read_command (const char* szPrompt, int* pargc, const char*** pargv)
{
  static char __xbss(console) rgb[CB_COMMAND_MAX]; /* Command line buffer */
  int cb;
#if defined (CONFIG_ANSI_KEYS)
  int ansi = 0;
  int ansi_value = 0;
#endif
  EDITING (int cursor = 0;)

#if 0
  {
    extern int ibHistoryNext;
    extern int ibHistoryScroll;
    printf ("{%d %d}\n", ibHistoryNext, ibHistoryScroll);
  }
#endif

  puts (szPrompt);

  for (cb = 0; cb < sizeof (rgb) - 1 && (cb == 0 || rgb[cb - 1]); ++cb) {
    console->read (0, &rgb[cb], 1);
    //    printf ("{%d-%d-%d}", rgb[cb], ansi, ansi_value);

#if defined (CONFIG_ANSI_KEYS)
    switch (ansi) {
    default:
      ansi = 0;
      goto pass;
    case 0:
      if (rgb[cb] == '\x1b') {
	++ansi;
	--cb;
      }
      else
	goto pass;
      break;
    case 1:
      if (rgb[cb] == '[')
	++ansi;
      else
	ansi = 0;
      --cb;
      break;
    case 2:
      ansi = 0;
      switch (rgb[cb]) {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
	ansi_value = rgb[cb] - '0';
	ansi = 3;
	--cb;
	break;
      case 'A':
	rgb[cb] = '\x10';	/* DOWN -> ^P */
	goto pass;
      case 'B':
	rgb[cb] = '\x0e';	/* UP -> ^N */
	goto pass;
      case 'C':
	rgb[cb] = '\x06';	/* RIGHT -> ^F */
	goto pass;
      case 'D':
	rgb[cb] = '\x02';	/* LEFT -> ^B */
	goto pass;
      default:
	--cb;
	break;
      }
      break;
    case 3:
      if (isdigit (rgb[cb]))
	ansi_value = ansi_value*10 + rgb[cb] - '0';
      else if (rgb[cb] == '~') {
	ansi = 0;
	switch (ansi_value) {
	default:
	  --cb;
	  break;
	case 3:
	  rgb[cb] = '\x04';	/* DELETE -> ^D */
	  goto pass;
	}
      }
      else {
	ansi = 0;		/* Abandon sequence */
	--cb;
      }
      break;
    }
    continue;
  pass:
#endif
    //    printf ("<%d>", rgb[cb]);
    switch (rgb[cb]) {
    case '\r':
      rgb[cb] = 0;		/* Mark end of input */
      console->write (0, "\r\n", 2);
      break;
    case '\b':			/* ^H - delete previous character */
#if defined (CONFIG_DEL_IS_BS)
    case '\x7f':		/* Allow DEL to be a synonym for BS */
#endif
#if defined (CONFIG_COMMAND_EDITING)
      if (cursor != cb) {
	if (cursor) {
	  backspaces (1);
	  memmove (rgb + cursor - 1, rgb + cursor, cb - cursor);
	  console->write (0, rgb + cursor - 1, cb - cursor);
	  console->write (0, " ", 1); /* Erase */
	  backspaces (cb - cursor + 1);
	  --cb;
	  --cursor;
	}
	--cb;
      }
      else
#endif
      if (cb) {
	console->write (0, "\b \b", 3);
	cb -= 2;
	EDITING (--cursor;)
      }
      else
	--cb;			/* Stay at the start */
      break;
    case '\x15':		/* ^U - erase line */
      EDITING (if (cursor != cb) /* Erase what is right of the cursor. */
	       console->write (0, rgb + cursor, cb - cursor);)
      while (cb--)
	console->write (0, "\b \b", 3);
      printf ("\r%s", szPrompt);
      EDITING (cursor = 0;)
      break;
#if defined (CONFIG_COMMAND_EDITING)
    case '\x0c':		/* ^L - redraw line */
      printf ("\r%s%*.*s", szPrompt, cb, cb, rgb);
      backspaces (cb - cursor);
      --cb;
      break;
    case '\x01':		/* ^A - beginning of line*/
      if (cursor) {
	backspaces (cursor);
	cursor = 0;
      }
      --cb;
      break;
    case '\x05':		/* ^E - end of line*/
      if (cursor != cb) {
	console->write (0, rgb + cursor, cb - cursor);
	cursor = cb;
      }
      --cb;
      break;
    case '\x02':		/* ^B - backward character */
      if (cursor) {
	--cursor;
	console->write (0, "\b", 1);
      }
      --cb;
      break;

    case '\x06':		/* ^F - forward character */
      if (cursor < cb) {
	console->write (0, &rgb[cursor++], 1);
      }
      --cb;
      break;

    case '\x04':		/* ^D - delete current character  */
      if (cb && cursor != cb) {
	memmove (rgb + cursor, rgb + cursor + 1, cb - cursor - 1);
	console->write (0, rgb + cursor, cb - cursor - 1);
	console->write (0, " ", 1); /* Erase */
	backspaces (cb - cursor);
	--cb;
      }
      --cb;
      break;

    case '\x0b':		/* ^K - delete to end of line  */
      if (cb && cursor != cb) {
	int c = cb - cursor;
	while (c--)
	  console->write (0, " ", 1);
	backspaces (cb - cursor);
	cb = cursor;
      }
      --cb;
      break;

#endif
#if defined (CONFIG_COMMAND_HISTORY)
    case '\x10':		/* ^P */
      if (history_retrieve (1, rgb, sizeof (rgb))) {
	while (cb--)
	  console->write (0, "\b \b", 3);
	printf ("\r%s%s", szPrompt, rgb);
	cb = strlen (rgb);
	EDITING (cursor = cb;)
      }
      --cb;
      break;
    case '\x0e':		/* ^N */
      if (history_retrieve (0, rgb, sizeof (rgb))) {
	while (cb--)
	  console->write (0, "\b \b", 3);
	printf ("\r%s%s", szPrompt, rgb);
	cb = strlen (rgb);
	EDITING (cursor = cb;)
      }
      --cb;
      break;
#endif
    default:
      if (isprint (rgb[cb])) {
	console->write (0, &rgb[cb], 1);
#if defined (CONFIG_COMMAND_EDITING)
	if (cursor != cb) {	/* Insert within the command string */
	  char ch = rgb[cb];
	  memmove (rgb + cursor + 1, rgb + cursor, cb - cursor);
	  rgb[cursor] = ch;
	  console->write (0, rgb + cursor + 1, cb - cursor);
	  backspaces (cb - cursor);
	}
	++cursor;
#endif
      }
      else
	--cb;
      break;
    }
  }

  rgb[cb] = 0;			/* Redundant except for overflow */

#if defined (CONFIG_COMMAND_HISTORY)
  history_add (rgb);
#endif

  return parse_command (rgb, pargc, pargv);
}

#if defined (CONFIG_COMMAND_EDITING) && defined (CONFIG_ALLHELP)
static __command struct command_d c_help_commandediting = {
  .command = "command-editing",
  COMMAND_DESCRIPTION ("help on edting APEX command lines")
  COMMAND_HELP(
"command-editing\n"
"  APEX support some Emacs bindings for command line editing.  If the ANSI\n"
"  key interpreter is enabled, the arrow keys and the Delete are supported\n"
"  as well.  These are the available editing commands.\n"
"    ^A	          - move cursor to the beginning of the line\n"
"    ^E           - move cursor to the end of the line\n"
"    ^F or RIGHT  - move cursor forward one character\n"
"    ^B or LEFT   - move cursor backward one character\n"
"    ^K           - delete from cursor to the end of the line\n"
"    ^H or DEL    - delete previous character\n"
"    ^D or DELETE - delete next character\n"
"    ^L           - redraw the command line\n"
  )
};
#endif
