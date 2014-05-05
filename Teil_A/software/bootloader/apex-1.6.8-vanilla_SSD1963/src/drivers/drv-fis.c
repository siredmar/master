/* drv-fis.c

   written by Marc Singer
   19 Jul 2006

   Copyright (C) 2006 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   FIS is a partition driver.  It parses flash image descriptors in an
   underlying region as files of a sort.  It is similar to the
   filesystem drivers in this way.

   IXP42X in LE Mode
   -----------------

     This Xscale processor does some shenanigans with byte & half-word
     swapping when it is in little-endian mode.  A flash part which
     was configured for a big-endian system doesn't read correctly
     when the CPU is in little-endian mode.  For the most part, this
     is handled by the NOR flash driver.

     The FIS driver needs to cope in that the data in the partition
     table will be ordered for big-endian consumption.  The strings
     will be OK because of the NOR flash driver's efforts.  However
     the start and length values will be BE and must be swapped for
     LE.

     This conversion is done by reading all of the partition entries,
     looking for the partition table's own entry.  When it is found,
     the driver compares the swapped start address of the directory
     partition with the address we're reading.  If both are in the
     same erase block, we know we have a swapped directory.

     For simplicity and general correctness, this is done every time
     this driver activates.  The table could be cached, but it is
     possible that the underlying block driver region is changed
     between invocations.  So, the easy solution is to read through
     the table twice: once to determine the endian-ness and a second
     time looking for the data we care about.

   Underlying Region
   -----------------

   Due to the way we map form the user's region onto the underlying
   region, this underlying region's offset and length are ignored.  We
   certainly could add it in, but the FIS structures don't let this
   make sense.  Unless our needs change, this is a fine assumption.
   All we really care about is finding the real flash driver for that
   the partition table describes.


   Open Call Pass Through
   ----------------------

   The open call converts the fis driver descriptor into a descriptor
   that points to the partition using the source driver.  Due to the
   fact that that partition table is physically addressed, the FIS
   driver has to subtract the base address of the underlying driver
   when passing this through.  Thus, the FIS driver has no
   read/seek/write methods.

   Skip Descriptors
   ----------------

   In order to handle skip descriptors, we need to either process all
   IO through this driver, or we need to pass the skip information to
   the underlying driver.  I think we're better off filtering as that
   makes for the most localized change.  Really, all we need to do is
   pass all IO calls through a simple check for the skipp'd areas and
   go from there.  This also means that the skip descriptors are a
   global resource as we may have more than one descriptor open.

   There is a short path that ignores the skip data if none of the
   open descriptors has any.

   Also, we can only handle one partition with skips at a time.  This
   ought not cause problems since it is really just a crutch for
   reading data from a partition.  The maximum number of skips per
   partition is determined by a static array that holds the skip data.

   It is important that the skip descriptors are sorted so that we
   don't have to pass through the descriptors more than once when
   performing each cycle through fis_read.  We really have two tasks.
   We want to determine which descriptors preceed the current IO
   position and we want to know if there is a descriptor that follows
   the current IO position that truncates the contiguous IO block.
   This could be done in two passes, but with a sort, it can be done
   in one.  Sort is available as long as the user sorts commands.
   Moreover, this is a driver that is used on a system that doesn't
   seem to need a SMALL version.

   FIS descriptors with or without skips, will show addresses from the
   base of the underlying region.  It would be nice if they could show
   partition relative addresses, but there isn't enough information
   for that in the descriptor structure.  Skips, on the other had,
   will not appear in the addresses, such that the flash addresses for
   the same area, when skips are ignored, will not match.  This is on
   purpose.

   fis_directory_entries
   ---------------------

   Except in the prescan call, the search through the directory is
   ultimately limited by the number of possible entries in the
   partition containing the directory.  The end_of_table () call is
   likely to limit the search even more, but this is an optimization
   introduced here in APEX.  In prescan, we depend on the end_of_table
   () call and the extent of the region passed via the environment or
   alias as "fis-drv".

   Deleted Entries and the End of the Table
   ----------------------------------------

   The Redboot will write 0xff to the start of a partition entry when
   it deletes it.  This is peculiar since the only way to write 0xff
   is to write the whole partition, so why not simply remove the
   partition instead.  Duh.

   The end of the table is marked by 0xff's in the first two bytes of
   the descriptor.

*/

#include <config.h>
#include <apex.h>
#include <driver.h>
#include <service.h>
#include <linux/string.h>
#include <spinner.h>
#include <asm/reg.h>
#include <error.h>
#include <environment.h>
#include <lookup.h>
#include <sort.h>

//#define FAKE
//#define TALK

