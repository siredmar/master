/***********************************************************************
 * $Id:: lpc_fat16_private.c 4976 2010-09-20 22:31:17Z usb10132        $
 *
 * Project: FAT16 support functions
 *
 * Description:
 *
 *
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
#include "lpc_types.h"
#include "lpc_fat16_private.h"


//**********************************************************************
// Private functions
//**********************************************************************

/***********************************************************************
 *
 * Function: fat16_moveto
 *
 * Purpose:
 *  Simple data movement routine.
 *
 * Processing:
 *  Move a number of bytes from the source to destination.
 *
 * Parameters:
 *  source : Source address
 *  dest   : Destination address
 *  size   : Number of bytes to move
 *
 * Outputs:
 *  Data pointed to by source will be updated.
 *
 * Returns:
 *  Nothing
 *
 * Notes:
 *  None
 *
 **********************************************************************/
void fat16_moveto(void *source, void *dest, INT_32 size)
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
 * Function: fat16_compare
 *
 * Purpose:
 *  Simple data comparison routine.
 *
 * Processing:
 *  Two strings are compared in lowercase up to the number of
 *  characters set by 'size'.
 *
 * Parameters:
 *  source : Source address
 *  dest   : Destination address
 *  size   : Number of characters to compare
 *
 * Outputs:
 *  Nothing
 *
 * Returns:
 *  '1' if the strings are the same, '0' otherwise
 *
 * Notes:
 *  None
 *
 **********************************************************************/
INT_32 fat16_compare(CHAR *source, CHAR *dest, INT_32 size)
{
  INT_32 index;
  INT_32 match = -1;
  CHAR sr, ds;

  index = 0;
  while (match == -1)
  {
    sr = source [index];
    ds = dest [index];

    // Convert to lowercase for the comparison
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
      // Look for string end
      if (index == (size - 1))
      {
        // Comparison matches
        match = 1;
      }
      else
      {
        index++;
      }
    }
    else
    {
      // No match
      match = 0;
    }
  }

  return match;
}

/***********************************************************************
 *
 * Function: fat16_wait_busy
 *
 * Purpose:
 *  Wait for the device to go 'unbusy'.
 *
 * Processing:
 *  Check the status of the device busy function. If the device is
 *  busy, perform a small loop and check again until the device is no
 *  longer busy.
 *
 * Parameters:
 *  fat_data : Pointer to a device data structure.
 *
 * Outputs:
 *  None
 *
 * Returns:
 *  Nothing
 *
 * Notes:
 *  None
 *
 **********************************************************************/
void fat16_wait_busy(FAT_DEVICE_TYPE *fat_data)
{
  INT_32 i;

  while (fat_data->func.busy_ck_func() == 1)
  {
    // Loop to prevent excessive bus usage
    for (i = 0; i < 1000; i++);
  }
}

/***********************************************************************
 *
 * Function: fat16_translate_cluster_to_sector
 *
 * Purpose:
 *  Translate a cluster number to a (absolute) sector number.
 *
 * Processing:
 *  See function.
 *
 * Parameters:
 *  fat_data : Pointer to a device data structure
 *  cluster  : Cluster number
 *
 * Outputs:
 *  None
 *
 * Returns:
 *  An absolute sector number.
 *
 * Notes:
 *  None
 *
 **********************************************************************/
UNS_32 fat16_translate_cluster_to_sector(FAT_DEVICE_TYPE *fat_data,
                                         UNS_16 cluster)
{
  return (UNS_32)((UNS_32) fat_data->cfat.first_data_sector +
                  (cluster - CLUSTERU_MIN) *
                  (UNS_32) fat_data->pat_hdr.sectors_cluster);
}

/***********************************************************************
 *
 * Function: fat16_read_mbr
 *
 * Purpose:
 *  Reads the FAT MBR and puts the partition tables in the passed
 *  structure.
 *
 * Processing:
 *  Read CHS (0, 0, 1) from the device (this is always the MBR in a
 *  storage device). Copy the partition data from the device data
 *  into the partition data table. Set the selected active partition
 *  to (-1), indicating that a partition has not been selected.
 *
 * Parameters:
 *  fat_data : Pointer to a device data structure.
 *
 * Outputs:
 *  Data in fat_data will be updated.
 *
 * Returns:
 *  Nothing
 *
 * Notes:
 *  None
 *
 **********************************************************************/
