/***********************************************************************
 * $Id:: s1l_cmds_flash.c 4930 2010-09-16 00:37:15Z usb10132           $
 *
 * Project: NAND group commands
 *
 * Description:
 *     NAND group commands
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
#include "s1l_sys.h"

/* erase command */
BOOL_32 cmd_erase(void);
static UNS_32 cmd_erase_plist[] =
{
	(PARSE_TYPE_STR), /* The "erase" command */
	(PARSE_TYPE_DEC),
	(PARSE_TYPE_DEC | PARSE_TYPE_END)
};
static CMD_ROUTE_T nand_erase_cmd =
{
	(UNS_8 *) "erase",
	cmd_erase,
	(UNS_8 *) "Erases a range of FLASH blocks",
	(UNS_8 *) "erase [starting block][number of blocks]",
	cmd_erase_plist,
	NULL
};

/* write command */
BOOL_32 cmd_write(void);
static UNS_32 cmd_write_plist[] =
{
	(PARSE_TYPE_STR), /* The "write" command */
	(PARSE_TYPE_HEX),
	(PARSE_TYPE_DEC),
	(PARSE_TYPE_DEC | PARSE_TYPE_END)
};
static CMD_ROUTE_T nand_write_cmd =
{
	(UNS_8 *) "write",
	cmd_write,
	(UNS_8 *) "Writes raw data from memory to FLASH",
	(UNS_8 *) "write [hex address][first sector][sectors to write]",
	cmd_write_plist,
	NULL
};

/* read command */
BOOL_32 cmd_read(void);
static UNS_32 cmd_read_plist[] =
{
	(PARSE_TYPE_STR), /* The "read" command */
	(PARSE_TYPE_HEX),
	(PARSE_TYPE_DEC),
	(PARSE_TYPE_DEC | PARSE_TYPE_END)
};
static CMD_ROUTE_T nand_read_cmd =
{
	(UNS_8 *) "read",
	cmd_read,
	(UNS_8 *) "Reads raw data from FLASH to memory",
	(UNS_8 *) "read [hex address][first sector][Sectors to read]",
	cmd_read_plist,
	NULL
};

/* dumpex command */
BOOL_32 cmd_dumpex(void);
static UNS_32 cmd_dumpex_plist[] =
{
	(PARSE_TYPE_STR), /* The "dumpex" command */
	(PARSE_TYPE_DEC),
	(PARSE_TYPE_DEC | PARSE_TYPE_END)
};
static CMD_ROUTE_T nand_dumpex_cmd =
{
	(UNS_8 *) "dumpex",
	cmd_dumpex,
	(UNS_8 *) "Dumps the spare area data for a range of sectors",
	(UNS_8 *) "dumpex [first sector][Sectors to read]",
	cmd_dumpex_plist,
	NULL
};

/* nandbb command */
BOOL_32 cmd_nandbb(void);
static UNS_32 cmd_nandbb_plist[] =
{
	(PARSE_TYPE_STR | PARSE_TYPE_END) /* The "nandbb" command */
};
static CMD_ROUTE_T nand_nandbb_cmd =
{
	(UNS_8 *) "nandbb",
	cmd_nandbb,
	(UNS_8 *) "Displays a list of bad NAND blocks",
	(UNS_8 *) "nandbb",
	cmd_nandbb_plist,
	NULL
};

/* nsave command */
BOOL_32 cmd_nsave(void);
static UNS_32 cmd_nsave_plist[] =
{
	(PARSE_TYPE_STR | PARSE_TYPE_END) /* The "nsave" command */
};
static CMD_ROUTE_T nand_nsave_cmd =
{
	(UNS_8 *) "nsave",
	cmd_nsave,
	(UNS_8 *) "Saves the current binary image loaded in memory into FLASH",
	(UNS_8 *) "nsave",
	cmd_nsave_plist,
	NULL
};

/* nload command */
BOOL_32 cmd_nload(void);
static UNS_32 cmd_nload_plist[] =
{
	(PARSE_TYPE_STR | PARSE_TYPE_END) /* The "nload" command */
};
static CMD_ROUTE_T nand_nload_cmd =
{
	(UNS_8 *) "nload",
	cmd_nload,
	(UNS_8 *) "Loads the binary image stored in FLASH into memory",
	(UNS_8 *) "nload",
	cmd_nload_plist,
	NULL
};

