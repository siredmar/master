/* link.cc

   written by Marc Singer
   15 Jan 2007

   Copyright (C) 2007 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   Endianness
   ----------

   The code that scans for the presence of the APEX environment link
   structure checks for a byte-reversed copy of the link magic number.
   If found, it triggers the code to swab32() all of the words to
   correct for the endianness mismatch.  It will assume that the flash
   environment does not need to be swapped.


   Environment Format
   ------------------

   The in-flash format of the user's contribution to the environment
   has the following format in types 1 and 2 of the environment link.
   Note that the endianness of the in-flash environment shouldn't
   matter since the writer of the environment needs to honor the
   essential endian-ness of the system.  On systems where there is no
   ambiguity between the inherent endianness and the run-time
   endianness, all is equivalent.  On systems where we run in one
   mode, but the system was engineered for the other, the flash
   drivers ought to swap so that we can read the FIS Directory.


     +---+---+----------+------+
     | A | e | Entry... | 0xff |
     +---+---+----------+------+

   The magic bytes, A and e prefix the environment data.  Following it
   are the entries with 0xff being the first byte after the last valid
   entry.  Entries have one of two forms depending on whether or not
   the key already exists in the flash environment.

     +-----------+-----+----+-------+----+
     | 0x80 + ID | Key | \0 | Value | \0 |
     +-----------+-----+----+-------+----+

   The high bit of the ID is 1 when the entry is valid and zero when
   the entry is erased.  The lower seven bits are an index, starting
   at zero, of the unique keys in the environment.  The first time an
   environment entry is added to the environment, an ID is allocated
   and the above format is emitted.  The second time the same key is
   used, the Key string is omitted and the entry looks as follows.

     +-----------+-------+----+
     | 0x80 + ID | Value | \0 |
     +-----------+-------+----+

   This scheme replaces an earlier one that used the key indices in
   the APEX image as the Key values.  The trouble with that format is
   that it required the in-flash format to match the APEX executable
   in a way that was impossible to guarantee even though a checksum of
   the available environment variables was used as the magic for the
   environment.  The current format allows for APEX upgrades without
   losing the user's environment.

   Orphans
   -------

   It is possible to have a variable set in the environment, but there
   be no environment record in APEX to support it.  These orphans are
   recgonized by the Link class.  The ID will be skipped when adding
   new variables.  Printing the environment usually shows them with #=
   as the equivalence operator.

   mtd devices and write behavior
   ------------------------------

   The MTD block device driver handles writes by caching a modified
   eraseblock and performing the erase and write back once another
   eraseblock is written.  While we *can* depend on this to support
   conveniently block device like behavior, I choose instead to be
   explicit in how I update the environment.  Namely, this code will
   use the environment in the same mode it does when there is no MTD
   driver.  Modifications to the environment are handles in a NOR
   flash consistent manner.  Erasing the whole environment region
   would normally require a read/modify/write, so here I use the block
   driver.

   safety
   ------

   While I don't access the environment as if it were bonafide without
   checking for validiy, it is possible that the environment becomes
   corrupted.  This code (nor APEX) can completely protect against
   bogus updates.  I make a reasonable attempt to detect corruption.

   mtd oddity
   ----------

   For reasons that I've never investigated, I was unable to read a
   single character from a /dev/mtd device.  The unsetenv code used to
   do something like this:

      if (seek && ch = read(1) && seek) { ch &= ~0x80; write (ch); }

   I found that the read was returning a zero instead of the expected
   value.  There was a ready alternative in using the pvEnv array so
   that is what the routine now does.  I suspect that this may be a
   bug in the driver, but that investigation will not be done soon.

*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/io.h>
#include <ctype.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>

#include "environment.h"
#include "mtdpartition.h"
#include "link.h"

//#define TALK

#if defined (TALK)
# define PRINTF(f ...)	printf(f)
# define DUMPW(a ...)  dumpw (a)
#else
# define PRINTF(f ...)
# define DUMPW(a ...)
#endif

typedef unsigned short u16;
typedef unsigned long u32;

#define CB_LINK_SCAN	(4096)

struct descriptor {
  char driver[80];
  unsigned long start;
  unsigned long length;
};


void dumpw (const void* pv, int cb, unsigned long index, int width)
{
  const unsigned char* rgb = (const unsigned char*) pv;
  int i;

  while (cb > 0) {
    printf ("%08lx: ", index);
    for (i = 0; i < 16; ++i) {
      if (i < cb) {
	switch (width) {
	default:
	case 1:
	  printf ("%02x ", rgb[i]);
	  break;
	case 2:
	  printf ("%04x ", *((u16*)&rgb[i]));
	  ++i;
	  break;
	case 4:
	  printf ("%08x ", *((u32*)&rgb[i]));
	  i += 3;
	  break;
	}
      }
      else
	printf ("   ");
      if (i%8 == 7)
	putchar (' ');
    }
    for (i = 0; i < 16; ++i) {
      if (i == 8)
	putchar (' ');
      putchar ( (i < cb) ? (isprint (rgb[i]) ? rgb[i] : '.') : ' ');
    }
    printf ("\n");

    cb -= 16;
    index += 16;
    rgb += 16;
  }
}

/* _strlen

   is an enhanced version of strlen that stops scanning for the end of
   the string when it finds a 0xff.  This is helpful in preventing
   segfaults while parsing the flash environment.  It does nothing
   more that terminating the string early in that case.

*/