void fat16_read_mbr(FAT_DEVICE_TYPE *fat_data)
{
  UNS_8 data [PTAB_SIZE];

  // Read head 0, cylinder 0, sector 1 (MBR sector 0 absolute)
  fat_data->func.set_sector_func(0);

  // Issue read command
  fat_data->func.start_read_func();

  // Wait for the operation to complete
  fat16_wait_busy(fat_data);

  // Read the MBR data from the device (PTAB_SIZE bytes)
  fat_data->func.read_func(data, PTAB_SIZE);

  // Strip out the 4 partition records into the FAT structure
  fat16_moveto(&data [0x1BE], fat_data->part,
               (4 * sizeof(PARTITION_TYPE)));

  // On a fresh MBR read, the active partition is invalidated
  fat_data->act_part = -1;
}

/***********************************************************************
 *
 * Function: fat16_read_sectors
 *
 * Purpose:
 *  Reads a number of sectors from a device into a buffer.
 *
 * Processing:
 *  See function.
 *
 * Parameters:
 *  fat_data     : Pointer to a device data structure
 *  data         : Pointer to data buffer to fill
 *  first_sector : Starting absolute sector to read
 *  num_sectors  : Number of sectors to read
 *
 * Outputs:
 *  None
 *
 * Returns:
 *  Nothing
 *
 * Notes:
 *  None
 *
 **********************************************************************/
void fat16_read_sectors(FAT_DEVICE_TYPE *fat_data, void *data,
                        UNS_32 first_sector, UNS_32 num_sectors)
{
  INT_32 index = 0;
  UNS_8 *data8 = (UNS_8 *) data;

  while (num_sectors > 0)
  {
    // Set sector number in device
    fat_data->func.set_sector_func(first_sector);

    // Issue a read command to the device
    fat_data->func.start_read_func();

    // Wait for operation to complete
    fat16_wait_busy(fat_data);

    // Read the sector data and update buffer index
    index = index + fat_data->func.read_func(&data8 [index],
                                             (INT_32) fat_data->pat_hdr.bytes_sector);

    // Update counters
    first_sector++;
    num_sectors--;
  }
}

/***********************************************************************
 *
 * Function: fat16_write_sectors
 *
 * Purpose:
 *  Writes a number of sectors from a buffer to a device.
 *
 * Processing:
 *  See function.
 *
 * Parameters:
 *  fat_data     : Pointer to a device data structure
 *  data         : Pointer to data buffer to copy from
 *  first_sector : Starting absolute sector to write
 *  num_sectors  : Number of sectors to write
 *
 * Outputs:
 *  None
 *
 * Returns:
 *  Nothing
 *
 * Notes:
 *  None
 *
 **********************************************************************/
void fat16_write_sectors(FAT_DEVICE_TYPE *fat_data, void *data,
                         UNS_32 first_sector, UNS_32 num_sectors)
{
  INT_32 index = 0;
  UNS_8 *data8 = (UNS_8 *) data;

  while (num_sectors > 0)
  {
    // Set sector number in device
    fat_data->func.set_sector_func(first_sector);

    // Issue write command
    fat_data->func.start_write_func();

    // Wait for operation to complete before writing data
    fat16_wait_busy(fat_data);

    // Put data in the write buffer
    index = index + fat_data->func.write_func(&data8 [index],
                                              (INT_32) fat_data->pat_hdr.bytes_sector);

    // Wait for write to complete
    fat16_wait_busy(fat_data);

    // Update counters
    first_sector++;
    num_sectors--;
  }
}

/***********************************************************************
 *
 * Function: fat16_parse_path
 *
 * Purpose:
 *  Finds the next directory name in a path.
 *
 * Processing:
 *  See function.
 *
 * Parameters:
 *  path : a path name string
 *
 * Outputs:
 *  None
 *
 * Returns:
 *  Size of data parsed if the operation was successful, otherwise -1.
 *
 * Notes:
 *  None
 *
 **********************************************************************/
INT_32 fat16_parse_path(CHAR *path)
{
  INT_32 i;

  // Find name up to next whitespace or '/' character
  i = 0;
  while ((i < 8) && (path [i] != '/') && (path [i] != '\0'))
  {
    // So far, size is valid
    i++;
  }

  // Was the size valid
  if (i >= 8)
  {
    i = -1;
  }

  return i;
}

