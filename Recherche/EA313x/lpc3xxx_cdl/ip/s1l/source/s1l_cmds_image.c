/***********************************************************************
 * $Id:: s1l_cmds_image.c 1229 2008-10-24 16:07:25Z wellsk             $
 *
 * Project: Command processor
 *
 * Description:
 *     Processes commands from the command prompt
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
#include "s1l_cmds.h"
#include "s1l_line_input.h"
#include "s1l_sys_inf.h"
#include "s1l_fat.h"

/* ls command */
BOOL_32 cmd_ls(void);
static UNS_32 cmd_ls_plist[] =
{
	(PARSE_TYPE_STR | PARSE_TYPE_END) /* The "ls" command */
};
static CMD_ROUTE_T core_ls_cmd =
{
	(UNS_8 *) "ls",
	cmd_ls,
	(UNS_8 *) "Displays files in the root directory of the block device",
	(UNS_8 *) "ls",
	cmd_ls_plist,
	NULL
};

/* load command */
BOOL_32 cmd_load(void);
static UNS_32 cmd_load_plist[] =
{
	(PARSE_TYPE_FREE) /* The "load" command */
};
static CMD_ROUTE_T core_load_cmd =
{
	(UNS_8 *) "load",
	cmd_load,
	(UNS_8 *) "Loads a file into memory from a source",
	(UNS_8 *) "load [source <filename>][type]<load address><exec address>",
	cmd_load_plist,
	NULL
};

/* exec command */
BOOL_32 cmd_exec(void);
static UNS_32 cmd_exec_plist[] =
{
	(PARSE_TYPE_FREE) /* The "exec" command */
};
static CMD_ROUTE_T core_exec_cmd =
{
	(UNS_8 *) "exec",
	cmd_exec,
	(UNS_8 *) "Executes an image loaded in memory or at an address",
	(UNS_8 *) "exec [execution address]",
	cmd_exec_plist,
	NULL
};

/* aboot command */
BOOL_32 cmd_aboot(void);
static UNS_32 cmd_aboot_plist[] =
{
	(PARSE_TYPE_FREE) /* The "aboot" command */
};
static CMD_ROUTE_T core_aboot_cmd =
{
	(UNS_8 *) "aboot",
	cmd_aboot,
	(UNS_8 *) "Sets up the board autoboot configuration",
	(UNS_8 *) "aboot [source <filename>][type]<load address><exec address>",
	cmd_aboot_plist,
	NULL
};

/* boot command */
BOOL_32 cmd_boot(void);
static UNS_32 cmd_boot_plist[] =
{
	(PARSE_TYPE_STR | PARSE_TYPE_END) /* The "boot" command */
};
static CMD_ROUTE_T core_boot_cmd =
{
	(UNS_8 *) "boot",
	cmd_boot,
	(UNS_8 *) "Boots with board with the current autoboot configuration",
	(UNS_8 *) "boot",
	cmd_boot_plist,
	NULL
};

/* Image group */
GROUP_LIST_T image_group =
{
	(UNS_8 *) "image", /* Image group */
	(UNS_8 *) "S1L image processing command group",
	NULL,
	NULL
};

static UNS_8 invalsrc_msg[] = "Invalid source for loading data";
static UNS_8 unkft_msg[] = "Unknown file type: ";
static UNS_8 rawna_msg[] = "Raw files need a load address";
static UNS_8 floaded_msg[] = "File loaded successfully";
static UNS_8 notloaded_msg[] = "File load failed";
extern UNS_8 blkdeverr_msg[];
static UNS_8 bsgood_msg[] = "Autoboot configuration updated";
static UNS_8 bsbad_msg[] = "Autoboot configuration failure/invalid";
static UNS_8 cnoboot_msg[] = "Autoboot is not currrently setup";

/***********************************************************************
 *
 * Function: cmd_load
 *
 * Purpose: Load command
 *
 * Processing:
 *     For the load command, start parsing subcommand elements and
 *     route to the specific handler.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Always returns TRUE
 *
 * Notes: None
 *
 **********************************************************************/
