/***********************************************************************
 * $Id:: s1l_bootmgr.c 5072 2010-09-29 16:41:57Z usb10132              $
 *
 * Project: Boot manager
 *
 * Description:
 *     Boot option control and application bootup support
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

#include "lpc_string.h"
#include "s1l_sys_inf.h"
#include "s1l_sys.h"
#include "s1l_line_input.h"
#include "s1l_cmds.h"

/* System bootup header */
static UNS_8 bdat_msg[] = "Build date: ";
static UNS_8 kp_msg[] =
	"Autoboot in progress, press any key to stop";
static UNS_8 cfggdef_msg[] =
	"Using default system configuration";
static UNS_8 nanderr_msg[] = "Error: No FLASH detected";

/***********************************************************************
 *
 * Function: term_boot
 *
 * Purpose: Load an image from the terminal
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: FALSE if the operation failed, otherwise TRUE.
 *
 * Notes: None
 *
 **********************************************************************/
BOOL_32 term_boot(void) 
{
	FILE_DATA_T fdata;
	BOOL_32 loaded = FALSE;

	switch (syscfg.aboot.flt)
	{
		case FLT_RAW:
			loaded = raw_load(&fdata, syscfg.aboot.loadaddr,
				syscfg.aboot.fname, SRC_TERM);
			sysinfo.lfile.loadaddr = syscfg.aboot.loadaddr;
			sysinfo.lfile.startaddr = (PFV) syscfg.aboot.startaddr;
			break;

		case FLT_SREC:
			loaded = srec_parse(&fdata, SRC_TERM, NULL);
			sysinfo.lfile.loadaddr = fdata.loadaddr;
			sysinfo.lfile.startaddr = (PFV) fdata.startaddr;
			sysinfo.lfile.num_bytes = fdata.num_bytes;
			break;

		case FLT_BIN:
			/* TBD not supported yet */
			break;

		case FLT_ELF:
			/* TBD not supported yet */
			break;

		default:
			break;
	}

	sysinfo.lfile.flt = FLT_RAW;
	sysinfo.lfile.contiguous = FALSE; /* Doesn't matter on autoboot */
	sysinfo.lfile.loaded = loaded;

	return loaded;
}

/***********************************************************************
 *
 * Function: blk_boot
 *
 * Purpose: Load am image from the block device
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: FALSE if the operation failed, otherwise TRUE.
 *
 * Notes: None
 *
 **********************************************************************/
BOOL_32 blk_boot(void) 
{
	FILE_DATA_T fdata;
	BOOL_32 loaded = FALSE;

	switch (syscfg.aboot.flt)
	{
		case FLT_RAW:
			/* Use stored NAND parameters for the load */
			loaded = raw_load(&fdata, syscfg.aboot.loadaddr,
				syscfg.aboot.fname, SRC_BLKDEV);
			sysinfo.lfile.loadaddr = syscfg.aboot.loadaddr;
			sysinfo.lfile.startaddr = (PFV) syscfg.aboot.startaddr;
			sysinfo.lfile.num_bytes = fdata.num_bytes;
			break;

		case FLT_SREC:
			loaded = srec_parse(&fdata, SRC_BLKDEV, syscfg.aboot.fname);
			sysinfo.lfile.loadaddr = fdata.loadaddr;
			sysinfo.lfile.startaddr = (PFV) fdata.startaddr;
			sysinfo.lfile.num_bytes = fdata.num_bytes;
			break;

		case FLT_BIN:
			/* TBD not supported yet */
			break;

		case FLT_ELF:
			/* TBD not supported yet */
			break;

		default:
			break;
	}

	sysinfo.lfile.flt = FLT_RAW;
	sysinfo.lfile.contiguous = FALSE; /* Doesn't matter on autoboot */
	sysinfo.lfile.loaded = loaded;

	return loaded;
}

/***********************************************************************
 *
 * Function: nand_boot
 *
 * Purpose: Load am image from NAND
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: FALSE if the operation failed, otherwise TRUE.
 *
 * Notes: None
 *
 **********************************************************************/
BOOL_32 nand_boot(void) 
{
	FILE_DATA_T fdata;
	BOOL_32 loaded = FALSE;

	switch (syscfg.aboot.flt)
	{
		case FLT_RAW:
			/* Use stored NAND parameters for the load */
			loaded = flash_image_load();
			break;

		case FLT_SREC:
			loaded = srec_parse(&fdata, SRC_NAND, NULL);
			sysinfo.lfile.loadaddr = fdata.loadaddr;
			sysinfo.lfile.startaddr = (PFV) fdata.startaddr;
			sysinfo.lfile.num_bytes = fdata.num_bytes;
			break;

		case FLT_BIN:
			/* TBD not supported yet */
			break;

		case FLT_ELF:
			/* TBD not supported yet */
			break;

		default:
			break;
	}

	sysinfo.lfile.flt = FLT_RAW;
	sysinfo.lfile.contiguous = FALSE; /* Doesn't matter on autoboot */
	sysinfo.lfile.loaded = loaded;

	return loaded;
}
/***********************************************************************
 *
 * Function: autoboot
 *
 * Purpose: Autoload an image
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: FALSE if the operation failed, otherwise TRUE.
 *
 * Notes: None
 *
 **********************************************************************/