size_t _strlen (const char* sz)
{
  const char* szStart = sz;
  for (; *sz != '\0' && *sz != '\xff'; ++sz)
    ;
  return sz - szStart;
}


static int compare_env (const void* pv1, const void* pv2)
{
  return strcasecmp (((struct env_d*) pv1)->key,
		     ((struct env_d*) pv2)->key);
}


/* parse_region

performs a simplified parse of a region descriptor

*/

static struct descriptor parse_region (const char* sz)
{
  struct descriptor d;
  memset (&d, 0, sizeof (d));

  char* pch;
  if ((pch = index (sz, ':'))) {
    int c = pch - sz;
    if (c > sizeof (d.driver) - 1)
      c = sizeof (d.driver) - 1;
    memcpy (d.driver, sz, c);
    d.driver[c] = 0;
    sz = pch + 1;
  }
  if (sz[0] == '0' && sz[1] == 'x')
    d.start = strtoul (sz, (char**) &sz, 16);
  else
    d.start = strtoul (sz, (char**) &sz, 10);
  if (*sz == 'k' || *sz == 'K') {
    ++sz;
    d.start *= 1024;
  }
  if (*sz == '+') {
    if (sz[0] == '0' && sz[1] == 'x')
      d.length = strtoul (sz, (char**) &sz, 16);
    d.length = strtoul (sz, (char**) &sz, 10);
    if (*sz == 'k' || *sz == 'K') {
      ++sz;
      d.length *= 1024;
    }
  }
  return d;
}


/* Link::containus_apex

   scans the MTD partition for APEX.  It returns true if APEX is
   found.

*/

bool Link::contains_apex (const MTDPartition& mtd) const
{
  PRINTF ("%s: '%s'\n", __FUNCTION__, mtd.dev_block ());
  int fh = ::open (mtd.dev_block (), O_RDONLY);
  if (fh == -1)
    throw "unable to open mtd device.  Root privileges may be required.";

  void* pv = mmap (NULL, CB_LINK_SCAN, PROT_READ, MAP_SHARED, fh, 0);
  if (pv == MAP_FAILED) {
    ::close (fh);
    throw "failed to mmap on open_apex";
  }

  int env_link_version = 0;
  bool fFound = false;

  {
    unsigned long* rgl = (unsigned long*) pv;
    for (int i = 0;
	 i < CB_LINK_SCAN/sizeof (unsigned long)
	   - sizeof (env_link)/sizeof (unsigned long);
	 ++i) {
//      printf ("%d 0x%x (%x %x)\n",
//	      i, rgl[i], ENV_LINK_MAGIC, swab32 (ENV_LINK_MAGIC));

      switch (rgl[i]) {
      case ENV_LINK_MAGIC_1:
	env_link_version = 1;
	break;
      case ENV_LINK_MAGIC:
	env_link_version = 2;
	break;
      }

      switch (swab32 (rgl[i])) {
      case ENV_LINK_MAGIC_1:
//	endian_mismatch = true;
	env_link_version = 1;
	break;
      case ENV_LINK_MAGIC:
//	endian_mismatch = true;
	env_link_version = 2;
	break;
      }

      if (!env_link_version)
	continue;

      fFound = true;
      break;
    }
  }

  munmap (pv, CB_LINK_SCAN);
  close (fh);

  return fFound;
}


