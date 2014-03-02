/***********************************************************************
 * $Id:: s1l_fat.c 4977 2010-09-20 22:31:34Z usb10132                  $
 *
 * Project: FAT16/32 boot loader
 *
 * Description:
 *     Supports chain loading from FAT16 and FAT32 devices
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

#include "lpc_fat_fs.h"
#include "lpc_string.h"
#include "s1l_sys_inf.h"
#include "lpc_line_parser.h"
#include "s1l_fat.h"

/***********************************************************************
 * Local data and types
 **********************************************************************/

/* FAT geometry structure - computed values from the partition boot
   record */
typedef struct
{
    /* These values are obtained from the partition boot record for
       FAT16 and FAT32 devices */
    UNS_32 bytes_sec;        /* Bytes per sector */
    UNS_32 secs_cluster;     /* Sectors per cluster */
    UNS_32 reserved_secs;    /* Reserved sectors before first FAT */
    UNS_32 number_fats;      /* Number of FATs */
    UNS_32 max_roots;        /* Maximum root entries */
    UNS_32 tsecs_small;      /* Total # of sectors, small FAT16 */
    UNS_32 sectors_fat16;    /* Sectors per FAT (FAT16 only) */
    UNS_32 sectors_track;    /* Sectors per track */
    UNS_32 heads_sides;      /* Total # of heads or sides */
    UNS_32 hidden_sectors;   /* Hidden sectors before the boot record
                                sector */
    UNS_32 tsecs_large;      /* Total # of sectors for large FAT16
                                and FAT 32 devices */

    /* These values are obtained from the partition boot record for
       FAT32 devices only */
    UNS_32 sectors_fat32;    /* Sectors for FAT table (FAT32 only) */
    UNS_32 active_fat32;     /* Active FAT32 table index */
    UNS_32 rdir_clus_fat32;  /* FAT32 root directory cluster */

    /* These values are computed from the obtained partition boot
       record values */
    BOOL_32 fat32_fs;        /* FAT32 filesystem flag */
    UNS_32 boot_part_off;    /* Device boot record sector offset */
    UNS_32 fat_st_sector;    /* First FAT sector offset */
    UNS_32 root_st16_sector; /* First root sector, FAT16 only */
    UNS_32 root_sectors;     /* Total # of root sectors */
    UNS_32 fat_sectors;      /* Total # of sectors in all FATs */
    UNS_32 data_st_sector;   /* First data sector offset */
    UNS_32 total_sectors;    /* Total # of sectors on the partition */
    UNS_32 data_sectors;     /* Total # of data sectors */
    UNS_32 cluster_size;     /* Size of a cluster in bytes */
} FAT_GEOM_T;

/* Cached sector stream work structure */
typedef struct 
{
    UNS_32 next_cluster;    /* For the read pointer */
    INT_32 secindex;
    INT_32 cachedbytes;
    INT_32 clus_index;
    UNS_32 tread;
    UNS_8  secdata [512];
} FAT_BUFF_T;

/* FAT device structure, used to bind a device driver to the FAT
   driver */
typedef struct
{
    bvfunc init_func;        /* Address of device init function */
    bvfunc dinit_func;       /* Address of device de-init function */
    bviifunc read_func;      /* Address of device read function */
    FAT_GEOM_T fgeom;        /* FAT device geometry */
    UNS_32 dir_sector;       /* File directory search sector */
    UNS_32 dir_cluster;      /* Current directory cluster (FAT32) */
    INT_32 dir_index;        /* Directory sector/cluter index */
    UNS_32 dir_dat[128];     /* Cached directory sector */
    UNS_32 dir_sub_sector;   /* Sector index for a directory cluster */
    UNS_32 filesize;         /* Size of the file in bytes */
    UNS_32 clusec_cached;    /* Cached cluster sector */
    UNS_32 clus_dat[128];    /* Cached cluster sector */
    BOOL_32 cluster_last;    /* Cluster last flag */
    FAT_BUFF_T fbuff;
} FAT_DATA_T;

/* Directory entry structure */
typedef struct
{
    CHAR     name [11];      /* Space padded name */
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
} DIR_ENTRY_T;

static FAT_DATA_T fat_data;

/***********************************************************************
 * Private functions
 **********************************************************************/

