/***********************************************************************
 * $Id:: s1l_sys_inf.h 3361 2010-05-03 20:12:19Z usb10132              $
 *
 * Project: S1L system interface functions
 *
 * Description:
 *     API prototypes for the common system interface functions that
 *     are needed for the S1L core to function.
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
#include "s1l_sys.h"

#ifndef S1L_SYS_INF_H
#define S1L_SYS_INF_H

/***********************************************************************
 * Terminal interface functions
 **********************************************************************/

/* Send some data on the terminal interface up to NULL character */
void term_dat_out(UNS_8 *dat);

/* Send some data on the terminal interface up to NULL character with
   a linefeed */
void term_dat_out_crlf(UNS_8 *dat);

/* Send a number of characters on the terminal interface */
void term_dat_out_len(UNS_8 *dat, int chars);

/* Read some data from the terminal interface */
int term_dat_in(UNS_8 *buff, int bytes);

/* Determine how many bytes are waiting on the terminal interface */
int term_dat_in_ready(void);

/* Flush data in the terminal input buffer */
void term_dat_flush(void);

/* Initialize terminal I/O interface */
void term_init(void);

/* Shutdown terminal I/O */
void term_deinit(void);

/* Reset terminal baud rate */
void term_setbaud(UNS_32 baud);

/* Check for latched terminal break condition */
BOOL_32 term_break(void);

/***********************************************************************
 * End of terminal interface functions
 **********************************************************************/

/***********************************************************************
 * Start of time measurement functions
 **********************************************************************/

/* Initializes time system */
INT_32 time_init(void);

/* Resets system timer */
void time_reset(void);

/* Starts system timer */
void time_start(void);

/* Stops system timer */
void time_stop(void);

/* Returns current system time value */
UNS_64 time_get(void);

/* Returns base tick rate (ticks per second) of the time counter */
UNS_64 time_get_rate(void);

/***********************************************************************
 * End of time measurement functions
 **********************************************************************/

/***********************************************************************
 * Start of miscellaneous functions
 **********************************************************************/

/* Initialize user defined system commands */
void ucmd_init(void);

/* Extends the output of the "info" command with user data */
void cmd_info_extend(void);

/* Get current hardware count value (in seconds) */
UNS_32 get_seconds(void);

/* System initialization/re-initialization */
void sys_up(void);

/* System De-initialization */
void sys_down(void);

/* Populate runtime system configuration data */
void get_rt_s1lsys_cfg(S1L_SYS_RTCFG_T *psysrt);

/* Jump to passed address (execute program), requires cache flush on
   some systems */
void jumptoprog(PFV progaddr);

/***********************************************************************
 * End of miscellaneous functions
 **********************************************************************/

/***********************************************************************
 * Start of block driver functions
 **********************************************************************/

/* Initialize block device */
BOOL_32 blkdev_init(void);

/* De-initialize block device */
BOOL_32 blkdev_deinit(void);

/* Read a block from the block device */
BOOL_32 blkdev_read(void *buff, UNS_32 sector);

/***********************************************************************
 * End of block driver functions
 **********************************************************************/

/***********************************************************************
 * Start of configuration save/load functions
 **********************************************************************/

/* Save a S1L configuration to non-volatile memory */
BOOL_32 cfg_save(S1L_CFG_T *pCfg);

/* Load an S1L conmfiguariton from non-volatile memory */
BOOL_32 cfg_load(S1L_CFG_T *pCfg);

/* If this is returned as TRUE (when S1L is booted), the S1L
   configuration will return to the default configuration, can be tied
   to a pushbutton state to restore the board to the default state if
   the saved configuration prevents the board from running. */
BOOL_32 cfg_override(void);

/* Reset user configuration data - used to reset user defined persistent
   data that is not stored as part of S1L data */
void cfg_user_reset(void);

/***********************************************************************
 * End of configuration save/load functions
 **********************************************************************/

/***********************************************************************
 * Start of NAND driver functions
 **********************************************************************/

/* Initialize NAND device, returns pointer to an initialized NAND
   structure if good, NULL if bad */
NAND_GEOM_T *flash_init(void);

/* De-initialize FLASH */
void flash_deinit(void);

/* Read a NAND sector, will skip bad blocks, returns >0 on success,
   -1 on fail, or -2 if the block associated with the sector is bad. */
int flash_read_sector(UNS_32 sector, void *buff, void *extra);

/* Writes a NAND sector, will skip bad blocks, returns >0 on success,
   -1 on fail, or -2 if the block associated with the sector is bad. */
int flash_write_sector(UNS_32 sector, void *buff, void *extra);

/* Erase a block */
BOOL_32 flash_erase_block(UNS_32 block);

/* Returns TRUE if the current block is a bad block */
BOOL_32 flash_is_bad_block(UNS_32 block);

/***********************************************************************
 * End of NAND driver functions
 **********************************************************************/

#endif /* S1L_SYS_INF_H */
