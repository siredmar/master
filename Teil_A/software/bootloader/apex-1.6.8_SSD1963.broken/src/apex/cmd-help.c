/* cmd-help.c

   written by Marc Singer
   3 Nov 2004

   Copyright (C) 2004 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   o ***FIXME: without the sort, there is no need for the rgc array.
     This function could be shorter without the sort.

*/

#include <linux/types.h>
#include <linux/string.h>
#include <apex.h>
#include <command.h>
#include <sort.h>
#include <error.h>

#define MAX_COMMANDS	128	/* Used to sort */

#if defined (CONFIG_ALPHABETIZE_COMMANDS)

static int compare_commands (const void* _a, const void* _b)
{
  struct command_d** a = (struct command_d**) _a;
  struct command_d** b = (struct command_d**) _b;

  return strcmp ((*a)->command, (*b)->command);
}

#endif

int cmd_help (int argc, const char** argv)
{
  extern char APEX_COMMAND_START;
  extern char APEX_COMMAND_END;
  struct command_d* rgc[MAX_COMMANDS];
  int cCommands;
  int i;

#if defined (CONFIG_ALLHELP)
  const char* sz = NULL;
  int cb = 0;
  int fAll = 0;
#endif

#if defined (CONFIG_ALLHELP)
  if (argc > 2)
    return ERROR_PARAM;
  if (argc == 2) {
    sz = argv[1];
    cb = strlen (sz);
    if (cb == 1 && *sz == '.')
      fAll = 1, cb = 0;
  }
#else
  if (argc > 2)
    return ERROR_PARAM;
#endif

  cCommands = 0;
  {
    struct command_d* command;
    for (command = (struct command_d*) &APEX_COMMAND_START;
         cCommands < sizeof (rgc)/sizeof (*rgc)
         && command < (struct command_d*) &APEX_COMMAND_END;
	 ++command, ++cCommands)
      rgc[cCommands] = command;
  }

#if defined (CONFIG_ALPHABETIZE_COMMANDS)
  printf ("sorting %d\n", cCommands);
  sort (rgc, cCommands, sizeof (struct command_d*), compare_commands, NULL);
#endif

  for (i = 0; i < cCommands; ++i) {
    if (!rgc[i]->command || (!rgc[i]->func
#if defined (CONFIG_ALLHELP)
			      && !fAll && cb == 0
#endif
			      ))
      continue;
#if defined (CONFIG_ALLHELP)
    if (cb) {
      if (!rgc[i]->help)
	continue;
      if (strnicmp (sz, rgc[i]->command, cb) == 0)
	printf (rgc[i]->help);
      continue;
    }
#endif
#if defined (CONFIG_NOHELP)
    printf (" %-*.*s\n", 16, 16, rgc[i]->command);
#else
    printf (" %-*.*s - %s\n", 16, 16, rgc[i]->command,
	    rgc[i]->description ? rgc[i]->description : "?");
#endif

  }

  return 0;
}

static __command struct command_d c_help = {
  .command = "help",
  .func = cmd_help,
  COMMAND_DESCRIPTION ("list available commands")
  COMMAND_HELP(
"help [. | COMMAND]\n"
"  Display help.\n"
"  The optional command parameter will show the detailed help\n"
"  for commands that would match that prefix.\n"
"  e.g.  help dump             # Show help for dump command\n"
"        help d                # Show help for all 'd' commands\n"
"        help                  # List the available commands\n"
"        help .                # Show all help topics\n"
  )
};

#if defined (CONFIG_ALLHELP)

static __command struct command_d c_help_region = {
  .command = "region",
  COMMAND_DESCRIPTION ("help on regions")
  COMMAND_HELP(
"region [DRIVER:][[@]START][+LENGTH]\n"
"       [DRIVER:][[//LOCATION][/PATH]][[@START][+LENGTH]]\n"
"  Regions define a source or destination for data.  Commands\n"
"  that use regions are abstracted from the underlying IO mechanism.\n"
"  This isn't as important for memory mapped devices as it is for\n"
"  others such as SPI eeproms, NAND flash, and network devices.\n\n"
"    The DRIVER name is matched case insensitively.  Any unambiguous\n"
"  prefix will match such that 'nor:' will map to the 'nor-cfi:'\n"
"  driver.  When the DRIVER field is absent, the default memory: driver\n"
"  is used.  The drvinfo command will display a list of available\n"
"  drivers.  The START value may be in hexadecimal, decimal, or decimal\n"
"  with a multipler, e.g. 10k for 10 kilobytes, 1m for one megabyte.  The\n"
"  LENGTH takes the same form as START.\n\n"
"    The second region form is used by drivers that support paths.\n"
"  The LOCATION is a driver specific element.  For fatfs: it is the\n"
"  partition number 1-4.  The path is a valid path for the filesystem\n"
"  but may not contain either an '@' or a '+'.\n"
"    The driver is responsible for parsing the region string.  It\n"
"  will add defaults when necessary or fail if the region is\n"
"  invalid.\n\n"
"  e.g.  mem:0+1k              # First kilobyte of physical memory \n"
"        mem:+1k               # Same as above\n"
"        +1k                   # Same as above\n"
"        +1024                 # Same as above\n"
"        nor:0                 # Start of nor flash\n"
"        nor:128k+64k          # 64 KiB of flash at the 128KiB boundary\n"
"        0x20200000+2m         # Two megabytes in memory\n"
"        fat:/zImage           # File zImage in the root fat: device\n"
"        fat:/zImage@2k        # File zImage, 2KiB bytes into the file\n"
  )
};

static __command struct command_d c_help_commandline = {
  .command = "command-line",
  COMMAND_DESCRIPTION ("help on the APEX command line")
  COMMAND_HELP(
"command-line\n"
"  The command line interpreter separates commands into works and passes\n"
"  then to the command interpreting function.  Sequences of characters\n"
"  may be enclosed with double quotes (\") to include whitespace in a \n"
"  command word.  Otherwise, command words are delimited by whitespace.\n"
"  Command lines may be edited with ^H and DEL to erase the previous\n"
"  character and ^U to clobber the whole line.\n"
//"  Multiple commands may be submitted in a single line when separated by\n"
//"  a semicolon.\n\n"
"  The interpreter is forgiving.  When matching commands, it is case\n"
"  insensitive.  It will accept a command from on an unambiguous prefix.\n"
"  Thus 'h' is sufficient to invoke help.\n"
  )
};

#endif
