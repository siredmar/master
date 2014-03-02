/***********************************************************************
 * $Id:: lpc_string.c 934 2008-07-24 18:24:16Z wellsk                  $
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

#include "lpc_string.h"

/***********************************************************************
 *
 * Function: str_upper_to_lower
 *
 * Purpose: Convert an NULL terminated uppercase string to lowercase
 *
 * Processing:
 *     Converts uppercase characters to lowercase.
 *
 * Parameters:
 *     str : String to convert
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void str_upper_to_lower(void *str)
{
  int idx = 0;
  char *strd = (char *) str;

  while (strd[idx] != '\0')
  {
    if ((strd[idx] >= 'A') && (strd[idx] <= 'Z'))
    {
      strd[idx] = (strd[idx] - 'A') + 'a';
    }

    idx++;
  }
}

/***********************************************************************
 *
 * Function: str_lower_to_upper
 *
 * Purpose: Convert an NULL terminated lowercase string to uppercase
 *
 * Processing:
 *     Converts uppercase characters to lowercase.
 *
 * Parameters:
 *     str : String to convert
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void str_lower_to_upper(void *str)
{
  int idx = 0;
  char *strd = (char *) str;

  while (strd[idx] != '\0')
  {
    if ((strd[idx] >= 'a') && (strd[idx] <= 'z'))
    {
      strd[idx] = (strd[idx] - 'a') + 'A';
    }

    idx++;
  }
}

/***********************************************************************
 *
 * Function: str_copy
 *
 * Purpose: Copies a NULL terminated string including NULL terminator
 *
 * Processing:
 *     Copy the string up to the string terminator.
 *
 * Parameters:
 *     str_dest : Destination string to copy to
 *     str_src  : Destination string to copy from
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void str_copy(void *str_dest,
              void *str_src)
{
  int idx = 0;
  char *strd = (char *) str_dest;
  char *strs = (char *) str_src;

  while (strs[idx] != '\0')
  {
    strd[idx] = strs[idx];
    idx++;
  }

  strd[idx] = '\0';
}

/***********************************************************************
 *
 * Function: str_ncopy
 *
 * Purpose:
 *     Copies a NULL terminated string up to the maxlen size or the
 *     NULL byte
 *
 * Processing:
 *     Copy the string up to the string terminator. If the string is
 *     longer than the maximum passed length, then terminate the
 *     destination string at the maximum length.
 *
 * Parameters:
 *     str_dest : Destination string to copy to
 *     str_src  : Destination string to copy from
 *     maxlen   : Maximum length of string (including terminator)
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void str_ncopy(void *str_dest,
               void *str_src,
               int  maxlen)
{
  int idx = 0;
  char *strd = (char *) str_dest;
  char *strs = (char *) str_src;

  while ((strs[idx] != '\0') && (idx < (maxlen - 1)))
  {
    strd[idx] = strs[idx];
    idx++;
  }

  strd[idx] = '\0';
}

/***********************************************************************
 *
 * Function: str_size
 *
 * Purpose:
 *     Determine the size of a string not including the NULL terminator
 *
 * Processing:
 *     Count up to the terminator and return the count.
 *
 * Parameters:
 *     str : String to compute size of
 *
 * Outputs: None
 *
 * Returns: Size of the string in characters
 *
 * Notes: None
 *
 **********************************************************************/
int str_size(void *str)
{
  int idx = 0;
  char *strl = (char *) str;

  while (strl[idx] != '\0')
  {
    idx++;
  }

  return idx;
}

/***********************************************************************
 *
 * Function: str_cmp
 *
 * Purpose: Compare strings up to the NULL terminator
 *
 * Processing:
 *     Calls str_ncmp() with the string size to compare.
 *
 * Parameters:
 *     str1 : String 1 to compare
 *     str2 : String 2 to compare
 *
 * Outputs: None
 *
 * Returns: 0 if the strings are equal, otherwise non-zero
 *
 * Notes: None
 *
 **********************************************************************/
int str_cmp(void *str1,
            void *str2)
{
  /* Compare including terminator */
  return str_ncmp(str1, str2, (1 + str_size(str1)));
}

/***********************************************************************
 *
 * Function: str_ncmp
 *
 * Purpose: Compare strings with the passed length
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     str1 : String 1 to compare
 *     str2 : String 2 to compare
 *
 * Outputs: None
 *
 * Returns: 0 if the strings are equal, otherwise non-zero
 *
 * Notes: None
 *
 **********************************************************************/
int str_ncmp(void *str1, void *str2, int sz)
{
  int notsame = 0;
  UNS_8 *str11 = (UNS_8 *) str1;
  UNS_8 *str22 = (UNS_8 *) str2;

  while (sz > 0)
  {
    if (*str22 != *str11)
    {
      sz = 0;
      notsame = 1;
    }

    sz--;
    str11++;
    str22++;
  }

  return notsame;
}

/***********************************************************************
 *
 * Function: val_to_hex_char
 *
 * Purpose: Generate a hex character for the passed value (0 to 15)
 *
 * Processing:
 *     Make a hex string from an input value.
 *
 * Parameters:
 *     str    : Where to place string
 *     val    : Value to generate hex string for
 *     places : Number of leading 0's for string
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
UNS_8 val_to_hex_char(UNS_32 val)
{
  UNS_8 ret = '0';

  val = val & 0xF;
  if (val <= 9)
  {
    ret = '0' + val;
  }
  else
  {
    ret = 'a' + (val - 10);
  }

  return ret;
}

/***********************************************************************
 *
 * Function: hex_char_to_val
 *
 * Purpose: Generate a value (0 to 15) for the passed hex char
 *
 * Processing:
 *     Determine the input value for the passed hex character..
 *
 * Parameters:
 *     ch  : Character to convert
 *     val : Where to place converted value
 *
 * Outputs: None
 *
 * Returns: TRUE if the conversion was good, otherwise FALSE
 *
 * Notes: None
 *
 **********************************************************************/