/* nburn command */
BOOL_32 cmd_nburn(void);
static UNS_32 cmd_nburn_plist[] =
{
	(PARSE_TYPE_STR), /* The "nburn" command */
	(PARSE_TYPE_DEC | PARSE_TYPE_END)
};
static CMD_ROUTE_T nand_nburn_cmd =
{
	(UNS_8 *) "nburn",
	cmd_nburn,
	(UNS_8 *) "Stored the current binary image in memory to FLASH",
	(UNS_8 *) "nburn [starting block]",
	cmd_nburn_plist,
	NULL
};

/* NAND group */
GROUP_LIST_T nand_group =
{
	(UNS_8 *) "nand", /* NAND group */
	(UNS_8 *) "NAND command group",
	NULL,
	NULL
};

static UNS_8 blkrangeerr_msg[] = "Block range is invalid";
static UNS_8 blkov_msg[] = "Operation will overwrite bootloader - ok?";
static UNS_8 starter_msg[] = "Starting block erase";
static UNS_8 ddbblist_msg[] = "Displaying NAND bad block list";
static UNS_8 bbat_msg[] = "Bad block at index ";
static UNS_8 fnldd_msg[] = "No image loaded to burn into FLASH";
static UNS_8 updnotc_msg[] = "Image in memory is not contiguous";
static UNS_8 tobig_msg[] = "Loaded image will not fit in FLASH";
static UNS_8 wr1_msg[] = "Bytes written     :";
static UNS_8 wr2_msg[] = "Last block        :";
static UNS_8 wr3_msg[] = "First sector      :";
static UNS_8 wr4_msg[] = "Number of sectors :";
extern UNS_8 noflash_msg[];

/***********************************************************************
 *
 * Function: check_bl_range
 *
 * Purpose: Returns TRUE if the block erase can be done
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     block_st : Starting block for erase to check
 *
 * Outputs: None
 *
 * Returns: TRUE if the block can be erased
 *
 * Notes: None
 *
 **********************************************************************/
static BOOL_32 check_bl_range(UNS_32 block_st)
{
	BOOL_32 ov = TRUE;

	/* Determine if the bootloader going to be overwritten */
	if (block_st < sysinfo.sysrtcfg.bl_num_blks)
	{
		term_dat_out_len(blkov_msg, str_size(blkov_msg));
		ov = prompt_yesno();
		term_dat_out_crlf((UNS_8 *) "");
	}

	return ov;
}

/***********************************************************************
 *
 * Function: cmd_erase
 *
 * Purpose: Erase a range of NAND blocks
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Always return TRUE
 *
 * Notes:
 *     erase [starting block][number of blocks]
 *
 **********************************************************************/
BOOL_32 cmd_erase(void) {
	UNS_32 first, numblks;
	BOOL_32 erase;
	UNS_8 str[16];

	/* Get arguments */
	first = cmd_get_field_val(1);
	numblks = cmd_get_field_val(2);

	if (sysinfo.nandgeom == NULL)
	{
		term_dat_out(noflash_msg);
	}
	else if ((first + numblks) > sysinfo.nandgeom->num_blocks)
	{
		term_dat_out(blkrangeerr_msg);
	}
	else
	{
		/* Verify boot loader won't be overwritten */
		erase = check_bl_range(first);
		if (erase == TRUE)
		{
			/* Erase */
			term_dat_out_crlf(starter_msg);
			while (numblks > 0)
			{
				if (flash_erase_block(first) == FALSE)
				{
					term_dat_out((UNS_8 *) "Bad block: ");
					str_makedec(str, first);
					term_dat_out_crlf(str);
				}
				first++;
				numblks--;
			}
		}
	}

	return TRUE;
}

/***********************************************************************
 *
 * Function: cmd_read
 *
 * Purpose: Read data from NAND into memory
 *
 * Processing:
 *     Load data from FLASH into memory
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: TRUE if the FLASH was read, otherwise FALSE
 *
 * Notes:
 *     read [hex address][first sector][Sectors to read]
 *
 **********************************************************************/
