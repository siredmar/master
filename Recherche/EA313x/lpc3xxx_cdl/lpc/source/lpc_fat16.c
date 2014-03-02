/***********************************************************************
 * $Id:: lpc_fat16.c 4976 2010-09-20 22:31:17Z usb10132                $
 *
 * Project: FAT16 driver
 *
 * Description:
 *  This package uses heap functions in lpc_heap.c
 *  All filenames must be in uppercase letters and 8.3 format
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
#include "lpc_heap.h"
#include "lpc_fat16.h"
#include "lpc_fat16_private.h"

//**********************************************************************
// Local defines
//**********************************************************************
// Computed offsets from the unaligned partition header
#define JUMP_OFS      0
#define JUMP_SZ       3
#define OEMID_OFS     (JUMP_OFS + JUMP_SZ)
#define OEMID_SZ      8
#define BYTES_SEC_OFS (OEMID_OFS + OEMID_SZ)
#define BYTES_SEC_SZ  2
#define SECS_CLUS_OFS (BYTES_SEC_OFS + BYTES_SEC_SZ)
#define SECS_CLUS_SZ  1
#define RES_SECT_OFS  (SECS_CLUS_OFS + SECS_CLUS_SZ)
#define RES_SECT_SZ   2
#define FAT_COPY_OFS  (RES_SECT_OFS + RES_SECT_SZ)
#define FAT_COPY_SZ   1
#define ROOT_ENT_OFS  (FAT_COPY_OFS + FAT_COPY_SZ)
#define ROOT_ENT_SZ   2
#define SMALL_SEC_OFS (ROOT_ENT_OFS + ROOT_ENT_SZ)
#define SMALL_SEC_SZ  2
#define MEDIA_DES_OFS (SMALL_SEC_OFS + SMALL_SEC_SZ)
#define MEDIA_DES_SZ  1
#define SECS_FAT_OFS  (MEDIA_DES_OFS + MEDIA_DES_SZ)
#define SECS_FAT_SZ   2
#define SECS_TK_OFS   (SECS_FAT_OFS + SECS_FAT_SZ)
#define SECS_TK_SZ    2
#define NUM_HDS_OFS   (SECS_TK_OFS + SECS_TK_SZ)
#define NUM_HDS_SZ    2
#define HDN_SECS_OFS  (NUM_HDS_OFS + NUM_HDS_SZ)
#define HDN_SECS_SZ   4
#define LG_SECS_OFS   (HDN_SECS_OFS + HDN_SECS_SZ)
#define LG_SECS_SZ    4
#define DV_NUM_OFS    (LG_SECS_OFS + LG_SECS_SZ)
#define DV_NUM_SZ     1
#define RSV_OFS       (DV_NUM_OFS + DV_NUM_SZ)
#define RSV_SZ        1
#define BT_SIG_OFS    (RSV_OFS + RSV_SZ)
#define BT_SIG_SZ     1
#define SERNUM_OFS    (BT_SIG_OFS + BT_SIG_SZ)
#define SERNUM_SZ     4
#define LABEL_OFS     (SERNUM_OFS + SERNUM_SZ)
#define LABEL_SZ      11
#define FSNAME_OFS    (LABEL_OFS + LABEL_SZ)
#define FSNAME_SZ     8

//**********************************************************************
// Public functions
//**********************************************************************
//**********************************************************************
// Initialization functions
//**********************************************************************

/***********************************************************************
 *
 * Function: fat16_init_device
 *
 * Purpose:
 *  Initializes the FAT16 interface for the selected device.
 *
 * Processing:
 *  Copy the device name and function pointers into the FAT device
 *  structure. Clear the commit flag to indicate the FAT cluster table
 *  does not need to be written back to the device. Call the device
 *  initialization function. If the device was initialized, read the
 *  MBR into the FAT device structure.
 *
 * Parameters:
 *  device           : Device name
 *  init_func        : Pointer to initialization function
 *  shutdown_func    : Pointer to shutdown function
 *  insert_ck_func   : Pointer to insertion check function
 *  ready_ck_func    : Pointer to ready check function
 *  busy_ck_func     : Pointer to bust check function
 *  set_sector_func  : Pointer to sector set function
 *  start_read_func  : Pointer to read start function
 *  start_write_func : Pointer to write start function
 *  read_func        : Pointer to read buffer function
 *  write_func       : Pointer to write buffer function
 *
 * Outputs:
 *  Data in fat_data will be updated.
 *
 * Returns:
 *  The pointer to a binded device structure, or NULL if the device
 *  was not detected.
 *
 * Notes:
 *  The calling function should check to make sure that NULL was not
 *  returned. If NULL was returned, the device does not exist or
 *  memory could not be allocated.
 *
 **********************************************************************/
FAT_DEVICE_TYPE * fat16_init_device(
  CHAR *device,              // Name of device
  ivfunc init_func,          // Pointer to device init function
  vvfunc shutdown_func,      // Pointer to device shutdown function
  ivfunc insert_ck_func,     // Pointer to check for insert func
  ivfunc ready_ck_func,      // Pointer for ready? check
  ivfunc busy_ck_func,       // Pointer for busy? check
  void (*set_sector_func)(UNS_32),  // sector setting
  vvfunc start_read_func,    // Pointer for read start
  vvfunc start_write_func,   // Pointer for write start
  ivifunc read_func,         // Pointer for read of data
  ivifunc write_func)        // Pointer for write of data
{
  FAT_DEVICE_TYPE *fat_data = (FAT_DEVICE_TYPE *) NULL;

  // Try to initialize the device
  if (init_func() == 1)
  {
    // Device initiailized, allocate the device data structure
    fat_data = lpc_new(sizeof(FAT_DEVICE_TYPE));

    if (fat_data != NULL)
    {
      // Copy device name into device name in structure
      fat16_moveto(device, fat_data->device, (DSIZE - 1));
      fat_data->device [DSIZE - 1] = '\0';

      // Save function pointers
      fat_data->func.init_func        = init_func;
      fat_data->func.shutdown_func    = shutdown_func;
      fat_data->func.insert_ck_func   = insert_ck_func;
      fat_data->func.ready_ck_func    = ready_ck_func;
      fat_data->func.busy_ck_func     = busy_ck_func;
      fat_data->func.set_sector_func  = set_sector_func;
      fat_data->func.start_read_func  = start_read_func;
      fat_data->func.start_write_func = start_write_func;
      fat_data->func.read_func        = read_func;
      fat_data->func.write_func       = write_func;

      // Set active partition to (-1), disable commit
      fat_data->fat_commit = 0;

      // Read MBR and populate MBR structure
      fat16_read_mbr(fat_data);
    }
  }

  return fat_data;
}

