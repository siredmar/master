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
   (improbable) that an environment may be maintained within eeprom
   (~128B) where there will be some adjustment to the algorithms.  For
   example, in eeprom, there is no reason to maintain the erased
   values.  Instead, we'd rewrite the whole kit every time.


   Environment Format
   ------------------

   Refer to README_environment for an explanation of the NVRAM format
   of the environment.


   Statelessness
   -------------

   These routines are stateless.  The idNext and the rgId array are
   filled every time the environment is scanned which is every time it
   is either used in a fetch or a store.  Should the environment
   become so large that the time to scan it is noticeable, it is
   probably time to rebuild it and remove the deleted entries.


   rgId
   ----

   This is an array of environment indices.  rgId[0] is the
   environment variable index for id 0.  An entry of 0xff means that
   that id hasn't been scanned or allocated.  An entry of 0x7f means
   that that ID was found in the environment, but there is no
   corresponding environment variable that matches it.

   env_check_magic and rgs[]
   -------------------------

   It may seem wasteful to use a large auto array to hold read data
   while looking for evidence that the environment is in-use.  I
   checked this with the stack extent monitoring code and it looks
   like the impact is minimal.  At the points in time where the
   environment is being scanned, the stack is well below 1KiB.  At
   128, the rgs array is only 256 which is the length of a moderately
   long string buffer.

   CONFIG_ENV_SAVEATONCE
   ---------------------

   In the case of some types of NAND flash, especially when there is a
   controller circuit involved, we need to rewrite the flash instead
   of updating the data.  NOR flash allows us to write bytes
   individually as long as we don't attempt to set a bit once it has
   been cleared.  Some NAND performs the same way but others,
   e.g. OneNAND, only allow four writes to a page before it must be
   erased.

   This code isn't very clever with respect to erasing the block.  It
   erases the whole environment region and doesn't attempt to preserve
   anything else in the erase block.  So, it is incumbent on the
   configuration to properly place the environment data in flash.

   An alternative would be to do some mumbo-jumbo with flash pages
   s.t. we clobber the page and write the next one.  No erase.  The
   environment load-up code must be changed as well to cope with this.
   We'd have to change the environment format as well to reflect the
   new form.

   ----

   - There may be a desire to be able to erase all of the environment
     and start again.  This might mitigate the issue of using EEPROM
     in that we would force the user to do the erase by hand.

   - *** Need to investigate why it fails to boot properly when the
	 environment region doesn't open, or nor flash driver.

*/

#include <config.h>
#include <apex.h>		/* printf */
#include <linux/types.h>
#include <environment.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <driver.h>

//#define TALK 2

#if defined (TALK)
#define PRINTF(f...)		printf (f)
#define ENTRY(l)		printf ("%s\n", __FUNCTION__)
#define DBG(l,f...)		do { if (TALK >= l) printf (f); } while (0)
#else
#define PRINTF(f...)		do {} while (0)
#define ENTRY(l)		do {} while (0)
#define DBG(l,f...)		do {} while (0)
#endif

extern char APEX_ENV_START;
extern char APEX_ENV_END;
extern char APEX_VMA_START;
extern char APEX_VMA_END;

#define C_ENV_CHECK_READ	(64)
#define ENV_CHECK_LEN	(CONFIG_ENV_CHECK_LEN)
#define ENV_CB_MAX	(512)
#define ENV_MASK_DELETED (0x80)
#define ENV_VAL_DELETED	 (0x00)
#define ENV_VAL_NOT_DELETED (0x80)
#define ENV_ID(m)	((int)((m) & 0x7f))
#define ENV_END		(0xff)
#define ENV_IS_DELETED(m) (((m) & ENV_MASK_DELETED) == ENV_VAL_DELETED)
#define C_ENV_KEYS	(((unsigned int) &APEX_ENV_END \
			- (unsigned int) &APEX_ENV_START)\
			/sizeof (struct env_d))
#define ENVLIST(i)	(((struct env_d*) &APEX_ENV_START)[i])

struct descriptor_d d_env;	/* Environment storage region */
struct descriptor_d d_envmem;	/* Environment storage region in memory */
struct descriptor_d* pd_env;	/* Pointer to current environment region */

static int idNext;		/* Next available index */
static unsigned char rgId[127]; /* *** I'd rather this not be fixed max */
static ssize_t ibLastFlag;
#if defined (CONFIG_ENV_SIZE)
char __xbss(env) g_rgbEnv[CONFIG_ENV_SIZE];
#endif


/* _env_reset_ids

   must be called before _env_locate () so that the id array may be
   recomputed.

*/

static void _env_reset_ids (void)
{
  idNext = 0;
  memset (rgId, 0xff, sizeof (rgId));
}

static ssize_t _env_read (void* pv, size_t cb)
{
  return pd_env->driver->read (pd_env, pv, cb);
}

static ssize_t _env_seek (ssize_t ib, int whence)
{
  DBG (2, "%s: %d %d\n", __FUNCTION__, ib, whence);
  return pd_env->driver->seek (pd_env, ib, whence);
}