BOOL_32 cmd_load(void) {
	UNS_8 *curp;
	UNS_32 addr;
	BOOL_32 processed = TRUE, loaded = FALSE;
	FILE_DATA_T fdata;
	SRC_LOAD_T src = SRC_TERM;
	INT_32 nexidx;
	UNS_8 *fname;

	if (parse_get_entry_count() >= 3) 
	{
		/* Get source */
		curp = get_parsed_entry(1);
		nexidx = 2;
		if (str_cmp(curp, "term") == 0) 
		{
			/* Clear break */
			term_break();
			src = SRC_TERM;
		}
		else if (str_cmp(curp, "blk") == 0) 
		{
			src = SRC_BLKDEV;
			nexidx = 3;
		}
		else if (str_cmp(curp, "flash") == 0) 
		{
			src = SRC_NAND;
		}
		else 
		{
			term_dat_out_crlf(invalsrc_msg);
			processed = FALSE;
		}

		/* Get file type */
		curp = get_parsed_entry(nexidx);
		fdata.flt = FLT_NONE;
		if (str_cmp(curp, "elf") == 0) 
		{
			fdata.flt = FLT_ELF;
		}
		else if (str_cmp(curp, "raw") == 0) 
		{
			fdata.flt = FLT_RAW;
		}
		else if (str_cmp(curp, "bin") == 0) 
		{
			fdata.flt = FLT_BIN;
		}
		else if (str_cmp(curp, "srec") == 0) 
		{
			fdata.flt = FLT_SREC;
		}
		else 
		{
			fdata.flt = FLT_NONE;
			term_dat_out(unkft_msg);
			term_dat_out_crlf(curp);
			processed = FALSE;
		}

		/* Next index */
		nexidx++;

		/* Handle each file type */
		if (processed == TRUE) 
		{
			/* Get filename */
			fname = get_parsed_entry(2);

			switch (fdata.flt) 
			{
				case FLT_RAW:
					/* Get load address */
					curp = get_parsed_entry(nexidx);
					loaded = str_hex_to_val(curp, &addr);
					if (loaded == TRUE) 
					{
						fdata.loadaddr = addr;
						fdata.startaddr = (PFV) addr;
					}
					else 
					{
						term_dat_out_crlf(rawna_msg);
					}

					/* Start address */
					nexidx++;
					curp = get_parsed_entry(nexidx);
					if (curp != NULL) 
					{
						loaded &= str_hex_to_val(curp, &addr);
						if (loaded == TRUE) 
						{
							fdata.startaddr = (PFV) addr;
						}
					}

					if (loaded == TRUE) 
					{
						loaded = raw_load(&fdata, fdata.loadaddr,
							fname, src);
					}

					processed = TRUE;
					break;

				case FLT_BIN:
					processed = TRUE; /* TBD not supported yet */
					break;

				case FLT_SREC:
					loaded = srec_parse(&fdata, src, fname);
					break;

				case FLT_ELF:
					processed = TRUE; /* TBD not supported yet */
					break;

				default:
					break;
			}
		}
	}

	if (loaded == TRUE) 
	{
		term_dat_out_crlf(floaded_msg);
		sysinfo.lfile.loadaddr = fdata.loadaddr;
		sysinfo.lfile.flt = fdata.flt;
		sysinfo.lfile.num_bytes = fdata.num_bytes;
		sysinfo.lfile.startaddr = fdata.startaddr;
		sysinfo.lfile.contiguous = fdata.contiguous;
		sysinfo.lfile.loaded = TRUE;
	}
	else 
	{
		term_dat_out_crlf(notloaded_msg);
		sysinfo.lfile.loadaddr = 0xFFFFFFFF;
		sysinfo.lfile.flt = FLT_NONE;
		sysinfo.lfile.num_bytes = 0;
		sysinfo.lfile.startaddr = (PFV) 0xFFFFFFFF;
		processed = FALSE;
	}

	return TRUE;
}


/***********************************************************************
 *
 * Function: cmd_ls
 *
 * Purpose: Displays files in root directory of block device
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Always returns TRUE
 *
 * Notes: None
 *
 **********************************************************************/
BOOL_32 cmd_ls(void) 
{
	UNS_8 dirname [32];
	BOOL_32 lp;

	/* Initialize FAT interface first */
	if (fat_init() == FALSE)
	{
		term_dat_out(blkdeverr_msg);
	}
	else
	{
		/* Show items in BLKDEV root directory */
		lp = fat_get_dir(dirname, TRUE);
		while (lp == FALSE) 
		{
			term_dat_out((UNS_8 *) " ");
			term_dat_out_crlf(dirname);
			lp = fat_get_dir(dirname, FALSE);
		}

		fat_deinit();
	}

	return TRUE;
}


/***********************************************************************
 *
 * Function: cmd_exec
 *
 * Purpose: Executes a program or at an address
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: TRUE if the command was good, otherwise FALSE
 *
 * Notes: None
 *
 **********************************************************************/
BOOL_32 cmd_exec(void) 
{
	UNS_8 *curp;
	UNS_32 ea;
	BOOL_32 execgood;

	/* An address was passed, use it */
	if (parse_get_entry_count() == 1) {
		if (sysinfo.lfile.startaddr != (PFV) 0xFFFFFFFF) 
		{
			menuexit = TRUE;
		}
	}
	else if (parse_get_entry_count() == 2) {
		curp = get_parsed_entry(1);
		execgood = str_hex_to_val(curp, &ea);
		if (execgood == TRUE) 
		{
			menuexit = TRUE;
			sysinfo.lfile.startaddr = (PFV) ea;
		}
	}

	return TRUE;
}