/***********************************************************************
 *
 * Function: fat16_shutdown
 *
 * Purpose:
 *  Shutdown the FAT16 interface for the selected device.
 *
 * Processing:
 *  If the commit flag is set, write the cached FAT cluster table back
 *  to the device. Free the allocated memory for the cluster table and
 *  device structure.
 *
 * Parameters:
 *  fat_data : Pointer to a FAT data structure
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
void fat16_shutdown(FAT_DEVICE_TYPE *fat_data)
{
  // If the commit flag in the FAT device structure is set, then a
  // write operation occurred to the device and the cached FAT cluster
  // table needs to be written back to the device before shutdown
  if (fat_data->fat_commit != 0)
  {
    // Write FAT back to the device - although only FAT1 is used,
    // this routine will write all FAT copies back to the device
    // so they stay consistent in other machines
    fat16_write_sectors(fat_data, fat_data->clusters,
                        fat_data->cfat.first_fat1_sector,
                        fat_data->cfat.fat_sectors);

    // Does more than 1 FAT need to be written?
    if (fat_data->pat_hdr.fat_copies > 1)
    {
      fat16_write_sectors(fat_data, fat_data->clusters,
                          fat_data->cfat.first_fat2_sector,
                          fat_data->cfat.fat_sectors);
    }
  }

  // Destroy cached cluster allocation
  lpc_free(fat_data->clusters);

  // Destroy the FAT device structure
  lpc_free(fat_data);
}

/***********************************************************************
 *
 * Function: fat16_get_status
 *
 * Purpose:
 *  Get the status of the partition from the MBR.
 *
 * Processing:
 *  Return the status and partition type values from the partition
 *  table in the FAT device structure.
 *
 * Parameters:
 *  fat_data : Pointer to a FAT data structure
 *  status   : Pointer to status flag to populate
 *  ptype    : Pointer to partition type flag to populate
 *  pnum     : Partition number to return status on (1 - 4)
 *
 * Outputs:
 *  The partition type and the status will be updated in memory
 *  pointed to by status and ptype. The only valid ptype and status
 *  values are (FAT16_LT32M, FAT16_EXDOS, FAT16_GT32M).
 *
 * Returns:
 *  Nothing
 *
 * Notes:
 *  Only partition numbers 1 through 4 are valid.
 *
 **********************************************************************/
void fat16_get_status(FAT_DEVICE_TYPE *fat_data, UNS_8 *status,
                      UNS_8 *ptype, INT_32 pnum)
{
  if ((pnum >= 1) && (pnum <= 4))
  {
    // Only return status for valid partition numbers
    *status = fat_data->part [pnum - 1].state;
    *ptype = fat_data->part [pnum - 1].partype;
  }
  else
  {
    // Invalid partition, return bad status
    *status = 0x0;
    *ptype = 0;
  }
}

/***********************************************************************
 *
 * Function: fat16_set_partition
 *
 * Purpose:
 *  Set the active partition.
 *
 * Processing:
 *  If the partition is a valid type (FAT16), the starting sector value
 *  for the partition will be determined and the appropriate sector
 *  containing the boot record will be read from the device. Once the
 *  boot record has been read in, the partition dimensions are
 *  computed. Appropriate space for the FAT cluster table is allocated
 *  and the cluster table is cached in memory.
 *
 * Parameters:
 *  partnum  : Partition number of set (1 - 4) on this device
 *  fat_data : Pointer to a FAT data structure
 *
 * Outputs:
 *  Data in fat_data will be updated.
 *
 * Returns:
 *  '1' if the partition was set, '0' otherwise.
 *
 * Notes:
 *  Only partition numbers 1 through 4 are valid.
 *
 **********************************************************************/
