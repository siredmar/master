/* timer-s3c2410.c

   written by Marc Singer
   1 Nov 2004

   with modifications by David Anders
   06 Nov 2005

   Copyright (C) 2004 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   the timer running at these settings gives us about a 5.125 second
   cycle time which is probably not as good as it should be. a more
   accurate method would be to set the timer to a 1 second cycle time
   and bond the timing to the real time clock seconds and minutes.


*/

#include <service.h>
#include "s3c2410.h"

static void s3c2410_timer_init(void)
{

	TCFG0 = 0xFF00;
	TCFG1 = 0x030000;
	TCNTB4 = 0xFFFF;
	TCON = 0x700000;	/* load counter value */

	TCON = 0x500000;	/* start timer */

}

static void s3c2410_timer_release(void)
{

	TCON = 0x00;		/* stop timer */

}

unsigned long timer_read(void)
{
	static unsigned long valueLast;
	static unsigned long wrap;

	unsigned long value;

	value = TCNTO4;

	if (value > valueLast)
		++wrap;

	valueLast = value;

	return (wrap * 0x10000) + 0x10000 - value;
}

/* timer_delta

   returns the difference in time in milliseconds.

 */

unsigned long timer_delta(unsigned long start, unsigned long end)
{
	return (end - start) / 12;
}

static __service_2 struct service_d s3c2410_timer_service = {
	.init = s3c2410_timer_init,
	.release = s3c2410_timer_release,
};