/* Link::open

   opens the link by locating APEX, copying the loader, generating a
   fixed-up env_link structure, scanning for the environment variables
   and their defaults, and opening the flash instance of the
   environment.  It first looks for the "Loader" partition.  If there
   is none, it uses the first partition with the assumption that APEX
   may be the primary boot loader.

*/

void Link::open (void)
{
  // Look for the loader by the name of the partition
  MTDPartition mtd = MTDPartition::find ("Loader");

  if (!mtd.is ())
    mtd = MTDPartition::first ();

  bool fFound = mtd.is () && open_apex (mtd);

  // *** FIXME: perhaps we should scan all partitions if there is no
  // "Loader"?
  if (!fFound)
    throw "No APEX partition found";

  load_env ();
  map_environment ();
  scan_environment ();

  PRINTF ("# %d entries in environment\n", entries->size ());
  //  show_environment (env, c_env, rgEntry, c_entry);
}


/* Link::load_env

   loads the environment from the APEX image and sorts the entries

*/

int Link::load_env (void)
{
  c_env = ((char*) env_link->env_end - (char*) env_link->env_start)
    /env_link->env_d_size;

  env = new struct env_d[c_env];
  memcpy (env, (const char*) pvApexSwab
	  + ((char *) env_link->env_start - (char*) env_link->apex_start),
	  c_env*sizeof (struct env_d));
  //  dumpw (env, c_env*sizeof (struct env_d), 0, 0);

  long delta =  (long) pvApexSwab - (long) env_link->apex_start;
  PRINTF ("delta 0x%x\n", delta);
  for (int i = 0; i < c_env; ++i) {
    env[i].key += delta;
    env[i].default_value += delta;
    env[i].description += delta;
    PRINTF ("# env %s %s (%s)\n",
	    env[i].key, env[i].default_value, env[i].description);
  }

  qsort (env, c_env, sizeof (struct env_d), compare_env);

  return c_env;
}


/* Link::locate

   searches the MTD partitions for APEX.  The return value is the name
   of the partition.

   Note that this function returns an allocated pointer.  On the one
   hand, this is a memory leak.  However, it is unnecessary to worry
   about it since this is a short-running program.

*/

const MTDPartition Link::locate (void) const
{
	// First, look for the loader by the name of the partition
  MTDPartition mtd = MTDPartition::find ("Loader");

  if (!mtd.is () || !contains_apex (mtd))
    for (mtd = MTDPartition::first (); mtd.is (); mtd = mtd.next ())
      if (contains_apex (mtd))
	break;

  return mtd;
}


/* Link::map_environment

   locates the user-modifiable environment in flash and maps it.

*/

int Link::map_environment (void)
{
  struct descriptor d = parse_region (env_link->region);

  MTDPartition mtd;
  if (strcasecmp (d.driver, "nor") == 0)
    mtd = MTDPartition::find (d.start);

  if (!mtd.is ())
    return -1;

  fhEnv = ::open (mtd.dev_block (), O_RDONLY);
  cbEnv = d.length;
  ibEnv = d.start - mtd.base;
  lseek (fhEnv, d.start - mtd.base, SEEK_SET);
  // *** FIXME: this mmap will only work with the offset if we are
  // mapping at a page boundary.  This call will fail completely,
  // perhaps even segfault, if the offset is not a multiple of 4KiB.
  // The only other way to handle this would be to round down to a
  // page boundary and keep track of the real start of the
  // environment.
  pvEnv = ::mmap (NULL, cbEnv, PROT_READ, MAP_SHARED, fhEnv,
		  d.start - mtd.base);
  // Open separate handle for writing to environment
  fhEnvChar = ::open (mtd.dev_char (), O_RDWR);

  // And another for erasing the environment
  fhEnvBlock = ::open (mtd.dev_block (), O_RDWR);

  PRINTF ("%s %s->(%d,%d,%d) 0x%x+0x%x->%p [%x %x]\n",
	  env_link->region, mtd.dev_block (),
	  fhEnv, fhEnvChar, fhEnvBlock, ibEnv, cbEnv,
	  pvEnv, d.start, mtd.base);
  DUMPW (pvEnv, 256, 0, 0);

  return cbEnv;
}


/* Link::open_apex

   reads APEX from the given partition.  It returns false if APEX
   wasn't found.

*/

