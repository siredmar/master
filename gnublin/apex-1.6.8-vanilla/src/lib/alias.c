/* alias.c

   written by Marc Singer
   14 Jun 2005

   Copyright (C) 2005 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   Simple alias association library.  This library handles the
   essentials of alias setting, enumeration, and substitution.

*/

#include <config.h>
//#include <apex.h>		/* printf */
#include <attributes.h>
#include <linux/types.h>
//#include <environment.h>
#include <linux/string.h>
#include <linux/kernel.h>
//#include <driver.h>
#include <alias.h>
#include <error.h>

#define CB_ALIAS_HEAP_MAX	(4*1024)

static unsigned char __xbss(alias) rgbAlias[CB_ALIAS_HEAP_MAX];

size_t ibAlias;

struct entry {
  unsigned short cb;		/* Length of this whole node. */
  char rgb[];
};

static struct entry* _alias_find (const char* szKey)
{
  struct entry* entry;
  size_t ib = 0;

  for (; ib < ibAlias; ib += entry->cb) {
    entry = (struct entry*) (rgbAlias + ib);
    if (!entry->rgb[0])		/* Deleted */
      continue;
//    printf ("  compare '%s' '%s'\n", szKey, entry->rgb);
    if (strcmp (szKey, entry->rgb) == 0)
      return entry;
  }

  return NULL;
}

void* alias_enumerate (void* pv, const char** pszKey, const char** pszValue)
{
  struct entry* entry;

  if (!pv)
    entry = (struct entry*) rgbAlias;
  else {
    entry = (struct entry*) pv;
    entry = (struct entry*) ((unsigned char*) pv + entry->cb);
  }

  for (; ((unsigned char*) entry - rgbAlias) < ibAlias;
       entry = (struct entry*) ((unsigned char*) entry + entry->cb)) {

    if (!entry->rgb[0])
      continue;

    *pszKey = entry->rgb;
    *pszValue = entry->rgb + strlen (entry->rgb) + 1;
    return entry;
  }

  return 0;
}

const char* alias_lookup (const char* szKey)
{
  struct entry* entry;

  if (!szKey)
    return NULL;

  entry = _alias_find (szKey);
  return entry ? (entry->rgb + strlen (entry->rgb) + 1) : NULL;
}

int alias_set_hex (const char* szKey, unsigned value)
{
  char sz[60];
  snprintf (sz, sizeof (sz), "0x%x", value);
  return alias_set (szKey, sz);
}

int alias_set (const char* szKey, const char* szValue)
{
  size_t cbKey = szKey ? strlen (szKey) : 0;
  size_t cbValue = szValue ? strlen (szValue) : 0;
  size_t cbEntry = (sizeof (struct entry)
		    + cbKey + 1 + cbValue + 1 + 0x3) & ~0x3;
  struct entry* entry = (struct entry*) &rgbAlias[ibAlias];

  if (cbKey == 0 || cbValue == 0)
    return ERROR_PARAM;
  if (ibAlias + cbEntry > CB_ALIAS_HEAP_MAX)
    return ERROR_OUTOFMEMORY;

  memset (entry, 0, sizeof (struct entry));
  entry->cb = cbEntry;
  memcpy (entry->rgb, szKey, cbKey + 1);
  memcpy (entry->rgb + cbKey + 1, szValue, cbValue + 1);

  ibAlias += cbEntry;

  return 0;
}

int alias_unset (const char* szKey)
{
  struct entry* entry;

  if (!szKey)
    return ERROR_PARAM;

  entry = _alias_find (szKey);
  if (entry)
    entry->rgb[0] = 0;		/* Delete */
  return entry ? 0 : ERROR_FALSE;
}