/***********************************************************************
 *
 * Function: fat_bmove
 *
 * Purpose: Simple byte movement function
 *
 * Processing:
 *     Move a number of bytes from the source to destination.
 *
 * Parameters:
 *     source : Source address
 *     dest   : Destination address
 *     size   : Number of bytes to move
 *
 * Outputs: Data pointed to by source will be updated.
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void fat_bmove(void *source,
               void *dest,
               INT_32 size)
{
    UNS_8 *cdest, *csource;

    cdest = (UNS_8 *) dest;
    csource = (UNS_8 *) source;

    while (size > 0)
    {
        *cdest = *csource;
        cdest++;
        csource++;
        size--;
    }
}

/***********************************************************************
 *
 * Function: fat_check_part
 *
 * Purpose: Determine if the passed data is a partition table
 *
 * Processing:
 *     For the buffer in the passed structure, check the partition
 *     type flag for a valid partition type. If the partition is a
 *     valid type, save the computed offset to the partition and return
 *     TRUE to the caller. Otherwise, return FALSE to the caller.
 *
 * Parameters:
 *     buff_ptr : Pointer to (expected) partition table record
 *
 * Outputs:
 *     fat_data.fgeom.boot_part_off may be updated with the parition
 *     boot record sector offset
 *
 * Returns: TRUE if the partition table was valid, otherwise FALSE
 *
 * Notes: None
 *
 **********************************************************************/
BOOL_32 fat_check_part(UNS_8 *buff_ptr)
{
    BOOL_32 status = FALSE;

    /* Check for valid parition flags */
    switch (buff_ptr[PART_TYPE_OFFSET])
    {
        case PART_TYPE_FAT16L:
        case PART_TYPE_FAT16G:
        case PART_TYPE_FAT32:
        case PATY_TYPE_FAT32LBA:
            /* Good partition, compute sector offset to the start
               of the partition */
            fat_data.fgeom.boot_part_off =
                (UNS_32) buff_ptr[PART_LGSPART_OFF] +
                ((UNS_32) buff_ptr[PART_LGSPART_OFF + 1] << 8) +
                ((UNS_32) buff_ptr[PART_LGSPART_OFF + 2] << 16) +
                ((UNS_32) buff_ptr[PART_LGSPART_OFF + 3] << 24);
            status = TRUE;
            break;

        default:
            /* Do nothing */
            break;
    }

    return status;
}

/***********************************************************************
 *
 * Function: fat_check_boot
 *
 * Purpose:
 *     Determine if the passed sector data is a boot record sector
 *
 * Processing:
 *     For the buffer in the passed structure, check the sector size
 *     and the executable end marker value. If they are correct, return
 *     0 to the caller and extract/compute the device geometry data.
 *     Otherwise, return -1 to the caller.
 *
 * Parameters:
 *     buff_ptr : Pointer to (expected) master boot record
 *
 * Outputs: None
 *
 * Returns: TRUE if the boot record was valid, otherwise FALSE
 *
 * Notes: None
 *
 **********************************************************************/
