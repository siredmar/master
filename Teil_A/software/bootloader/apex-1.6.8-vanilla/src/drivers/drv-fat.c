/* drv-fat.c

   written by Marc Singer
   7 Feb 2005

   Copyright (C) 2005 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   FAT Filesystem Driver.  This driver should be platform independent.
   The only hook to the rest of the system is the name of the driver
   that handles the underlying I/O.

   o bootsector

     It might be better to read the boot sector than to read the
     partition table in case the user has a floppy drive.  Could do
     this, but we don't have the hardware, so why fuss with it now?

   o seek

     will require that we catch the seeking call so that seeking from
     an arbitrary point in the file can be accomplished.  Really,
     we're not going to do it, so we might make it illegal...or at
     least challenge it.

     Seeking from the end of the file will be very expensive if the
     target isn't in the last cluster.  Either we need to keep a list
     of clusters as long as the offset/cluster_size, or we have to
     fail to do the right thing.

     Probably, the best we can do is handle seeking before the current
     position is to rewind to the beginning of the file and seek
     forward.

   o FAT caching

     one FAT sector is cached to make it somewhat efficient to read
     through a file.

     It used to be one sector, but now it is three.  We do this
     because the FAT12 cluster values may span sectors.  It is far
     easier to cache three sectors than to deal with this anomaly.

   o Filename handling

     It is cheaper, overall to put the user's filename in the same
     form as the directory entries that it is to fixup each entry as
     it is read.  However, we don't really care how long it takes to
     find a file.

   o SECTOR_SIZE and parameter.bytes_per_sector

     We cannot handle a sector size that isn't 512 bytes.  So, for the
     time being, the parameter entry for the size of a sector is
     ignored.

   o FAT formats

     Only FAT16 and FAT12 are supported.  Others may be, but only when
     we have a sample to test.  Fortunately, the details are
     concentrated mostly in the next_cluster function.

   o FILE I/O

     Information about the current file is cached in the fat info
     structure.  Thus precluding operations on two files.  Be warned.

   o The code path needs to either cope properly with no filename and
     return the partition table, or it needs to drop the feature.  The
     open call starts the ball rolling, but the read code doesn't know
     what to do.

   o vfat.  We presently don't parse vfat directory entries.  This
     means that only short filenames are recognized.  By implementing
     a general purpose directory enumeration function, we should be
     able to cope with vfat names.  Writing a function is going to be
     necessary in order to search for files along a path.

   o directories and subdirectories.  The reason this code doesn't yet
     support directories is that the fs format isn't uniform.  The
     root directory is handled differently from other directories.
     When I have the patience, I'll add some general directory
     handling code which will make it possible to implement full paths
     as well as fat_info().

*/

#include <config.h>
#include <apex.h>
#include <driver.h>
#include <service.h>
#include <linux/string.h>
#include <linux/ctype.h>
#include <spinner.h>
#include <linux/kernel.h>
#include <error.h>
#include <environment.h>
#include <lookup.h>

//#define TALK

#if defined TALK
# define PRINTF(v...)	printf (v)
#else
# define PRINTF(v...)	do {} while (0)
#endif

#define ENTRY(l) PRINTF ("%s\n", __FUNCTION__)


#define FAT_READONLY	(1<<0)
#define FAT_HIDDEN	(1<<1)
#define FAT_SYSTEM	(1<<2)
#define FAT_VOLUME	(1<<3)
#define FAT_DIRECTORY	(1<<4)
#define FAT_ARCHIVE	(1<<5)

#define SECTOR_SIZE	512

enum {
  fat12 = 1,
  fat16 = 2,
};

#define CLUSTERS_PER_3FAT_FAT12	(SECTOR_SIZE*4)
#define CLUSTERS_PER_3FAT_FAT16	(SECTOR_SIZE*3/2)

struct partition {
  unsigned char boot;
  unsigned char begin_chs[3];
  unsigned char type;
  unsigned char end_chs[3];
  unsigned long start;
  unsigned long length;
};

