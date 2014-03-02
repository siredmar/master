/***********************************************************************
 * $Id:: s1l_cmds.c 3357 2010-05-03 18:21:24Z usb10132                 $
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
#include "lpc_line_parser.h"
#include "s1l_sys_inf.h"
#include "s1l_line_input.h"
#include "s1l_cmds.h"

static UNS_8 errorgrp_msg[] = "Group names do nothing";
static UNS_8 errorcmd_msg[] = "Command not recognized";
static UNS_8 errorparse_msg[] =
	"Syntax is not correct for the command :";
static UNS_8 helperr_msg[] =
	"Type 'help [menu, group name, command name]' for a list of all\r\n"
	"commands, commands in a group, or information on a specific\r\n"
	"function.";

/* Parsed field data */
UNS_32 pFData [MAX_ENTRIES];

/* Function prototypes */
BOOL_32 cmd_help(void);

/* Starting command list */
static UNS_32 cmd_help_plist[] =
{
	(PARSE_TYPE_STR | PARSE_TYPE_OPT), /* The "help" command */
	(PARSE_TYPE_STR | PARSE_TYPE_END)
};
static CMD_ROUTE_T cmd_list_head =
{
	(UNS_8 *) "help",
	cmd_help,
	(UNS_8 *) "S1L help group",
	(UNS_8 *) "help [all, menu, group name, command name]",
	cmd_help_plist,
	NULL
};

/* Starting group list */
static GROUP_LIST_T group_list_head =
{
	(UNS_8 *) "help sys", /* help group */
	(UNS_8 *) "Displays help and syntax for a specific command\r\n"
	"Use 'help [all, menu, group name, command name]'",
	&cmd_list_head,
	NULL
};

/***********************************************************************
 *
 * Function: cmd_find_index
 *
 * Purpose: Finds group and command index for a passed name
 *
 * Processing:
 *     Cycles through group lists first. If it finds a match, then it
 *     returns the group index ID to that match with an invalid cmd
 *     index. If a group isn't found, it cycles through the commands in
 *     all groups. If ti finds a match, it returns the group and cmd
 *     index for the command. If a matching group ro command cannot
 *     be found, an invalid group is returned.
 *
 * Parameters:
 *     cname : Name to lookup
 *     pGroup: Pointer to maching group descriptor to populate
 *     pCmd  : Pointer to command index to populate
 *
 * Outputs: None
 *
 * Returns: TRUE if a match was found, otherwise FALSE.
 *
 * Notes: None
 *
 **********************************************************************/
BOOL_32 cmd_find_index(UNS_8 *cname,
					   GROUP_LIST_T **pGroup,
					   CMD_ROUTE_T **pCmd)
{
	GROUP_LIST_T *PGroupList;
	CMD_ROUTE_T *PCmdList = NULL;
	BOOL_32 match = FALSE;

	/* Start at beginning of group list */
	PGroupList = &group_list_head;
	while ((match == FALSE) && (PGroupList != NULL))
	{
		if (str_cmp(cname, PGroupList->group_name) == 0)
		{
			match = TRUE;
		}
		else
		{
			PGroupList = PGroupList->next_group;
		}
	}

	/* Try the command list */
	if (match == FALSE)
	{
		/* Start at beginning of group list */
		PGroupList = &group_list_head;
		while ((match == FALSE) && (PGroupList != NULL))
		{
			/* Start at beginning of command list */
			PCmdList = PGroupList->cmd_list;
			while ((match == FALSE) && (PCmdList != NULL))
			{
				if (str_cmp(cname, PCmdList->keystr) == 0)
				{
					match = TRUE;
				}
				else
				{
					PCmdList = PCmdList->next_cmd;
				}
			}

			if (PCmdList == NULL)
			{
				PGroupList = PGroupList->next_group;
			}
		}
	}

	*pGroup = PGroupList;
	*pCmd = PCmdList;
	if (PGroupList != NULL)
	{
		match = TRUE;
	}

	return match;
}

/***********************************************************************
 *
 * Function: cmd_strout_format
 *
 * Purpose: Aligned output for commands and groups
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     start_offs : Index where to place first string
 *     start_str  : First string
 *     next_offs  : Index where to place second string
 *     next_str   : Second string
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void cmd_strout_format(int start_offs,
				  UNS_8 *start_str,
				  int next_offs,
				  UNS_8 *next_str)
{
	int idx1 = 0;
	UNS_8 *space = (UNS_8 *) " ";

	/* Fill spaces first */
	while (idx1 < start_offs)
	{
		term_dat_out(space);
		idx1++;
	}
	term_dat_out(start_str);

	/* Increment past the first string */
	idx1 += str_size(start_str);
	while (idx1 < next_offs)
	{
		term_dat_out(space);
		idx1++;
	}
	term_dat_out_crlf(next_str);
}

