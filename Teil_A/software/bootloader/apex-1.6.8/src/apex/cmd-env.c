/* env.c

   written by Marc Singer
   15 Oct 2004

   Copyright (C) 2004 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   Code to handle an environment.  The application can maintain a
   persistent environment in flash with key/value pairs.  These
   routines read and write the environment.  It is possible
   (improbably) that an environment may be maintained within eeprom
   (~128B) where there will be some adjustment to the algorithms.
   Perhaps the most likely variation will be to limit the keys to a
   predefined set.

   Some likely environment keywords are

     cmdline		   - Default kernel command line
     autoboot		   - Set to non-zero to boot automatically
     autoboot_timeout	   - number of milliseconds before autoboot-ing


   TODO
   ----

   - Seems reasonable to aggregate the keys into an array and leave
     them out of flash since we can only use environment keys that are
     already defined within the program.
   - The previous change also means that we could get rid of the
     length from the marker and put the key ID in there.  The size of
     each entry shall be determined by the presence of a null byte.
     This is no worse (and really much better) than adding a length to
     the header.
   - The environment variables could come from initializations in the
     code.  Each module that wants variables would call here to
     register them.  The indices for each would be transparent since
     the caller would always pass/receive strings.  Moreover, the
     environment would only be valid with the version of the firmware
     that created it.   It would be handy to use special initializers
     to define and reference these keys.  There should be a checksum
     at the top of the environment that make it impossible to load the
     values incorrectly.
   - Setup should come from the lds file and the board configuration.
   - With the from an array, we could probably put the environment
     into EEPROM.

*/


#include <config.h>
#include <linux/string.h>
#include <apex.h>
#include <command.h>
#include <error.h>
#include <environment.h>
#include <driver.h>

/* cmd_printenv

   show the contents of the environment.  This command is defined if
   any of the environment commands are enabled.

*/

int cmd_printenv (int argc, const char** argv)
{
  void* pv = 0;
  const char* szKey;
  const char* szValue;
  int fDefault;

  while ((pv = env_enumerate (pv, &szKey, &szValue, &fDefault))) {
    puts (szKey);
    if (fDefault)
      puts (" *= ");
    else
      puts (" = ");
    printf ("%s\n", szValue);
  }
  return 0;
}

static __command struct command_d c_printenv = {
  .command = "printenv",
  .func = cmd_printenv,
  COMMAND_DESCRIPTION ("show the environment")
  COMMAND_HELP(
"printenv\n"
"  Display the environment.\n"
"  The output is KEY [*]= VALUE where KEY is an environment\n"
"  variable name and VALUE is the current seting.  The '*'\n"
"  denotes a variable set to the default value.\n"
  )
};

#if defined (CONFIG_CMD_SETENV)

static int cmd_setenv (int argc, const char** argv)
{
  int result = 0;
  int i;
  int ib = 0;
  int cb;
  static char __xbss(env) sz[ENV_CB_MAX];

  if (!is_descriptor_open (pd_env))
    ERROR_RETURN (ERROR_UNSUPPORTED, "environment descriptor not open");

  if (argc < 3)
    return ERROR_PARAM;

  for (i = 2; i < argc; ++i) {
    cb = strlen (argv[i]);
    strlcpy (sz + ib , argv[i], sizeof (sz) - ib);
    ib += cb;
    if (i + 1 < argc) {
      strlcpy (sz + ib, " ", sizeof (sz) - ib);
      ++ib;
    }
  }

  result = env_store (argv[1], sz);

#if defined (CONFIG_SMALL)
  return result ? ERROR_FAILURE : 0;
#else
  if (result < 0)
    printf ("Environment region is unwritable\n");
  else if (result)
    printf ("Unrecognized variable\n");

  return 0;
#endif
}

static __command struct command_d c_setenv = {
  .command = "setenv",
  .func = cmd_setenv,
  COMMAND_DESCRIPTION ("set environment variable")
  COMMAND_HELP(
"setenv KEY VALUE\n"
"  Set environment variable KEY to value VALUE.\n"
"  The set of KEYs is fixed when APEX is built.  It will\n"
"  only allow preexisting variables to be set.\n\n"
"  e.g.  setenv cmdline console=ttyAM1 root=/dev/hda1\n"
  )
};

static int cmd_unsetenv (int argc, const char** argv)
{
  if (!is_descriptor_open (pd_env))
    return ERROR_UNSUPPORTED;

  if (argc != 2)
    return ERROR_PARAM;

  env_erase (argv[1]);

  return 0;
}

static __command struct command_d c_unsetenv = {
  .command = "unsetenv",
  .func = cmd_unsetenv,
  COMMAND_DESCRIPTION ("restore environment variable to default")
  COMMAND_HELP(
"unsetenv KEY\n"
"  Restore environment variable KEY to the default value.\n\n"
"  e.g.  unsetenv cmdline\n"
  )
};
#endif

#if defined (CONFIG_CMD_SETENV) && defined (CONFIG_ENV_SAVEATONCE)

static int cmd_saveenv (int argc, const char** argv)
{
  if (!is_descriptor_open (&d_env))
    ERROR_RETURN (ERROR_UNSUPPORTED, "environment descriptor not open");

  if (argc != 1)
    return ERROR_PARAM;

  d_env.driver->seek (&d_env, 0, SEEK_SET);
  d_env.driver->erase (&d_env, d_env.length);
  d_env.driver->seek (&d_env, 0, SEEK_SET);
  if (d_env.driver->write (&d_env, g_rgbEnv, CONFIG_ENV_SIZE)
      != CONFIG_ENV_SIZE)
    ERROR_RETURN (ERROR_IOFAILURE, "truncated write");

  return 0;
}

static __command struct command_d c_saveenv = {
  .command = "saveenv",
  .func = cmd_saveenv,
  COMMAND_DESCRIPTION ("save environment to non-volatile memory")
  COMMAND_HELP(
"saveenv\n"
"  Write current environment to non-volatile memory.\n"
"  Changes to the environment are held in RAM until this command\n"
"  is used.  This command commits the changes to the environment\n"
"  region: " CONFIG_ENV_REGION ".\n")
};
#endif

#if defined (CONFIG_CMD_ERASEENV)
static int cmd_eraseenv (int argc, const char** argv)
{
  if (!is_descriptor_open (pd_env))
    return ERROR_UNSUPPORTED;

  if (argc != 1)
    return ERROR_PARAM;

  env_erase_all ();

  return 0;
}

static __command struct command_d c_eraseenv = {
  .command = "eraseenv",
  .func = cmd_eraseenv,
  COMMAND_DESCRIPTION ("Erase environment")
  COMMAND_HELP(
"eraseenv\n"
"  Erase the region that stores the environment.\n"
"  All environment variables will be reset to the default values.\n"
"  The location of the environment is available from the version\n"
"  command.  Beware.  If the erase granularity of the region holding\n"
"  the environment is large then non-environment data may be erased\n"
"  as well\n"
  )
};
#endif
