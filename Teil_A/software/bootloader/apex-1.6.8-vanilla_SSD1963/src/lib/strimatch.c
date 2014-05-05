/* strimatch.c

   written by Marc Singer
   4 Aug 2008

   Copyright (C) 2008 Marc Singer

   -----------
   DESCRIPTION
   -----------

*/

#include <linux/types.h>
#include <linux/string.h>
#include <linux/ctype.h>
#include <strimatch.h>

/**
 * strimatch - Case insensitive prefix-able matching.
 * @sz1: String to check for a match
 * @sz2: String against which to match.
 * @cbMin: Minimum match length of sz2.
 *
 * sz1 must be at least cbPrefix characters long, less that or equal
 * in length to sz1, and all characters of sz1 must match sz2.
 */

int strimatch (const char* sz1, const char* sz2, size_t cbMin)
{
  char ch1 = 0;
  char ch2 = 0;

  while (1) {
    ch1 = *sz1++;
    ch2 = *sz2++;

    if (!ch1)
      break;
    if (!ch2)
      break;

    ch1 = tolower (ch1);
    ch2 = tolower (ch2);

    if (ch1 != ch2)
      break;

    if (cbMin)
      --cbMin;
  }

  if (ch1 == 0 && cbMin == 0)
    return 0;                   /* Match */

  return ch1 - ch2;
}

