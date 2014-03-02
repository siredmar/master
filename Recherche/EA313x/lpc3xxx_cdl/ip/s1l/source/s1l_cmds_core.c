/***********************************************************************
 * $Id:: s1l_cmds_core.c 4854 2010-09-08 23:33:09Z usb10132            $
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
#include "s1l_memtests.h"

/* Peek saved data */
static UNS_32 last_addr = 0;
static UNS_32 last_width = 4;

/* Dump saved data */
static UNS_32 dump_addr = 0;
static UNS_32 dump_width = 4;
static UNS_32 dump_trans = 1;

/* Peek command */
static BOOL_32 cmd_peek(void);
static UNS_32 cmd_peek_plist[] =
{
	(PARSE_TYPE_STR | PARSE_TYPE_OPT), /* The "peek" command */
	(PARSE_TYPE_HEX | PARSE_TYPE_OPT),
	(PARSE_TYPE_DEC | PARSE_TYPE_END)
};
static CMD_ROUTE_T core_peek_cmd =
{
	(UNS_8 *) "peek",
	cmd_peek,
	(UNS_8 *) "Displays a byte, half-word, or word value at an address",
	(UNS_8 *) "peek <hex address><1, 2, or 4 bytes>",
	cmd_peek_plist,
	NULL
};

/* Poke command */
static BOOL_32 cmd_poke(void);
static UNS_32 cmd_poke_plist[] =
{
	(PARSE_TYPE_STR), /* The "poke" command */
	(PARSE_TYPE_HEX),
	(PARSE_TYPE_HEX),
	(PARSE_TYPE_DEC | PARSE_TYPE_END)
};
static CMD_ROUTE_T core_poke_cmd =
{
	(UNS_8 *) "poke",
	cmd_poke,
	(UNS_8 *) "Places a byte, half-word, or word value at an address",
	(UNS_8 *) "poke [hex address][hex value][1, 2, or 4 bytes]",
	cmd_poke_plist,
	NULL
};

/* Dump command */
static BOOL_32 cmd_dump(void);
static UNS_32 cmd_dump_plist[] =
{
	(PARSE_TYPE_STR | PARSE_TYPE_OPT), /* The "dump" command */
	(PARSE_TYPE_HEX | PARSE_TYPE_OPT),
	(PARSE_TYPE_DEC | PARSE_TYPE_OPT),
	(PARSE_TYPE_DEC | PARSE_TYPE_END)
};
static CMD_ROUTE_T core_dump_cmd =
{
	(UNS_8 *) "dump",
	cmd_dump,
	(UNS_8 *) "Dumps a range of memory",
	(UNS_8 *) "dump <hex address><values to dump><1, 2, or 4 bytes>",
	cmd_dump_plist,
	NULL
};

/* Fill command */
static BOOL_32 cmd_fill(void);
static UNS_32 cmd_fill_plist[] =
{
	(PARSE_TYPE_STR), /* The "fill" command */
	(PARSE_TYPE_HEX),
	(PARSE_TYPE_HEX),
	(PARSE_TYPE_HEX),
	(PARSE_TYPE_DEC | PARSE_TYPE_END)
};
static CMD_ROUTE_T core_fill_cmd =
{
	(UNS_8 *) "fill",
	cmd_fill,
	(UNS_8 *) "Fills a range of memory with a value",
	(UNS_8 *) "fill [hex address][bytes to fill(hex)][hex value][1, 2, or 4 bytes]",
	cmd_fill_plist,
	NULL
};

/* Memory test command */
static BOOL_32 cmd_memtst(void);
static UNS_32 cmd_memtst_plist[] =
{
	(PARSE_TYPE_STR), /* The "memtst" command */
	(PARSE_TYPE_HEX),
	(PARSE_TYPE_DEC),
	(PARSE_TYPE_DEC),
	(PARSE_TYPE_DEC),
	(PARSE_TYPE_DEC | PARSE_TYPE_END)
};
static CMD_ROUTE_T core_memtst_cmd =
{
	(UNS_8 *) "memtst",
	cmd_memtst,
	(UNS_8 *) "Performs 1 or more memory tests",
	(UNS_8 *) "memtst [hex address][bytes to test][1, 2, or 4 bytes][0(all) - 5][iterations]",
	cmd_memtst_plist,
	NULL
};

