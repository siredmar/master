/***********************************************************************
 * $Id:: s1l_fat.h 871 2008-07-07 22:36:49Z wellsk                     $
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
#ifndef S1l_FAT_H
#define S1l_FAT_H

#include "lpc_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************************
 * Boot code support functions
 **********************************************************************/

/* Prototype for device init function, returns a boolean */
typedef BOOL_32 (*bvfunc) (void);

/* Prototype for device read function, returns a boolean, accepts a
   pointer to a buffer, and a sector value */
typedef BOOL_32 (*bviifunc) (void *, UNS_32);

/* Initialize device and file system */
BOOL_32 fat_init(void);

/* Close device and file system */
BOOL_32 fat_deinit(void);

/* Open a file for reading */
BOOL_32 fat_file_open(UNS_8 *name);

/* Stream data from an open file */
INT_32 fat_file_read(UNS_8 *buff,
                     INT_32 bytes);

/* Get a directory entry */
BOOL_32 fat_get_dir(UNS_8 *dir,
                    BOOL_32 reset);

#ifdef __cplusplus
}
#endif

#endif /* S1l_FAT_H */
