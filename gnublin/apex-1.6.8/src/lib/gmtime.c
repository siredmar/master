/* gmtime.c

   written by Marc Singer
   26 Aug 2008

   Copyright (C) 2008 Marc Singer

   -----------
   DESCRIPTION
   -----------

   Simple time conversion functions.

*/

#include <config.h>
#include <linux/string.h>
#include "simple-time.h"

#define SECONDS_PER_DAY		(24*60*60) /* 86400 */

static const char szDay[] =
  "Sun Mon Tue Wed Thu Fri Sat ";
static const char szMonth[] =
  "Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec ";

static const char days_in_month[] = {
  31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };


/** Return whether or not the year is a leap year.  The algoritm we
    use is simple: every fourth year is a leap year except for each
    century (evenly divisible by 100).  Years evenly divisible by 400
    are leap years any way.  That should give us enough accuracy.

    The expression reads that the year modulo 4 is zero and it is not
    modulo 100 unless it is modulo 400.

    Note that an alternative would be to use a bit array.  This isn't
    really a bad idea since the range of time_t value is 31 bits which
    is only 68 years.  The leap years could be encoded as bits in an
    array for 9 bytes.  The annoying part would be constructing the
    bit array. */

static int is_leap (int year)
{
  return !(year%4) && ((year%100) || !(year%400));
}


/** Convert an integer to a pair of ASCII characters.  The two digits
    are written to the first two characters of the given buffer.  The
    function only works for positive integers less than 100.  The
    result is zero padded. */

static void convert_two_digits (char* rgb, int value)
{
  rgb[0] = (value / 10) + '0';
  rgb[1] = (value % 10) + '0';
}


/** Compute the struct tm breakdown for a time_t in the UTC (GMT)
    timezone. */

struct tm* gmtime_r (const time_t* time_p, struct tm* tm_p)
{
  time_t t = *time_p%SECONDS_PER_DAY;
  int year;
  int month;

	/* Time of day is easy */
  tm_p->tm_sec = t%60;
  t /= 60;
  tm_p->tm_min = t%60;
  tm_p->tm_hour = t/60;

	/* Year and day w/in year require a search */
  t = *time_p/SECONDS_PER_DAY;
  tm_p->tm_wday = (t + 4)%7;    /* Day zero was a Thursday */
  for (year = 1970; year < 1970 + 68; ++year) {
    int days = is_leap (year) ? 366 : 365;
    if (t < days)
      break;
    t -= days;
  }
  tm_p->tm_year = year - 1900;
  tm_p->tm_yday = t;

	/* Account for leap day */
  tm_p->tm_mday = 1;
  if (is_leap (year) && t >= 31 + 28) {
    if (t == 31 + 28)
      ++tm_p->tm_mday;
    --t;
  }

	/* Day of the month requires a search */
  for (month = 0;
       month < sizeof (days_in_month)/sizeof (*days_in_month)
         && t > days_in_month[month];
       ++month)
    t -= days_in_month[month];
  tm_p->tm_mday += t;
  tm_p->tm_mon = month;

  return tm_p;
}


/** Convert a broken down time structure to a POSIX standard format,
    "Tue Aug 26 21:49:08 2008\n".  The given buffer must be at least
    25 bytes long. */

char* asctime_r (const struct tm* tm_p, char* sz)
{
  memcpy (sz + 0, &szDay[tm_p->tm_wday * 4], 4);
  memcpy (sz + 4, &szMonth[tm_p->tm_mon * 4], 4);
  convert_two_digits (sz + 8, tm_p->tm_mday);
  if (sz[8] == '0')
    sz[8] = ' ';

  sz[10] = ' ';

  convert_two_digits (sz + 11, tm_p->tm_hour);
  sz[13] = ':';
  convert_two_digits (sz + 14, tm_p->tm_min);
  sz[16] = ':';
  convert_two_digits (sz + 17, tm_p->tm_sec);

  sz[19] = ' ';

  convert_two_digits (sz + 20, (tm_p->tm_year + 1900)/100);
  convert_two_digits (sz + 22, (tm_p->tm_year + 1900)%100);
  sz[24] = '\n';

  return sz;
}
