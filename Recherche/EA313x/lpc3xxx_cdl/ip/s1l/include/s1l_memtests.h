/***********************************************************************
 * $Id:: s1l_memtests.h 871 2008-07-07 22:36:49Z wellsk                $
 *
 * Project: Memory tests
 *
 * Description:
 *     Various memory tests
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

#ifndef S1L_MEMTESTS_H
#define S1L_MEMTESTS_H

/* Memory test selection */
typedef enum
{
	MTST_FIRST = 0,
	MTST_WALKING1 = MTST_FIRST, /* Walking 1 test */
	MTST_WALKING0,              /* Walking 0 test */
	MTST_INVADDR,               /* Inverse address test */
	MTST_PATTERN,               /* 0xAA and 0x55 pattern test */
    MTST_SET_CLR,               /* All 0x0 and 0xFF test */
	MTST_LAST,
	MTST_ALL = 0x7FFFFFFF       /* All tests */
} MEMTEST_TEST_T;

/* High level memory test start function */
void memory_test(UNS_32 hexaddr,
				 UNS_32 bytes,
				 UNS_32 width,
				 UNS_32 tstnum,
				 UNS_32 iters);

/* Bandwidth test */
void bw_test(UNS_32 hexaddr1,
			 UNS_32 hexaddr2,
			 UNS_32 bytes,
  		     UNS_32 loops);     /* Number of times to run the test */

void mmove(UNS_32 hexaddr1,   /* Must be 32-bit aligned */
		   UNS_32 hexaddr2,   /* Must be 32-bit aligned */
		   UNS_32 bytes);     /* Must be dividable by 16 */

#endif /* S1L_MEMTESTS_H */