struct parameter {
  unsigned char jump[3];
  unsigned char oemname[8];
  unsigned short bytes_per_sector;
  unsigned char sectors_per_cluster;
  unsigned short reserved_sectors;
  unsigned char fats;
  unsigned short root_entries;
  unsigned short small_sectors;
  unsigned char media;
  unsigned short sectors_per_fat;
  unsigned short sectors_per_track;
  unsigned short heads;
  unsigned long hidden_sectors;
  unsigned long large_sectors;
  unsigned char logical_drive;
  unsigned char reserved;
  unsigned char signature;	/* Must be 0x29 */
  unsigned long serial;
  unsigned char volume[11];
  unsigned char type[8];
  char dummy[2];
} __attribute__ ((packed));

struct directory {
  unsigned char file[8];
  unsigned char extension[3];
  unsigned char attribute;
  unsigned char type;
  unsigned char checksum;
  unsigned char name2[8];
  unsigned short time;
  unsigned short date;
  unsigned short cluster;
  unsigned long length;
} __attribute__ ((packed));

struct fat_info {
  struct descriptor_d d;	/* Descriptor for underlying driver */

  int fOK;			/* True when the block device recognizable */
  u32 region_crc;               /* CRC of region for detecting changes */
  struct partition partition[4];
  struct parameter parameter;	/* Parameter info for the partition */
  size_t index_cluster_2;	/* Partition index of first cluster */
  size_t bytes_per_cluster;	/* Precomputed cluster size */

  /* *** FIXME: directory clusters not yet implemented */
//unsigned cluster_dir;		/* Current directory cluster being read  */

  int fat_type;			/* Decided FAT format */
  /* *** FIXME: buffers should be in .xbss section */
  char fat[SECTOR_SIZE*3];	/* Cached FAT sectors */
  int sector_fat;		/* Sector number of the cached FAT */

  struct directory file;	/* Directory entry for the current file */
  unsigned cluster_file;	/* Current file cluster being read */
  size_t index_cluster_file;	/* Index at base of the cluster */
};

static struct fat_info fat;

//struct driver_d* fs_driver;	/* *** FIXME: underlying driver link hack */

static inline unsigned short read_short (void* pv)
{
  unsigned char* pb = (unsigned char*) pv;
  return  ((unsigned short) pb[0])
       + (((unsigned short) pb[1]) << 8);
}

static inline unsigned long read_long (void* pv)
{
  unsigned char* pb = (unsigned char*) pv;
  return  ((unsigned long) pb[0])
       + (((unsigned long) pb[1]) <<  8)
       + (((unsigned long) pb[2]) << 16)
       + (((unsigned long) pb[3]) << 24);
}

#if defined (CONFIG_ENV)
static __env struct env_d e_fat_drv = {
  .key = "fat-drv",
  .default_value = CONFIG_DRIVER_FAT_BLOCKDEVICE,
  .description = "Block device region for FAT filesystem driver",
};
#endif

static const char* block_driver (void)
{
  return lookup_alias_or_env ("fat-drv", CONFIG_DRIVER_FAT_BLOCKDEVICE);
}

static void clear_ok_by_crc (void)
{
  extern unsigned long compute_crc32 (unsigned long, const void*, size_t);
  const char* sz = block_driver ();
  u32 crc = compute_crc32 (0, sz, strlen (sz));
  if (fat.region_crc != crc)
    fat.fOK = false;
}

#if 0
static void fat_init (void)
{
  ENTRY (0);
}
#endif

static void fat_report (void);


/* fat_identify

   performs an initial read on the fat device to get partition
   information.

*/

static int fat_identify (void)
{
  int result;
  char sz[128];
  struct descriptor_d d;

  snprintf (sz, sizeof (sz), "%s%%+1s", block_driver ());
  if (   (result = parse_descriptor (sz, &d))
      || (result = open_descriptor (&d)))
    return result;


  /* Check for signature */
  {
    unsigned char rgb[2];

    d.driver->seek (&d, SECTOR_SIZE - 2, SEEK_SET);
    if (d.driver->read (&d, &rgb, 2) != 2
	|| rgb[0] != 0x55
	|| rgb[1] != 0xaa) {
      close_descriptor (&d);
      return -1;
    }
  }

  d.driver->seek (&d, SECTOR_SIZE - 66, SEEK_SET);
  d.driver->read (&d, fat.partition, 16*4);

  close_descriptor (&d);

  return 0;
}


