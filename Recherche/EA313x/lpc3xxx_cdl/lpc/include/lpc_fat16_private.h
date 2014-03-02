/**********************************************************************
 * $Id:: lpc_fat16_private.h 745 2008-05-13 19:59:29Z pdurgesh         $
 *
 * Project: FAT16 support functions
 *
 * Description:
 *  This package contains support functions for the FAT16 driver.
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
#ifndef LPC_FAT16_PRIVATE_H
#define LPC_FAT16_PRIVATE_H

#include "lpc_types.h"
#include "lpc_fat16.h"

#if defined (__cplusplus)
extern "C"
{
#endif

/***********************************************************************
 * Defines
 **********************************************************************/
#define PTAB_SIZE 512 // Size of MBR and boot records

/***********************************************************************
 * Support functions for the FAT16 driver
 **********************************************************************/
// Moves a number of bytes from a source to destination
void fat16_moveto(void *source, void *dest, INT_32 size);

// Compares two strings for similarity
INT_32 fat16_compare(CHAR *source, CHAR *dest, INT_32 size);

// Wait for the device to go 'unbusy'
void fat16_wait_busy(FAT_DEVICE_TYPE *fat_data);

// Translate a cluster number to a (absolute) sector number
UNS_32 fat16_translate_cluster_to_sector(FAT_DEVICE_TYPE *fat_data,
                                         UNS_16 cluster);

// Reads the FAT MBR and puts the partition tables in the passed
// structure
void fat16_read_mbr(FAT_DEVICE_TYPE *fat_data);

// Reads a number of sectors from a device into a buffer
void fat16_read_sectors(FAT_DEVICE_TYPE *fat_data, void *data,
                        UNS_32 first_sector, UNS_32 num_sectors);

// Writes a number of sectors from a buffer to a device
void fat16_write_sectors(FAT_DEVICE_TYPE *fat_data, void *data,
                         UNS_32 first_sector, UNS_32 num_sectors);

// Finds the next directory name in a path
INT_32 fat16_parse_path(CHAR *path);

// Converts a filename in unpadded 8.3 format to a format that is
// compatible with a directory format
void fat16_name_break(CHAR *full_name, CHAR *name);

// Compares a passed name in padded 8.3 format with a name in a
// directory entry structure
INT_32 fat16_name_check(CHAR *name, ROOT_ENTRY_TYPE *dir_data);

// Finds and returns the directory structure of the passed name
// in the active directory
INT_32 fat16_find_file(CHAR *name, FILE_TYPE *file_data);

// Find the next free cluster in the cluster list. Searches down
// from the passed cluster
UNS_16 fat16_find_free_cluster(FAT_DEVICE_TYPE *fat_data,
                               UNS_16 cluster_start);

// Allocates a new directory entry for the passed name
INT_32 fat16_get_free_dir_entry(FILE_TYPE *file_data);

// Returns the next cluster in a cluster link chain
UNS_32 fat16_get_next_cluster(FAT_DEVICE_TYPE *fat_data,
                              UNS_16 cluster_num);

// Support function to set up the first partition in the driver
// to point to sector 1 for the boot record
void fat16_set_no_mbr(FAT_DEVICE_TYPE *fat_data);

#if defined (__cplusplus)
}
#endif /*__cplusplus */

#endif // LPC_FAT16_PRIVATE_H