#if defined (TALK)
# define PRINTF(v...)	printf (v)
#else
# define PRINTF(v...)	do {} while (0)
#endif

#define ENTRY(l) PRINTF ("%s\n", __FUNCTION__)
struct fis_descriptor {
  char name[16];		/* Image name, null terminated */
  unsigned long start;		/* Physical memory address of image */
  unsigned long lma;		/* Load Memory Address for image */
  unsigned long length;		/* Byte length of partition */
  unsigned long entry;		/* Image entry point */
  unsigned long data_length;	/* Byte length of image */
  unsigned char _pad[256 - 16 - 7*sizeof(unsigned long)];
  unsigned long cksum_desc;	/* Checksum over descriptor */
  unsigned long cksum_image;	/* Checksum over image data */
};

static int fis_directory_swap;	/* Set for a byte swapped directory */
static int fis_directory_entries; /* Computed from the size of the partition */

struct fis_skip_descriptor {
  char magic[4];		/* 's' 'k' 'i' 'p' */
  unsigned long offset;		/* Offset from partition start */
  unsigned long length;		/* Size of skip in bytes */
};

static struct fis_skip_descriptor rgskip[4];
static struct descriptor_d dskip; /* Underlying skip IO descriptor */

#if defined (CONFIG_ENV)
static __env struct env_d e_fis_drv = {
  .key = "fis-drv",
  .default_value = CONFIG_DRIVER_FIS_BLOCKDEVICE,
  .description = "Block device region for FIS partition driver",
};
#endif

static inline unsigned long swab32(unsigned long l)
{
  return (  ((l & 0x000000ffUL) << 24)
	  | ((l & 0x0000ff00UL) << 8)
	  | ((l & 0x00ff0000UL) >> 8)
	  | ((l & 0xff000000UL) >> 24));
}

static inline const char* block_driver (void)
{
  return lookup_alias_or_env ("fis-drv", CONFIG_DRIVER_FIS_BLOCKDEVICE);
}

static inline int deleted_entry (struct fis_descriptor* partition)
{
	/* Erased entry -- dumb, but that's RedBoot */
  return partition->name[0] == 0xff;
}

static inline int end_of_table (struct fis_descriptor* partition)
{
  return (*(u16*) &partition->name[0]) == 0xffff;
}

static int compare_skips (const void* _a, const void* _b)
{
  struct fis_skip_descriptor* a = (struct fis_skip_descriptor*) _a;
  struct fis_skip_descriptor* b = (struct fis_skip_descriptor*) _b;

  return a->offset - b->offset;
}

static void prescan_directory (struct descriptor_d* d)
{
  unsigned long start = 0;
  unsigned long size = 0;
  unsigned long eraseblocksize = 0;

  fis_directory_swap = 0;
  fis_directory_entries = 0;

  descriptor_query (d, QUERY_START, &start);
  descriptor_query (d, QUERY_SIZE, &size);

  PRINTF ("%s: start %lx size %ld start 0x%lx index 0x%x\n",
	  __FUNCTION__, start, size, d->start, d->index);
  if (size == 0)
    return;			/* Unable to perform queries */

  d->driver->seek (d, 0, SEEK_SET);

  while (1) {
    struct fis_descriptor partition;
    if (d->driver->read (d, &partition, sizeof (partition))
	!= sizeof (partition))
      break;
    if (end_of_table (&partition))
      break;
    if (deleted_entry (&partition))
      continue;

    if (strnicmp (partition.name, "fis directory", 16) == 0) {
      /* Read the erase block size where we need it since it is
	 possible we're crossing boundaries within flash where the
	 erase block size changes. */
      descriptor_query (d, QUERY_ERASEBLOCKSIZE, &eraseblocksize);
      /* The original implementation of this check was to compare the
	 the FIS directory partition entry's length field with
	 eraseblocksize swapped.  This, however, assumed that the
	 directory was always the full size of an erase block,
	 something which is not (nor ever was) true.  So we changed
	 the test to something more robust.  Now, we check that the
	 partition table start address is in the same erase block from
	 which we are reading. */
      PRINTF ("%s: partition.start 0x%lx (0x%lx)  &partition 0x%lx"
	      "  mask 0x%lx\n",
	      __FUNCTION__, partition.start, swab32 (partition.start),
	      start + d->start + d->index, ~(eraseblocksize - 1));
      if ((swab32 (partition.start) & ~(eraseblocksize - 1))
	  == ((unsigned long) (start + d->start + d->index)
	      & ~(eraseblocksize - 1))) {
	fis_directory_swap = 1;
	fis_directory_entries = swab32 (partition.length)/sizeof (partition);
      }
      else
	fis_directory_entries = partition.length/sizeof (partition);
      PRINTF ("%s: %d entries, swapped %d\n",
	      __FUNCTION__, fis_directory_entries, fis_directory_swap);
      break;
    }
  }
  d->driver->seek (d, 0, SEEK_SET);
}