BOOL_32 fat_check_boot(UNS_8 *buff_ptr)
{
    UNS_8 byte;
    UNS_16 word;
    BOOL_32 status = FALSE;

    /* Before tagging this as the boot sector, perform a sanity check
       on a few values to make sure they are ok */
    if ((buff_ptr[BOOTSEC_BPS_LOW] == 0x00) &&
        (buff_ptr[BOOTSEC_BPS_HIGH] == 0x02) &&
        (buff_ptr[BOOTSEC_EXMKR_LOW] == 0xAA) &&
        (buff_ptr[BOOTSEC_EXMKR_HIGH] == 0x55))
    {
        /* Extract the important information about the boot record
           into the device geometry structures */

        /* Assume default filesystem is FAT16 */
        fat_data.fgeom.fat32_fs = FALSE;

        /* Get the Bytes per sector */
        fat_bmove(&buff_ptr[0x0B], &word, 2);
        fat_data.fgeom.bytes_sec = (UNS_32) word;

        /* Get the Sectors per cluster */
        fat_bmove(&buff_ptr[0x0D], &byte, 1);
        fat_data.fgeom.secs_cluster = (UNS_32) byte;

        /* Get the Reserved sectors before first FAT */
        fat_bmove(&buff_ptr[0x0E], &word, 2);
        fat_data.fgeom.reserved_secs = (UNS_32) word;

        /* Get the Number of FATs */
        fat_bmove(&buff_ptr[0x10], &byte, 1);
        fat_data.fgeom.number_fats = (UNS_32) byte;

        /* Get the Maximum root entries */
        fat_bmove(&buff_ptr[0x11], &word, 2);
        fat_data.fgeom.max_roots = (UNS_32) word;

        /* Get the Total # of sectors, small FAT16 */
        fat_bmove(&buff_ptr[0x13], &word, 2);
        fat_data.fgeom.tsecs_small = (UNS_32) word;

        /* Get the Sectors per FAT (FAT16 only) */
        fat_bmove(&buff_ptr[0x16], &word, 2);
        fat_data.fgeom.sectors_fat16 = (UNS_32) word;

        /* Get the Sectors per track */
        fat_bmove(&buff_ptr[0x18], &word, 2);
        fat_data.fgeom.sectors_track = (UNS_32) word;

        /* Get the Total # of heads or sides */
        fat_bmove(&buff_ptr[0x1A], &word, 2);
        fat_data.fgeom.heads_sides = (UNS_32) word;

        /* Get the Hidden sectors */
        fat_bmove(&buff_ptr[0x1C],
            &fat_data.fgeom.hidden_sectors , 4);

        /* Get the Total # of sectors for large devices */
        fat_bmove(&buff_ptr[0x20],
            &fat_data.fgeom.tsecs_large , 4);

        /* Get the Sectors for FAT table (FAT32 only) */
        fat_bmove(&buff_ptr[0x24],
            &fat_data.fgeom.sectors_fat32 , 4);

        /* Active FAT32 table index */
        fat_bmove(&buff_ptr[0x28], &word, 2);
        fat_data.fgeom.active_fat32 = (UNS_32) word;

        /* Get the FAT32 root directory cluster */
        fat_bmove(&buff_ptr[0x2C],
            &fat_data.fgeom.rdir_clus_fat32 , 4);

        /* From the extracted boot record data, determine how the
           device is organized */

        /* Compute cluster size in bytes */
        fat_data.fgeom.cluster_size = fat_data.fgeom.bytes_sec *
            fat_data.fgeom.secs_cluster;

        /* Examine the tsecs_small and sectors_fat16 fields - if they
           are both 0, then the filesystem is FAT32 */
        if ((fat_data.fgeom.tsecs_small +
            fat_data.fgeom.sectors_fat16) == 0)
        {
            fat_data.fgeom.fat32_fs = TRUE;
            fat_data.fgeom.fat_sectors =
                (fat_data.fgeom.sectors_fat32 *
                fat_data.fgeom.number_fats);

            /* Determine which FAT32 table is active */
            if ((fat_data.fgeom.active_fat32 & 0x80) != 0)
            {
                /* All are active, so just use table 0 */
                fat_data.fgeom.active_fat32 = 0;
            }

            /* Compute first sector of FAT table */
            fat_data.fgeom.fat_st_sector =
                fat_data.fgeom.reserved_secs +
                (fat_data.fgeom.active_fat32 *
                fat_data.fgeom.fat_sectors);

            /* No root directory structure in FAT32 */
            fat_data.fgeom.root_st16_sector = 0;
            fat_data.fgeom.root_sectors = 0;

            /* Compute start of data sectors (not including the
               boot record offset from the partition table) */
            fat_data.fgeom.data_st_sector = 
                fat_data.fgeom.fat_st_sector +
                fat_data.fgeom.fat_sectors;
        }
        else
        {
            fat_data.fgeom.fat_sectors =
                (fat_data.fgeom.sectors_fat16 *
                fat_data.fgeom.number_fats);

            /* Compute first sector FAT table (not including the
               boot record offset from the partition table) */
            fat_data.fgeom.fat_st_sector =
                fat_data.fgeom.reserved_secs;

            /* Compute offset to the root directory structure (not
               including the boot record offset from the partition
               table) */
            fat_data.fgeom.root_st16_sector =
                fat_data.fgeom.fat_st_sector +
                fat_data.fgeom.fat_sectors;

            /* Compute number of root directory sector */
            fat_data.fgeom.root_sectors = (32 *
                fat_data.fgeom.max_roots) / 512;

            /* Compute start of data sectors (not including the
               boot record offset from the partition table) */
            fat_data.fgeom.data_st_sector = 
                fat_data.fgeom.root_st16_sector +
                fat_data.fgeom.root_sectors;
        }

        /* Compute Total number of sectors */
        fat_data.fgeom.total_sectors = fat_data.fgeom.tsecs_small;
        if (fat_data.fgeom.total_sectors == 0)
        {
            fat_data.fgeom.total_sectors = fat_data.fgeom.tsecs_large;
        }

        /* Compute total number of data sectors */
        fat_data.fgeom.data_sectors = fat_data.fgeom.total_sectors -
            fat_data.fgeom.data_st_sector;

        status = TRUE;
    }

    return status;
}

/***********************************************************************
 *
 * Function: fat_device_init
 *
 * Purpose: Initialize device and fetch boot record
 *
 * Processing:
 *     Call the device specific initialization function. Read sector
 *     0 and check to see if it is the boot sector. If it isn't, check
 *     to see if it is a partition sector. If it is, read the sector
 *     offset by the partition table and check it as the boot sector.
 *     If the sector is a valid boot sector, return TRUE to the caller.
 *     Otherwise, return FALSE to the caller.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: TRUE if the device was intialized, otherwise FALSE
 *
 * Notes: None
 *
 **********************************************************************/