/* fat_next_cluster

   searches the FAT for the next cluster in the chain.

   *** This implementation works only for FAT12 and FAT16

   *** The cheat is there to save and restore the index pointer of the
   *** descriptor being used to read from the CF.  It isn't heinous,
   *** but it should be more above board.
*/

static unsigned fat_next_cluster (unsigned cluster)
{
  int sector = 0;

  if (fat.fat_type == fat12)
    sector = cluster/CLUSTERS_PER_3FAT_FAT12;
  if (fat.fat_type == fat16)
    sector = cluster/CLUSTERS_PER_3FAT_FAT16;

  //  PRINTF ("fnc: clus %x  sec %d", cluster, sector);
  sector -= sector%3;	      /* We only care about groups of three */
  sector += fat.parameter.reserved_sectors;
  //  PRINTF ("->sec %d", sector);

  if (sector != fat.sector_fat) {
    size_t index = fat.d.index;	/* *** FIXME: This is a cheat */
    fat.d.driver->seek (&fat.d, SECTOR_SIZE*sector, SEEK_SET);
    fat.d.driver->read (&fat.d, &fat.fat, SECTOR_SIZE*3);
    fat.sector_fat = sector;
    fat.d.index = index;	/* *** FIXME: This is a cheat */
  }

  if (fat.fat_type == fat12) {
    int index = cluster%CLUSTERS_PER_3FAT_FAT12;
    unsigned short v = read_short (fat.fat + index*3/2);
    //    PRINTF ("ind %x v %x\n", index, v);
    return (index & 1) ? ((v >> 4) & 0xfff): (v & 0xfff);
  }

  if (fat.fat_type == fat16) {
    //    PRINTF ("\n");
    return read_short (fat.fat + (cluster%CLUSTERS_PER_3FAT_FAT16)*2);
  }

  return 0;			/* an error, really */
}


/* fat_find

   is a good part of the meat of this driver.  It takes a descriptor,
   fat.d having been opened on the partition, and the parameter block
   read from the partition and it find the directory entry of the
   indicated file.  It returns an error code.

   The root directory is simple, a contiguous sequence of directory
   entries.  Subdirectories are themselves files, so the traversal is
   a little more complex.

   Storing of the file information is a side-effect of this call.  The
   returned cluster number is, really, just informative.

*/

static int fat_find (struct descriptor_d* d)
{
  int i;

	/* Start reading the root directory */
  fat.d.driver->seek (&fat.d,
		      (fat.parameter.sectors_per_fat*fat.parameter.fats
		       + fat.parameter.reserved_sectors)*SECTOR_SIZE,
		      SEEK_SET);
  for (i = 0; i < fat.parameter.root_entries; ++i) {
    char sz[12];
    int cb;
    int cbRead = fat.d.driver->read (&fat.d, &fat.file, sizeof (fat.file));
    if (cbRead != sizeof (fat.file))
      break;
    if (fat.file.attribute == 0xf) {	/* vfat entry */
      continue;
    }
    if (fat.file.cluster == 0
	|| fat.file.file[0] == 0
	|| fat.file.file[0] == 0xe5)
      continue;
    {
      int j, k = 0;
      for (j = 0; j < 11; ++j) {
	if (fat.file.file[j] != ' ') {
	  if (j == 8)
	    sz[k++] = '.';
	  sz[k++] = tolower (fat.file.file[j]);
	}
      }
      sz[k] = 0;
      cb = k + 1;
    }
#if defined (TALK)
    {
      unsigned cluster_next = fat_next_cluster (fat.file.cluster);
      PRINTF ("  (%12.12s) @ %5d  %8ld bytes  %d (0x%x)\n",
	      sz, fat.file.cluster, fat.file.length,
	      cluster_next, cluster_next);
    }
#endif
    if (strnicmp (d->pb[d->iRoot], sz, cb) == 0)
      return 0;
  }

  return ERROR_FILENOTFOUND;
}