/* Bandwidth test command */
static BOOL_32 cmd_bwtest(void);
static UNS_32 cmd_bwtest_plist[] =
{
	(PARSE_TYPE_STR), /* The "bwtest" command */
	(PARSE_TYPE_HEX),
	(PARSE_TYPE_HEX),
	(PARSE_TYPE_DEC),
	(PARSE_TYPE_DEC | PARSE_TYPE_END)
};
static CMD_ROUTE_T core_bwtest_cmd =
{
	(UNS_8 *) "bwtest",
	cmd_bwtest,
	(UNS_8 *) "Performs a bandwidth test on a range of memory",
	(UNS_8 *) "bwtest [source hex address][destination hex address][bytes][loops]",
	cmd_bwtest_plist,
	NULL
};

/* comp command */
static BOOL_32 cmd_comp(void);
static UNS_32 cmd_comp_plist[] =
{
	(PARSE_TYPE_STR), /* The "comp" command */
	(PARSE_TYPE_HEX),
	(PARSE_TYPE_HEX),
	(PARSE_TYPE_DEC | PARSE_TYPE_END)
};
static CMD_ROUTE_T core_comp_cmd =
{
	(UNS_8 *) "comp",
	cmd_comp,
	(UNS_8 *) "Performs a bytewise data comparison between 2 data regions",
	(UNS_8 *) "comp [hex address 1][hex address 2][bytes]",
	cmd_comp_plist,
	NULL
};

/* copy command */
static BOOL_32 cmd_copy(void);
static UNS_32 cmd_copy_plist[] =
{
	(PARSE_TYPE_STR), /* The "copy" command */
	(PARSE_TYPE_HEX),
	(PARSE_TYPE_HEX),
	(PARSE_TYPE_DEC | PARSE_TYPE_END)
};
static CMD_ROUTE_T core_copy_cmd =
{
	(UNS_8 *) "copy",
	cmd_copy,
	(UNS_8 *) "Performs a bytewise data copy between 2 data regions",
	(UNS_8 *) "copy [source hex address][destination hex address][bytes]",
	cmd_copy_plist,
	NULL
};

/* baud command */
static BOOL_32 cmd_baud(void);
static UNS_32 cmd_baud_plist[] =
{
	(PARSE_TYPE_STR), /* The "baud" command */
	(PARSE_TYPE_DEC | PARSE_TYPE_END)
};
static CMD_ROUTE_T core_baud_cmd =
{
	(UNS_8 *) "baud",
	cmd_baud,
	(UNS_8 *) "Changes the system terminal baud rate",
	(UNS_8 *) "baud [new baud rate (115200, 57600, 38400, 19200, 9600)]",
	cmd_baud_plist,
	NULL
};

/* rstcfg command */
static BOOL_32 cmd_rstcfg(void);
static UNS_32 cmd_rstcfg_plist[] =
{
	(PARSE_TYPE_STR | PARSE_TYPE_END) /* The "rstcfg" command */
};
static CMD_ROUTE_T core_rstcfg_cmd =
{
	(UNS_8 *) "rstcfg",
	cmd_rstcfg,
	(UNS_8 *) "Restores the default system configuration",
	(UNS_8 *) "rstcfg",
	cmd_rstcfg_plist,
	NULL
};

/* prompt command */
static BOOL_32 cmd_prompt(void);
static UNS_32 cmd_prompt_plist[] =
{
	(PARSE_TYPE_STR), /* The "prompt" command */
	(PARSE_TYPE_STR),
	(PARSE_TYPE_DEC | PARSE_TYPE_END)
};
static CMD_ROUTE_T core_prompt_cmd =
{
	(UNS_8 *) "prompt",
	cmd_prompt,
	(UNS_8 *) "Changes the system prompt and autoboot timeout",
	(UNS_8 *) "prompt [name][timeout(secs)]",
	cmd_prompt_plist,
	NULL
};

/* info command */
static BOOL_32 cmd_info(void);
static UNS_32 cmd_info_plist[] =
{
	(PARSE_TYPE_STR | PARSE_TYPE_END) /* The "info" command */
};
static CMD_ROUTE_T core_info_cmd =
{
	(UNS_8 *) "info",
	cmd_info,
	(UNS_8 *) "Displays the system information and configuration",
	(UNS_8 *) "info",
	cmd_info_plist,
	NULL
};

/* script command */
static BOOL_32 cmd_script(void);
static UNS_32 cmd_script_plist[] =
{
	(PARSE_TYPE_STR), /* The "script" command */
	(PARSE_TYPE_STR | PARSE_TYPE_END)
};
static CMD_ROUTE_T core_script_cmd =
{
	(UNS_8 *) "script",
	cmd_script,
	(UNS_8 *) "Sets up the board's startup script",
	(UNS_8 *) "script [on, off, setup]",
	cmd_script_plist,
	NULL
};