INT_32 fat16_set_partition(INT_32 partnum, FAT_DEVICE_TYPE *fat_data)
{
  UNS_32 table_size;
  UNS_8 data [PTAB_SIZE];
  FATGEOM_TYPE *fg;
  INT_32 valid = 0;

  // Reset partnum to work with indices
  partnum--;

  // Is partition of correct type?
  if (((partnum >= 0) && (partnum <= 3)) &&
      ((fat_data->part [partnum].partype == FAT16_LT32M) ||
       (fat_data->part [partnum].partype == FAT16_EXDOS) ||
       (fat_data->part [partnum].partype == FAT16_GT32M)))
  {
    // Set the sector to the start of the partition
    fat_data->func.set_sector_func(
      fat_data->part [partnum].mbr_sec_offset);

    // Issue read command
    fat_data->func.start_read_func();

    // Wait for the operation to complete
    fat16_wait_busy(fat_data);

    // Read the MBR data from the device (PTAB_SIZE bytes)
    fat_data->func.read_func(data, PTAB_SIZE);

    // Reformat the unaligned structure into the aligned
    // fatgeom_type structure
    fg = &fat_data->pat_hdr;
    fat16_moveto(&data [JUMP_OFS], &fg->jump, JUMP_SZ);
    fat16_moveto(&data [OEMID_OFS], &fg->oem_id, OEMID_SZ);
    fat16_moveto(&data [BYTES_SEC_OFS], &fg->bytes_sector,
                 BYTES_SEC_SZ);
    fat16_moveto(&data [SECS_CLUS_OFS], &fg->sectors_cluster,
                 SECS_CLUS_SZ);
    fat16_moveto(&data [RES_SECT_OFS], &fg->res_sectors,
                 RES_SECT_SZ);
    fat16_moveto(&data [FAT_COPY_OFS], &fg->fat_copies,
                 FAT_COPY_SZ);
    fat16_moveto(&data [ROOT_ENT_OFS], &fg->root_entries,
                 ROOT_ENT_SZ);
    fat16_moveto(&data [SMALL_SEC_OFS], &fg->small_sectors,
                 SMALL_SEC_SZ);
    fat16_moveto(&data [MEDIA_DES_OFS], &fg->media_desc,
                 MEDIA_DES_SZ);
    fat16_moveto(&data [SECS_FAT_OFS], &fg->sectors_fat,
                 SECS_FAT_SZ);
    fat16_moveto(&data [SECS_TK_OFS], &fg->sectors_track,
                 SECS_TK_SZ);
    fat16_moveto(&data [NUM_HDS_OFS], &fg->number_heads,
                 NUM_HDS_SZ);
    fat16_moveto(&data [HDN_SECS_OFS], &fg->hidden_sectors,
                 HDN_SECS_SZ);
    fat16_moveto(&data [LG_SECS_OFS], &fg->large_sectors,
                 LG_SECS_SZ);
    fat16_moveto(&data [DV_NUM_OFS], &fg->drive_number, DV_NUM_SZ);
    fat16_moveto(&data [RSV_OFS], &fg->reserved, RSV_SZ);
    fat16_moveto(&data [BT_SIG_OFS], &fg->ext_boot_sig, BT_SIG_SZ);
    fat16_moveto(&data [SERNUM_OFS], &fg->serial_number, SERNUM_SZ);
    fat16_moveto(&data [LABEL_OFS], &fg->label, LABEL_SZ);
    fat16_moveto(&data [FSNAME_OFS], &fg->fs_name, FSNAME_SZ);

    // Compute the first sector in the partition (fatgeom)
    fat_data->cfat.first_boot_sector =
      fat_data->part [partnum].mbr_sec_offset;

    // Compute reserved sectors
    fat_data->cfat.boot_sectors = fat_data->pat_hdr.res_sectors;

    // Compute sectors used for a single FAT
    fat_data->cfat.fat_sectors = fat_data->pat_hdr.sectors_fat;

    // Compute location of first and last FAT(1) sectors
    fat_data->cfat.first_fat1_sector =
      fat_data->cfat.first_boot_sector +
      fat_data->cfat.boot_sectors;
    fat_data->cfat.last_fat1_sector =
      fat_data->cfat.first_fat1_sector +
      fat_data->cfat.fat_sectors - 1;

    // Compute location of first and last FAT(2) sectors. This may
    // not exist in all implementations
    fat_data->cfat.first_fat2_sector =
      fat_data->cfat.last_fat1_sector + 1;
    fat_data->cfat.last_fat2_sector =
      fat_data->cfat.first_fat2_sector +
      fat_data->cfat.fat_sectors - 1;

    // Compute location of the root directory and # of root sectors
    fat_data->cfat.first_root_sector =
      fat_data->cfat.first_fat1_sector +
      (fat_data->cfat.fat_sectors *
       fat_data->pat_hdr.fat_copies);
    fat_data->cfat.root_sectors = (fat_data->pat_hdr.root_entries *
                                   sizeof(ROOT_ENTRY_TYPE) / fat_data->pat_hdr.bytes_sector);

    // Compute total number of sectors
    fat_data->cfat.total_sectors = fat_data->pat_hdr.small_sectors;
    if (fat_data->cfat.total_sectors == 0)
    {
      fat_data->cfat.total_sectors =
        fat_data->pat_hdr.large_sectors;
    }

    // Compute location of the first data sector
    fat_data->cfat.first_data_sector =
      fat_data->cfat.first_root_sector +
      fat_data->cfat.root_sectors;

    // Compute total number of data sectors
    fat_data->cfat.data_sectors = fat_data->cfat.total_sectors -
                                  fat_data->cfat.root_sectors - (fat_data->cfat.fat_sectors *
                                                                 (UNS_32) fat_data->pat_hdr.fat_copies) -
                                  fat_data->cfat.boot_sectors;

    // Compute the number of data clusters available
    fat_data->cfat.clusters = fat_data->cfat.data_sectors /
                              fat_data->pat_hdr.sectors_cluster;

    // Compute the total size of the device partition
    fat_data->cfat.total_size = fat_data->cfat.total_sectors *
                                fat_data->pat_hdr.bytes_sector;

    // Save computed cluster size (in bytes)
    fat_data->cfat.cluster_size =
      (UNS_16) fat_data->pat_hdr.sectors_cluster *
      (UNS_16) fat_data->pat_hdr.bytes_sector;

    // Compute the required size of the file cluster table
    table_size = (UNS_32)(fat_data->cfat.fat_sectors *
                          (UNS_32) fat_data->pat_hdr.bytes_sector);
    fat_data->clusters = (UNS_16 *) lpc_new(table_size);

    // Read cluster data into file cluster table
    fat16_read_sectors(fat_data, fat_data->clusters,
                       fat_data->cfat.first_fat1_sector,
                       fat_data->cfat.fat_sectors);

    // Save new active partition number
    fat_data->act_part = (INT_8) partnum;

    // Valid operation
    valid = 1;
  }

  return valid;
}