static int fat_open (struct descriptor_d* d)
{
  int result = 0;
  char sz[128];

  ENTRY (0);

  fat.fOK = 0;
  fat.sector_fat = 0;

  if ((result = fat_identify ()))
    return result;
  fat.fOK = 1;

#if defined (TALK)
  PRINTF ("descript %d %d\n", d->c, d->iRoot);
  {
    int i;
    for (i = 0; i < d->c; ++i)
      PRINTF ("  %d: (%s)\n", i, d->pb[i]);
  }
#endif

		/* Read just the partition table */
  if (d->c == 0) {
    snprintf (sz, sizeof (sz), "%s%%@%d+%d",
	      block_driver (),
	      SECTOR_SIZE - 66, 16*4);
    d->length = 16*4;
  }
  else {
    int partition = 0;
    if (d->iRoot > 0 && d->c)
      partition = simple_strtoul (d->pb[0], NULL, 10) - 1;
    if (partition < 0 || partition > 3 || fat.partition[partition].length == 0)
      ERROR_RETURN (ERROR_BADPARTITION, "invalid partition");

    snprintf (sz, sizeof (sz), "%s%%@%lds+%lds",
	      block_driver (),
	      fat.partition[partition].start, fat.partition[partition].length);
  }

  PRINTF ("  opening %s\n", sz);

	/* Open descriptor for the partition */
  if (   (result = parse_descriptor (sz, &fat.d))
      || (result = open_descriptor (&fat.d)))
    return result;

	/* Read parameter block */
  fat.d.driver->seek (&fat.d, 0, SEEK_SET);
  fat.d.driver->read (&fat.d, &fat.parameter, sizeof (struct parameter));
  fat.index_cluster_2
    = (fat.parameter.reserved_sectors
       + fat.parameter.fats*fat.parameter.sectors_per_fat)
    *SECTOR_SIZE
    + fat.parameter.root_entries*32;
  fat.bytes_per_cluster = fat.parameter.sectors_per_cluster*SECTOR_SIZE;

	/* Decode FAT type */
  fat.fat_type = 0;
  if (memcmp (fat.parameter.type, "FAT12", 5) == 0)
    fat.fat_type = fat12;
  if (memcmp (fat.parameter.type, "FAT16", 5) == 0)
    fat.fat_type = fat16;

	/* Open file by finding directory entry and thus, the first cluster */
  if (d->c != 0) {
    result = fat_find (d);
    fat.cluster_file = fat.file.cluster;
    fat.index_cluster_file = 0;

    if (d->length == 0)		/* Defaulting to length of whole file */
      d->length = fat.file.length;

    if (d->start > fat.file.length)
      d->start = fat.file.length;
    if (d->start + d->length > fat.file.length)
      d->length = fat.file.length - d->start;
  }

  PRINTF ("%s: length %ld\n", __FUNCTION__, d->length);

  return result;
}

static void fat_close (struct descriptor_d* d)
{
  ENTRY (0);

  close_descriptor (&fat.d);
  close_helper (d);
}