/* Core group */
GROUP_LIST_T core_group =
{
	(UNS_8 *) "core", /* Core group */
	(UNS_8 *) "S1L core system command group",
	NULL,
	NULL
};

/* Various messages */
static UNS_8 scomp_msg [] ="Starting data compare...";
static UNS_8 ssame_msg [] ="data areas match\r\n";
static UNS_8 snsame_msg [] ="\r\nMatch error at addresses ";
static UNS_8 and [] = " and ";
static UNS_8 scp_msg [] ="Data copy complete\r\n";
static UNS_8 prto_msg[] = "Prompt bootup timeout (secs) = ";
static UNS_8 flashsts_msg[] = "FLASH device : ";
static UNS_8 present_msg[] = "detected";
static UNS_8 notpresent_msg[] = "not detected";
static UNS_8 numblks_msg[] = " Number of FLASH blocks   : ";
static UNS_8 pgsblk_msg[]  = " FLASH pages per block    : ";
static UNS_8 pgsbyt_msg[]  = " FLASH bytes per page     : ";
static UNS_8 ttlsz_msg[]   = " Total FLASH size (Mbytes): ";
static UNS_8 blspace_msg[] = "Stage 1 loader number of blocks used: ";
static UNS_8 ftloaded_msg[] = "File loaded in memory: ";
static UNS_8 ftnone_msg[] = "None";
static UNS_8 ftimage_msg[] = "Image";
static UNS_8 ftaddr_msg[] =   " Address                 : ";
static UNS_8 fteaddr_msg[] =  " Execution entry address : ";
static UNS_8 ftmbytes_msg[] = " Size of image (bytes)   : ";
static UNS_8 fteaddrunk_msg[] = "Unknown";
static UNS_8 flsnone_msg[] = "No image stored in FLASH";
static UNS_8 fifb_msg[] =   "FLASH image first block used  : ";
static UNS_8 finbu_msg[] =  "FLASH image blocks used       : ";
static UNS_8 finsu_msg[] =  "FLASH image sectors used      : ";
static UNS_8 finnbu_msg[] = "FLASH image size in bytes     : ";
static UNS_8 fladd_msg[] = "FLASH image load address      : ";
static UNS_8 fsadd_msg[] = "FLASH image execution address : ";
static UNS_8 absrc_msg[]   = "Autoboot source                  : ";
static UNS_8 absrcft_msg[] = "Autoboot image type              : ";
static UNS_8 abadd_msg[]   = "Autoboot image load address      : ";
static UNS_8 abeadd_msg[]  = "Autoboot image execution address : ";
static UNS_8 absrcterm_msg[] = "Terminal";
static UNS_8 absrcnand_msg[] = "FLASH";
static UNS_8 absrcsd_msg[] = "Block : ";
static UNS_8 absnone_msg[] = "Disabled";
static UNS_8 absrcraw_msg[] = "RAW";
static UNS_8 absrcnbin_msg[] = "BIN";
static UNS_8 absrcsrec_msg[] = "SREC";
static UNS_8 absrcelf_msg[] = "ELF";
static UNS_8 script_entry_msg[] =
	"Enter each command on a seperate line. Enter an empty line to\r\n"
	"exit. A maximum of 12 commands totaling 256 bytes may be entered\r\n"
	"Do not use backspace or any other keys during script entry.";