/***********************************************************************
 *
 * Function: fat16_create_new_file_descriptor
 *
 * Purpose:
 *  Creates a file structure with the device and FAT data.
 *
 * Processing:
 *  Allocates memory for a new file descriptor. Sets the initial file
 *  mode to FINVALID. Links the FAT device structure to the file
 *  descriptor. Sets up and caches the default directory used with the
 *  file descriptor as the root directory with an initial directory
 *  index at the start of the directory table. Allocates space for
 *  data storage during file operations (read/write).
 *
 * Parameters:
 *  fat_data  : Pointer to a FAT device structure
 *
 * Outputs:
 *  None
 *
 * Returns:
 *  A pointer to a new file descriptor or NULL if there was not enough
 *  memory available.
 *
 * Notes:
 *  None
 *
 **********************************************************************/
FILE_TYPE * fat16_create_new_file_descriptor(FAT_DEVICE_TYPE *fat_data)
{
  FILE_TYPE *file_data;
  UNS_32 table_size;

  // Allocate a new file data structure
  file_data = lpc_new(sizeof(FILE_TYPE));
  if (file_data != (FILE_TYPE *) NULL)
  {
    // Mode is initially invalid
    file_data->fmode = FINVALID;

    // Save binded FAT/device structure
    file_data->fat_data = fat_data;

    // Set the default startup directory to 'root'
    file_data->sector_dir = fat_data->cfat.first_root_sector;

    // Create data buffer pointer and clear buffer index
    file_data->data = (UNS_8 *) lpc_new((INT_32)
                                        file_data->fat_data->cfat.cluster_size);

    // Compute required size of the directory listing and
    // allocate a new directory table
    table_size = fat_data->cfat.root_sectors *
                 (UNS_32) fat_data->pat_hdr.bytes_sector;
    file_data->dir_data = (ROOT_ENTRY_TYPE *) lpc_new(
                            table_size);

    // Cache in the directory table
    fat16_read_sectors(fat_data, file_data->dir_data,
                       file_data->sector_dir, fat_data->cfat.root_sectors);

    // Clear initial directory index
    file_data->dir_index = 0;

    // Initial directory is unchanged
    file_data->dir_commit = 0;
  }

  return file_data;
}

/***********************************************************************
 *
 * Function: fat16_destroy_file_descriptor
 *
 * Purpose:
 *  Destroys a created file descriptor.
 *
 * Processing:
 *  Prior to destroying the file descriptor, a call to fat16_close is
 *  performed to write any data in the write buffer out to the device.
 *  If the directory has been changed in any way, the cached directory
 *  is written back to the device. The structures used in the file
 *  descriptor and the file descriptor itself are then de-allocated.
 *
 * Parameters:
 *  file_data : Pointer to a file descriptor to free.
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
void fat16_destroy_file_descriptor(FILE_TYPE *file_data)
{
  if (file_data != NULL)
  {
    // Close an open file
    fat16_close_file(file_data);

    // Write the active directory back to the device is it has
    // changed
    if (file_data->dir_commit == 1)
    {
      fat16_write_sectors(file_data->fat_data,
                          file_data->dir_data, file_data->sector_dir,
                          file_data->fat_data->cfat.root_sectors);
    }

    // Destroy the data and directory
    lpc_free(file_data->data);
    lpc_free(file_data->dir_data);

    // Destroy the file data structure
    lpc_free(file_data);
  }
}

/***********************************************************************
 *
 * Function: fat16_cd
 *
 * Purpose:
 *  Set the active directory.
 *
 * Processing:
 *  Prior to any operations, the current directory index data is saved.
 *  If the first character is a '/', the directory pointer is set to
 *  the root directory. If the dir_commit flag is set, the cached
 *  directory will be written back to the device before the change.
 *
 *  The next name in the path will then be parsed. The active directory
 *  will be searched for the name. If the name is found, the cluster
 *  number to the new directory will be fetched and the new directory
 *  cached in. This process continues for all parsed names. If no errors
 *  occurred, the active directory index is updated to the new index.
 *  If an error occurred, the original directory and index are restored.
 *
 * Parameters:
 *  path      : Path of new directory
 *  file_data : Pointer to a FILE data structure to populate
 *
 * Outputs:
 *  Data in file_data will be updated.
 *
 * Returns:
 *  '1' if the operation was successful, '0' otherwise.
 *
 * Notes:
 *  None
 *
 **********************************************************************/
INT_32 fat16_cd(CHAR *path, FILE_TYPE *file_data)
{
  CHAR dirname [16];
  INT_32 previous_dir, previous_dir_index, dir_index;
  INT_32 size = 0;
  INT_32 valid = 0;
  INT_32 index = 0;

  // Save previous path and index, in case this operation fails
  previous_dir = file_data->sector_dir;
  previous_dir_index = file_data->dir_index;

  // Reset path to root if first character in the pathname is '/'
  if (path [index] == '/')
  {

    // Before caching in new directory, rewrite the old  directory
    // back to the device if the directory has changed
    if (file_data->dir_commit == 1)
    {
      fat16_write_sectors(file_data->fat_data,
                          file_data->dir_data, file_data->sector_dir,
                          file_data->fat_data->cfat.root_sectors);
      file_data->dir_commit = 0;
    }

    // Set the default startup directory to 'root'
    file_data->sector_dir =
      file_data->fat_data->cfat.first_root_sector;
    file_data->dir_index = 0;

    // Cache in the new directory table
    fat16_read_sectors(file_data->fat_data,
                       file_data->dir_data, file_data->sector_dir,
                       file_data->fat_data->cfat.root_sectors);

    index = 1;
    valid = 1;
  }

  // More data in the path name
  if (path [1] != '\0')
  {
    valid = 1;

    // Get the next directory name
    size = fat16_parse_path(&path [index]);
    while (size > 0)
    {
      // A valid name was found, copy name locally with terminator
      fat16_moveto(&path [index], dirname, size);
      dirname [size] = '\0';

      dir_index = fat16_find_file(dirname, file_data);
      if (dir_index == -1)
      {
        // Path/directory was not found, so quit
        valid = 0;
        size = -1;
      }
      // Only switch directories if it actually is a directory
      else if (
        file_data->dir_data [dir_index].attribute == ATTB_DIR)
      {
        // Before caching in new directory, rewrite the old
        // directory back to the device if the directory has
        // changed
        if (file_data->dir_commit == 1)
        {
          fat16_write_sectors(file_data->fat_data,
                              file_data->dir_data, file_data->sector_dir,
                              file_data->fat_data->cfat.root_sectors);
          file_data->dir_commit = 0;
        }

        // Good so far, set file directory pointer to new
        // directory table
        file_data->sector_dir =
          fat16_translate_cluster_to_sector(
            file_data->fat_data,
            file_data->dir_data [dir_index].clusternum);
        file_data->dir_index = 0;

        // Cache in the new directory table
        fat16_read_sectors(file_data->fat_data,
                           file_data->dir_data, file_data->sector_dir,
                           file_data->fat_data->cfat.root_sectors);

        // Update index, skip past delimiters
        index = index + size;
        if (path [index] == '/')
        {
          index++;
        }

        // Check next field
        size = fat16_parse_path(&path [index]);
      }
      else
      {
        // Check next field
        size = fat16_parse_path(&path [index]);
      }
    }
  }

  if ((valid == 0) || (size < 0))
  {
    // Restore previous directory if needed
    if (file_data->sector_dir != previous_dir)
    {
      // Error somewhere, restore previous path and index
      file_data->sector_dir = previous_dir;
      file_data->dir_index = previous_dir_index;

      fat16_read_sectors(file_data->fat_data,
                         file_data->dir_data, file_data->sector_dir,
                         file_data->fat_data->cfat.root_sectors);
    }

    valid = 0;
  }

  return valid;
}

