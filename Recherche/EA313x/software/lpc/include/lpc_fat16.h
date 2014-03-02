/**********************************************************************
 * $Id:: lpc_fat16.h 745 2008-05-13 19:59:29Z pdurgesh                 $
 *
 * Project: FAT16 driver
 *
 * Description:
 *  This package contains a set of functions to provide simple
 *  management functions for FAT16 devices, such as a Compact Flash
 *  card or MMC card. The actual device type does not matter, and a
 *  set of standard routines are needed to bind the device driver
 *  to this FAT16 driver.
 *
 *  This driver supports functions only related to FAT16 functionality
 *  and has very simple error checking. Some file related functions
 *  that are normally not included in the FAT16 layer are included in
 *  this driver to keep functionality simple. MBR functions are also
 *  included as part of the FAT16 driver for convienence only.
 *
 *  The following functions are supported in this driver:
 *     FAT16 to device binding (initialization and shutdown)
 *        Get device partition data (active status, type)
 *        Mount partition/filesystem
 *     Set an active directory
 *        Reset a directory pointer to the head of directory table
 *        Get a directory entry (may be a file or other directory)
 *     File operations (operations occur in active directory)
 *        Open a file, read data from a file, close file
 *        Create a file, write data to a file, close file
 *        Delete a file.
 *
 *  Use of this driver is explained in the fat16.txt document. There
 *  are limitations with this driver - read the fat16.txt file for
 *  important information on opening multiple files.
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
 *********************************************************************/
#ifndef LPC_FAT16_H
#define LPC_FAT16_H

#include "lpc_types.h"

#if defined (__cplusplus)
extern "C"
{
#endif

/***********************************************************************
 * Defines
 **********************************************************************/
#define FAT12        0x01    // Partition type FAT12
#define FAT16_LT32M  0x04    // Partition type FAT16 size less than 32M
#define FAT16_EXDOS  0x05    // Partition type extended MSDOS
#define FAT16_GT32M  0x06    // Partition type FAT16 size more than 32M

#define PART_ACTV    0x80    // Partition active flag bit

// Return values for some functions
#define DSIZE        16      // Device name string size

// Cluster flags
#define CLUSTER_AV   0x0000  // Cluster available
#define CLUSTERU_MIN 0x0002  // Minimum cluster chain range
#define CLUSTERU_MAX 0xFFEF  // Maximum cluster chain range
#define CLUSTERR_MIN 0xFFF0  // Minimum reserved cluster flag
#define CLUSTERR_MAX 0xFFF6  // Maximum reserved cluster flag
#define CLUSTER_BAD  0xFFF7  // Bad cluster flag
#define CLUSTER_LAST 0xFFF8  // Minimum (16-bit) value for last cluster
#define CLUSTER_MAX  0xFFFF  // Maximum amount of cluster entries

// Directory structure/file attributes
#define ATTB_NORMAL  0x00    // Normal file type (no bits set)
#define ATTB_RO      0x01    // Read only bit
#define ATTB_SYS     0x02    // System file bit
#define ATTB_HIDDEN  0x04    // Hidden file bit
#define ATTB_VOLUME  0x08    // Volume bit
#define ATTB_DIR     0x10    // Directory bit
#define ATTB_ARCHIVE 0x20    // Archive bit
#define ATTB_LFN     0x0F    // LFN entry flag

// Directory entry flags
#define DIR_FREE     0x00    // Free directory entry
#define DIR_ERASED   0xE5    // Erased (free) directory entry

// Default creation time/date for files
#define DEFAULT_CR_TIME 0xC000 // 12:00:00
#define DEFAULT_CR_DATE 0x2C21 // January 1, 2002

// FAT16 extended signature
#define EXTENDED_SIG    0x29
#define EXTENDED_SIG_IDX 0x26  // Extended signature index in data

/***********************************************************************
  Initialization functions
 **********************************************************************/
// Directory structure root entry stored on device
typedef struct
{
  CHAR     name [8];       // Left space padded name
  CHAR     ext [3];        // Left space padded extension
  UNS_8    attribute;      // File attribute
  UNS_8    reserved1;
  UNS_8    createtimems;   // timestamp in 10mS
  UNS_16   createtime;     // timestamp, time
  UNS_16   createdate;     // timestamp, date
  UNS_16   accessdate;     // Last date of access
  UNS_16   clusterhi;      // High cluster (FAT32 only)
  UNS_16   updatetime;     // Last time of change
  UNS_16   updatedate;     // Last date of change
  UNS_16   clusternum;     // Cluster link number
  UNS_32   filesize;       // Size of file in bytes
} ROOT_ENTRY_TYPE;

// Partition entries
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
} PARTITION_TYPE;