static ssize_t fat_read (struct descriptor_d* d, void* pv, size_t cb)
{
  ssize_t cbRead = 0;

  ENTRY (0);

  while (cb) {
    size_t available = cb;
    size_t index = d->start + d->index;
    size_t remain = d->length - d->index;
    if (index < fat.index_cluster_file) {
      /* Rewinding not supported   */
      return -1;
    }

    /* Find next cluster */
    while (index >= fat.index_cluster_file + fat.bytes_per_cluster) {
      fat.cluster_file = fat_next_cluster (fat.cluster_file);
      if (fat.cluster_file == 0xffff)
	return -1;		/* An unrecoverable error */
      fat.index_cluster_file += fat.parameter.sectors_per_cluster*SECTOR_SIZE;
    }

    /* Bound within region */
    if (available > remain)
      available = remain;

    /* Bound within the cluster */
    if (index + available > fat.index_cluster_file + fat.bytes_per_cluster)
      available = fat.index_cluster_file + fat.bytes_per_cluster - index;

    /* Bound within exact extent of file */
    if (index + available > fat.file.length)
      available = fat.file.length - index;

#if 0
    PRINTF (" fat.idx_clus_2 %d (%x) "
	    " fat.clus_file %d "
	    " fat.by_p_clus %d\n"
	    " idx %d (%x) "
	    " fat.idx_clus_f %d (%x)\n",
	    fat.index_cluster_2, fat.index_cluster_2,
	    fat.cluster_file,
	    fat.bytes_per_cluster,
	    index, index,
	    fat.index_cluster_file, fat.index_cluster_file);
#endif

    fat.d.driver->seek (&fat.d,
			fat.index_cluster_2
			+ (fat.cluster_file - 2)*fat.bytes_per_cluster
			+ index - fat.index_cluster_file,
			SEEK_SET);

    {
      ssize_t cbThis = fat.d.driver->read (&fat.d, pv, available);
      if (cbThis <= 0)
	break;
      d->index += cbThis;
      cb -= cbThis;
      cbRead += cbThis;
      pv += cbThis;
    }
  }
  return cbRead;
}

#if defined (CONFIG_CMD_INFO) && 0

static int fat_info (struct descriptor_d* d)
{
  int result = 0;
  char sz[128];

  ENTRY (0);

  fat.fOK = 0;
  fat.sector_fat = 0;

  if ((result = fat_identify ()))
    return result;
  fat.fOK = 1;

#if defined (TALK)
  PRINTF ("descript %d %d\n", d->c, d->iRoot);
  {
    int i;
    for (i = 0; i < d->c; ++i)
      PRINTF ("  %d: (%s)\n", i, d->pb[i]);
  }
#endif

		/* Read just the partition table */
  if (d->c == 0) {
    snprintf (sz, sizeof (sz), "%s%%@%d+%d",
	      block_driver (),
	      SECTOR_SIZE - 66, 16*4);
    d->length = 16*4;
  }
  else {
    int partition = 0;
    if (d->iRoot > 0 && d->c)
      partition = simple_strtoul (d->pb[0], NULL, 10) - 1;
    if (partition < 0 || partition > 3 || fat.partition[partition].length == 0)
      ERROR_RETURN (ERROR_BADPARTITION, "invalid partition");

    snprintf (sz, sizeof (sz), "%s%%@%lds+%lds",
	      block_driver (),
	      fat.partition[partition].start, fat.partition[partition].length);
  }

  PRINTF ("  opening %s\n", sz);

	/* Open descriptor for the partition */
  if (   (result = parse_descriptor (sz, &fat.d))
      || (result = open_descriptor (&fat.d)))
    return result;

	/* Read parameter block */
  fat.d.driver->seek (&fat.d, 0, SEEK_SET);
  fat.d.driver->read (&fat.d, &fat.parameter, sizeof (struct parameter));
  fat.index_cluster_2
    = (fat.parameter.reserved_sectors
       + fat.parameter.fats*fat.parameter.sectors_per_fat)
    *SECTOR_SIZE
    + fat.parameter.root_entries*32;
  fat.bytes_per_cluster = fat.parameter.sectors_per_cluster*SECTOR_SIZE;

	/* Decode FAT type */
  fat.fat_type = 0;
  if (memcmp (fat.parameter.type, "FAT12", 5) == 0)
    fat.fat_type = fat12;
  if (memcmp (fat.parameter.type, "FAT16", 5) == 0)
    fat.fat_type = fat16;

	/* Open file by finding directory entry and thus, the first cluster */
  if (d->c != 0) {
    result = fat_find (d);
    fat.cluster_file = fat.file.cluster;
    fat.index_cluster_file = 0;

    if (d->length == 0)		/* Defaulting to length of whole file */
      d->length = fat.file.length;

    if (d->start > fat.file.length)
      d->start = fat.file.length;
    if (d->start + d->length > fat.file.length)
      d->length = fat.file.length - d->start;
  }

  /* *** Do we want to set the default length to be the whole file?  */

  return 0;
}