/***********************************************************************
 *
 * Function: cmd_show_cmd_help
 *
 * Purpose: Show help for a specific command
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     pCmd  : Pointer to command to show help for
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void cmd_show_cmd_help(CMD_ROUTE_T *pCmd)
{
	cmd_strout_format(0, (UNS_8 *) "Command:", 12, pCmd->keystr);
	cmd_strout_format(0, (UNS_8 *) "Purpose:", 12, pCmd->help_str);
	cmd_strout_format(0, (UNS_8 *) "syntax:", 12, pCmd->syntax);
}

/***********************************************************************
 *
 * Function: cmd_show_group_help
 *
 * Purpose: Show help for a specific group
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     pGroup     : Pointer to group to show help for
 *     show_cmds : If TRUE, shows commands for group too
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void cmd_show_group_help(GROUP_LIST_T *pGroup,
						 BOOL_32 show_cmds)
{
	CMD_ROUTE_T *pCmd = pGroup->cmd_list;

	/* Display group name and help message first */
	cmd_strout_format(0, pGroup->group_name, 12, pGroup->group_help);

	/* Go through the command list */
	while ((pCmd != NULL) && (show_cmds == TRUE))
	{
		cmd_strout_format(1, pCmd->keystr, 12, pCmd->help_str);
		pCmd = pCmd->next_cmd;
	}
}

/***********************************************************************
 *
 * Function: cmd_show_cmd_syntax
 *
 * Purpose: Show syntax for a specific command
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     pCmd  : Pointer to command to show syntax for
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void cmd_show_cmd_syntax(CMD_ROUTE_T *pCmd)
{
	term_dat_out_crlf(pCmd->keystr);
	term_dat_out_crlf(pCmd->syntax);
}

/***********************************************************************
 *
 * Function: cmd_help
 *
 * Purpose: Processes the help command
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
BOOL_32 cmd_help(void)
{
	GROUP_LIST_T *pGroup = &group_list_head;
	UNS_8 *str = get_parsed_entry(1);
	CMD_ROUTE_T *pCmd;

	/* If this is just the help command with no arguments, then display
	   the standard help message */
	if (parse_get_entry_count() == 1)
	{
		cmd_show_group_help(pGroup, FALSE);
	}
	else
	{
		/* Verify second argument */
		if (str_cmp(str, "menu") == 0)
		{
			/* Show everything */
			while (pGroup != NULL)
			{
				cmd_show_group_help(pGroup, FALSE);
				pGroup = pGroup->next_group;
			}
		}
		else if (str_cmp(str, "all") == 0)
		{
			/* Show everything */
			while (pGroup != NULL)
			{
				cmd_show_group_help(pGroup, TRUE);
				pGroup = pGroup->next_group;
			}
		}
		else
		{
			/* Second argument is either a command or group name, so
			   attempt to find the matching name */
			if (cmd_find_index(str, &pGroup, &pCmd) == FALSE)
			{
				/* No matching group or command */
				term_dat_out_crlf(helperr_msg);
			}
			else
			{
				if (pCmd != NULL)
				{
					cmd_show_cmd_help(pCmd);
				}
				else
				{
					cmd_show_group_help(pGroup, TRUE);
				}
			}
		}
	}

	return TRUE;
}

/***********************************************************************
 *
 * Function: cmd_parse_fields
 *
 * Purpose: Parses and verifies parsed field entries
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     pCmd : Pointer to command to verify parsing for
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
BOOL_32 cmd_parse_fields(CMD_ROUTE_T *pCmd)
{
	UNS_32 tval, *parse_list = pCmd->parse_list;
	int pfield, fields = parse_get_entry_count();
	BOOL_32 good = TRUE;

	/* If the command has a free-form parse, then just exit */
	if ((*parse_list & PARSE_TYPE_FREE) != 0)
	{
		return TRUE;
	}

	/* Process list */
	pfield = 0;
	while ((pfield < fields) && (good == TRUE))
	{
		if (pfield == 0)
		{
			/* This is just the command name */
			tval = (UNS_32) get_parsed_entry(0);
		}
		else
		{
			/* Parse based on expected type */
			switch (*parse_list & 0xF)
			{
				case PARSE_TYPE_STR:
					tval = (UNS_32) get_parsed_entry(pfield);
					break;

				case PARSE_TYPE_DEC:
					good = str_dec_to_val(get_parsed_entry(pfield),
						&tval);
					break;

				case PARSE_TYPE_HEX:
					good = str_hex_to_val(get_parsed_entry(pfield),
						&tval);
					break;

				default:
					good = FALSE;
			}
		}

		/* Save pointer or value */
		pFData [pfield] = tval;

		/* Is this the last entry? */
		if (pfield >= (fields - 1))
		{
			/* Is this entry marked with the end or optional flag? */
			if ((*parse_list & (PARSE_TYPE_OPT | PARSE_TYPE_END)) == 0)
			{
				/* Not, parse line ended unexpectedly */
				good = FALSE;
			}
		}
		else
		{
			/* Verify the parse line isn't too long */
			if ((*parse_list & PARSE_TYPE_END) != 0)
			{
				/* Command line is too long */
				good = FALSE;
			}
		}

		/* Next entries */
		parse_list++;
		pfield++;
	}

	return good;
}