BOOL_32 fat_device_init(void)
{
    UNS_8 boot_rec[512];

    /* Try to initialize the device first */
    if (fat_data.init_func() == FALSE)
    {
        /* Cannot init, exit */
        return FALSE;
    }

    /* Set default sector offset for parition boot record to 0 */
    fat_data.fgeom.boot_part_off = 0;

    /* Read sector 0, this sector may be a partition boot record or
       a device master boot record */
    if (fat_data.read_func(&boot_rec[0], 0) == FALSE)
    {
        /* Cannot read sector, exit */
        return FALSE;
    }

    /* Determine if this is a partition boot record */
    if (fat_check_boot(&boot_rec[0]) == FALSE)
    {
        /* This may be a partition table, test it for a partition
           data entry and try to determine where the partition boot
           record is located */
        if (fat_check_part(&boot_rec[0]) == TRUE)
        {
            /* Yes, this was a partition entry, use the computed
               parition offset to try to read the partition boot
               record again */
            if (fat_data.read_func(&boot_rec[0],
                fat_data.fgeom.boot_part_off) == FALSE)
            {
                /* Sector not readable */
                return FALSE;
            }
            else if (fat_check_boot(&boot_rec[0]) == FALSE)
            {
                /* Device has no discernable FAT structure */
                return FALSE;
            }
        }
        else
        {
            /* This is not a partition entry */
            return FALSE;
        }
    }

    return TRUE;
}

/***********************************************************************
 *
 * Function: name_check
 *
 * Purpose: Simple data comparison routine
 *
 * Processing:
 *     Two strings are compared in lowercase up to the number of
 *     characters set by 'size'.
 *
 * Parameters:
 *     source : Source name address
 *     dest   : Destination name address
 *     size   : Number of characters to compare
 *
 * Outputs: None
 *
 * Returns: TRUE if the strings are the same, otherwise FALSE
 *
 * Notes: None
 *
 **********************************************************************/
BOOL_32 name_check(CHAR *source,
                   CHAR *dest,
                   INT_32 size)
{
    BOOL_32 same = FALSE;
    INT_32 index;
    INT_32 match = -1;
    CHAR sr, ds;

    index = 0;
    while (match == -1)
    {
        sr = source [index];
        ds = dest [index];

        /* Convert to lowercase for the comparison */
        if ((sr >= 'A') && (sr <= 'Z'))
        {
            sr = sr - 'A' + 'a';
        }
        if ((ds >= 'A') && (ds <= 'Z'))
        {
            ds = ds - 'A' + 'a';
        }

        if (sr == ds)
        {
            /* Look for string end */
            if (index == (size - 1))
            {
                /* Comparison matches */
                match = 1;
                same = TRUE;
            }
            else
            {
                index++;
            }
        }
        else
        {
            /* No match */
            match = 0;
        }
    }

    return same;
}

/***********************************************************************
 *
 * Function: get_next_cluster
 *
 * Purpose: Return the next cluster number on a cluster chain
 *
 * Processing:
 *     Get the next cluster in the cluster chain. See function.
 *
 * Parameters:
 *     cluster   : Current cluster to link against
 *
 * Outputs: None
 *
 * Returns: The next cluster in the chain, or 0xFFFFFFFE on an error
 *
 * Notes: None
 *
 **********************************************************************/
UNS_32 get_next_cluster(UNS_32 cluster)
{
    UNS_16 *clu16;
    UNS_32 secoff, csize, *clu32, next_cluster = 0xFFFFFFFE;

    /* Determine if the cluster sector has been cached already */
    /* Determine a FAT entry size in the cluster */
    if (fat_data.fgeom.fat32_fs == TRUE)
    {
        /* Each FAT entry in a FAT32 system is 4 bytes */
        csize = sizeof (UNS_32);
    }
    else
    {
        /* Each FAT entry in a FAT16 system is 2 bytes */
        csize = sizeof (UNS_16);
    }

    /* Determine the sector offset to search for the cluster */
    secoff = (cluster * csize) / 512;
    secoff = secoff + fat_data.fgeom.fat_st_sector +
        fat_data.fgeom.boot_part_off;
    if (fat_data.clusec_cached != secoff)
    {
        /* Sector to determine the next cluster is not cached yet,
           so read it in */
        if (fat_data.read_func(&fat_data.clus_dat[0], secoff) ==
            FALSE)
        {
            return FALSE;
        }

        fat_data.clusec_cached = secoff;
    }

    /* The cluster index contains an absolute index into the cluster
       array that points to the next cluster to load. Convert this
       cluster index into a sector specific index */
    secoff = cluster / (512 / csize);
    cluster = cluster - (secoff * (512 / csize));

    /* Get next cluster */
    if (fat_data.fgeom.fat32_fs == TRUE)
    {
        clu32 = (UNS_32 *) &fat_data.clus_dat[0];
        next_cluster = clu32[cluster];
    }
    else
    {
        clu16 = (UNS_16 *) &fat_data.clus_dat[0];
        next_cluster = (UNS_32) clu16[cluster];
        if (next_cluster == 0xFFFF)
        {
            /* Change a smaller FAT16 tag to a usable tag */
            next_cluster = 0xFFFFFFFF;
        }
    }

    return next_cluster;
}