/***********************************************************************
 *
 * Function: cmd_aboot
 *
 * Purpose: Sets autoboot source
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Always returns TRUE
 *
 * Notes: None
 *
 **********************************************************************/
BOOL_32 cmd_aboot(void) 
{
	UNS_8 *curp;
	UNS_32 addr;
	BOOL_32 processed = TRUE, ready = FALSE;
	INT_32 nexidx;
	ABOOT_SETUP_T abs;

	abs.fname [0] = '\0';
	if (parse_get_entry_count() == 2)
	{
		curp = get_parsed_entry(1);
		nexidx = 2;
		if (str_cmp(curp, "none") == 0) 
		{
			abs.abootsrc = SRC_NONE;
		}

		ready = TRUE;
	}
	else if (parse_get_entry_count() >= 3) 
	{
		/* Get source */
		curp = get_parsed_entry(1);
		nexidx = 2;
		if (str_cmp(curp, "term") == 0) 
		{
			/* Clear break */
			term_break();
			abs.abootsrc = SRC_TERM;
		}
		else if (str_cmp(curp, "blk") == 0) 
		{
			abs.abootsrc = SRC_BLKDEV;

			/* Get filename */
			curp = get_parsed_entry(2);
			memcpy(abs.fname, curp, str_size(curp));
			abs.fname[str_size(curp)] = '\0';
			nexidx = 3;
		}
		else if (str_cmp(curp, "flash") == 0) 
		{
			abs.abootsrc = SRC_NAND;
		}
		else if (str_cmp(curp, "none") == 0) 
		{
			abs.abootsrc = SRC_NONE;
		}
		else
		{
			term_dat_out_crlf(invalsrc_msg);
			processed = FALSE;
		}

		/* Get file type */
		curp = get_parsed_entry(nexidx);
		abs.flt = FLT_NONE;
		if (str_cmp(curp, "elf") == 0) 
		{
			abs.flt = FLT_ELF;
		}
		else if (str_cmp(curp, "raw") == 0) 
		{
			abs.flt = FLT_RAW;
		}
		else if (str_cmp(curp, "bin") == 0) 
		{
			abs.flt = FLT_BIN;
		}
		else if (str_cmp(curp, "srec") == 0) 
		{
			abs.flt = FLT_SREC;
		}
		else 
		{
			abs.flt = FLT_NONE;
			term_dat_out(unkft_msg);
			term_dat_out_crlf(curp);
			processed = FALSE;
		}

		/* Next index */
		nexidx++;

		/* Handle each file type */
		if (processed == TRUE) 
		{
			switch (abs.flt) 
			{
				case FLT_RAW:
					/* Get load address */
					curp = get_parsed_entry(nexidx);
					ready = str_hex_to_val(curp, &addr);
					if (ready == TRUE) 
					{
						abs.loadaddr = addr;
						abs.startaddr = addr;
					}
					else 
					{
						term_dat_out_crlf(rawna_msg);
					}

					/* Start address */
					nexidx++;
					curp = get_parsed_entry(nexidx);
					if (curp != NULL) 
					{
						ready &= str_hex_to_val(curp, &addr);
						if (ready == TRUE) 
						{
							abs.startaddr = addr;
						}
					}
					break;

				case FLT_BIN:
					ready = FALSE; /* TBD not supported yet */
					processed = TRUE;
					break;

				case FLT_SREC:
					ready = TRUE;
					processed = TRUE;
					break;

				case FLT_ELF:
					ready = FALSE; /* TBD not supported yet */
					processed = TRUE;
					break;

				default:
					break;
			}
		}
	}

	if (ready == TRUE) 
	{
		syscfg.aboot = abs;
		cfg_save(&syscfg);
		term_dat_out_crlf(bsgood_msg);
	}
	else 
	{
		term_dat_out_crlf(bsbad_msg);
	}

	return TRUE;
}

/***********************************************************************
 *
 * Function: cmd_boot
 *
 * Purpose: Autoloads and image and starts boot of it
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: TRUE if the command was good, otherwise FALSE
 *
 * Notes: None
 *
 **********************************************************************/
BOOL_32 cmd_boot(void) {
	if (autoboot() == TRUE)
	{
		menuexit = TRUE;
	}
	else
	{
		term_dat_out_crlf(cnoboot_msg);
	}

	return TRUE;
}

/***********************************************************************
 *
 * Function: cmd_image_add_commands
 *
 * Purpose: Initialize this command block
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void cmd_image_add_commands(void)
{
	/* Add image  group */
	cmd_add_group(&image_group);

	/* Add commands to the image group */
	cmd_add_new_command(&image_group, &core_aboot_cmd);
	cmd_add_new_command(&image_group, &core_boot_cmd);
	cmd_add_new_command(&image_group, &core_exec_cmd);
	cmd_add_new_command(&image_group, &core_load_cmd);
	cmd_add_new_command(&image_group, &core_ls_cmd);
}