static const char* map_region (struct descriptor_d* d,
			       struct fis_descriptor* partition)
{
  static char sz[64];
  unsigned long start = 0;
  descriptor_query (d, QUERY_START, &start);

  snprintf (sz, sizeof (sz), "%s:0x%08lx+0x%08lx",
	    d->driver_name,
	    partition->start - start, partition->length);
  return sz;
}

static int fis_open (struct descriptor_d* d)
{
  int result;
  struct descriptor_d fis_d;
  int cEntries;

  if (d->c != 1 || d->iRoot != 1)
    ERROR_RETURN (ERROR_BADPARTITION, "path must include partition");

  if (   (result = parse_descriptor (block_driver (), &fis_d))
      || (result = open_descriptor (&fis_d)))
    return result;

  prescan_directory (&fis_d);

  PRINTF ("%s: d@0x%lx+0x%lx\n", __FUNCTION__, d->start, d->length);

  for (cEntries = fis_directory_entries ; cEntries--; ) {
    struct fis_descriptor partition;
    int skip = 0;
    size_t cbSkip = 0;

    if (fis_d.driver->read (&fis_d, &partition, sizeof (partition))
	!= sizeof (partition)) {
      close_descriptor (&fis_d);
      ERROR_RETURN (ERROR_IOFAILURE, "premature end of fis-drv");
    }
    if (end_of_table (&partition))
      break;
    if (deleted_entry (&partition))
      continue;
    if (strnicmp (d->pb[0], partition.name, sizeof (partition.name)))
      continue;

    if (fis_directory_swap) {
      partition.start = swab32 (partition.start);
      partition.length = swab32 (partition.length);
    }

    PRINTF ("%s: partition %s @0x%lx+0x%lx\n",
	    __FUNCTION__, partition.name, partition.start, partition.length);

    if (d->start >= partition.length) {
      close_descriptor (&fis_d);
      ERROR_RETURN (ERROR_OPEN, "region exceeds partition size");
    }

	/* Look for skips */
    {
      int i;
      unsigned long start = 0;
      descriptor_query (&fis_d, QUERY_START, &start);
      start = partition.start - start;
      memset (rgskip, 0, sizeof (rgskip));
      for (i = 0;
	   skip < sizeof (rgskip)/sizeof (*rgskip)
	     && i < sizeof (partition._pad); i +=4) {
	if ((  partition._pad[i + 0] != 's'
	    || partition._pad[i + 1] != 'k'
	    || partition._pad[i + 2] != 'i'
	    || partition._pad[i + 3] != 'p'
	    )
#if defined (FAKE)
	    && skip > 0
#endif
)
	  continue;
	memcpy (&rgskip[skip], (void*) &partition._pad[i],
		sizeof (struct fis_skip_descriptor));

#if defined (FAKE)
	rgskip[skip].magic[0] = 's';
	rgskip[skip].offset = swab32 (32); /* @32 */
	rgskip[skip].length = swab32 (16); /* +16 */
#endif

	if (fis_directory_swap) {
	  rgskip[skip].offset = swab32 (rgskip[skip].offset);
	  rgskip[skip].length = swab32 (rgskip[skip].length);
	}
	PRINTF ("%s: skip @0x%lx+0x%lx",
		__FUNCTION__, rgskip[skip].offset, rgskip[skip].length);
	rgskip[skip].offset += start; /* Offsets relative to base region */
	PRINTF (" -> @0x%lx+0x%lx\n",
		rgskip[skip].offset, rgskip[skip].length);
	cbSkip += rgskip[skip].length;
	++skip;
      }
      if (skip)
	sort (rgskip, skip, sizeof (*rgskip), compare_skips, NULL);
    }

    /* Fixup the partition/request length to cope with skips.  Skips
       come directly out of the partition size, but shouldn't affect
       the request size as long as there is enough data to satisfy it.
       Note that the skip data is added back to the dskip descriptor
       when it is opened. */
    partition.start += d->start;
    partition.length -= d->start + cbSkip;
    if (d->length && d->length < partition.length - cbSkip)
      partition.length = d->length;

    /* If there is a skip, we open a skip descriptor over the
       underlying region and return to the user a descriptor over the
       fis driver.  All IO will go through proxy functions in this
       driver.

       If there is are skips, we return a descriptor over the
       underlying region. */

    if (skip) {
      const char* sz;
      d->length = partition.length;
      partition.length += cbSkip;
      sz = map_region (&fis_d, &partition);
      PRINTF ("%s: %s\n", __FUNCTION__, sz);
      if (   (result = parse_descriptor (sz, &dskip))
	  || (result = open_descriptor (&dskip)))
	return result;
      d->start = dskip.start;
      PRINTF ("%s: d @0x%lx+0x%lx\n", __FUNCTION__, d->start, d->length);
    }
    else {
      const char* sz = map_region (&fis_d, &partition);
      close_helper (d);
      PRINTF ("%s: %s\n", __FUNCTION__, sz);
      if (   (result = parse_descriptor (sz, d))
	  || (result = open_descriptor (d)))
	return result;
    }

    return 0;
  }

  close_descriptor (&fis_d);
  ERROR_RETURN (ERROR_BADPARTITION, "partition not found");
}