bool Link::open_apex (const MTDPartition& mtd)
{
  PRINTF ("%s: '%s'\n", __FUNCTION__, mtd.dev_block ());
  int fh = ::open (mtd.dev_block (), O_RDONLY);
  if (fh == -1)
    throw "unable to open mtd device.  Root privileges may be required.";

  void* pv = mmap (NULL, CB_LINK_SCAN, PROT_READ, MAP_SHARED, fh, 0);
  if (pv == MAP_FAILED) {
    ::close (fh);
    throw "failed to mmap on open_apex";
  }

  int index_env_link = 0;
  env_link_version = 0;
  cbApex = 0;

  {
    unsigned long* rgl = (unsigned long*) pv;
    for (int i = 0;
	 i < CB_LINK_SCAN/sizeof (unsigned long)
	   - sizeof (env_link)/sizeof (unsigned long);
	 ++i) {
//      printf ("%d 0x%x (%x %x)\n",
//	      i, rgl[i], ENV_LINK_MAGIC, swab32 (ENV_LINK_MAGIC));

      switch (rgl[i]) {
      case ENV_LINK_MAGIC_1:
	env_link_version = 1;
	break;
      case ENV_LINK_MAGIC:
	env_link_version = 2;
	break;
      }

      switch (swab32 (rgl[i])) {
      case ENV_LINK_MAGIC_1:
	endian_mismatch = true;
	env_link_version = 1;
	break;
      case ENV_LINK_MAGIC:
	endian_mismatch = true;
	env_link_version = 2;
	break;
      }

      if (!env_link_version)
	continue;

      switch (env_link_version) {
      case 1:
	{
	  struct env_link_1* env_link = (struct env_link_1*) &rgl[i];
	  index_env_link = i*sizeof (unsigned long);
	  cbApex = (char*) swab32_maybe ((u32) env_link->apex_end)
	    - (char*) swab32_maybe ((u32) env_link->apex_start);
	}
	break;
      case 2:
	{
	  struct env_link* env_link = (struct env_link*) &rgl[i];
	  index_env_link = i*sizeof (unsigned long);
	  cbApex = (char*) swab32_maybe ((u32) env_link->apex_end)
	    - (char*) swab32_maybe ((u32) env_link->apex_start);
	}
	break;
      }
      break;
    }
  }

  munmap (pv, CB_LINK_SCAN);

  if (!index_env_link) {
    close (fh);
    return false;
  }

  pv = mmap (NULL, cbApex, PROT_READ, MAP_PRIVATE, fh, 0);
  if (pv == MAP_FAILED) {
    close (fh);
    return false;
  }

  int cbLink = (sizeof (struct env_link) + 1024 + 4096 - 1) & ~4096;
  env_link = (struct env_link*) new char[cbLink];
  bzero (env_link, cbLink);

  switch (env_link_version) {
  case 1:
    {
      const struct env_link_1& env_link_1
	= *(const struct env_link_1*) ((unsigned char*) pv + index_env_link);
      env_link->magic		= env_link_1.magic;
      env_link->apex_start	= env_link_1.apex_start;
      env_link->apex_end	= env_link_1.apex_end;
      env_link->env_start	= env_link_1.env_start;
      env_link->env_end		= env_link_1.env_end;
      env_link->env_link	// *** Hack to accomodate partition w/swap
	= (void*) swab32_maybe (swab32_maybe ((u32) env_link->apex_start)
				+ index_env_link - 16);
      env_link->env_d_size	= env_link_1.env_d_size;
      memcpy ((void*) env_link->region, env_link_1.region,
	      cbLink - sizeof (struct env_link));
    }
    break;

  case 2:
    memcpy (env_link, (const char*) pv + index_env_link, cbLink);
    break;
  }

  swab32_block_maybe (env_link, cbLink);

  mapping_offset = index_env_link
    - ((char*) env_link->env_link - (char*) env_link->apex_start);

// *** FIXME: due to mapping_offset it could be the case that we
// haven't mapped enough of APEX.
  pvApex = (void*) new char[cbApex];
  memcpy (pvApex, (const char*) pv + mapping_offset, cbApex);

  munmap (pv, cbApex);
  close (fh);

  pvApexSwab = (void*) new char[cbApex];
  memcpy (pvApexSwab, pvApex, cbApex);
  swab32_block_maybe (pvApexSwab, cbApex);

				// Guarantee termination
  *(char*) &env_link->apexversion[sizeof (env_link->apexversion) - 1] = 0;

  return true;
}