#if defined (CONFIG_CMD_SETENV)

static ssize_t _env_write (const void* pv, size_t cb)
{
  ssize_t result;
//  dumpw ((char*) pd_env->start, 32, 0, 0);
//  dumpw ((char*) pv, 32, 0, 0);
  DBG (2, "%s: (%d) %p [%x %x]\n", __FUNCTION__,
       cb, pv, ((char*)pv)[0], ((char*)pv)[1]);
  DBG (2, "  %lx %d %ld\n", pd_env->start, pd_env->index, pd_env->length);
  result = pd_env->driver->write (pd_env, pv, cb);
//  dumpw ((char*) pd_env->start, 32, 0, 0);
  return result;
}


/* _env_id

   returns the id for the given environment index or -1 if there isn't
   one yet.

*/

static int _env_id (int index)
{
  int i;
  for (i = 0; i < sizeof (rgId) && rgId[i] != 0xff; ++i)
    if (rgId[i] == index)
      return i;
  return -1;
}

#endif

static void _env_rewind (void)
{
  pd_env->driver->seek (pd_env, 0, SEEK_SET);
}

static int _env_index (const char* sz)
{
  int i;

  for (i = 0; i < C_ENV_KEYS; ++i)
    if (strnicmp (sz, ENVLIST(i).key, strlen (ENVLIST (i).key) + 1) == 0)
      return i;

  return -1;
}


/* _env_locate

   is the only function that scans the environment.  This makes it
   easy to concentrate all of the odd logic to handle the special form
   of the environment entries.

   The parameter is the index of the desired environment variable.

   A side effect of this function is to set the ibLastFlag global
   variable to the index of the last flag found.  This is used to seek
   back to the flag to erase the entry.

*/

static char _env_locate (int i)
{
  char ch;

  while ((ibLastFlag = _env_seek (0, SEEK_CUR), _env_read (&ch, 1) == 1)
	 && ch != ENV_END) {
    int id = ENV_ID (ch);
    if (rgId[id] == 0xff) {	/* New ID to this scan */
      char rgb[ENV_CB_MAX];
      char* pb;
      for (pb = rgb;
	   pb < rgb + sizeof (rgb) && _env_read (pb, 1) == 1 && *pb;
	   ++pb)
	;
      *pb = 0;
      ++idNext;

      rgId[id] = _env_index (rgb) & 0x7f;
    }

    if (!ENV_IS_DELETED (ch) && rgId[id] == i)
      return ch;
    do {
      _env_read (&ch, 1);
    } while (ch && ch != 0xff);
  }

  return ch;
}


/* env_check_magic

   performs a rewind of the environment descriptor and checks for the
   presence of the environment magic number at the start of the
   region.  It returns 0 if the magic number is present.  If the
   region is uninitialized, it returns 1.  The return value is -1 if
   there is data in the environment region that is not an APEX
   environment.

   The original version of this function only checked for a single
   0xffff.  We're modifying this code, conditionally, to check for at
   least 1K of 0xff's.  An ideal implementation would check all of the
   environment space, just before a write, just to make sure it is
   clear.

   The parameter is true when we are looking for a complete scan of
   the environment region viability.  For reading from the
   environment, only a magic number check is necessary.

   0   -> MAGIC found
   1   -> Region uninitialized
   -1  -> Region filled with foreign data
   -2  -> Region invalid

*/

int env_check_magic (int full_check)
{
  unsigned char __aligned rgb[2];

  if (!is_descriptor_open (&d_env)) /* Check for bogus environment region */
    return -2;

  if (!is_descriptor_open (pd_env)) /* Check for bogus env IO region */
    return -1;

  _env_rewind ();
  _env_read (rgb, 2);
  if (rgb[0] == ENV_MAGIC_0 && rgb[1] == ENV_MAGIC_1)
    return 0;

  if (!full_check)	  /* !full_check good for reading */
    return -1;

  if (rgb[0] != 0xff || rgb[1] != 0xff) {
    DBG (1, "%s: bad magic %x %x\n", __FUNCTION__, rgb[0], rgb[1]);
    return -1;
  }

#if defined (ENV_CHECK_LEN) && ENV_CHECK_LEN > 0
  {
    int c = ENV_CHECK_LEN/2;
    unsigned short rgs[C_ENV_CHECK_READ];	/* For efficient reading */
    while (c > 0) {
      int cRead = c;
      if (cRead > sizeof (rgs)/sizeof (*rgs))
	cRead = sizeof (rgs)/sizeof (*rgs);
      _env_read (rgs, cRead*2);
      c -= cRead;
      while (cRead-- > 0)
	if (rgs[cRead] != 0xffff)
	  return -1;
    }
    _env_rewind ();
    _env_read (rgs, 2);
  }
#endif

  return 1;
}


/* _env_find

   looks for a user's set environment data in the non-volatile store.
   The index, i, is the index of the variable in the list of variable
   structures.

*/