/***********************************************************************
 *
 * Function: hex_dump
 *
 * Purpose: Dumps a series of data values
 *
 * Processing:
 *     Format and dump data based on address and width.
 *
 * Parameters:
 *     addr     : Starting address of dump
 *     width    : Width of the dump in bytes (1, 2, or 4)
 *     numtrans : Number of items to dump
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
static void hex_dump(UNS_32 addr,
					 UNS_32 width,
					 UNS_32 numtrans) 
{
	int dumpsperline, ldspl, idx;
	UNS_32 tmp, laddr;
	UNS_8 daddr [16];

	/* Based on width, determine number of dumps per line */
	switch (width) 
	{
		case 4:
			dumpsperline = 4;
			break;

		case 2:
			dumpsperline = 8;
			break;

		case 1:
		default:
			width = 1;
			dumpsperline = 8;
			break;
	}
	laddr = addr + (width * numtrans);

	/* Start dump */
	while (addr < laddr) 
	{
		/* Dump address first */
		str_makehex(daddr, addr, 8);
		term_dat_out(daddr);
		term_dat_out((UNS_8 *) " : ");

		ldspl = dumpsperline;
		if (ldspl > numtrans) 
		{
			ldspl = numtrans;
		}
		numtrans = numtrans - ldspl;

		/* Dump data in the line */
		for (idx = 0; idx < ldspl; idx++) 
		{
			if (width == 4) 
			{
				tmp = * (UNS_32 *) addr;
				addr = addr + 4;
			}
			else if (width == 2) 
			{
				tmp = (UNS_32) (* (UNS_16 *) addr);
				addr = addr + 2;
			}
			else
			{
				tmp = (UNS_32) (* (UNS_8 *) addr);
				addr = addr + 1;
			}

			str_makehex(daddr, tmp, (width * 2));
			term_dat_out(daddr);
			term_dat_out((UNS_8 *) " ");
		}

		term_dat_out_crlf((UNS_8 *) "");
	}
}

    /***********************************************************************
 *
 * Function: cmd_peek
 *
 * Purpose: Peek command
 *
 * Processing:
 *     Parse the string elements for the peek command and display the
 *     peeked value.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: TRUE if the command was processed, otherwise FALSE
 *
 * Notes: None
 *
 **********************************************************************/
static BOOL_32 cmd_peek(void) 
{
	UNS_32 newaddr, newwidth;

	/* If only 1 parsed command, then print at current address and
	   width */
	if (parse_get_entry_count() > 1) 
	{
		/* Get address */
		newaddr = cmd_get_field_val(1);

		/* Width */
		newwidth = last_width;
		if (parse_get_entry_count() >= 3) 
		{
			newwidth = cmd_get_field_val(2);
			if (!((newwidth == 1) || (newwidth == 2) ||
				(newwidth == 4)))
			{
				newwidth = 1;
			}
		}

		last_addr = newaddr;
		last_width = newwidth;
	}

	/* Based on width, limit address start */
	last_addr = last_addr & ~(last_width - 1);

	/* Dump data */
	hex_dump(last_addr, last_width, 1);

	return TRUE;
}

/***********************************************************************
 *
 * Function: cmd_poke
 *
 * Purpose: Poke command
 *
 * Processing:
 *     Parse the string elements for the poke command and update
 *     requested memory.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: TRUE if the command was processed, otherwise FALSE
 *
 * Notes: None
 *
 **********************************************************************/
static BOOL_32 cmd_poke(void)
{
	UNS_32 addr, width, fillval;

	/* Get address */
	addr = cmd_get_field_val(1);

	/* Hex fill value */
	fillval = cmd_get_field_val(2);

	/* Width */
	width = cmd_get_field_val(3);

	if (!((width == 1) || (width == 2) || (width == 4)))
	{
		width = 1;
	}

	/* Limit addressing size */
	addr = addr & ~(width - 1);

	if (width == 1) 
	{
		UNS_8 *faddr = (UNS_8 *) addr;
		*faddr = (UNS_8) fillval;
	}
	else if (width == 2) 
	{
		UNS_16 *faddr = (UNS_16 *) addr;
		*faddr = (UNS_16) fillval;
	}
	else
	{
		UNS_32 *faddr = (UNS_32 *) addr;
		*faddr = fillval;
	}

	return TRUE;
}

/***********************************************************************
 *
 * Function: cmd_dump
 *
 * Purpose: Dump command
 *
 * Processing:
 *     Parse the string elements for the dump command and display the
 *     dumped values.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: TRUE if the command was processed, otherwise FALSE
 *
 * Notes: None
 *
 **********************************************************************/
static BOOL_32 cmd_dump(void) 
{
	UNS_32 newaddr, newwidth, newtrans;

	/* If only 1 parsed command, then print at current address and
	   width */
	if (parse_get_entry_count() > 1)
	{
		newaddr = dump_addr;
		newwidth = dump_width;
		newtrans = dump_trans;

		/* Get address */
		newaddr = cmd_get_field_val(1);

		/* Convert decimal value to width */
		if (parse_get_entry_count() >= 3) 
		{
			newtrans = cmd_get_field_val(2);
		}
		if (parse_get_entry_count() >= 4) 
		{
			newwidth = cmd_get_field_val(3);
			if (!((newwidth == 1) || (newwidth == 2) ||
				(newwidth == 4)))
			{
				newwidth = 1;
			}
		}

		dump_addr = newaddr;
		dump_width = newwidth;
		dump_trans = newtrans;

		/* Limit dump size to 1024 bytes */
		if ((dump_trans * dump_width) > 1024)
		{
			dump_trans = dump_trans / dump_width;
		}
	}

	/* Based on width, limit address start */
	dump_addr = dump_addr & ~(dump_width - 1);

	/* Dump data and increment address */
	hex_dump(dump_addr, dump_width, dump_trans);
	dump_addr = dump_addr + (dump_width * dump_trans);

	return TRUE;
}