/***********************************************************************
 *
 * Function: fat16_get_dirname
 *
 * Purpose:
 *  Returns the name and type of the entry in the active directory (in
 *  unpadded 8.3 format).
 *
 * Processing:
 *  See function.
 *
 * Parameters:
 *  file_data : Pointer to a file data structure
 *  name      : Pointer of where to return name
 *  etype     : Pointer of where to return dir entry type
 *  empty     : Pointer of where to return dir entry use flag
 *  last      : If set, this was the last entry in the directory
 *
 * Outputs:
 *  None
 *
 * Returns:
 *  The index to the active directory entry (only valid if empty and
 *  last are not set).
 *
 * Notes:
 *  The type and empty flags should be checked after a call to this
 *  function. If empty is set(1), the dir entry is not used.
 *
 **********************************************************************/
INT_32 fat16_get_dirname(FILE_TYPE *file_data, CHAR *name,
                         UNS_8 *etype, INT_32 *empty, INT_32 *last)
{
  INT_32 index, i, catch_char;
  index = i = 0;
  catch_char = 1;

  *last = 0;
  while ((catch_char == 1) && (i < 8))
  {
    // Look for spaces marking end of name string
    if (file_data->dir_data [file_data->dir_index].name [i] == ' ')
    {
      // Space found,
      catch_char = 0;
    }
    else
    {
      // Not the end of the string, copy into name
      name [index] =
        file_data->dir_data [file_data->dir_index].name [i];
      index++;
    }

    i++;
  }

  i = 0;
  catch_char = 1;
  while ((catch_char == 1) && (i < 3))
  {
    // Look for spaces marking end of extension string
    if (file_data->dir_data [file_data->dir_index].ext [i] == ' ')
    {
      // Space found,
      catch_char = 0;
    }
    else
    {
      // Add dot for extension seperation
      if (i == 0)
      {
        name [index] = '.';
        index++;
      }

      // Not the end of the string, copy into name
      name [index] =
        file_data->dir_data [file_data->dir_index].ext [i];
      index++;
    }

    i++;
  }

  // Terminate string
  name [index] = '\0';

  // Save type of entry
  *etype = file_data->dir_data [file_data->dir_index].attribute;

  // Set state of empty flag based on if the directory is used
  if (file_data->dir_data [file_data->dir_index].name [0] ==
      DIR_FREE)
  {
    // This is the last entry in the directory
    *empty = 1;
    *last = 1;

    // Loop back to first entry number
    file_data->dir_index = 0;
  }
  else if (file_data->dir_data [file_data->dir_index].name [0] ==
           (char) DIR_ERASED)
  {
    *empty = 1;
    file_data->dir_index++;
  }
  else
  {
    *empty = 0;
    index = file_data->dir_index;
    file_data->dir_index++;
  }

  // Rollover dir index if it goes too far (should never make it here)
  if (file_data->dir_index >=
      file_data->fat_data->pat_hdr.root_entries)
  {
    fat16_set_dir_index(file_data, 0);
  }

  return index;
}

/***********************************************************************
 *
 * Function: fat16_set_dir_index
 *
 * Purpose:
 *  Resets the directory index to a location of the directory (used
 *  with get_dirname)
 *
 * Processing:
 *  See function.
 *
 * Parameters:
 *  file_data : Pointer to a file data structure
 *  index     : DIR entry index to set the active dir entry to
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
void fat16_set_dir_index(FILE_TYPE *file_data, INT_32 index)
{
  // Limit index value to valid range
  if ((index >= 0) &&
      (index < file_data->fat_data->pat_hdr.root_entries))
  {
    file_data->dir_index = index;
  }
}

/***********************************************************************
 *
 * Function: fat16_delete
 *
 * Purpose:
 *  Deletes a file in the active directory.
 *
 * Processing:
 *  See function.
 *
 * Parameters:
 *  file_data : Pointer to a file data structure
 *  name      : Name of file to delete
 *
 * Outputs:
 *  None
 *
 * Returns:
 *  '1' if the operation was successful, '0' otherwise.
 *
 * Notes:
 *  None
 *
 **********************************************************************/