/***********************************************************************
 *
 * Function: cmd_add_group
 *
 * Purpose: Adds a new group entry to the group list
 *
 * Processing:
 *     Add the current group link to the end of the list and set the
 *     current links next pointer to NULL.
 *
 * Parameters:
 *     pGroup: Pointer to group to add
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void cmd_add_group(GROUP_LIST_T *pGroup)
{
	GROUP_LIST_T *pCGroup = &group_list_head;

	/* Find tail of list */
	while (pCGroup->next_group != NULL)
	{
		pCGroup = pCGroup->next_group;
	}

	/* Add group to end of the list */
	pCGroup->next_group = pGroup;
	pGroup->next_group = NULL;
}

/***********************************************************************
 *
 * Function: cmd_add_new_command
 *
 * Purpose: Adds a new command entry to the group
 *
 * Processing:
 *     Add the current commands link to the end of the list and set the
 *     current links next pointer to NULL.
 *
 * Parameters:
 *     pGroup: Pointer to group to add command to
 *     pCmd  : Pointer to command to add to group
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void cmd_add_new_command(GROUP_LIST_T *pGroup,
						 CMD_ROUTE_T *pCmd)
{
	CMD_ROUTE_T *pCCmd = pGroup->cmd_list;

	/* Add entry to start of group if NULL */
	if (pCCmd == NULL)
	{
		pGroup->cmd_list = pCmd;
	}
	else
	{
		/* Find end of the command list */
		while (pCCmd->next_cmd != NULL)
		{
			pCCmd = pCCmd->next_cmd;
		}

	/* Add command link */
	pCCmd->next_cmd = pCmd;
	}

	pCmd->next_cmd = NULL;
}

/***********************************************************************
 *
 * Function: cmd_process
 *
 * Purpose: Parses and processes the passed command string
 *
 * Processing:
 *     Break up the parse string into whitespace seperated components
 *     and convert the components according to the parse list. Route
 *     to the command function based on the parsed command.
 *
 * Parameters:
 *     parse_str : String to parse
 *
 * Outputs: None
 *
 * Returns: TRUE if the command was processed, otherwise FALSE
 *
 * Notes: None
 *
 **********************************************************************/
BOOL_32 cmd_process(UNS_8 *parse_str)
{
	CMD_ROUTE_T *pCmd;
	BOOL_32 cmdok = TRUE;
	GROUP_LIST_T *pGroup = &group_list_head;

	/* Parse the string into whitespace seperated elements and find
	   matching command */
	parse_string(parse_str);
	cmdok = cmd_find_index(get_parsed_entry(0), &pGroup, &pCmd);
	if (cmdok == FALSE)
	{
		term_dat_out_crlf(errorcmd_msg);
	}
	else if (pCmd == NULL)
	{
		/* Group command only */
		term_dat_out_crlf(errorgrp_msg);
	}
	else if (cmd_parse_fields(pCmd) == FALSE)
	{
		term_dat_out(errorparse_msg);
		cmd_show_cmd_syntax(pCmd);
		cmdok = FALSE;
	}
	else
	{
		/* Process command */
		cmdok = pCmd->func();
	}

	return cmdok;
}

/***********************************************************************
 *
 * Function: cmd_get_field_val
 *
 * Purpose:
 *     Retrieves strings or converted values from a parsed command
 *     string
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     parse_index :  Index to return parsed value for
 *
 * Outputs: None
 *
 * Returns: Values for the parsed field
 *
 * Notes: None
 *
 **********************************************************************/
UNS_32 cmd_get_field_val(int parse_index)
{
	return pFData [parse_index];
}