static void fis_close (struct descriptor_d* d)
{
  close_helper (&dskip);
  close_helper (d);
}

static ssize_t fis_read (struct descriptor_d* d, void* pv, size_t cb)
{
  ssize_t cbRead = 0;

  if (d->index + cb > d->length)
    cb = d->length - d->index;

  PRINTF ("%s: request 0x%lx 0x%lx 0x%x 0x%x\n",
	  __FUNCTION__, d->start, d->length, d->index, cb);

  while (cb > 0) {
    size_t ib = d->start + d->index;
    size_t available = cb;
    size_t cbSkip = 0;
    ssize_t cbThis;
    int i;

    for (i = 0; i < sizeof (rgskip)/sizeof (*rgskip)
	   && rgskip[i].magic[0] == 's'; ++i) {
      PRINTF ("%s: skip %d <%lx %lx>\n",
	      __FUNCTION__, i, rgskip[i].offset, rgskip[i].length);
      PRINTF ("  ib 0x%x  av 0x%x  cbSkip 0x%x\n", ib, available, cbSkip);
      if (ib + cbSkip < rgskip[i].offset) {
		/* truncate IO when necessary */
	if (available > rgskip[i].offset - ib - cbSkip)
	  available = rgskip[i].offset - ib - cbSkip;
	break;
      }
      cbSkip += rgskip[i].length;
    }

    PRINTF ("%s: read 0x%x %d (%d)\n", __FUNCTION__, ib, available, cbSkip);

    seek_helper (&dskip, ib - d->start + cbSkip, SEEK_SET);
    cbThis = dskip.driver->read (&dskip, pv, available);
    if (cbThis != available)
      ERROR_RETURN (ERROR_IOFAILURE, "short read");

    pv += available;
    cb -= available;
    d->index += available;
    cbRead += available;
  }

  return cbRead;
}

#if !defined (CONFIG_SMALL)
static void fis_report (void)
{
  int result;
  struct descriptor_d d;
  int cEntries;

  if (   (result = parse_descriptor (block_driver (), &d))
      || (result = open_descriptor (&d)))
    return;

  prescan_directory (&d);

  printf ("  fis:\n");

  for (cEntries = fis_directory_entries; cEntries--; ) {
    struct fis_descriptor partition;
    int i;

    if (d.driver->read (&d, &partition, sizeof (partition))
	!= sizeof (partition))
      break;
    if (end_of_table (&partition))
      break;
    if (deleted_entry (&partition))
      continue;

    if (fis_directory_swap) {
      partition.start = swab32 (partition.start);
      partition.length = swab32 (partition.length);
    }

    printf ("          %s %s\n", map_region (&d, &partition), partition.name);

	/* Show skips as well */
    for (i = 0; i < sizeof (partition._pad); i += 4) {
      struct fis_skip_descriptor skip;
      if (   partition._pad[i + 0] != 's'
	  || partition._pad[i + 1] != 'k'
	  || partition._pad[i + 2] != 'i'
	  || partition._pad[i + 3] != 'p')
	continue;
      memcpy (&skip, &partition._pad[i], sizeof (skip));
      if (fis_directory_swap) {
	skip.offset = swab32 (skip.offset);
	skip.length = swab32 (skip.length);
      }
      printf ("             @0x%08lx+0x%08lx  (skip)\n",
	      skip.offset, skip.length);
    }
  }

  close_descriptor (&d);
}
#endif

static __driver_6 struct driver_d fis_driver = {
  .name		= "fis-part",
  .description	= "FIS partition driver",
  .flags	= DRIVER_DESCRIP_FS,
  .open		= fis_open,
  .close	= fis_close,
  .read		= fis_read,
  /* Write and erase could be implemented to cope with skips. */
  .seek		= seek_helper,
};

static __service_6 struct service_d fis_service = {
#if !defined (CONFIG_SMALL)
  .report = fis_report,
#endif
};