/***********************************************************************
 *
 * Function: cmd_fill
 *
 * Purpose: Fill command
 *
 * Processing:
 *     Parse the string elements for the fill command and fill the
 *     requested data range with data.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: TRUE if the command was processed, otherwise FALSE
 *
 * Notes: None
 *
 **********************************************************************/
static BOOL_32 cmd_fill(void) {
	UNS_32 addr, width, bytes, fillval;

	/* Get address */
	addr = cmd_get_field_val(1);

	/* Number of bytes */
	bytes = cmd_get_field_val(2);

	/* Hex fill value */
	fillval = cmd_get_field_val(3);

	/* Width */
	width = cmd_get_field_val(4);

	if (!((width == 1) || (width == 2) || (width == 4)))
	{
		width = 1;
	}

	/* Limit number of bytes based on addressing size */
	bytes = bytes & ~(width - 1);
	addr = addr & ~(width - 1);

	if (width == 1) 
	{
		UNS_8 *faddr = (UNS_8 *) addr;
		while (bytes > 0) 
		{
			*faddr = (UNS_8) fillval;
			faddr++;
			bytes--;
		}
	}
	else if (width == 2) 
	{
		UNS_16 *faddr = (UNS_16 *) addr;
		while (bytes > 0) 
		{
			*faddr = (UNS_16) fillval;
			faddr++;
			bytes -= 2;
		}
	}
	else
	{
		UNS_32 *faddr = (UNS_32 *) addr;
		while (bytes > 0) 
		{
			*faddr = fillval;
			faddr++;
			bytes -= 4;
		}
	}

	return TRUE;
}

/***********************************************************************
 *
 * Function: cmd_memtst
 *
 * Purpose: Performs memory tests
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
 * Notes:
 *     memtst [hex address][bytes to test][1, 2, or 4 bytes][0(all) - 5][iterations]
 *
 **********************************************************************/
static BOOL_32 cmd_memtst(void) {
	UNS_32 iters, hexaddr, bytes, width, tstnum;

	/* Get arguments */
	hexaddr = cmd_get_field_val(1);
	bytes = cmd_get_field_val(2);
	width = cmd_get_field_val(3);
	tstnum = cmd_get_field_val(4);
	iters = cmd_get_field_val(5);
	if (tstnum == 0)
	{
		tstnum = MTST_ALL;
	}
	else
	{
		tstnum--;
	}

	memory_test(hexaddr, bytes, width, tstnum, iters);

	return TRUE;
}

/***********************************************************************
 *
 * Function: cmd_bwtest
 *
 * Purpose: Performs memory bandwidth tests
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
static BOOL_32 cmd_bwtest(void) {
	UNS_32 hexaddr1, hexaddr2, bytes, loops;

	/* Get arguments */
	hexaddr1 = cmd_get_field_val(1);
	hexaddr2 = cmd_get_field_val(2);
	bytes = cmd_get_field_val(3);
	loops = cmd_get_field_val(4);
	bw_test(hexaddr1, hexaddr2, bytes, loops);

	return TRUE;
}

/***********************************************************************
 *
 * Function: cmd_comp
 *
 * Purpose: Compare data between 2 regions
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
static BOOL_32 cmd_comp(void) {
	UNS_8 *ha1, *ha2;
	UNS_32 hexaddr1, hexaddr2, bytes;
	UNS_8 str [16];
	BOOL_32 mtch;

	/* Get arguments */
	hexaddr1 = cmd_get_field_val(1);
	hexaddr2 = cmd_get_field_val(2);
	bytes = cmd_get_field_val(3);

	mtch = TRUE;
	ha1 = (UNS_8 *) hexaddr1;
	ha2 = (UNS_8 *) hexaddr2;
	term_dat_out(scomp_msg);
	while (bytes > 0)
	{
		if (*ha1 != *ha2)
		{
			mtch = FALSE;
			bytes = 0;
		}
		else
		{
			bytes--;
			ha1++;
			ha2++;
		}
	}

	if (mtch == TRUE)
	{
		term_dat_out_crlf(ssame_msg);
	}
	else
	{
		term_dat_out(snsame_msg);
		str_makehex(str, (UNS_32) ha1, 8);
		term_dat_out(str);
		term_dat_out(and);
		str_makehex(str, (UNS_32) ha2, 8);
		term_dat_out_crlf(str);
	}

	return TRUE;
}