static const char* _env_find (int i)
{
  static char rgb[ENV_CB_MAX];

  if (!is_descriptor_open (pd_env))
    return NULL;

  if (env_check_magic (0))
    return NULL;

  _env_reset_ids ();

  if (_env_locate (i) != ENV_END) {
    /*** FIXME: should be just read until we find a null? */
    _env_read (rgb, sizeof (rgb));
    return rgb;
  }

  return NULL;
}


/* _env_fetch

   returns the index and value for an environment key.  If the user
   has not set the key, the *pszValue will be NULL.  The return value
   is the key index or -1 if the key is not found.n

*/

static int _env_fetch (const char* szKey, const char** pszValue)
{
  int i = _env_index (szKey);;
  if (i != -1)
    *pszValue = _env_find (i);
  return i;
}


/* env_enumerate

   walks the caller through all of the undeleted environment entries.
   The return value is a handle to the item being enumerated.  The
   function returns NULL when all of the items are enumerated.  When
   the return value is not NULL, *pszKey points to a key and *pszValue
   points to the corresponding value.

*/

void* env_enumerate (void* pv, const char** pszKey,
		     const char** pszValue, int* pfDefault)
{
  int i = (int) pv + 1;

  if (i - 1 >= C_ENV_KEYS)
    return 0;

  *pszKey = ENVLIST(i - 1).key;
  *pszValue = _env_find (i - 1);
  if (*pszValue == NULL) {
    *pszValue = ENVLIST(i - 1).default_value;
    *pfDefault = 1;
  }
  else
    *pfDefault = 0;

  return (void*) i;
}


/* env_fetch

   returns a read-only pointer to the value for the given environment
   key entry.  If there are no matching, not-deleted entries, the
   return value is NULL.

*/

const char* env_fetch (const char* szKey)
{
  const char* szValue = NULL;
  int i;

  //  printf ("env_fetch %s\n", szKey);
  i = _env_fetch (szKey, &szValue);

  if (i != -1 && szValue == NULL)
    return ENVLIST(i).default_value;
  return szValue;
}


int env_fetch_int (const char* szKey, int valueDefault)
{
  const char* sz = env_fetch (szKey);
  char* pbEnd;
  u32 value;

  if (sz == NULL)
    return valueDefault;

  value = simple_strtoul (sz, &pbEnd, 0);
  return sz == pbEnd ? valueDefault : value;
}


#if defined (CONFIG_CMD_SETENV)

/* env_erase

   removes a key from the environment.

   This function requires that the d_env descriptor be open.

*/

void env_erase (const char* szKey)
{
  int i = _env_index (szKey);
  char ch;

  ENTRY (0);

  if (i < 0)
    return;

  if (env_check_magic (1))
    return;

  _env_reset_ids ();

  if ((ch = _env_locate (i)) != ENV_END) {
    ch = (ch & ~ENV_MASK_DELETED) | ENV_VAL_DELETED;
    _env_seek (ibLastFlag, SEEK_SET);
    _env_write (&ch, 1);
  }
}


/* env_store

   adds a key/value pair to the environment.  An existing version of
   the key will be erased.  The return value is non-zero if the data
   cannot be written to flash.

   This function requires that the d_env descriptor be open.

*/

int env_store (const char* szKey, const char* szValue)
{
  int i = _env_index (szKey);
  int   cch = strlen (szValue);
  char ch;

  ENTRY (0);

  if (i < 0 || cch > ENV_CB_MAX - 1)
    return 1;

  switch (env_check_magic (1)) {
  case 0:			/* magic present */
    break;
  case 1:			/* uninitialized */
    {
      char rgb[2] = { ENV_MAGIC_0, ENV_MAGIC_1 };
      _env_rewind ();
      _env_write (rgb, 2);
    }
    break;
  default:
  case -1:			/* unrecognized */
    return -1;
  }

  _env_reset_ids ();

	/* Erase existing environment entries */
  while ((ch = _env_locate (i)) != ENV_END) {
    ssize_t ib = _env_seek (0, SEEK_CUR);
    DBG (2, "locate %x %d\n\n", ch, ibLastFlag);
    ch = (ch & ~ENV_MASK_DELETED) | ENV_VAL_DELETED;
    _env_seek (ibLastFlag, SEEK_SET);
    _env_write (&ch, 1);
    _env_seek (ib, SEEK_SET);
    while (_env_read (&ch, 1) == 1 && ch != 0)
      ;
  }

	/* Append new environment entry */
  {
    int isNew = 0;
    int id = _env_id (i);
    if (id == -1) {
      id = idNext++;
      rgId[id] = i;
      isNew = 1;
    }
    ch = ENV_VAL_NOT_DELETED | (id & 0x7f);
    _env_seek (-1, SEEK_CUR);
    _env_write (&ch, 1);
    if (isNew)
      _env_write (ENVLIST(i).key, strlen (ENVLIST(i).key) + 1);
    _env_write (szValue, cch + 1);
  }

  return 0;
}
#endif

#if defined (CONFIG_CMD_ERASEENV) && defined (CONFIG_CMD_SETENV)

void env_erase_all (void)
{
  _env_rewind ();
  pd_env->driver->erase (pd_env, pd_env->length);
}

#endif