BOOL_32 cmd_read(void) {
	UNS_32 first, hexaddr, sectors;

	/* Get arguments */
	hexaddr = cmd_get_field_val(1);
	first = cmd_get_field_val(2);
	sectors = cmd_get_field_val(3);

	nand_to_mem(first, (void *) hexaddr,
		(sectors * sysinfo.nandgeom->data_bytes_per_page));

	return TRUE;
}

/***********************************************************************
 *
 * Function: cmd_dumpex
 *
 * Purpose: Dumps the spare area data for a range of sectors
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
 * Notes:
 *     dumpex [first sector][Sectors to read]
 *
 **********************************************************************/
BOOL_32 cmd_dumpex(void) {
	UNS_32 first, sectors;
	int idx, lfcount;
	UNS_8 str[64];

	if (sysinfo.nandgeom == NULL)
	{
		term_dat_out(noflash_msg);
	}
	else
	{
		/* Get arguments */
		first = cmd_get_field_val(1);
		sectors = cmd_get_field_val(2);

		while (sectors > 0)
		{
			/* Ignore ECC return status, only interested in spare */
			flash_read_sector(first, secdat, &secdat[2048]);

			/* Dump spare data area */
			lfcount = 0;
			for (idx = 0;
				idx < sysinfo.nandgeom->spare_bytes_per_page;
				idx++)
			{
				if (lfcount == 0)
				{
					term_dat_out((UNS_8 *) "Sector ");
					str_makehex(str, first, 6);
					term_dat_out(str);
					term_dat_out((UNS_8 *) "+");
					str_makehex(str, idx, 2);
					term_dat_out(str);
					term_dat_out((UNS_8 *) ":");
				}

				str_makehex(str, secdat[2048 + idx], 2);
				term_dat_out(str);
				term_dat_out((UNS_8 *) " ");

				lfcount++;
				if (lfcount >= 16)
				{
					term_dat_out_crlf((UNS_8 *) "");
					lfcount = 0;
				}
			}

			first++;
			sectors--;
		}
	}

	return TRUE;
}

/***********************************************************************
 *
 * Function: cmd_write
 *
 * Purpose: Move data into NAND from memory
 *
 * Processing:
 *     Load data from memory into FLASH
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: TRUE
 *
 * Notes:
 *     write [hex address][first sector][sectors to write][0, 1(skip bad blocks)]"
 *
 **********************************************************************/
BOOL_32 cmd_write(void) {
	UNS_32 first, hexaddr, sectors;

	/* Get arguments */
	hexaddr = cmd_get_field_val(1);
	first = cmd_get_field_val(2);
	sectors = cmd_get_field_val(3);

	mem_to_nand(first, (void *) hexaddr,
		(sectors * sysinfo.nandgeom->data_bytes_per_page));

	return TRUE;
}

/***********************************************************************
 *
 * Function: cmd_nandbb
 *
 * Purpose: Displays a list of bad blocks in NAND
 *
 * Processing:
 *     Read page 0 of each NAND block and check the bad block marker.
 *     Display a message if the block is bad.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Always returns TRUE.
 *
 * Notes: None
 *
 **********************************************************************/
BOOL_32 cmd_nandbb(void) 
{
	UNS_8 blk [16];
	UNS_32 idx = 0;

	if (sysinfo.nandgeom == NULL)
	{
		term_dat_out(noflash_msg);
	}
	else 
	{
		term_dat_out_crlf(ddbblist_msg);
		while (idx < sysinfo.nandgeom->num_blocks) 
		{
			if (flash_is_bad_block(idx) != FALSE)
			{
				term_dat_out(bbat_msg);
				str_makedec(blk, idx);
				term_dat_out_crlf(blk);
			}
		
			idx++;
		}
	}

	return TRUE;
}

/***********************************************************************
 *
 * Function: cmd_nsave
 *
 * Purpose: Saves image in memory to FLASH
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: TRUE if the command was processed NAND ok.
 *
 * Notes: None
 *
 **********************************************************************/