/***********************************************************************
 *
 * Function: fat16_name_break
 *
 * Purpose:
 *  Converts a filename in unpadded 8.3 format to a format that is
 *  compatible with a directory format.
 *
 * Processing:
 *  See function.
 *
 * Parameters:
 *  full_name : a name string in unpadded 8.3 format
 *  name      : a name string in padded 8.3 format
 *
 * Outputs:
 *  None
 *
 * Returns:
 *  Nothing
 *
 * Notes:
 *  None
 *
 **********************************************************************/
void fat16_name_break(CHAR *full_name, CHAR *name)
{
  INT_32 i, index;

  // Copy the name (up to the dot) into vname, pad up to 8 chars
  // with spaces
  index = 0;
  for (i = 0; i < 8; i++)
  {
    if ((full_name [index] == '\0') || (full_name [index] == '.'))
    {
      // Pad rest of the name with spaces
      name [i] = ' ';
    }
    else
    {
      // Use original name
      name [i] = full_name [index];
      index++;
    }
  }

  // Skip to extension if '.' was found
  if (full_name [index] == '.')
  {
    index++;
  }

  // Extension
  for (i = 0; i < 3; i++)
  {
    if (full_name [index] != '\0')
    {
      name [8 + i] = full_name [index];
      index++;
    }
    else
    {
      name [8 + i] = ' ';
    }
  }

  // Terminate name and extension strings
  name [12] = '\0';
}

/***********************************************************************
 *
 * Function: fat16_name_check
 *
 * Purpose:
 *  Compares a passed name in padded 8.3 format with a name in a
 *  directory entry structure.
 *
 * Processing:
 *  Compare the first 11 characters of the passed name with the 11
 *  characters in the passed directory structure.
 *
 * Parameters:
 *  name     : Padded 8.3 name
 *  dir_data : Pointer to a directory structure
 *
 * Outputs:
 *  None
 *
 * Returns:
 *  '1' if the name matches the directory entry name
 *
 * Notes:
 *  None
 *
 **********************************************************************/
INT_32 fat16_name_check(CHAR *name, ROOT_ENTRY_TYPE *dir_data)
{
  return fat16_compare(name, dir_data->name, 11);
}

/***********************************************************************
 *
 * Function: fat16_find_file
 *
 * Purpose:
 *  Finds and returns the directory structure of the passed name in the
 *  active directory.
 *
 * Processing:
 *  See function.
 *
 * Parameters:
 *  name      : Unpadded 8.3 name to search for in the directory
 *  file_data : Pointer to a file data structure
 *
 * Outputs:
 *  If the file was found, the structure pointed to by newdir will be
 *  populated with the file/directory information.
 *
 * Returns:
 *  Index to matching directory structure in active dir, or (-1) if a
 *  match was not found.
 *
 * Notes:
 *  None
 *
 **********************************************************************/
INT_32 fat16_find_file(CHAR *name, FILE_TYPE *file_data)
{
  INT_32 dir_index, found;
  CHAR vname [16];

  found = -1;

  // Break name into a format that can be easily compared
  fat16_name_break(name, vname);

  // Process directory entries
  dir_index = 0;
  while (dir_index <
         (INT_32) file_data->fat_data->pat_hdr.root_entries)
  {
    // Is this entry valid (not an erased entry and not an LFN
    // entry)
    if ((file_data->dir_data [dir_index].name [0] != (char) DIR_FREE) &&
        (file_data->dir_data [dir_index].name [0] != (char) DIR_ERASED) &&
        (file_data->dir_data [dir_index].attribute != ATTB_LFN))
    {
      // Check for matching name
      if (fat16_name_check(vname,
                           &file_data->dir_data [dir_index]) == 1)
      {
        // Name matches a directory entry, use it
        found = dir_index;

        // Force exit from loop
        dir_index =
          (INT_32) file_data->fat_data->pat_hdr.root_entries;
      }
    }

    dir_index++;
  }

  return found;
}

/***********************************************************************
 *
 * Function: fat16_find_free_cluster
 *
 * Purpose:
 *  Find the next free cluster in the cluster list. Searches down from
 *  the passed cluster.
 *
 * Processing:
 *  See function.
 *
 * Parameters:
 *  fat_data      : Pointer to a FAT device structure
 *  cluster_start : Starting cluster in list where to search
 *
 * Outputs:
 *  None
 *
 * Returns:
 *  Next free cluster, or '0' if a free cluster was not found
 *
 * Notes:
 *  None
 *
 **********************************************************************/