INT_32 fat16_delete(FILE_TYPE *file_data, CHAR *name)
{
  INT_32 dir_index, loop;
  UNS_16 cluster_num, next_cluster;
  INT_32 valid = 0;

  // Find the directory entry for the file
  dir_index = fat16_find_file(name, file_data);

  // Continue if the directory is valid
  if (dir_index >= 0)
  {
    // Get first cluster number
    cluster_num = file_data->dir_data [dir_index].clusternum;

    valid = 1;

    // Continue until all clusters are cleared
    loop = 1;
    while (loop == 1)
    {
      // Get next cluster
      next_cluster = fat16_get_next_cluster(
                       file_data->fat_data, cluster_num);

      // Free this cluster
      file_data->fat_data->clusters [cluster_num] = CLUSTER_AV;

      // Exit conditions
      if (next_cluster > CLUSTERR_MAX)
      {
        loop = 0;
      }
      else
      {
        cluster_num = next_cluster;
      }
    }

    // Free up the directory entry
    file_data->dir_data [dir_index].name [0] = DIR_ERASED;

    // Update FAT and directory update flags
    file_data->dir_commit = 1;
    file_data->fat_data->fat_commit = 1;
  }

  return valid;
}

/***********************************************************************
 *
 * Function: fat16_open_file
 *
 * Purpose:
 *  Open a file for reading or writing.
 *
 * Processing:
 *  See function.
 *
 * Parameters:
 *  name      : Name of file
 *  file_data : Pointer to a FILE data structure to use
 *  mode      : File mode (FREAD or FWRITE)
 *
 * Outputs:
 *  None
 *
 * Returns:
 *  '1' if the operation was successful, '0' otherwise.
 *
 * Notes:
 *  None
 *
 **********************************************************************/
INT_32 fat16_open_file(CHAR *name, FILE_TYPE *file_data, INT_32 mode)
{
  INT_32 dir_index, index;
  INT_32 valid = 0;

  // If file is already open, then exit with error
  if (file_data->fmode == FINVALID)
  {
    // Find the file or see if it already exists
    dir_index = fat16_find_file(name, file_data);

    // Some OSes cannot read the file if the name is not in
    // upercase, so convert it just to be compatible
    index = 0;
    while (name [index] != '\0')
    {
      if ((name [index] >= 'a') && (name [index] <= 'z'))
      {
        name [index] = name [index] - 'a' + 'A';
      }
      index++;
    }

    // Reset buffer pointer index
    file_data->buf_index = 0;

    if ((mode == FREAD) && (dir_index >= 0))
    {
      // Read operation
      // Make sure the type is a archive file
      if ((file_data->dir_data [dir_index].attribute &
           ATTB_ARCHIVE) != 0)
      {
        // Save active file entry
        file_data->file_dir_entry = dir_index;

        // Set cluster pointer to start of file
        file_data->clusternum =
          file_data->dir_data [dir_index].clusternum;
        file_data->fmode = FREAD;

        // Save filesize
        file_data->filesize =
          file_data->dir_data [dir_index].filesize;

        // Perform an initial cluster read
        fat16_read_sectors(file_data->fat_data,
                           file_data->data,
                           fat16_translate_cluster_to_sector(
                             file_data->fat_data, file_data->clusternum),
                           (UNS_32) file_data->fat_data->pat_hdr.sectors_cluster);

        valid = 1;
      }
    }
    else if (mode == FWRITE)
    {
      // Write mode
      if (dir_index >= 0)
      {
        // File already exists, so delete existing versions
        fat16_delete(file_data, name);
      }

      // Find a free cluster to point the file to
      file_data->clusternum = fat16_find_free_cluster(
                                file_data->fat_data, 0);

      // Only continue if there is enough room left
      if (file_data->clusternum != 0)
      {
        // There is some room left, add directory
        dir_index = fat16_get_free_dir_entry(file_data);
        file_data->file_dir_entry = dir_index;

        // Only continue if entry was valid
        if (dir_index >= 0)
        {
          valid = 1;
          file_data->fmode = FWRITE;

          // Clear initial filesize of written data
          file_data->filesize = 0;

          // Save some basic directory information
          file_data->dir_data [dir_index].attribute =
            ATTB_ARCHIVE;
          file_data->dir_data [dir_index].createtimems = 0;
          file_data->dir_data [dir_index].createtime = 0;
          file_data->dir_data [dir_index].createdate = 0;
          file_data->dir_data [dir_index].createtimems = 0;
          file_data->dir_data [dir_index].accessdate = 0;
          file_data->dir_data [dir_index].clusterhi = 0;
          file_data->dir_data [dir_index].updatetime =
            DEFAULT_CR_TIME;
          file_data->dir_data [dir_index].updatedate =
            DEFAULT_CR_DATE;
          file_data->dir_data [dir_index].filesize = 0;

          // Normally, the reserved field should be set to 0x0,
          // but some NT systems will not correctly recognize
          // the file is not set to 0x18.
          file_data->dir_data [dir_index].reserved1 = 0x18;

          // Save starting cluster for this file
          file_data->dir_data [dir_index].clusternum =
            file_data->clusternum;

          // Convert name to correct space padded format
          fat16_name_break(name,
                           file_data->dir_data [dir_index].name);

          // Also tag the present cluster as 'used' by linking
          // to itself
          file_data->fat_data->clusters [file_data->clusternum] =
            file_data->clusternum;
        }
      }
    }
  }

  return valid;
}

/***********************************************************************
 *
 * Function: fat16_read
 *
 * Purpose:
 *  Read data from a file.
 *
 * Processing:
 *  See function.
 *
 * Parameters:
 *  file_data     : Pointer to a file data structure
 *  bytes_to_copy : Number of bytes to copy
 *  buffer_ptr    : Pointer to buffer to copy
 *  bytes_copied  : Pointer to where to return number of bytes copied
 *  eof           : Pointer to end of file flag, set on eof
 *
 * Outputs:
 *  None
 *
 * Returns:
 *  '1' if the operation was successful, '0' otherwise.
 *
 * Notes:
 *  None
 *
 **********************************************************************/
