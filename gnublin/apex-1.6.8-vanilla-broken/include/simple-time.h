/* time.h

   written by Marc Singer
   26 Aug 2008

   Copyright (C) 2008 Marc Singer

   -----------
   DESCRIPTION
   -----------

*/

#if !defined (__TIME_H__)
#    define   __TIME_H__

/* ----- Includes */

#include <linux/types.h>

/* ----- Types */

//typedef signed long time_t;

struct tm {
  int tm_sec;                   /* Seconds.     [0-60] (1 leap second) */
  int tm_min;                   /* Minutes.     [0-59] */
  int tm_hour;                  /* Hours.       [0-23] */
  int tm_mday;                  /* Day.         [1-31] */
  int tm_mon;                   /* Month.       [0-11] */
  int tm_year;                  /* Year - 1900. */
  int tm_wday;                  /* Day of week. [0-6] */
  int tm_yday;                  /* Days in year.[0-365] */
  int tm_isdst;                 /* DST.         [-1/0/1]*/

  long int tm_gmtoff;           /* Seconds east of UTC.  */
  const char *tm_zone;          /* Timezone abbreviation.  */
};

/* ----- Globals */

/* ----- Prototypes */

struct tm* gmtime_r (const time_t* time_p, struct tm* tm_p);
char *asctime_r (const struct tm *t, char *buf);

#endif  /* __TIME_H__ */