UNS_16 fat16_find_free_cluster(FAT_DEVICE_TYPE *fat_data,
                               UNS_16 cluster_start)
{
  UNS_16 found_cluster;
  INT_32 exit = 0;

  // Start searching from top-down on cluster list
  found_cluster = cluster_start;
  while ((found_cluster < fat_data->cfat.clusters) &&
         (exit == 0))
  {
    if (fat_data->clusters [(INT_32) found_cluster] ==
        0)
    {
      // Cluster is free, so use it
      exit = 1;
    }
    else
    {
      // Cluster not free, check next cluster
      found_cluster++;
    }
  }

  // Only continue if a free cluster was not found
  if ((exit == 0) && (cluster_start >= CLUSTERU_MIN))
  {
    // Try finding a cluster at the top of the list
    found_cluster = CLUSTERU_MIN;
    while ((found_cluster < cluster_start)
           && (exit == 0))
    {
      if (fat_data->clusters [(INT_32) found_cluster]
          == 0)
      {
        // Cluster is free, so use it
        exit = 1;
      }
      else
      {
        // Cluster not free, check next cluster
        found_cluster++;
      }
    }
  }

  // Was a free cluster ever found?
  if (exit == 0)
  {
    // No free clusters, return 0
    found_cluster = 0;
  }

  return found_cluster;
}

/***********************************************************************
 *
 * Function: fat16_get_free_dir_entry
 *
 * Purpose:
 *  Allocates a new directory entry for the passed name.
 *
 * Processing:
 *  See function.
 *
 * Parameters:
 *  file_data : Pointer to a file data structure
 *
 * Outputs:
 *  None
 *
 * Returns:
 *  The index of the added dir entry, or (-1) if unsuccessful.
 *
 * Notes:
 *  None
 *
 **********************************************************************/
INT_32 fat16_get_free_dir_entry(FILE_TYPE *file_data)
{
  INT_32 index = 0;
  INT_32 valid = -1;

  // Loop through the entrie directory looking for the first free
  // directory entry
  while (index < (INT_32) file_data->fat_data->pat_hdr.root_entries)
  {
    if ((file_data->dir_data [index].name [0] == (char) DIR_FREE) ||
        (file_data->dir_data [index].name [0] == (char) DIR_ERASED))
    {
      // Free entry, set return value to entry index
      valid = index;

      // Force exit condition
      index = (INT_32) file_data->fat_data->pat_hdr.root_entries;
    }

    // Next index
    index++;
  }

  return valid;
}

/***********************************************************************
 *
 * Function: fat16_get_next_cluster
 *
 * Purpose:
 *  Returns the next cluster in a cluster link chain.
 *
 * Processing:
 *  See function.
 *
 * Parameters:
 *  fat_data    : Pointer to a FAT device structure
 *  cluster_num : Cluster number to use for next cluster search
 *
 * Outputs:
 *  None
 *
 * Returns:
 *  The next cluster in the list.
 *
 * Notes:
 *  None
 *
 **********************************************************************/
UNS_32 fat16_get_next_cluster(FAT_DEVICE_TYPE *fat_data,
                              UNS_16 cluster_num)
{
  return fat_data->clusters [(INT_32) cluster_num];
}

/***********************************************************************
 *
 * Function: fat16_set_no_mbr
 *
 * Purpose:
 *  Sets up the first partition in the cached parition table to point
 *  to sector 1 as a FAT16 boot record.
 *
 * Processing:
 *  See function.
 *
 * Parameters:
 *  fat_data : Pointer to a FAT device structure
 *
 * Outputs:
 *  None
 *
 * Returns:
 *  1 if the partition was mounted as FAT16, '-1' otherwise.
 *
 * Notes:
 *  This function can be used to setup the cached partition table to
 *  use the fat16 functions without an MBR. (Some smaller storage
 *  devices may not have an MBR).
 *
 **********************************************************************/
void fat16_set_no_mbr(FAT_DEVICE_TYPE *fat_data)
{
  fat_data->part [0].state          = PART_ACTV;
  fat_data->part [0].head_start     = 0x0000;
  fat_data->part [0].cyl_sec_start  = 0x0001;
  fat_data->part [0].partype        = FAT16_LT32M;
  fat_data->part [0].head_end       = 0x0000;
  fat_data->part [0].cyl_sec_end    = 0x0000;
  fat_data->part [0].mbr_sec_offset = 0x0000;
  fat_data->part [0].partsecs       = 0x0000;
}
