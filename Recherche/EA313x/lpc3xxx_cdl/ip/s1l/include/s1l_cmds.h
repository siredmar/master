/***********************************************************************
 * $Id:: s1l_cmds.h 871 2008-07-07 22:36:49Z wellsk                    $
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

#include "lpc_types.h"
#include "lpc_line_parser.h"

#ifndef S1L_CMDS_H
#define S1L_CMDS_H

/* Parse definitions */
#define PARSE_TYPE_STR    0x0000    /* Parse field is a string */
#define PARSE_TYPE_DEC    0x0001    /* Parse field is a decimal value */
#define PARSE_TYPE_HEX    0x0002    /* Parse field is a hex value */
#define PARSE_TYPE_FREE   0x0800    /* Free-form fields */
#define PARSE_TYPE_OPT    0x1000    /* Field is an optional end */
#define PARSE_TYPE_END    0x8000    /* Field is last in parse list */

/* Command and routing structure */
typedef BOOL_32 (*PFVRB)(void);
struct CMD_ROUTE_E
{
	UNS_8 *keystr;                   /* Name of the command */
	PFVRB func;                      /* Function pointer for the command */
	UNS_8 *help_str;                 /* Help message for the command */
	UNS_8 *syntax;                   /* Syntax of the command */
	UNS_32 *parse_list;              /* Expected parse list for the command */
	struct CMD_ROUTE_E *next_cmd;    /* Pointer to the next command */
};
typedef struct CMD_ROUTE_E CMD_ROUTE_T;

/* A list of commands for a specific group */
struct GROUP_LIST_E
{
	UNS_8 *group_name;              /* Name of this group */
	UNS_8 *group_help;              /* Description of this group used
									   with the 'help group' command */
	CMD_ROUTE_T *cmd_list;          /* Pointer to the command list */
	struct GROUP_LIST_E *next_group;/* Pointer to the next group */
};
typedef struct GROUP_LIST_E GROUP_LIST_T;

/***********************************************************************
 * S1L commands groups (can add commands to these groups)
 **********************************************************************/

extern GROUP_LIST_T core_group;
extern GROUP_LIST_T nand_group;
extern GROUP_LIST_T image_group;

/* Parsed field data */
extern UNS_32 pFData [MAX_ENTRIES];

/* Adds a new group entry to the group list */
void cmd_add_group(GROUP_LIST_T *pGroup);

/* Adds a new command entry to the group */
void cmd_add_new_command(GROUP_LIST_T *pGroup,
						 CMD_ROUTE_T *pCmd);

/* Parses and processes the passed command string */
BOOL_32 cmd_process(UNS_8 *parse_str);

/* Retrieves strings or converted values from a parsed command string */
UNS_32 cmd_get_field_val(int parse_index);

/* Command block init functions */
void cmd_image_add_commands(void);
void cmd_core_add_commands(void);
void cmd_nand_add_commands(void);

#endif /* S1L_CMDS_H */