BOOL_32 autoboot(void) 
{
	BOOL_32 loaded = FALSE;

	switch (syscfg.aboot.abootsrc) 
	{
		case SRC_TERM:
			loaded = term_boot();
			break;

		case SRC_BLKDEV:
			loaded = blk_boot();
			break;

		case SRC_NAND:
			loaded = nand_boot();
			break;

		default:
			break;
	}

	return loaded;
}

/***********************************************************************
 *
 * Function: boot_manager
 *
 * Purpose: Handle boot configuation and options
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     allow_boot : TRUE to allow the system to autoboot
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void boot_manager(BOOL_32 allow_boot) {
	UNS_8 key, str[255];
	int i, idx;
	UNS_32 secsmt;
	BOOL_32 usedef = FALSE;

	/* Get runtime configuration */
	get_rt_s1lsys_cfg(&sysinfo.sysrtcfg);

	/* Query FLASH */
	sysinfo.nandgeom = flash_init();

	/* Get S1L configuration */
	if (cfg_override() != FALSE)
	{
		cfg_default(&syscfg);
		usedef = TRUE;
	}
	else if (cfg_load(&syscfg) == FALSE)
	{
		cfg_default(&syscfg);
		syscfg.scr.number_entries = 0;
		cfg_save(&syscfg);
		usedef = TRUE;
	}

	/* Initial system setup */
	sys_up();

	if (sysinfo.nandgeom == NULL)
	{
		term_dat_out_crlf(nanderr_msg);
	}

	/* Set saved baud rate */
	term_setbaud(syscfg.baudrate);

	/* Default configuration used? */
	if (usedef != FALSE)
	{
		term_dat_out_crlf(cfggdef_msg);
	}

	/* Display system header */
	term_dat_out_crlf((UNS_8 *) "");
	term_dat_out_crlf(sysinfo.sysrtcfg.system_name);
	term_dat_out(bdat_msg);
	term_dat_out((UNS_8 *) __DATE__);
	term_dat_out((UNS_8 *) " ");
	term_dat_out_crlf((UNS_8 *) __TIME__);

	/* No file currently loaded in memory */
	sysinfo.lfile.loadaddr = 0xFFFFFFFF;
	sysinfo.lfile.flt = FLT_NONE;
	sysinfo.lfile.num_bytes = 0;
	sysinfo.lfile.startaddr = (PFV) 0xFFFFFFFF;
	sysinfo.lfile.loaded = FALSE;

	/* Initialize commands */
	cmd_core_add_commands();
	cmd_image_add_commands();
	cmd_nand_add_commands();
	ucmd_init();

	/* Initialize line prompt and parser */
	key_line_init(syscfg.prmpt);

	/* Prompt usually appears */
	menuexit = FALSE;

	/* Use built in script capability? */
	if ((syscfg.scr.enabled == TRUE) && (syscfg.scr.number_entries > 0)) {
		term_dat_out_crlf((UNS_8 *) "Running built-in script...\n");

		i = idx = 0;
		while (i < syscfg.scr.number_entries) {
			/* Execute commands */
			term_dat_out((UNS_8 *) "-S>");
			term_dat_out_crlf(&syscfg.scr.script_data[idx]);
			cmd_process(&syscfg.scr.script_data[idx]);
			idx = idx + syscfg.scr.entry_size[i] + 1;
			i++;
		}
	}
	else {
		/* In prompt bypass mode? */
		if (syscfg.aboot.abootsrc != SRC_NONE)
			menuexit = allow_boot;

		if ((syscfg.prmpt_to > 0) && (menuexit == TRUE))
		{
			secsmt = get_seconds() + syscfg.prmpt_to;
			term_dat_out_crlf(kp_msg);
			while (get_seconds() < secsmt)
			{
				if (term_dat_in_ready() > 0) {
					term_dat_in(&key, 1);
					menuexit = FALSE;
					secsmt = get_seconds();
				}
			}
		}

		/* Perform autoboot if possible */
		if (menuexit == TRUE) 
		{
			menuexit = autoboot();
		}
	}

	while (menuexit == FALSE) {
		key_get_command(str);
		str_upper_to_lower(str);
		cmd_process(str);
	}

	/* Bring down some system items */
	sys_down();

	/* Execute program */
	jumptoprog(sysinfo.lfile.startaddr);
}