/***********************************************************************
 *
 * Function: get_next_f32_dir
 *
 * Purpose: Get the next directory entry in the current FAT32 DIR
 *
 * Processing:
 *     Read the next FAT32 directory entry. This may involve reading
 *     in sectors for a new cluster. See function.
 *
 * Parameters:
 *     dir_entry : Address of where to return a pointer to the entry
 *
 * Outputs: None
 *
 * Returns:
 *     TRUE if the directory entry was the last entry in the list,
 *     otherwise FALSE
 *
 * Notes: None
 *
 **********************************************************************/
BOOL_32 get_next_f32_dir(DIR_ENTRY_T **dir_entry)
{
    UNS_8 *dir_lin;

    /* Determine if the next cluster has to be started */
    if (fat_data.dir_cluster != fat_data.clusec_cached)
    {
        /* A new cluster needs to be started, reset directory index
           to 0, sector subindex to 0, and directory sector index to
           start of directory cluster sector */
        fat_data.dir_cluster = fat_data.clusec_cached;
        fat_data.dir_index = 0;
        fat_data.dir_sub_sector = 0;
        fat_data.dir_sector = fat_data.fgeom.data_st_sector +
            fat_data.fgeom.boot_part_off + ((fat_data.dir_cluster -
            CLUSTERU_MIN) * fat_data.fgeom.secs_cluster);
    }

    /* If the next directory index exceeds the amount of entries
       in the directory, then increment the sub sector index and try
       the next sector */
    if (fat_data.dir_index >= (512 / sizeof (DIR_ENTRY_T)))
    {
        fat_data.dir_sub_sector++;
        fat_data.dir_sector++;
        fat_data.dir_index = 0;
        if (fat_data.dir_sub_sector >= fat_data.fgeom.secs_cluster)
        {
            /* The next cluster needs to be read in */
            fat_data.clusec_cached = get_next_cluster(
                fat_data.dir_cluster);
            return get_next_f32_dir(dir_entry);
        }
    }
    else
    {
        /* If this is the first entry in the directory, the directory
           sector needs to be read first */
        if (fat_data.dir_index == 0)
        {
            /* Read directory sector */
            if (fat_data.read_func(&fat_data.dir_dat[0],
                fat_data.dir_sector) == FALSE)
            {
                /* Can't read sector, return an error */
                return TRUE;
            }
        }

        dir_lin = (UNS_8 *) &fat_data.dir_dat[0];
        *dir_entry = (DIR_ENTRY_T *)
            &dir_lin[fat_data.dir_index *
            sizeof (DIR_ENTRY_T)];
        fat_data.dir_index++;
    }

    return FALSE;
}

/***********************************************************************
 *
 * Function: get_next_dir
 *
 * Purpose: Get the next directory entry in the current DIR
 *
 * Processing:
 *     Get the next directory entry from the root directory area of
 *     a FAT16 parition. See function.
 *
 * Parameters:
 *     dir_entry : Address of where to return a pointer to the entry
 *
 * Outputs: None
 *
 * Returns:
 *     TRUE if the directory entry was the last entry in the list,
 *     otherwise FALSE
 *
 * Notes: None
 *
 **********************************************************************/
BOOL_32 get_next_dir(DIR_ENTRY_T **dir_entry)
{
    INT_32 next_dir, tdir;
    UNS_8 *dir_lin;

    /* If this is the start of a directory search, then load the
       first directory sector */
    if (fat_data.dir_index == -1)
    {
        /* Clear FAT16 and FAT32 sector load indicators */
        fat_data.dir_sector = 0xFFFFFFFF;

        /* FAT32 systems start the search at the first DIR cluster */
        fat_data.dir_cluster = 0xFFFFFFFF;
        fat_data.clusec_cached = fat_data.fgeom.rdir_clus_fat32;
        fat_data.dir_sub_sector = 0;

        /* Set current directory index */
        fat_data.dir_index = 0;
    }

    /* Is this a FAT16 search? */
    if (fat_data.fgeom.fat32_fs == FALSE)
    {
        /* Are there any more entries left? */
        if (fat_data.dir_index >= 512)
        {
            /* No more, return TRUE */
            return TRUE;
        }
        else
        {
            /* Compute the needed directory sector that has to be read
               in for the next directory entry */
            tdir = (fat_data.dir_index * sizeof (DIR_ENTRY_T) /
                512);
            next_dir = tdir + fat_data.fgeom.boot_part_off +
                fat_data.fgeom.root_st16_sector;
            if (next_dir != fat_data.dir_sector)
            {
                /* Read in the next directory sector */
                fat_data.dir_sector = next_dir;
                if (fat_data.read_func(&fat_data.dir_dat[0],
                    fat_data.dir_sector) == FALSE)
                {
                    /* Error reading sector, return TRUE to quit */
                    return TRUE;
                }
            }

            /* Get index into this directory structure and
               return pointer to DIR entry */
            next_dir = fat_data.dir_index - (tdir *
                (512 / sizeof (DIR_ENTRY_T)));
            dir_lin = (UNS_8 *) &fat_data.dir_dat[0];
            *dir_entry = (DIR_ENTRY_T *)
                &dir_lin[next_dir * sizeof (DIR_ENTRY_T)];
        }

        /* Increment file index for next search */
        fat_data.dir_index++;
    }
    else
    {
        /* A FAT32 search requires traversal through the FAT clusters
           to get directory entries - a single FAT32 directory may
           have links all over the place */
        return get_next_f32_dir(dir_entry);
    }

    return FALSE;
}