/* Link::scan_environment

   is the essential routine of this class.  It scans the flash
   environment and builds the array of entries.  These will be all of
   the entries that exist in non-volatile memory, but these may not
   all have corresponding entries among APEXs environment descriptors.

   The return value is the count of unique ids found, or -1 if the is
   environment is unrecognized.

*/

int Link::scan_environment (void)
{
  const char* pb = (const char*) pvEnv;

  entries->clear ();
  idNext = 0;

  if (pb[0] == (char)0xff && pb[1] == (char)0xff) {
    PRINTF ("# empty environment\n");
    m_state = envEmpty;
    return entries->size ();
  }

  if (pb[0] != ENV_MAGIC_0 || pb[1] != ENV_MAGIC_1) {
    PRINTF ("# environment contains unrecognized data\n");
    m_state = envNoWrite;
    return -1;
  }

  m_state = envInUse;

  pb += 2;

  const char* pbEnd = pb + cbEnv;
  while (pb < pbEnd && *pb != (char) 0xff) {
    const char* head = pb;
    char flags = *pb++;
    int id = (unsigned char) flags & 0x7f;
    if (id >= idNext)
      idNext = id + 1;

    if (!entries->contains (id)) {
      entry e;
      e.key = pb;
      //      (*entries)[id].key = pb;

      //      int cb = strlen ((char*) pb);
      //      rgEntry[id].key = new char [cb + 1];
      //      strcpy (rgEntry[id].key, (char*) pb);
      PRINTF ("# found %s\n", e.key);
      pb += _strlen (pb);
      if (*pb != '\0') {
	m_state = envCorrupt;
	break;
      }
      ++pb;
      for (int index = 0; index < c_env; ++index)
	if (strcasecmp (e.key, env[index].key) == 0) {
	  e.index = index;
	  break;
	}
      (*entries)[id] = e;
    }
    PRINTF ("# %s = %s\n", (*entries)[id].key, pb);
    //    int cb = strlen ((char*) pb);
    if (flags & 0x80) {
      (*entries)[id].value = pb;
      (*entries)[id].active = head;
    }
    pb += strlen (pb);
    if (*pb != '\0') {
      m_state = envCorrupt;
      break;
    }
    ++pb;
  }
  cbEnvUsed = pb - (char*) pvEnv;

  return entries->size ();
}


void Link::describe (const char* key)
{
  for (int i = 0; i < c_env; ++i)
    if (!key || strcasecmp (env[i].key, key) == 0) {
      printf ("%s: %s\n", env[i].key, env[i].description);
      if (key)
	return;
    }
  if (key)
    throw "unknown environment variable";
}


void Link::dump (void)
{
  if (pvEnv && cbEnv)
    dumpw (pvEnv, cbEnv, 0, 0);
  else
    throw "no flash environment available";
}

void Link::eraseenv (bool force)
{
  if (m_state == envNull)
    throw "no writable environment available";
  if (m_state == envNoWrite && !force)
    throw "use force option to erase region containing foreign data";

  char rgb[cbEnv];
  memset (rgb, 0xff, sizeof (rgb));

  if (   ::lseek (fhEnvBlock, ibEnv, SEEK_SET) != ibEnv
      || ::write (fhEnvBlock, rgb, cbEnv) != cbEnv)
      throw "failed to write environment";
}


/* Link::printenv

   prints the value for a given key.  It is the value from flash if
   one exists, or the default value if there is none.

*/

void Link::printenv (const char* key)
{
  for (int i = 0; i < c_env; ++i) {
    if (!key || strcasecmp (env[i].key, key) == 0) {
      EntryMap::iterator it = entries->find_index (i);
      if (it != entries->end ())
	printf ("%s = %s\n", key, (*it).second.value);
      else
	printf ("%s *= %s\n", key, env[i].default_value);
      if (key)
	return;
    }
  }
  if (key)
    throw "unknown environment variable";
}


/* Link::unsetenv

   deletes a key/value pair from flash if there is one.  It returns
   true if there was a successful delete.

*/