INT_32 fat16_read(FILE_TYPE *file_data, INT_32 bytes_to_copy,
                  void *buffer_ptr, INT_32 *bytes_copied, INT_32 *eof)
{
  INT_32 index;
  UNS_8 *buffer = (UNS_8 *) buffer_ptr;
  INT_32 valid = 0;

  // Set up initial values for function
  *bytes_copied = 0;
  *eof = 0;

  // Only continue if the file is open for read operations
  if (file_data->fmode == FREAD)
  {
    valid = 1;
    index = 0;

    // Copy out the specified number of bytes or until file end
    while ((bytes_to_copy > 0) && (file_data->filesize > 0))
    {
      // Does a new cluster need to be read in?
      if (file_data->buf_index >=
          (INT_32) file_data->fat_data->cfat.cluster_size)
      {
        // Entire cluster buffer has been emptied, buffer a
        // new cluster and reset index
        file_data->buf_index = 0;
        file_data->clusternum = fat16_get_next_cluster(
                                  file_data->fat_data, file_data->clusternum);
        fat16_read_sectors(file_data->fat_data,
                           file_data->data,
                           fat16_translate_cluster_to_sector(
                             file_data->fat_data, file_data->clusternum),
                           (UNS_32) file_data->fat_data->pat_hdr.sectors_cluster);
      }
      else
      {
        // Start reading data up to the filesize limit or the
        // copy size, whichever comes first
        while ((bytes_to_copy > 0) &&
               (file_data->filesize > 0) &&
               (file_data->buf_index <
                file_data->fat_data->cfat.cluster_size))
        {
          buffer [index] =
            file_data->data [file_data->buf_index];

          // Update buffered index, filesize, and bytes copied
          index++;
          file_data->buf_index++;
          file_data->filesize--;
          bytes_to_copy--;
          *bytes_copied = *bytes_copied + 1;
        }
      }
    }
  }

  // If there were no errors, and total file was copied, set EOF flag
  if ((valid == 1) && (file_data->filesize == 0))
  {
    *eof = 1;
  }

  return valid;
}

/***********************************************************************
 *
 * Function: fat16_write
 *
 * Purpose:
 *  Write data to a file.
 *
 * Processing:
 *  See function.
 *
 * Parameters:
 *  file_data     : Pointer to a file data structure
 *  bytes_to_copy : Number of bytes to write
 *  buffer_ptr    : Pointer to buffer to copy
 *
 * Outputs:
 *  None
 *
 * Returns:
 *  '1' if the operation was successful, '0' if the device is out of
 *  storage space.
 *
 * Notes:
 *  None
 *
 **********************************************************************/
INT_32 fat16_write(FILE_TYPE *file_data, void *buffer_ptr,
                   INT_32 bytes_to_copy)
{
  INT_32 index;
  UNS_16 next_cluster, save_cluster;
  UNS_8 *buffer = (UNS_8 *) buffer_ptr;
  INT_32 valid = 0;

  // Only continue if the file is open for write operations
  if (file_data->fmode == FWRITE)
  {
    valid = 1;
    index = 0;

    // Continue copying until buffer is empty or an error occurs
    while ((bytes_to_copy > 0) && (valid == 1))
    {
      // Is the data ready to write?
      if (file_data->buf_index >=
          file_data->fat_data->cfat.cluster_size)
      {
        // Buffer is the size of a cluster, write data
        fat16_write_sectors(file_data->fat_data,
                            file_data->data,
                            fat16_translate_cluster_to_sector(
                              file_data->fat_data, file_data->clusternum),
                            (UNS_32) file_data->fat_data->pat_hdr.sectors_cluster);

        // Clear buffer index
        file_data->buf_index = 0;

        // Get next free cluster number
        next_cluster = fat16_find_free_cluster(
                         file_data->fat_data, file_data->clusternum);

        // Is cluster valid (indicating the device is not full)?
        if (next_cluster > 0)
        {
          // Cluster is good, setup link to cluster
          file_data->fat_data->clusters [file_data->clusternum]
          = next_cluster;
          file_data->clusternum = next_cluster;

          // Temporarily tag present cluster as 'pointing to
          // itself' so the clear logic can clear it if needed
          file_data->fat_data->clusters [next_cluster] =
            next_cluster;
        }
        else
        {
          // No more free clusters, error
          valid = 0;

          // Since there is no more room, remove the file from
          // the device completely (by clearing the directory
          // entry and allocated FAT clusters)
          file_data->dir_data [file_data->file_dir_entry].name [0] =
            DIR_ERASED;

          // Return the used clusters to 'free'
          next_cluster =
            file_data->dir_data [file_data->file_dir_entry].clusternum;
          while (next_cluster != 0)
          {
            save_cluster = fat16_get_next_cluster(
                             file_data->fat_data, next_cluster);
            file_data->fat_data->clusters [next_cluster] = 0;
            next_cluster = save_cluster;
          }

          // The file descriptor is no longer valid
          file_data->fmode = FINVALID;
        }
      }
      else
      {
        // Keep moving data into the work buffer
        file_data->data [file_data->buf_index] = buffer [index];

        // update counters
        index++;
        file_data->buf_index++;
        file_data->filesize++;
        bytes_to_copy--;
      }
    }
  }

  return valid;
}

/***********************************************************************
 *
 * Function: fat16_close_file
 *
 * Purpose:
 *  Close a file that was open for reading or writing.
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
 *  Nothing
 *
 * Notes:
 *  None
 *
 **********************************************************************/