/***********************************************************************
 *
 * Function: find_first_cluster
 *
 * Purpose: Finds the starting cluster for a file
 *
 * Processing:
 *     For the passed filename, locate the file in the directory and
 *     determine the first cluster number for the file.
 *
 * Parameters:
 *     name      : Pointer to 8.3 filename (with padded spaces)
 *
 * Outputs: None
 *
 * Returns:
 *     The starting cluster of the file, or 0xFFFFFFFF if the file
 *     doesn't exist
 *
 * Notes: None
 *
 **********************************************************************/
UNS_32 find_first_cluster(CHAR *name)
{
    DIR_ENTRY_T *dir_entry;
    UNS_32 maxr, first_cluster = 0xFFFFFFFF;
    BOOL_32 last_file = FALSE;

    /* Search through the directory for the directory entry matching
       the filename */
    fat_data.dir_index = -1;
    maxr = 0;
    while ((last_file == FALSE) && (maxr < 512))
    {
        last_file = get_next_dir(&dir_entry);
        if (last_file == FALSE)
        {
            /* Compare names to see if they match */
            if (name_check(name, &dir_entry->name[0], 11) == TRUE)
            {
                /* Name matches, set loop exit flag */
                last_file = TRUE;

                /* Save cluster number */
                first_cluster = (UNS_32) dir_entry->clusternum;

                /* Save the high part of the starting cluster for this
                   file if the file system is FAT32 */
                if (fat_data.fgeom.fat32_fs == TRUE)
                {
                    first_cluster = first_cluster |
                        ((UNS_32) dir_entry->clusterhi) << 16;
                }

                /* Save the filesize */
                fat_data.filesize = dir_entry->filesize;
            }
        }
        
        maxr++;
    }

    return first_cluster;
}

#if 0
/***********************************************************************
 *
 * Function: file_track
 *
 * Purpose: Load a chained FAT entry's data to memory
 *
 * Processing:
 *     Following a file's cluster chain, read in the file data into
 *     the passed memory location.
 *
 * Parameters:
 *     fcluster  : First cluster in cluster chain
 *     addr      : Address of where to place file data
 *
 * Outputs: None
 *
 * Returns: TRUE if the file was transferred, otherwise FALSE
 *
 * Notes: None
 *
 **********************************************************************/
BOOL_32 file_track(UNS_32 fcluster,
                   void *addr)
{
    INT_32 idx;
    UNS_32 sector, *saddr = (UNS_32 *) addr;
    UNS_8 dump [512];

    /* Initial cluster data is not cached */
    fat_data.clusec_cached = 0;

    /* Loop until all data is read or until an error occurs */
    while (fat_data.filesize > 0)
    {
        /* Get the starting sector for the current cluster number */
        sector = fat_data.fgeom.data_st_sector +
            fat_data.fgeom.boot_part_off + ((fcluster - CLUSTERU_MIN) *
            fat_data.fgeom.secs_cluster);

        /* Read the cluster's sectors into memory */
        for (idx = 0; idx < fat_data.fgeom.secs_cluster; idx++)
        {
            /* If the size of the sector is greater than what's left
               to read in the file, read the sector into a temporary
               buffer instead and move just what's needed of the
               sector data into memory */
            if (fat_data.filesize < 512)
            {
                if (fat_data.read_func(&dump[0], sector) == FALSE)
                {
                    return FALSE;
                }
                else
                {
                    /* Copy just whats left and exit */
                    fat_bmove(&dump[0], saddr, fat_data.filesize);
                    idx = fat_data.fgeom.secs_cluster;
                    fat_data.filesize = 0;
                }
            }
            else
            {
                /* Read next sector in the cluster into memory */
                if (fat_data.read_func(saddr, sector) == FALSE)
                {
                    return FALSE;
                }
                else
                {
                    /* Increment load address for next sector */
                    saddr = saddr + (512 / 4);
                    sector++;
                    fat_data.filesize = fat_data.filesize - 512;
                }
            }
        }

        /* Was the last cluster read? */
        if (fat_data.cluster_last == TRUE)
        {
            /* If there is any file info left to read, return an error
               as something went wrong */
            if (fat_data.filesize != 0)
            {
                /* Directory file size doesn't match cluster link */
                return FALSE;
            }
            else
            {
                /* Exit with a good status */
                return TRUE;
            }
        }
        else
        {
            /* Get next cluster in the chain */
            fcluster = get_next_cluster(fcluster);
            if (fcluster == 0xFFFFFFFF)
            {
                /* This is the last cluster */
                fat_data.cluster_last = TRUE;
            }
            else if (fcluster == 0xFFFFFFFE)
            {
                /* Error */
                return FALSE;
            }
        }
    }

    return TRUE;
}
#endif