BOOL_32 hex_char_to_val(UNS_8 ch,
                        UNS_32 *val)
{
  UNS_32 ret = 0;
  BOOL_32 good = TRUE;

  if ((ch >= '0') && (ch <= '9'))
  {
    ret = (UNS_32)(ch - '0');
  }
  else if ((ch >= 'a') && (ch <= 'f'))
  {
    ret = 10 + (UNS_32)(ch - 'a');
  }
  else
  {
    good = FALSE;
  }
  *val = ret;

  return good;
}

/***********************************************************************
 *
 * Function: dec_char_to_val
 *
 * Purpose: Generate a value (0 to 15) for the passed decimal char
 *
 * Processing:
 *     Determine the input value for the passed decimal character..
 *
 * Parameters:
 *     ch  : Character to convert
 *     val : Where to place converted value
 *
 * Outputs: None
 *
 * Returns: TRUE if the conversion was good, otherwise FALSE
 *
 * Notes: None
 *
 **********************************************************************/
BOOL_32 dec_char_to_val(UNS_8 ch,
                        UNS_32 *val)
{
  UNS_32 ret = 0;
  BOOL_32 good = TRUE;

  if ((ch >= '0') && (ch <= '9'))
  {
    ret = (UNS_32)(ch - '0');
  }
  else
  {
    good = FALSE;
  }
  *val = ret;

  return good;
}

/***********************************************************************
 *
 * Function: str_makehex
 *
 * Purpose: Convert a value to a hex string
 *
 * Processing:
 *     Make a terminated hex string from an input value.
 *
 * Parameters:
 *     str    : Where to place string
 *     val    : Value to generate hex string for
 *     places : Number of leading 0's for string
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void str_makehex(UNS_8 *str,
                 UNS_32 val,
                 int places)
{
  UNS_32 vm;
  int shift, idx;

  str [0] = '0';
  str [1] = 'x';

  idx = 2;
  while (places > 0)
  {
    shift = (places - 1) * 4;
    vm = (val >> shift) & 0xF;
    str [idx] = val_to_hex_char(vm);
    idx++;
    places--;
  }
  str [idx] = '\0';
}

/***********************************************************************
 *
 * Function: str_makedec
 *
 * Purpose: Convert a value to a decimal string
 *
 * Processing:
 *     Make a terminated decimal string from an input value.
 *
 * Parameters:
 *     str    : Where to place string
 *     val    : Value to generate decimal string for
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void str_makedec(UNS_8 *str,
                 UNS_32 val)
{
  UNS_32 wc, inx;
  UNS_8 rv [32];
  int idx, lp;

  idx = 0;
  if (val == 0)
  {
    rv [0] = '0';
    idx = 1;
  }
  else
  {
    while (val > 0)
    {
      inx = val / 10;
      wc = val - (inx * 10);
      rv [idx] = '0' + wc;
      val = inx;
      idx++;
    }
  }

  /* Generate return string */
  lp = 0;
  idx--;
  while (idx >= 0)
  {
    str [lp] = rv [idx];
    lp++;
    idx--;
  }
  str [lp] = '\0';
}

/***********************************************************************
 *
 * Function: str_hex_to_val
 *
 * Purpose: Convert a hex string to a value
 *
 * Processing:
 *     Convert a hex string.to a value.
 *
 * Parameters:
 *     str    : Pointer to string to convert
 *     val    : Where to place the generated value
 *
 * Outputs: None
 *
 * Returns: TRUE if the convesion was successful, otherwise FALSE
 *
 * Notes: None
 *
 **********************************************************************/
BOOL_32 str_hex_to_val(UNS_8 *str,
                       UNS_32 *val)
{
  int idx, shift = 0;
  UNS_32 lval, tval = 0;
  BOOL_32 good = TRUE;

  /* Is start of string valid? */
  if ((str[0] == '0') && (str[1] == 'x'))
  {
    /* Find end of string */
    idx = str_size(str) - 1;

    /* Start at end of the string */
    while (idx > 1)
    {
      good &= hex_char_to_val(str [idx], &lval);
      tval = tval + (lval << shift);
      idx--;
      shift += 4;
    }
  }
  else
  {
    good = FALSE;
  }
  *val = tval;

  return good;
}

/***********************************************************************
 *
 * Function: str_dec_to_val
 *
 * Purpose: Convert a decimal string to a value
 *
 * Processing:
 *     Convert a hex string.to a value.
 *
 * Parameters:
 *     str : Pointer to string to convert
 *     val : Where to place the generated value
 *
 * Outputs: None
 *
 * Returns: TRUE if the conversion was successful, otherwise FALSE
 *
 * Notes: None
 *
 **********************************************************************/
BOOL_32 str_dec_to_val(UNS_8 *str,
                       UNS_32 *val)
{
  int idx, dec10 = 1, neg = 0;
  UNS_32 lval, tval = 0;
  BOOL_32 good = TRUE;

  /* Find end of string */
  idx = str_size(str) - 1;

  /* Negative sign? */
  if (str [0] == '-')
  {
    neg = 1;
  }

  /* Start at end of the string */
  while (idx >= neg)
  {
    good &= dec_char_to_val(str [idx], &lval);
    tval = tval + (lval * dec10);
    idx--;
    dec10 = dec10 * 10;
  }

  if (neg == 1)
  {
    tval = -tval;
  }
  *val = tval;

  return good;
}