void fat16_close_file(FILE_TYPE *file_data)
{
  INT_32 i;

  // If the last operation was a write operation, update the DIR
  // structure
  if (file_data->fmode == FWRITE)
  {
    // Fill the reset of the buffer with zeros, and write the
    // buffer out to the device
    for (i = file_data->buf_index;
         i < file_data->fat_data->cfat.cluster_size; i++)
    {
      file_data->data [i] = 0;
    }

    // Write cluster to device
    fat16_write_sectors(file_data->fat_data,
                        file_data->data,
                        fat16_translate_cluster_to_sector(
                          file_data->fat_data, file_data->clusternum),
                        (UNS_32) file_data->fat_data->pat_hdr.sectors_cluster);

    // Set the present cluster to the last flasg in the list
    file_data->fat_data->clusters [file_data->clusternum] =
      CLUSTER_MAX;

    // Update file size in directory structure
    file_data->dir_data [file_data->file_dir_entry].filesize =
      file_data->filesize;

    // Set cached dir change and FAT cluster flags
    file_data->dir_commit = 1;
    file_data->fat_data->fat_commit = 1;
  }

  file_data->fmode = FINVALID;
}

/***********************************************************************
 *
 * Function: fat16_get_active_mbr
 *
 * Purpose:
 *  Returns an index to the first FAT partition.
 *
 * Processing:
 *  See function.
 *
 * Parameters:
 *  file_data       : Pointer to a file data structure
 *  use_active_only : Flag that indicates that active partions are used
 *  support_no_mbr  : Flag that allows MBR-less device support
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
INT_32 fat16_get_active_mbr(FAT_DEVICE_TYPE *fat_data,
                            INT_32 use_active_only, INT_32 support_no_mbr)
{
  INT_32 i = 0, active = -2;
  UNS_8 data [PTAB_SIZE];

  // Loop until first valid partition found or until all have been
  // processed
  while (active == -2)
  {
    if ((fat_data->part [i].partype == FAT16_LT32M) ||
        (fat_data->part [i].partype == FAT16_EXDOS) ||
        (fat_data->part [i].partype == FAT16_GT32M))
    {
      // Valid partition flag
      if (use_active_only == 0)
      {
        active = i + 1;
      }
      else if ((fat_data->part [i].state & PART_ACTV) != 0)
      {
        active = i + 1;
      }
    }
    else
    {
      i++;
      if (i >= 4)
      {
        // No partition found
        active = -1;
      }
    }
  }

  // If no partition is usable and support for mbr-less devices is
  // needed, then read sector 0 again
  if ((active == -1) && (support_no_mbr != 0))
  {
    fat16_set_no_mbr(fat_data);

    // Read sector 1 again
    fat16_read_sectors(fat_data, &data, 1, 1);

    // Is extended signature valid?
    if (data [EXTENDED_SIG_IDX] == EXTENDED_SIG)
    {
      active = 1;
    }
  }

  return active;
}

/***********************************************************************
 *
 * Function: fat16_save_all
 *
 * Purpose:
 *  Shutdown the FAT16 interface for the selected device.
 *
 * Processing:
 *  If the commit flag is set, write the cached FAT cluster table back
 *  to the device. Free the allocated memory for the cluster table and
 *  device structure.
 *
 * Parameters:
 *  fat_data : Pointer to a FAT data structure
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
void fat16_save_all(FILE_TYPE *file_data, FAT_DEVICE_TYPE *fat_data)
{

  // Write the active directory back to the device is it has
  // changed
  if (file_data->dir_commit == 1)
  {
    fat16_write_sectors(file_data->fat_data,
                        file_data->dir_data, file_data->sector_dir,
                        file_data->fat_data->cfat.root_sectors);
  }

  // If the commit flag in the FAT device structure is set, then a
  // write operation occurred to the device and the cached FAT cluster
  // table needs to be written back to the device before shutdown
  if (fat_data->fat_commit != 0)
  {
    // Write FAT back to the device - although only FAT1 is used,
    // this routine will write all FAT copies back to the device
    // so they stay consistent in other machines
    fat16_write_sectors(fat_data, fat_data->clusters,
                        fat_data->cfat.first_fat1_sector,
                        fat_data->cfat.fat_sectors);

    // Does more than 1 FAT need to be written?
    if (fat_data->pat_hdr.fat_copies > 1)
    {
      fat16_write_sectors(fat_data, fat_data->clusters,
                          fat_data->cfat.first_fat2_sector,
                          fat_data->cfat.fat_sectors);
    }
  }

}

/***********************************************************************
 *
 * Function: fat16_seek
 *
 * Purpose:
 *  Seek data pointer.
 *
 * Processing:
 *  See function.
 *
 * Parameters:
 *  file_data     : Pointer to a file data structure
 *  bytes_to_copy : Number of bytes to copy
 *  buffer_ptr    : Pointer to buffer to copy
 *  bytes_copied  : Pointer to where to return number of bytes copied
 *  eof           : Pointer to end of file flag, set on eof
 *
 * Outputs:
 *  None
 *
 * Returns:
 *  '1' if the operation was successful, '0' otherwise.
 *
 * Notes:
 *  None
 *
 **********************************************************************/
INT_32 fat16_seek(FILE_TYPE *file_data, INT_32 seek_bytes)
{
  INT_32 valid = 1;
  INT_32 remain_buf_data;

  // Copy out the specified number of bytes or until file end
  while (seek_bytes > 0)
  {
    remain_buf_data = file_data->fat_data->cfat.cluster_size - file_data->buf_index;
    if (seek_bytes > remain_buf_data)
    {
      file_data->buf_index = 0;
      file_data->clusternum = fat16_get_next_cluster(
                                file_data->fat_data, file_data->clusternum);
      if (seek_bytes < (file_data->fat_data->cfat.cluster_size * 2))
      {
        fat16_read_sectors(file_data->fat_data,
                           file_data->data,
                           fat16_translate_cluster_to_sector(
                             file_data->fat_data, file_data->clusternum),
                           (UNS_32) file_data->fat_data->pat_hdr.sectors_cluster);
      }
      seek_bytes -= remain_buf_data;
      file_data->filesize -= remain_buf_data;
    }
    else
    {
      file_data->buf_index += seek_bytes;
      file_data->filesize -= seek_bytes;
      seek_bytes = 0;
    }
  }

  return valid;
}