/***********************************************************************
 *
 * Function: fat_data_cache_read
 *
 * Purpose: Cache and read data
 *
 * Processing:
 *     Initialize device and filesystem.
 *
 * Parameters:
 *     buff  : Pointer to data area
 *     bytes : Number of bytes to read
 *
 * Outputs: None
 *
 * Returns: Number of bytes read
 *
 * Notes: None
 *
 **********************************************************************/
INT_32 fat_data_cache_read(UNS_8 *buff,
                           INT_32 bytes) 
{
	INT_32 bread = 0;

    INT_32 idx, toread;
    UNS_32 sector;

	while ((bytes > 0) && (fat_data.filesize > fat_data.fbuff.tread))
	{
		/* Is there any data in the cached sector field? */
		if (fat_data.fbuff.cachedbytes == 0) 
		{
			/* Data data, need to read a sector into cache */
	        /* Get the starting sector for the current cluster number */
	        sector = fat_data.fgeom.data_st_sector +
    	        fat_data.fgeom.boot_part_off +
        	    ((fat_data.fbuff.next_cluster - CLUSTERU_MIN) *
            	fat_data.fgeom.secs_cluster);
            sector += fat_data.fbuff.clus_index;
	        if (fat_data.read_func(fat_data.fbuff.secdata,
	        	sector) == FALSE)
	        {
    	        return 0;
        	}

			/* Sector has been read in, reset indices */
			fat_data.fbuff.secindex = 0;
			fat_data.fbuff.cachedbytes = fat_data.fgeom.bytes_sec;
		
			/* Increment cluster index */
			fat_data.fbuff.clus_index++;
			if (fat_data.fbuff.clus_index >=
				fat_data.fgeom.secs_cluster)
			{
				fat_data.fbuff.clus_index = 0;

	            /* Get next cluster in the chain */
    	        fat_data.fbuff.next_cluster =
        	    	get_next_cluster(fat_data.fbuff.next_cluster);
	            if (fat_data.fbuff.next_cluster == 0xFFFFFFFF)
    	        {
        	        /* This is the last cluster */
            	    fat_data.cluster_last = TRUE;
	            }
    	        else if (fat_data.fbuff.next_cluster == 0xFFFFFFFE)
        	    {
	                /* Error */
    	            return 0;
        	    }
			}
		}

		/* Read data from cached sector */
		toread = bytes;
		if (toread > fat_data.fbuff.cachedbytes) 
		{
			toread = fat_data.fbuff.cachedbytes;
		}
		if ((fat_data.fbuff.tread + toread) > fat_data.filesize) 
		{
			toread = fat_data.filesize - fat_data.fbuff.tread;
			bytes = toread;
		}

		/* Move data */
		for (idx = 0; idx < toread; idx++) 
		{
			*buff = fat_data.fbuff.secdata[fat_data.fbuff.secindex];
			fat_data.fbuff.secindex++;
			fat_data.fbuff.cachedbytes--;
			bread++;
			buff++;
			bytes--;
			fat_data.fbuff.tread++;
		}
	}

	return bread;
}

/***********************************************************************
 * Public functions
 **********************************************************************/

/***********************************************************************
 *
 * Function: fat_init
 *
 * Purpose: Initialize device and file system
 *
 * Processing:
 *     Initialize device and filesystem.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: TRUE if the initialization was ok, otherwise FALSE
 *
 * Notes: None
 *
 **********************************************************************/
BOOL_32 fat_init(void)
{
    /* Save pointers to functions */
    fat_data.init_func  = blkdev_init;
    fat_data.read_func  = blkdev_read;
    fat_data.dinit_func = blkdev_deinit;

    /* Try to initialize interface first */
    if (fat_device_init() == FALSE)
    {
        /* Device did not initialize */
        return FALSE;
    }
    
    return TRUE;
}