/***********************************************************************
 *
 * Function: cmd_copy
 *
 * Purpose: Copy data between 2 regions
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
static BOOL_32 cmd_copy(void) {
	UNS_8 *ha1, *ha2;
	UNS_32 hexaddr1, hexaddr2, bytes;

	/* Get arguments */
	hexaddr1 = cmd_get_field_val(1);
	hexaddr2 = cmd_get_field_val(2);
	bytes = cmd_get_field_val(3);

	ha1 = (UNS_8 *) hexaddr1;
	ha2 = (UNS_8 *) hexaddr2;
	while (bytes > 0)
	{
		*ha2 = *ha1;
		ha1++;
		ha2++;
		bytes--;
	}

	term_dat_out_crlf(scp_msg);

	return TRUE;
}

/***********************************************************************
 *
 * Function: cmd_prompt
 *
 * Purpose: prompt command
 *
 * Processing:
 *     Parse the string elements for the prompt command and set the new
 *     prompt and timeout values.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: TRUE if the command was processed, otherwise FALSE
 *
 * Notes: None
 *
 **********************************************************************/
static BOOL_32 cmd_prompt(void) {
	UNS_8 tmpprmpt [16];
	UNS_32 to;

	/* Get new prompt */
	str_copy(tmpprmpt, (void *) cmd_get_field_val(1));

	/* Get new timeout */
	to = cmd_get_field_val(2);

	str_copy(syscfg.prmpt, tmpprmpt);
	syscfg.prmpt_to = to;
	key_prompt_set(syscfg.prmpt);

	cfg_save(&syscfg);

	return TRUE;
}

/***********************************************************************
 *
 * Function: cmd_rstcfg
 *
 * Purpose: Restores default system configuration
 *
 * Processing:
 *     Call cfg_default to restore default configuation data.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Always TRUE
 *
 * Notes: None
 *
 **********************************************************************/
static BOOL_32 cmd_rstcfg(void) 
{
	cfg_default(&syscfg);
	key_prompt_set(syscfg.prmpt);
	cfg_save(&syscfg);

	return TRUE;
}

/***********************************************************************
 *
 * Function: cmd_baud
 *
 * Purpose: Sets new baud rate
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: TRUE if the baud rate change was good, otherwise FALSE
 *
 * Notes: None
 *
 **********************************************************************/
static BOOL_32 cmd_baud(void) 
{
	UNS_32 nb;

	nb = cmd_get_field_val(1);
	if ((nb == 115200) || (nb == 57600) || (nb == 38400) ||
		(nb == 19200) || (nb == 9600))
	{
		term_setbaud(nb);
		syscfg.baudrate = nb;
		cfg_save(&syscfg);
	}

	return TRUE;
}

/***********************************************************************
 *
 * Function: cmd_info
 *
 * Purpose: Info command
 *
 * Processing:
 *     For the load command, start parsing subcommand elements and
 *     route to the specific handler.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: TRUE if the command was processed, otherwise FALSE
 *
 * Notes: None
 *
 **********************************************************************/
