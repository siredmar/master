/***********************************************************************
 * $Id:: lpc_line_parser.h 933 2008-07-24 18:23:27Z wellsk             $
 *
 * Project: Line parser
 *
 * Description:
 *     Will break up a string of up to 255 characters into multiple
 *     string components (32 max)
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

#ifndef LINEPARSER_H
#define LINEPARSER_H

// Maximum number of parsible entry fields
#ifndef MAX_ENTRIES
#define MAX_ENTRIES 16
#endif

/* Determine if the passed character is a whitespace character, a
   whitespace character is any character that is not between the ASCII
   values of 33 to 126 */
BOOL_32 is_whitespace(UNS_8 ch);

/* Returns the index to non-whitespace from a passed string and index */
int skip_whitespace(UNS_8 *str,
                    int idx);

/* Parses a string into whitespace seperated components */
int parse_string(UNS_8 *str_in);

/* Returns number of parsed entries */
int parse_get_entry_count(void);

/* Returns a parsed entry pointer from an index */
UNS_8 *get_parsed_entry(int index);

#endif /* LINEPARSER_H */