/***********************************************************************
 *
 * Function: fat_deinit
 *
 * Purpose: Close device and file system
 *
 * Processing:
 *     Initialize device and filesystem.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: TRUE if the de-initialization was ok, otherwise FALSE
 *
 * Notes: None
 *
 **********************************************************************/
BOOL_32 fat_deinit(void)
{
	return fat_data.dinit_func();
}

/***********************************************************************
 *
 * Function: fat_file_open
 *
 * Purpose: Open a file for reading
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     name : Name of file (root directory only)
 *
 * Outputs: None
 *
 * Returns: TRUE if the file was opened, otherwise FALSE
 *
 * Notes: None
 *
 **********************************************************************/
BOOL_32 fat_file_open(UNS_8 *name) 
{
	UNS_8 fname[14];
	int idx, fidx;

	/* Convert filename to correct format */
	idx = fidx = 0;
	while (name [idx] != '\0') 
	{
		if (name [idx] == '.') 
		{
			/* Fill rest of string with spaces */
			while (fidx < 8) 
			{
				fname [fidx] = ' ';
				fidx++;
			}

			idx++;
		}
		else 
		{
			fname [fidx] = name [idx];
			fidx++;
			idx++;
		}
	}

	/* Fill the rest of the name with spaces */
	for (; fidx <= 11; fidx++)
	{
		fname [fidx] = ' ';
	}
	fname [fidx] = '\0';
	
	/* For the passed filename, find the starting file cluster for
       the file (if it exists) */
   	fat_data.fbuff.next_cluster = find_first_cluster((CHAR *) fname);
    if (fat_data.fbuff.next_cluster == 0xFFFFFFFF)
   	{
       	/* File does not exist, or not found */
        return FALSE;
   	}

	fat_data.fbuff.secindex = 0;
   	fat_data.fbuff.clus_index = 0;
    fat_data.fbuff.cachedbytes = 0;
    fat_data.fbuff.tread = 0;

   	/* Follow and load the file from the cluster chain starting with
       the first cluster */
   	fat_data.cluster_last = FALSE;

    /* Initial cluster data is not cached */
   	fat_data.clusec_cached = 0;

	/* Start file data read */
	fat_data_cache_read(NULL, 0);

	return TRUE;
}

/***********************************************************************
 *
 * Function: fat_file_read
 *
 * Purpose: Read data from an open file
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     buff  : Buffer pointer for read data
 *     bytes : Number of bytes to read
 *
 * Outputs: None
 *
 * Returns: The number of bytes read
 *
 * Notes: None
 *
 **********************************************************************/
INT_32 fat_file_read(UNS_8 *buff,
                     INT_32 bytes) 
{
	return fat_data_cache_read(buff, bytes);
}

/***********************************************************************
 *
 * Function: fat_get_dir
 *
 * Purpose: Get a directory entry
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     dir   : Where to place directory name
 *     reset : TRUE to reset directory lookup to beginning
 *
 * Outputs: None
 *
 * Returns: TRUE if the current entry is the last entry
 *
 * Notes: None
 *
 **********************************************************************/
BOOL_32 fat_get_dir(UNS_8 *dir,
                    BOOL_32 reset) 
{
    DIR_ENTRY_T *dir_entry;
    BOOL_32 gooddir, ldir = FALSE;
    static int di;
    int offs, offs2;
    UNS_8 tdir [16];

	dir[0] = '\0';
	if (reset == TRUE) 
	{
		fat_data.dir_index = -1;
		di = 0;
	}

	gooddir = FALSE;
	while ((di < 512) && (gooddir == FALSE))
	{
		ldir = get_next_dir(&dir_entry);
		di++;
		if ((dir_entry->attribute & ATTB_LFN) != ATTB_LFN)
		{
			if (dir_entry->name[0] == 0) 
			{
				di = 512;
			}
			else if (dir_entry->name[0] != (char) 0xE5)
			{
				str_ncopy(tdir, dir_entry->name, 9);
				offs = 0;
				while (is_whitespace(tdir[offs]) == FALSE) {
					dir [offs] = tdir [offs];
					offs++;
				}

				str_ncopy(tdir, &dir_entry->name[8], 4);
				if (is_whitespace(tdir[0]) == FALSE) 
				{
					dir [offs] = '.';
					offs++;
					offs2 = 0;
					while (is_whitespace(tdir[offs2]) == FALSE) {
						dir [offs] = tdir [offs2];
						offs++;
						offs2++;
					}
				}

				dir [offs] = '\0';
				gooddir = TRUE;
			}
		}
	}

	if (di >= 512) 
	{
		ldir = TRUE;
	}
	
	return ldir;
}
