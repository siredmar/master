/***********************************************************************
 * $Id:: lpc_line_parser.c 934 2008-07-24 18:24:16Z wellsk             $
 *
 * Project: Line parser utilities
 *
 * Description:
 *     Functions for parsing and querying line fields.
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

#include "lpc_line_parser.h"

/* Local data */
static UNS_8 *parsed_entries[MAX_ENTRIES];
static int str_index;
static int parsed_entry_count;
static UNS_8 str_image[256];

/* Structure defining a region of white data */
typedef struct
{
  UNS_8 minchar;
  UNS_8 maxchar;
} PARSER_WS_CHAR_T;

/* Array of whitespace regions */
static PARSER_WS_CHAR_T wspace_data[] =
{
  {0, 32},
  {127, 255},
  {255, 0} /* Exit sequence */
};

/***********************************************************************
 *
 * Function: parse_limit_str
 *
 * Purpose: Limits string size to 255 characters
 *
 * Processing:
 *     Finds the string terminator. If the terminator exceeds a string
 *     length fo 255 characters, the terminator is added at position
 *     256.
 *
 * Parameters:
 *     str : Input string
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
static void parse_limit_str(UNS_8 *str)
{
  int idx = 0;

  while ((str[idx] != '\0') && (idx < 256))
  {
    idx++;
  }

  if (idx == 256)
  {
    str[255] = '\0';
  }
}

/***********************************************************************
 *
 * Function: copy_field
 *
 * Purpose: Copies a parsed field to the local string
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     str : Pointer to a string
 *     idx : Index into string to start of parsed data
 *
 * Outputs: None
 *
 * Returns: The index in the string passed the copied field
 *
 * Notes: None
 *
 **********************************************************************/
static int copy_field(UNS_8 *str,
                      int idx)
{
  parsed_entries[parsed_entry_count] = &str_image[str_index];
  parsed_entry_count++;

  /* Copy string */
  while (is_whitespace(str[idx]) == FALSE)
  {
    str_image[str_index] = str[idx];
    str_index++;
    idx++;
  }
  str_image[str_index] = '\0';
  str_index++;

  return idx;
}

/***********************************************************************
 *
 * Function: is_whitespace
 *
 * Purpose:
 *     Determine if the passed character is a whitespace character, a
 *     whitespace character is any character that is not between the
 *     ASCII values of 33 to 126
 *
 * Processing:
 *     Loop through the whitespace list and check the passed character
 *     against the list.
 *
 * Parameters:
 *     ch : Character to check if it is whitespace
 *
 * Outputs: None
 *
 * Returns: The number of parsed elements
 *
 * Notes: None
 *
 **********************************************************************/
BOOL_32 is_whitespace(UNS_8 ch)
{
  int idx = 0;
  BOOL_32 ws = FALSE;

  while ((wspace_data[idx].minchar <= wspace_data[idx].maxchar) &&
         (ws == FALSE))
  {
    if ((ch >= wspace_data[idx].minchar) &&
        (ch <= wspace_data[idx].maxchar))
    {
      ws = TRUE;
    }
    idx++;
  }

  return ws;
}

/***********************************************************************
 *
 * Function: skip_whitespace
 *
 * Purpose:
 *     Returns the index to non-whitespace from a passed string and
 *     index
 *
 * Processing:
 *     Loop through passed string at the passed index and exit with the
 *     new index when whitespace isn't found.
 *
 * Parameters:
 *     str : Input string
 *     idx : Index into string to start search
 *
 * Outputs: None
 *
 * Returns: The index to where whitespace ends
 *
 * Notes: None
 *
 **********************************************************************/
int skip_whitespace(UNS_8 *str,
                    int idx)
{
  while (is_whitespace(str[idx]) == TRUE)
  {
    idx++;
  }

  return idx;
}

/***********************************************************************
 *
 * Function: parse_string
 *
 * Purpose: Parses a string into whitespace seperated components
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     str_in : Input string terminated by \0 (max 255 characters)
 *
 * Outputs: None
 *
 * Returns: The number of parsed entries
 *
 * Notes: None
 *
 **********************************************************************/
int parse_string(UNS_8 *str_in)
{
  int idx = 0;

  /* Limit string size */
  parse_limit_str(str_in);
  str_index = 0;
  parsed_entry_count = 0;

  /* Parse until line terminator is found */
  while ((str_in[idx] != '\0') && (parsed_entry_count < MAX_ENTRIES))
  {
    idx = skip_whitespace(str_in, idx);
    if (str_in[idx] != '\0')
    {
      idx = copy_field(str_in, idx);
    }
  }

  return parsed_entry_count;
}

/***********************************************************************
 *
 * Function: parse_get_entry_count
 *
 * Purpose: Returns number of parsed entries
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Number of parsed entries on last parse
 *
 * Notes: None
 *
 **********************************************************************/
int parse_get_entry_count(void)
{
  return parsed_entry_count;
}

/***********************************************************************
 *
 * Function: get_parsed_entry
 *
 * Purpose: Returns a parsed entry pointer from an index
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     index : Indexed entry to get
 *
 * Outputs: None
 *
 * Returns: String pointer to parsed entry
 *
 * Notes: None
 *
 **********************************************************************/
UNS_8 *get_parsed_entry(int index)
{
  if (index < parsed_entry_count)
  {
    return parsed_entries[index];
  }
  else
  {
    return NULL;
  }
}