// Drive geometry structure for partition, filled in by the driver.
// (Not everything in this sector is saved)
typedef struct
{
  UNS_8    jump [3];       // Boot code jump point
  UNS_8    oem_id [8];     // Name of formatting OS
  UNS_16   bytes_sector;   // Bytes per sector
  UNS_8    sectors_cluster;// Sectors per cluster
  UNS_16   res_sectors;    // Reserved sectors from start
  UNS_8    fat_copies;     // Number of FAT copies
  UNS_16   root_entries;   // Number of root entries
  UNS_16   small_sectors;  // Small number of sectors
  UNS_8    media_desc;     // Media descriptor
  UNS_16   sectors_fat;    // Sectors per FAT
  UNS_16   sectors_track;  // Sectors per track
  UNS_16   number_heads;   // Number of heads
  UNS_32   hidden_sectors; // Number of hidden sectors
  UNS_32   large_sectors;  // Large number of sectors
  UNS_8    drive_number;   // Drive number
  UNS_8    reserved;
  UNS_8    ext_boot_sig;   // Extended boot signature
  UNS_32   serial_number;  // Volume serial number
  CHAR     label [11];     // Volume label
  CHAR     fs_name [8];    // File system name (FAT16)
} FATGEOM_TYPE;

// The following structure holds computed information about the device
typedef struct
{
  UNS_32   first_boot_sector; // First boot sector
  UNS_32   boot_sectors;      // Total boot sectors
  UNS_32   fat_sectors;       // FAT sectors (single FAT)
  UNS_32   first_fat1_sector; // First FAT1 sector
  UNS_32   last_fat1_sector;  // Last FAT1 sector
  UNS_32   first_fat2_sector; // First FAT2 sector
  UNS_32   last_fat2_sector;  // Last FAT2 sector
  UNS_32   first_root_sector; // First root sector
  UNS_32   root_sectors;      // Total root sectors
  UNS_32   first_data_sector; // First data sector
  UNS_32   total_sectors;     // Total sectors on device
  UNS_32   data_sectors;      // Total data sectors
  UNS_32   clusters;          // Total number of clusters
  UNS_32   total_size;        // Total size of device in bytes
  UNS_16   cluster_size;      // Cluster size in bytes
} FATDATA_TYPE;

// Device function list
typedef void (*vvfunc)(void);
typedef INT_32(*ivfunc)(void);
typedef INT_32(*ivifunc)(void *, INT_32);
typedef struct
{
  ivfunc init_func;
  vvfunc shutdown_func;
  ivfunc insert_ck_func;
  ivfunc ready_ck_func;
  ivfunc busy_ck_func;
  void (*set_sector_func)(UNS_32);
  vvfunc start_read_func;
  vvfunc start_write_func;
  ivifunc read_func;
  ivifunc write_func;
} DEVICE_FUNCS_TYPE;

// FAT device structure, used to bind a device driver to the FAT
// driver
typedef struct
{
  CHAR     device [DSIZE]; // Name of device
  INT_8    act_part;       // Active partition number (0 - 3), or (-1)
  INT_32   fat_commit;     // FAT commit flag, if set, FAT has changed
  PARTITION_TYPE part [4]; // Information about the 4 partitions
  FATGEOM_TYPE pat_hdr;    // Partition header from selected part.
  FATDATA_TYPE cfat;       // Computed FAT architecture data
  DEVICE_FUNCS_TYPE func;  // Pointer to device driver functions
  UNS_16   *clusters;      // Cached cluster table
} FAT_DEVICE_TYPE;

// File modes
typedef enum {FINVALID, FREAD, FWRITE} FILE_MODE_TYPE;

