/*----------------------------------------------------------------------------
 * U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 * Name:    mscuser.h
 * Purpose: Mass Storage Class Custom User Definitions
 * Version: V1.20
 *----------------------------------------------------------------------------
 * This software is supplied "AS IS" without any warranties, express,
 * implied or statutory, including but not limited to the implied
 * warranties of fitness for purpose, satisfactory quality and
 * noninfringement. Keil extends you a royalty-free right to reproduce
 * and distribute executable files created using this software for use
 * on NXP ARM microcontroller devices only. Nothing else gives
 * you the right to use this software.
 *
 * Copyright (c) 2008 Keil - An ARM Company. All rights reserved.
 * Adaption to LPCxxxx, Copyright (c) 2009 NXP.
 *----------------------------------------------------------------------------*/
#include <lpc_types.h>

#ifndef __MSCUSER_H__
#define __MSCUSER_H__


/* Mass Storage Memory Layout */
/* MSC Disk Image Definitions */
/* Mass Storage Memory Layout */
#define DIR_ENTRY           32
#define BLOCKS_PER_CLUSTER  16
#define MSC_BlockSize       512
//#define MAX_FILE_SZ	        (32 * 1024 * 1024)
#define BOOT_SECT_SIZE 	    MSC_BlockSize
#define ROOT_DIR_SIZE       (MSC_BlockSize * 1)
#define FAT_SIZE	          (MSC_BlockSize * 2)

#define MSC_ImageSize	      (BOOT_SECT_SIZE + FAT_SIZE + ROOT_DIR_SIZE + MSC_BlockSize)
#define MSC_MemorySize      ((UNS_32)(31 * 1024 * 1024)) //( BOOT_SECT_SIZE + FAT_SIZE + ROOT_DIR_SIZE + MAX_FILE_SZ ) 
#define MSC_BlockCount      (MSC_MemorySize / MSC_BlockSize)
#define NO_OF_CLUSTERS 	    (MSC_BlockCount/BLOCKS_PER_CLUSTER)


/* Max In/Out Packet Size */
#define MSC_FS_MAX_PACKET  64
#define MSC_HS_MAX_PACKET  512

/* MSC In/Out Endpoint Address */
#define MSC_EP_IN       0x81
#define MSC_EP_OUT      0x01

extern UNS_8*  Memory;  /* MSC RAM */
extern const UNS_8 DiskImage[MSC_ImageSize];  /* Initial fat table & MBR */

/* MSC Requests Callback Functions */
extern UNS_32 MSC_Reset(void);
extern UNS_32 MSC_GetMaxLUN(void);

/* MSC Bulk Callback Functions */
extern void MSC_GetCBW(void);
extern void MSC_SetCSW(void);
extern void MSC_BulkIn(void);
extern void MSC_BulkOut(void);
extern void MSC_BulkOutNak(void);


#endif  /* __MSCUSER_H__ */
