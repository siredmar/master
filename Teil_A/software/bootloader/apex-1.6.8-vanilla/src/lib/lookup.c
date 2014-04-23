/* lookup.c

   written by Marc Singer
   20 Jul 2006

   Copyright (C) 2006 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#include <config.h>
#include <apex.h>
#include <lookup.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <environment.h>
#include <alias.h>

int lookup_alias_or_env_int (const char* szKey, int valueDefault)
{
  const char* sz = lookup_alias_or_env (szKey, NULL);
  if (sz)
    valueDefault = simple_strtoul (sz, NULL, 0);
  return valueDefault;
}

unsigned lookup_alias_or_env_unsigned (const char* szKey, unsigned valueDefault)
{
  const char* sz = lookup_alias_or_env (szKey, NULL);
  if (sz)
    valueDefault = simple_strtoul (sz, NULL, 0);
  return valueDefault;
}

const char* lookup_alias_or_env (const char* szKey,
				 const char* szDefault)
{
  const char* sz = NULL;

#if defined (CONFIG_VARIATIONS)
  const char* szAlt = NULL;
  static char szKeyAlt[80];

# if defined (CONFIG_ALIASES)
  szAlt = alias_lookup ("variation");
# endif

# if defined (CONFIG_ENV)
  if (!szAlt)
    szAlt = env_fetch ("variation");
# endif

  if (szAlt) {
    int cch = strlcpy (szKeyAlt, szKey, sizeof (szKeyAlt));
    strlcpy (szKeyAlt + cch, szAlt, sizeof (szKeyAlt) - cch);

# if defined (CONFIG_ALIASES)
    if (!sz)
      sz = alias_lookup (szKeyAlt);
# endif
# if defined (CONFIG_ENV)
    if (!sz)
      sz = env_fetch (szKeyAlt);
# endif
  }
#endif

#if defined (CONFIG_ALIASES)
  if (!sz)
    sz = alias_lookup (szKey);
#endif
#if defined (CONFIG_ENV)
  if (!sz)
    sz = env_fetch (szKey);
#endif
  if (!sz)
    sz = szDefault;
  return sz;
}