BOOL_32 cmd_info(void) {
	int i, idx;
	UNS_8 str [128];
	UNS_32 tmp;

	/* S1L configuration */
	term_dat_out(prto_msg);
	str_makedec(str, syscfg.prmpt_to);
	term_dat_out_crlf(str);

	/* Dump FLASH geometry */
	term_dat_out(flashsts_msg);
	if (sysinfo.nandgeom != NULL) 
	{
		term_dat_out_crlf(present_msg);
		term_dat_out(numblks_msg);
		str_makedec(str, sysinfo.nandgeom->num_blocks);
		term_dat_out_crlf(str);
		term_dat_out(pgsblk_msg);
		str_makedec(str, sysinfo.nandgeom->pages_per_block);
		term_dat_out_crlf(str);
		term_dat_out(pgsbyt_msg);
		str_makedec(str, sysinfo.nandgeom->data_bytes_per_page);
		term_dat_out_crlf(str);
		tmp = sysinfo.nandgeom->num_blocks *
			sysinfo.nandgeom->pages_per_block *
			sysinfo.nandgeom->data_bytes_per_page;
		tmp = tmp / (1024 * 1024);
		term_dat_out(ttlsz_msg);
		str_makedec(str, tmp);
		term_dat_out_crlf(str);

		/* Dump FLASH usage for stage 1 loader */
		tmp = sysinfo.sysrtcfg.bl_num_blks;
		term_dat_out(blspace_msg);
		str_makedec(str, tmp);
		term_dat_out_crlf(str);
	}
	else 
	{
		term_dat_out_crlf(notpresent_msg);
	}

	/* Loaded program status */
	term_dat_out(ftloaded_msg);
	if (sysinfo.lfile.flt == FLT_NONE) 
	{
		term_dat_out_crlf(ftnone_msg);
	}
	else 
	{
		term_dat_out_crlf(ftimage_msg);
		term_dat_out(ftaddr_msg);
		str_makehex(str, sysinfo.lfile.loadaddr, 8);
		term_dat_out_crlf(str);
		term_dat_out(ftmbytes_msg);
		str_makedec(str, sysinfo.lfile.num_bytes);
		term_dat_out_crlf(str);
		term_dat_out(fteaddr_msg);
		if ((UNS_32) sysinfo.lfile.startaddr != 0xFFFFFFFF) 
		{
			str_makehex(str, (UNS_32) sysinfo.lfile.startaddr, 8);
			term_dat_out_crlf(str);
		}
		else 
		{
			term_dat_out_crlf(fteaddrunk_msg);
		}
	}

	/* FLASHed program status */
	if (syscfg.fsave.valid == FALSE) 
	{
		term_dat_out_crlf(flsnone_msg);
	}
	else 
	{
		term_dat_out(fifb_msg);
		str_makedec(str, syscfg.fsave.block_first);
		term_dat_out_crlf(str);
		term_dat_out(finbu_msg);
		str_makedec(str, syscfg.fsave.blocks_used);
		term_dat_out_crlf(str);
		term_dat_out(finsu_msg);
		str_makedec(str, syscfg.fsave.secs_used);
		term_dat_out_crlf(str);
		term_dat_out(finnbu_msg);
		str_makedec(str, syscfg.fsave.num_bytes);
		term_dat_out_crlf(str);
		term_dat_out(fladd_msg);
		str_makehex(str, syscfg.fsave.loadaddr, 8);
		term_dat_out_crlf(str);
		term_dat_out(fsadd_msg);
		str_makehex(str, syscfg.fsave.startaddr, 8);
		term_dat_out_crlf(str);
	}

	/* Autoboot source */
	term_dat_out(absrc_msg);
	switch (syscfg.aboot.abootsrc) 
	{
		case SRC_TERM:
			term_dat_out_crlf(absrcterm_msg);
			break;

		case SRC_NAND:
			term_dat_out_crlf(absrcnand_msg);
			break;

		case SRC_BLKDEV:
			term_dat_out(absrcsd_msg);
			term_dat_out_crlf(syscfg.aboot.fname);
			break;

		case SRC_NONE:
		default:
			term_dat_out_crlf(absnone_msg);
			break;
	}
	if (syscfg.aboot.abootsrc != SRC_NONE) 
	{
		term_dat_out(absrcft_msg);
		switch (syscfg.aboot.flt) 
		{
			case FLT_RAW:
				term_dat_out_crlf(absrcraw_msg);
				break;

			case FLT_BIN:
				term_dat_out_crlf(absrcnbin_msg);
				break;

			case FLT_SREC:
				term_dat_out_crlf(absrcsrec_msg);
				break;

			case FLT_ELF:
				term_dat_out_crlf(absrcelf_msg);
				break;

			default:
				/* These should never happen */
				break;
		}

		if (syscfg.aboot.abootsrc != SRC_NAND) 
		{
			term_dat_out(abadd_msg);
			str_makehex(str, syscfg.aboot.loadaddr, 8);
			term_dat_out_crlf(str);
			term_dat_out(abeadd_msg);
			str_makehex(str, syscfg.aboot.startaddr, 8);
			term_dat_out_crlf(str);
		}
	}

	/* Show any scripts */
	term_dat_out((UNS_8 *) "Scripting: ");
	if ((syscfg.scr.number_entries == 0) ||
		(syscfg.scr.enabled == FALSE)) {
		term_dat_out_crlf((UNS_8 *) "Disabled");
	}
	else {
		term_dat_out_crlf((UNS_8 *) "Enabled");
		idx = 0;
		for (i = 0; i < syscfg.scr.number_entries; i++) {
			term_dat_out((UNS_8 *) "-S>");
			term_dat_out_crlf(&syscfg.scr.script_data[idx]);
			idx = idx + syscfg.scr.entry_size[i] + 1;
		}
	}

	/* Call user defined extended info */
	cmd_info_extend();

	return TRUE;
}