// File descriptor
typedef struct
{
  FILE_MODE_TYPE fmode;    // File operational mode
  INT_32   dir_commit;     // DIR commit flag, if set, DIR has changed
  UNS_16   clusternum;     // Present working cluster number
  UNS_32   filesize;       // File size in bytes
  INT_32   file_dir_entry; // Active file working entry (read/write)
  FAT_DEVICE_TYPE *fat_data; // Pointer to binded FAT structure
  UNS_32   sector_dir;     // Sector number of start of active dir
  UNS_8    *data;          // Pointer to allocated data buffer
  UNS_32   buf_index;      // Buffer read/write index
  ROOT_ENTRY_TYPE *dir_data; // Cached active directory structure
  INT_32   dir_index;      // Directory entry lookup index
} FILE_TYPE;

/***********************************************************************
 * Initialization functions
 **********************************************************************/
// Initializes the FAT16 interface for the selected device
FAT_DEVICE_TYPE * fat16_init_device(
  CHAR *device,              // Name of device
  ivfunc init_func,          // Pointer to device init function
  vvfunc shutdown_func,      // Pointer to device shutdown function
  ivfunc insert_ck_func,     // Pointer to check for insert func
  ivfunc ready_ck_func,      // Pointer for ready? check
  ivfunc busy_ck_func,       // Pointer for busy? check
  void (*set_sector_func)(UNS_32),  // Sector setting
  vvfunc start_read_func,    // Pointer for read start
  vvfunc start_write_func,   // Pointer for write start
  ivifunc read_func,         // Pointer for read of data
  ivifunc write_func);       // Pointer for write of data

// Shutdowns the FAT16 interface for the selected device (will destroy
// the FAT device structure)
void fat16_shutdown(FAT_DEVICE_TYPE *fat_data);

/***********************************************************************
 * MBR functions
 **********************************************************************/
// Get the status of the partition from the MBR
void fat16_get_status(FAT_DEVICE_TYPE *fat_data, UNS_8 *status,
                      UNS_8 *ptype, INT_32 pnum);

// Set the active (FAT16) partition and cache cluster table
INT_32 fat16_set_partition(INT_32 partnum, FAT_DEVICE_TYPE *fat_data);

//**********************************************************************
// File descriptor creation/destroy functions
//**********************************************************************
// Fills a file structure with the device and FAT data - multiple file
// structures can be created to access and control multiple files
FILE_TYPE * fat16_create_new_file_descriptor(
  FAT_DEVICE_TYPE *fat_data);

// Destroys a created file descriptor
void fat16_destroy_file_descriptor(FILE_TYPE *file_data);

/***********************************************************************
 * Directory management functions
 **********************************************************************/
// Set the active directory
INT_32 fat16_cd(CHAR *path, FILE_TYPE *file_data);

// Returns the name and type of the (next) entry in the active
// directory
INT_32 fat16_get_dirname(FILE_TYPE *file_data, CHAR *name,
                         UNS_8 *etype, INT_32 *empty, INT_32 *last);

// Resets the directory index to a location of the directory (used
// with get_dirname)
void fat16_set_dir_index(FILE_TYPE *file_data, INT_32 index);

/***********************************************************************
 * Basic FAT16 filesystem functions
 **********************************************************************/
// Deletes a file in the active directory
INT_32 fat16_delete(FILE_TYPE *file_data, CHAR *name);

// Open a file for reading or writing
INT_32 fat16_open_file(CHAR *name, FILE_TYPE *file_data, INT_32 mode);

// Read data from a file
INT_32 fat16_read(FILE_TYPE *file_data, INT_32 bytes_to_copy,
                  void *buffer_ptr, INT_32 *bytes_copied, INT_32 *eof);

// Write data to a file
INT_32 fat16_write(FILE_TYPE *file_data, void *buffer_ptr,
                   INT_32 bytes_to_copy);

// Close a file that was open for reading or writing, or anything
// else (will destroy the file descriptor)
void fat16_close_file(FILE_TYPE *file_data);

/***********************************************************************
 * Extended/extra functions
 **********************************************************************/
// Returns an index to the first FAT partition
INT_32 fat16_get_active_mbr(FAT_DEVICE_TYPE *fat_data,
                            INT_32 use_active_only, INT_32 support_no_mbr);

void fat16_save_all(FILE_TYPE *file_data, FAT_DEVICE_TYPE *fat_data);

INT_32 fat16_seek(FILE_TYPE *file_data, INT_32 seek_bytes);

#if defined (__cplusplus)
}
#endif /*__cplusplus */

#endif // LPC_FAT16_H
