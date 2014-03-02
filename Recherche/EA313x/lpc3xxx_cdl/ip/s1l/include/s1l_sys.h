/***********************************************************************
 * $Id:: s1l_sys.h 4853 2010-09-08 23:32:52Z usb10132                  $
 *
 * Project: System structures and functions
 *
 * Description:
 *     Various system specific items
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

#include "lpc_types.h"
#include "lpc_nandflash_params.h"

#ifndef S1L_SYS_H
#define S1L_SYS_H

/* Enables history support, saves a little space if undefined */
#define HISTORY_ENABLE

/***********************************************************************
 * Load/boot image types and sources
 **********************************************************************/

/* Possible file load types */
typedef enum 
{
	FLT_RAW, FLT_BIN, FLT_SREC, FLT_ELF, FLT_NONE
} FILE_LOAD_T;

/* Autoboot sources */
typedef enum
{
	SRC_TERM, SRC_NAND, SRC_BLKDEV, SRC_NONE
} SRC_LOAD_T;

/***********************************************************************
 * Persistent data structures saved across S1L sessions
 **********************************************************************/

/* Structure for image saved in FLASH */
typedef struct 
{
	UNS_32 block_first; /* Where file is stored */
	UNS_32 blocks_used; /* Blocks used */
	UNS_32 secs_used;   /* Sectors used */
	UNS_32 num_bytes;   /* Number of bytes used */
	UNS_32 loadaddr;    /* Where file is loaded in memory */
	UNS_32 startaddr;   /* Executable entry address */
	BOOL_32 valid;      /* Image is valid flag */
} FLASH_SAVE_T;

/* Autoboot configuration */
typedef struct 
{
	SRC_LOAD_T abootsrc; /* Autoboot source */
	FILE_LOAD_T flt;     /* Autoboot file type */
	UNS_8 fname [16];    /* Filename for block device */
	UNS_32 loadaddr;
	UNS_32 startaddr;
} ABOOT_SETUP_T;

/* Scripted startup support */
#define MAX_SCRIPT_ENTRIES 12
typedef struct
{
	BOOL_32 enabled;
	int number_entries;
	int entry_size[MAX_SCRIPT_ENTRIES];
	UNS_8 script_data[256 + 12];
} S1L_SCRIPT_T;

/* Save system setup */
typedef struct 
{
	UNS_8 prmpt[16];     /* Prompt name */
	int prmpt_to;        /* Prompt timeout on boot */
	UNS_32 baudrate;     /* Serial port baud rate */
	FLASH_SAVE_T fsave;  /* Saved FLASH image */
	ABOOT_SETUP_T aboot; /* Autoboot setup */
	S1L_SCRIPT_T scr;    /* Startup scripts */
} S1L_CFG_T;
extern S1L_CFG_T syscfg;

/* Sets or resets default system configuration */
void cfg_default(S1L_CFG_T *pCfg);

/***********************************************************************
 * Runtime S1L data and support functions
 **********************************************************************/

/* Structure populated by the system that is used to configure S1L
   at runtime */
typedef struct
{
	UNS_8 default_prompt [16];
	UNS_8 system_name [64];
	/* Number of blocks dedicated to the bootloader */
	UNS_32 bl_num_blks;
	/* Maximum amount of blocks available for saved FLASH applications
	   stored after bootloader blocks, use 0xFFFFFFFF for maximum
	   blocks */
	UNS_32 app_num_blks;
} S1L_SYS_RTCFG_T;

/* Structure defining the image loaded in memory */
typedef struct 
{
	UNS_32 loadaddr;    /* Address that the file is loaded at */
	FILE_LOAD_T flt;    /* Type of file loaded */
	UNS_32 num_bytes;   /* Size of the file in bytes */
	PFV startaddr;      /* Loaded file's executable entry address */
	BOOL_32 contiguous; /* Image is contiguous */
	BOOL_32 loaded;     /* File loaded flag */
} FILE_DATA_T;

/* Volatile system information structure */
typedef struct
{
	NAND_GEOM_T *nandgeom;
	S1L_SYS_RTCFG_T sysrtcfg;
	FILE_DATA_T lfile;
} S1L_VSYSCFG_T;
extern S1L_VSYSCFG_T sysinfo;

/* Moves image in memory to FLASH application load region */
BOOL_32 memimage_to_flash(FILE_DATA_T *fdata);

/* Moves FLASH application in load region to memory */
BOOL_32 flashimage_to_mem(FILE_DATA_T *fdata);

/* Load a raw file from a source to memory */
BOOL_32 raw_load(FILE_DATA_T *fdata,
                 UNS_32 addr,
                 UNS_8 *filename,
                 SRC_LOAD_T src);

/* Load and parse an S-record file */
BOOL_32 srec_parse(FILE_DATA_T *ft,
                   SRC_LOAD_T src,
                   UNS_8 *filename);

/***********************************************************************
 * Exception handler structures and functions
 **********************************************************************/

/* Exception data structure */
typedef struct
{
	UNS_32 cpsr;
	UNS_32 spsr;    /* Previous status */
	UNS_32 lr;      /* Exception address */
	UNS_32 r[13];   /* Registers */
} EXCEPT_DATA_T;
extern EXCEPT_DATA_T excdata;

/* Displays exception data when an exception occurs */
void trap_exception(void);

/* Prototype for exception pre-handler */
void arm9_exchand(void);

/***********************************************************************
 * Miscellaneous functions and data
 **********************************************************************/

/* A sector of data (enough for large block) */
extern UNS_8 secdat[2048 + 64];

/* Boot manager - entry point to S1L */
void boot_manager(BOOL_32 allow_boot);

/* Autoboot handler - autoboots the system is configured */
BOOL_32 autoboot(void);

/* Convert a FLASH block and page values to a sector number */
UNS_32 conv_to_sector(UNS_32 block,
					  UNS_32 page);

/* Moves data from memory to FLASH */
BOOL_32 mem_to_nand(UNS_32 starting_sector,
					void *buff,
					UNS_32 bytes);

/* Moves data from FLASH to memory */
BOOL_32 nand_to_mem(UNS_32 starting_sector,
					void *buff,
					UNS_32 bytes);

/* Saves the current image to FLASH */
BOOL_32 flash_image_save(void);

/* Loads the current image from FLASH */
BOOL_32 flash_image_load(void);

/* Flag used to force prompt exit and load image execution */
extern BOOL_32 menuexit;

#endif /* S1L_SYS_H */