BOOL_32 cmd_nsave(void) 
{
	if (sysinfo.lfile.loaded == FALSE) 
	{
		term_dat_out_crlf(fnldd_msg);
		return TRUE;
	}

	if (sysinfo.lfile.contiguous == FALSE) 
	{
		term_dat_out_crlf(updnotc_msg);
		return TRUE;
	}

	flash_image_save();

	return TRUE;
}

/***********************************************************************
 *
 * Function: cmd_nload
 *
 * Purpose: Loads image from FLASH into memory
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Returns TRUE if the image was loaded, otherwise FALSE
 *
 * Notes: None
 *
 **********************************************************************/
BOOL_32 cmd_nload(void) 
{
	/* Load image */
	flash_image_load();

	return TRUE;
}

/***********************************************************************
 *
 * Function: cmd_nburn
 *
 * Purpose: Burns a loaded image into NAND FLASH
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Returns TRUE if the command was good, otherwise FALSE
 *
 * Notes: None
 *
 **********************************************************************/
BOOL_32 cmd_nburn(void)
{
	UNS_8 str[16];
	UNS_32 numsecs;
	UNS_32 block, fblock, fsector, sector, nblks;

	if (sysinfo.nandgeom == NULL) 
	{
		term_dat_out_crlf(noflash_msg);
	}
	else
	{
		block = cmd_get_field_val(1);

		if (sysinfo.lfile.loaded == FALSE) 
		{
			term_dat_out_crlf(fnldd_msg);
			return TRUE;
		}

		if (sysinfo.lfile.contiguous == FALSE) 
		{
			term_dat_out_crlf(updnotc_msg);
			return TRUE;
		}

		/* Get first sector */
		fblock = block;
		fsector = conv_to_sector(block, 0);

		/* Get number of sectors for image */
		numsecs = sysinfo.lfile.num_bytes /
			sysinfo.nandgeom->data_bytes_per_page;
		if ((numsecs * sysinfo.nandgeom->data_bytes_per_page) <
			sysinfo.lfile.num_bytes) 
		{
			numsecs++;
		}

		/* Will exceed end of FLASH? */
		if ((fsector + numsecs) >= (sysinfo.nandgeom->pages_per_block *
			sysinfo.nandgeom->num_blocks))
		{
			term_dat_out_crlf(tobig_msg);
			return TRUE;
		}

		/* Erase necessary blocks first */
		nblks = numsecs / sysinfo.nandgeom->pages_per_block;
		if ((nblks * sysinfo.nandgeom->pages_per_block) < numsecs)
		{
			nblks++;
		}
		while (nblks > 0) 
		{
			if (flash_is_bad_block(fblock) == FALSE)
			{
				flash_erase_block(fblock);
				nblks--;
			}

			fblock++;
		}

		sector = conv_to_sector(block, 0);
		mem_to_nand(sector, (void *) sysinfo.lfile.loadaddr,
			sysinfo.lfile.num_bytes);

		/* Display statistics */
		term_dat_out(wr1_msg);
		str_makedec(str, sysinfo.lfile.num_bytes);
		term_dat_out_crlf(str);
		term_dat_out(wr2_msg);
		str_makedec(str, (fblock - 1));
		term_dat_out_crlf(str);
		term_dat_out(wr3_msg);
		str_makedec(str, fsector);
		term_dat_out_crlf(str);
		term_dat_out(wr4_msg);
		str_makedec(str, numsecs);
		term_dat_out_crlf(str);
	}

	return TRUE;
}

/***********************************************************************
 *
 * Function: cmd_nand_add_commands
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
void cmd_nand_add_commands(void)
{
	/* Add NAND group */
	cmd_add_group(&nand_group);

	/* Add commands to the NAND group */
	cmd_add_new_command(&nand_group, &nand_erase_cmd);
	cmd_add_new_command(&nand_group, &nand_nandbb_cmd);
	cmd_add_new_command(&nand_group, &nand_nburn_cmd);
	cmd_add_new_command(&nand_group, &nand_nload_cmd);
	cmd_add_new_command(&nand_group, &nand_nsave_cmd);
	cmd_add_new_command(&nand_group, &nand_read_cmd);
	cmd_add_new_command(&nand_group, &nand_dumpex_cmd);
	cmd_add_new_command(&nand_group, &nand_write_cmd);
}