/***********************************************************************
 *
 * Function: script_capture
 *
 * Purpose: Captures script entries to play back on S1L boot
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: TRUE if the commands were captured, otherwise FALSE
 *
 * Notes: None
 *
 **********************************************************************/
static BOOL_32 script_capture(void) {
	int sz, idx = 0, i = 0;
	BOOL_32 entered = FALSE, exits = FALSE, ilp;

	term_dat_out_crlf(script_entry_msg);

	/* Get each line */
	while ((exits == FALSE) && (idx < 1024) &&
		(i < MAX_SCRIPT_ENTRIES)) {
		term_dat_out((UNS_8 *) "-S>");

		sz = 0;
		ilp = FALSE;
		while ((idx < 1024) && (ilp == FALSE)) {
			if (term_dat_in_ready() > 0) {
				term_dat_in(&syscfg.scr.script_data[idx], 1);
				syscfg.scr.script_data[idx + 1] = '\0';

				if (syscfg.scr.script_data[idx] == 13) {
					syscfg.scr.script_data[idx] = '\0';
					term_dat_out_crlf((UNS_8 *) "");
					ilp = TRUE;

					if (sz == 0)
						exits = TRUE;
				}
				else {
					term_dat_out(&syscfg.scr.script_data[idx]);
					sz++;
				}

				idx++;
			}
		}

		syscfg.scr.entry_size[i] = sz;
		i++;
	}

	if (idx >= 256)
		term_dat_out_crlf((UNS_8 *)
			"Script buffer overflow, limit 256 bytes!");
	else if (i >= MAX_SCRIPT_ENTRIES)
		term_dat_out_crlf((UNS_8 *)
			"Script entry limit overflow, limit 12 entries!");
	else {
		entered = TRUE;
		syscfg.scr.number_entries = i - 1;
	}

	return entered;
}

/***********************************************************************
 *
 * Function: cmd_script
 *
 * Purpose: Sets up, enables, or disables startup scripting
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: TRUE if the command was accepted, otherwise FALSE
 *
 * Notes: None
 *
 **********************************************************************/
static BOOL_32 cmd_script(void) 
{
	BOOL_32 update = FALSE, parsed = FALSE;
	UNS_8 *str = get_parsed_entry(1);

	if (parse_get_entry_count() == 2) {
		/* on, off, or setup commands? */
		if (str_cmp(str, "on") == 0)
		{
			if ((syscfg.scr.number_entries > 0) &&
				(syscfg.scr.enabled == FALSE)) {
				syscfg.scr.enabled = TRUE;
				update = TRUE;
			}
			parsed = TRUE;
		}
		else if (str_cmp(str, "off") == 0)
		{
			if (syscfg.scr.enabled == TRUE) {
				syscfg.scr.enabled = FALSE;
				update = TRUE;
			}

			parsed = TRUE;
		}
		else if (str_cmp(str, "setup") == 0)
		{
			syscfg.scr.enabled = TRUE;
			update = parsed = script_capture();
		}
		else
			term_dat_out_crlf((UNS_8 *)
				"Invalid script command, type help script for more "
				"info");

		if (update)
			cfg_save(&syscfg);
	}

	return parsed;
}

/***********************************************************************
 *
 * Function: cmd_core_add_commands
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
void cmd_core_add_commands(void)
{
	/* Add core group */
	cmd_add_group(&core_group);

	/* Add commands to the core group */
	cmd_add_new_command(&core_group, &core_baud_cmd);
	cmd_add_new_command(&core_group, &core_bwtest_cmd);
	cmd_add_new_command(&core_group, &core_comp_cmd);
	cmd_add_new_command(&core_group, &core_copy_cmd);
	cmd_add_new_command(&core_group, &core_dump_cmd);
	cmd_add_new_command(&core_group, &core_fill_cmd);
	cmd_add_new_command(&core_group, &core_info_cmd);
	cmd_add_new_command(&core_group, &core_memtst_cmd);
	cmd_add_new_command(&core_group, &core_peek_cmd);
	cmd_add_new_command(&core_group, &core_poke_cmd);
	cmd_add_new_command(&core_group, &core_prompt_cmd);
	cmd_add_new_command(&core_group, &core_rstcfg_cmd);
	cmd_add_new_command(&core_group, &core_script_cmd);
}