#endif

#if !defined (CONFIG_SMALL)

static void fat_report (void)
{
  int i;

  clear_ok_by_crc ();
  if (!fat.fOK) {
    if (fat_identify ())
      return;
    fat.fOK = 1;
  }

//  if (   fat.bootsector[SECTOR_SIZE - 2] == 0x55
//      && fat.bootsector[SECTOR_SIZE - 1] == 0xaa) {
  printf ("  fat:");

  for (i = 0; i < 4; ++i)
    if (fat.partition[i].type || i == 0) {
      if (i != 0)
	printf ("      ");
      printf ("    partition %d: %c %02x 0x%08lx 0x%08lx\n",
              i + 1,
	      fat.partition[i].boot ? '*' : ' ',
	      fat.partition[i].type,
	      fat.partition[i].start,
	      fat.partition[i].length);
    }

  printf ("          bps %d spc %d res %d fats %d re %d sec %d\n",
	  read_short (&fat.parameter.bytes_per_sector),
	  fat.parameter.sectors_per_cluster,
	  read_short (&fat.parameter.reserved_sectors),
	  fat.parameter.fats,
	  read_short (&fat.parameter.root_entries),
	  read_short  (&fat.parameter.small_sectors));
  printf ("          med 0x%x spf %d spt %d heads %d hidden %ld sec %ld\n",
	  fat.parameter.media,
	  read_short (&fat.parameter.sectors_per_fat),
	  read_short (&fat.parameter.sectors_per_track),
	  read_short (&fat.parameter.heads),
	  read_long (&fat.parameter.hidden_sectors),
	  read_long  (&fat.parameter.large_sectors));
  printf ("          log 0x%02x sig 0x%x serial %08lx\n"
	  "          vol '%11.11s' type '%8.8s' fat_type %d\n",
	  read_short (&fat.parameter.logical_drive),
	  fat.parameter.signature,
	  read_long (&fat.parameter.serial),
	  fat.parameter.volume,
	  fat.parameter.type,
	  fat.fat_type);

#if 0
    for (i = 0; i < SECTOR_SIZE/sizeof (struct directory); ++i) {
      if (fat.root[i].attribute == 0xf) {
	int j;
	printf ("%12.12s0x%x ", "", fat.root[i].file[0]);
	for (j = 0; j < 14; ++j) {
	  char ch = 0;
	  switch (j) {
	  case 0 ... 5:   ch = ((char*) &fat.root[i])[ 1 -  0 + j*2]; break;
	  case 6 ... 11:  ch = ((char*) &fat.root[i])[14 - 12 + j*2]; break;
	  case 12 ... 13: ch = ((char*) &fat.root[i])[28 - 24 + j*2]; break;
	  }
	  if (!ch)
	    break;
	  printf ("%c", ch);
	}
	printf ("\n");
      }
      else if (fat.root[i].cluster == 0)
	continue;
      else
	printf ("%12.12s%-11.11s 0x%x #%d %ld\n",
		"",
		fat.root[i].file,
		fat.root[i].attribute,
		fat.root[i].cluster,
		fat.root[i].length);
    }
#endif

}

#endif

static __driver_6 struct driver_d fat_driver = {
  .name = "fatfs",
  .description = "FAT filesystem driver",
  .flags = DRIVER_DESCRIP_FS,
  .open = fat_open,
  .close = fat_close,
  .read = fat_read,
//  .write = cf_write,
//  .erase = cf_erase,
  .seek = seek_helper,
#if defined (CONFIG_CMD_INFO) && 0
  .info = fat_info,
#endif
};

static __service_6 struct service_d fat_service = {
//  .init = fat_init,
#if !defined (CONFIG_SMALL)
  .report = fat_report,
#endif
};
