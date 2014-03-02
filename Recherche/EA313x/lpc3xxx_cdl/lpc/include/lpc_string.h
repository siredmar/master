/***********************************************************************
 * $Id:: lpc_string.h 933 2008-07-24 18:23:27Z wellsk                  $
 *
 * Project: Miscellaneous string utilities
 *
 * Description:
 *     Just miscellaneous string utilities
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

#ifndef LPC_STRING_H
#define LPC_STRING_H

/* Convert an NULL terminated uppercase string to lowercase */
void str_upper_to_lower(void *str);

/* Convert an NULL terminated lowercase string to uppercase */
void str_lower_to_upper(void *str);

/* Copies a NULL terminated string including NULL terminator */
void str_copy(void *str_dest,
              void *str_src);

/* Copies a NULL terminated string up to the maxlen size or the NULL
   byte */
void str_ncopy(void *str_dest,
               void *str_src,
               int  maxlen);

/* Determine the size of a string not including the NULL terminator */
int str_size(void *str);

/* Compare strings up to the NULL terminator */
int str_cmp(void *str1,
            void *str2);

/* Compare strings with the passed length */
int str_ncmp(void *str1,
             void *str2,
             int sz);

/* Generate a hex character for the passed value (0 to 15) */
UNS_8 val_to_hex_char(UNS_32 val);

/* Generate a value (0 to 15) for the passed hex char */
BOOL_32 hex_char_to_val(UNS_8 ch,
                        UNS_32 *val);

/* Generate a value (0 to 15) for the passed decimal char */
BOOL_32 dec_char_to_val(UNS_8 ch,
                        UNS_32 *val);

/* Convert a value to a hex string */
void str_makehex(UNS_8 *str,
                 UNS_32 val,
                 int places);

/* Convert a value to a decimal string */
void str_makedec(UNS_8 *str,
                 UNS_32 val);

/* Convert a hex string to a value */
BOOL_32 str_hex_to_val(UNS_8 *str,
                       UNS_32 *val);

/* Convert a decimal string to a value */
BOOL_32 str_dec_to_val(UNS_8 *str,
                       UNS_32 *val);

#endif /* LPC_STRING_H */