void Link::unsetenv (const char* key)
{
  if (m_state == envNull)
    throw "no writable environment available";
  if (m_state == envNoWrite)
    throw "environment region contents unrecognized and unwritable";
  if (m_state == envCorrupt)
    throw "unable to update corrupt environment";

  if (key == NULL)
    throw "NULL key passed to unsetenv";

  // Look for the key among the flash entries
  EntryMap::iterator it;
  for (it = entries->begin (); it != entries->end (); ++it)
    if (strcasecmp (key, (*it).second.key) == 0)
      break;

  if (it == entries->end ())
    throw "variable not present in flash envronment";

  if (it != entries->end () && (*it).second.active) {
    int ib = (*it).second.active - (const char*) pvEnv;
    if (::lseek (fhEnvChar, ibEnv + ib, SEEK_SET) == ibEnv + ib) {
      char ch = ((const char*) pvEnv)[ib];
      ch &= ~0x80;		// Clear high bit indicating deletion
      if (::write (fhEnvChar, &ch, 1) != 1)
	throw "failed to write environment";
    }
    else
      throw "unable to read from environment";
  }
}


/* Link::setenv

   accepts a key/value pair and sets this value in the environment.
   Previously set instances of the key are marked as deleted.

*/

void Link::setenv (const char* key, const char* value)
{
  if (m_state == envNull)
    throw "no writable environment available";
  if (m_state == envNoWrite)
    throw "environment region contents unrecognized and unwritable";
  if (m_state == envCorrupt)
    throw "unable to update corrupt environment";

  if (key == NULL)
    throw "NULL key passed to setenv";
  if (value == NULL)
    throw "NULL value passed to setenv";

  // Look for the key among the environment variables that APEX
  // recognizes.
  int index;
  for (index = 0; index < c_env; ++index)
    if (strcasecmp (key, env[index].key) == 0)
      break;
  if (index >= c_env)
    throw "unknown environment variable";

  // Look for the key among the flash entries
  EntryMap::iterator it;
  for (it = entries->begin (); it != entries->end (); ++it)
    if (strcasecmp (key, (*it).second.key) == 0)
      break;

  if (it != entries->end () && (*it).second.active)
    unsetenv (key);

  char rgb[2 + 2 + strlen (key) + strlen (value) + 2];
  char* pb = rgb;
  if (m_state == envEmpty) {
    *pb++ = ENV_MAGIC_0;
    *pb++ = ENV_MAGIC_1;
  }

  if (it != entries->end ()) {
    int cb = strlen (value);
    if (cbEnv - cbEnvUsed < cb + 2)
      throw "insufficient free space in environment";
    *pb++ = 0x80 + (*it).first;
    memcpy (pb, value, cb + 1);
    pb += cb + 1;
    if (::lseek (fhEnvChar, ibEnv + cbEnvUsed, SEEK_SET) != ibEnv + cbEnvUsed
	|| ::write (fhEnvChar, rgb, pb - rgb) != pb - rgb)
      throw "failed to write environment";
  }
  else {
    int cbKey = strlen (key);
    int cbValue = strlen (value);
    if (cbEnv - cbEnvUsed < cbKey + cbValue + 3)
      throw "insufficient free space in environment";
    *pb++ = 0x80 + idNext;
    memcpy (pb, key, cbKey + 1);
    pb += cbKey + 1;
    memcpy (pb, value, cbValue + 1);
    pb += cbValue + 1;
    if (::lseek (fhEnvChar, ibEnv + cbEnvUsed, SEEK_SET) != ibEnv + cbEnvUsed
	|| ::write (fhEnvChar, rgb, pb - rgb)
	!= pb - rgb)
      throw "failed to write environment";
  }
}


/* Link::show_environment

   knits together the two different kinds of environment data and
   displays the values.

*/

void Link::show_environment (void)
{
  for (int i = 0; i < c_env; ++i) {
    const char* value = NULL;
    for (EntryMap::iterator it = entries->begin ();
	 it != entries->end (); ++it)
      if ((*it).second.index == i) {
	value = (*it).second.value;
	break;
      }

    if (value)
      printf ("%s = %s\n", env[i].key, value);
    else
      printf ("%s *= %s\n", env[i].key, env[i].default_value);
  }
  for (EntryMap::iterator it = entries->begin ();
       it != entries->end (); ++it)
    if ((*it).second.index == -1)
      printf ("%s #= %s\n", (*it).second.key, (*it).second.value);
}
