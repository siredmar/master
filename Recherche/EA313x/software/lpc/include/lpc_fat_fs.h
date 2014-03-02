/***********************************************************************
 * $Id:: lpc_fat_fs.h 745 2008-05-13 19:59:29Z pdurgesh                $
 *
 * Project: FAT filesystem definitions
 *
 * Description:
 *     This file contains the structure definitions and manifest
 *     constants for the following component:
 *         FAT filesystem (and partitioned storage media)
 *
 ***********************************************************************
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * NXP Semiconductors assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. NXP Semiconductors
 * reserves the right to make changes in the software without
 * notification. NXP Semiconductors also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
 **********************************************************************/

#ifndef LPC_FAT_FS_H
#define LPC_FAT_FS_H

#include "lpc_types.h"

#if defined (__cplusplus)
extern "C"
{
#endif


/***********************************************************************
 * Partition entry
 **********************************************************************/

/* This entry is used in the partition table (if it exists) to define
   the partition type and location on the storage media */
typedef struct
{
  UNS_8    state;          // State of the partition
  UNS_8    head_start;     // Sector start head
  UNS_16   cyl_sec_start;  // Partition cylinder/sector start
  UNS_8    partype;        // Partition type
  UNS_8    head_end;       // Sector start end
  UNS_16   cyl_sec_end;    // Partition cylinder/sector end
  UNS_32   mbr_sec_offset; // Offset from MBR to start of part.
  UNS_32   partsecs;       // Number of sectors in the partition
} LPC_PARTITION_T;

/***********************************************************************
 * Filesystem structures - these structures are not aligned in memory
 * to the organization on the storage media.
 **********************************************************************/

/* FAT directory structure root entry stored on device */
typedef struct
{
  CHAR     name [8];       /* Left space padded name */
  CHAR     ext [3];        /* Left space padded extension */
  UNS_8    attribute;      /* File attribute */
  UNS_8    reserved1;
  UNS_8    createtimems;   /* timestamp in 10mS */
  UNS_16   createtime;     /* timestamp, time */
  UNS_16   createdate;     /* timestamp, date */
  UNS_16   accessdate;     /* Last date of access */
  UNS_16   clusterhi;      /* High cluster (FAT32 only) */
  UNS_16   updatetime;     /* Last time of change */
  UNS_16   updatedate;     /* Last date of change */
  UNS_16   clusternum;     /* Cluster link number */
  UNS_32   filesize;       /* Size of file in bytes */
} FAT_ROOT_ENTRY_T;

/* Drive geometry structure for partition - this information is
   stored in the first sector on every partition. Not all the
   information in that sector is shown here */
typedef struct
{
  UNS_8    jump [3];       /* Boot code jump point */
  UNS_8    oem_id [8];     /* Name of formatting OS */
  UNS_16   bytes_sector;   /* Bytes per sector */
  UNS_8    sectors_cluster;/* Sectors per cluster */
  UNS_16   res_sectors;    /* Reserved sectors from start */
  UNS_8    fat_copies;     /* Number of FAT copies */
  UNS_16   root_entries;   /* Number of root entries */
  UNS_16   small_sectors;  /* Small number of sectors */
  UNS_8    media_desc;     /* Media descriptor */
  UNS_16   sectors_fat;    /* Sectors per FAT */
  UNS_16   sectors_track;  /* Sectors per track */
  UNS_16   number_heads;   /* Number of heads */
  UNS_32   hidden_sectors; /* Number of hidden sectors */
  UNS_32   large_sectors;  /* Large number of sectors */
  UNS_8    drive_number;   /* Drive number */
  UNS_8    reserved;
  UNS_8    ext_boot_sig;   /* Extended boot signature */
  UNS_32   serial_number;  /* Volume serial number */
  CHAR     label [11];     /* Volume label */
  CHAR     fs_name [8];    /* File system name (FAT16) */
} FAT_MEDIAGEOM_T;

/***********************************************************************
 * Miscellaneous defines
 **********************************************************************/

/* Storage media partition ID tags */
#define FAT12              0x01  /* Partition type FAT12 */
#define PART_TYPE_FAT12    0x01  /* FAT12 partition flag */
#define PART_TYPE_FAT16L   0x04  /* FAT12 < 32M partition flag */
#define PART_TYPE_EXT      0x05  /* Extended MS-DOS FAT partition */
#define PART_TYPE_FAT16G   0x06  /* FAT16 > 32M partition flag */
#define PART_TYPE_FAT32    0x0B  /* FAT32 partition flag */
#define PATY_TYPE_FAT32LBA 0x0C  /* FAT32 with LBA extensions */
#define PATY_TYPE_FAT16LBA 0x0E  /* FAT16 with LBA extensions */
#define PATY_TYPE_EXTLBA   0x0F  /* Extended with LBA extensions */
#define PART_ACTV          0x80  /* Partition active flag bit */

/* Various offsets */
#define PART_TYPE_OFFSET   450   /* Part table offset to part type */
#define PART_LGSPART_OFF   454   /* Sector offset to first partition */
#define BOOTSEC_BPS_LOW    0x0B  /* Bytes per sector low offset */
#define BOOTSEC_BPS_HIGH   0x0C  /* Bytes per sector high offset */
#define BOOTSEC_EXMKR_LOW  0x1FF /* Executable marker low offset */
#define BOOTSEC_EXMKR_HIGH 0x1FE /* Executable marker high offset */

/* Cluster flags (FAT16 system) */
#define CLUSTER_AV         0x0000 /* Cluster available */
#define CLUSTERU_MIN       0x0002 /* Min cluster chain range */
#define CLUSTERU_MAX       0xFFEF /* Max cluster chain range */
#define CLUSTERR_MIN       0xFFF0 /* Min reserved cluster flag */
#define CLUSTERR_MAX       0xFFF6 /* Max reserved cluster flag */
#define CLUSTER_BAD        0xFFF7 /* Bad cluster flag */
#define CLUSTER_LAST       0xFFF8 /* Min value for last cluster */
#define CLUSTER_MAX        0xFFFF /* Max amount of cluster entries */

/* FAT Directory structure/file attributes */
#define ATTB_NORMAL        0x00  /* Normal file type (no bits set) */
#define ATTB_RO            0x01  /* Read only bit */
#define ATTB_SYS           0x02  /* System file bit */
#define ATTB_HIDDEN        0x04  /* Hidden file bit */
#define ATTB_VOLUME        0x08  /* Volume bit */
#define ATTB_DIR           0x10  /* Directory bit */
#define ATTB_ARCHIVE       0x20  /* Archive bit */
#define ATTB_LFN           0x0F  /* LFN entry flag */
#define DIR_FREE           0x00  /* Free directory entry */
#define DIR_ERASED         0xE5  /* Erased (free) directory entry */

/* Default FAT creation time/date for files */
#define DEFAULT_CR_TIME    0xC000 /* 12:00:00 FAT file creation time */
#define DEFAULT_CR_DATE    0x2C21 /* January 1, 2002 FAT file time */

/* FAT extended signature index and tag values */
#define EXTENDED_SIG       0x29  /* FAT boot sector extended media */
#define EXTENDED_SIG_IDX   0x26  /* Extended signature index in data */

#if defined (__cplusplus)
}
#endif /*__cplusplus */

#endif /* LPC_FAT_FS_H */
