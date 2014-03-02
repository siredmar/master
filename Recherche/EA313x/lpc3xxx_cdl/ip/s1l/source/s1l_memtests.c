/***********************************************************************
 * $Id:: s1l_memtests.c 871 2008-07-07 22:36:49Z wellsk                $
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
#include "lpc_string.h"
#include "s1l_sys_inf.h"
#include "s1l_memtests.h"

static UNS_8 w1s_msg[] = "Starting walking 1 test\n\r";
static UNS_8 w0s_msg[] = "Starting walking 0 test\n\r";
static UNS_8 invtst_msg[] = "Inverse address test\n\r";
static UNS_8 pattst_msg[] = "Pattern test\n\r";
static UNS_8 invwidth_msg[] =
	"Invalid width, must be 1, 2, or 4 bytes\n\r";
static UNS_8 addr_msg[]  = "Address      :";
static UNS_8 width_msg[] = "Width (bytes):";
static UNS_8 bytes_msg[] = "Bytes        :";
static UNS_8 passed_msg[] = "Test passed\n\r";
static UNS_8 failed1_msg[] = "Test failed at address ";
static UNS_8 failed2_msg[] = "Expected pattern: ";
static UNS_8 failed3_msg[] = "Actual pattern  : ";
static UNS_8 start_msg[] = "Starting test....";
static UNS_8 verify_msg[] = "Verifying data....";
static UNS_8 bwc_msg[] = "Bandwidth test complete\n\r";
static UNS_8 bwc1_msg[] = "Number of bytes transferred:";
static UNS_8 bwc2_msg[] = "Timer ticks to transfer    :";
static UNS_8 bwc3_msg[] = "Timer rate (ticks/sec)     :";

/* Patterns for tests */
static UNS_32 patts_55aa [] = {
	0xAAAAAAAA, 0x55555555, 0xA5A5A5A5, 0x5A5A5A5A};
static UNS_32 patts_00ff [] = {0x00000000, 0xFFFFFFFF};

/***********************************************************************
 *
 * Function: memtest_disp_addr
 *
 * Purpose: Display test message string and values
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     hdr     : Test message to display
 *     hexaddr : Pointer to starting address of test
 *     bytes   : Pointer to number of bytes to test
 *     width   : Pointer to data width (1, 2, or 4)
 *
 * Outputs: None
 *
 * Returns: TRUE if the test is ok, otherwise FALSE
 *
 * Notes: None
 *
 **********************************************************************/
BOOL_32 memtest_disp_addr(UNS_8 *hdr,
	   				      UNS_32 *hexaddr,
					      UNS_32 *bytes,
					      UNS_32 *width)
{
	UNS_8 str [16];
	UNS_32 tmp32;
	BOOL_32 goodw = FALSE;

	if ((*width == 1) || (*width == 2) || (*width == 4))
	{
		/* Adjust address and size to data width if necessary */
		if ((*hexaddr & ~(*width - 1)) != *hexaddr)
		{
			tmp32 = *hexaddr & ~(*width - 1);
			tmp32 = tmp32 + *width;
			*bytes = *bytes - (*hexaddr - tmp32);
			*bytes = *bytes & ~(*width - 1);
			*hexaddr = tmp32;
		}

		if (hdr != NULL)
		{
			term_dat_out(hdr);

			term_dat_out(addr_msg);
			str_makehex(str, *hexaddr, 8);
			term_dat_out_crlf(str);

			term_dat_out(bytes_msg);
			str_makedec(str, *bytes);
			term_dat_out_crlf(str);

			term_dat_out(width_msg);
			str_makedec(str, *width);
			term_dat_out_crlf(str);
		}

		goodw = TRUE;
	}
	else
	{
		term_dat_out(invwidth_msg);
	}

	return goodw;
}

/***********************************************************************
 *
 * Function: memtest_pf
 *
 * Purpose: Display test padd/fail status and results
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     pass     : Test pass/fail flag
 *     width    : Data width (1, 2, or 4)
 *     addr     : Starting address of test
 *     actual   : Actual value
 *     expected : Expected value
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void memtest_pf(BOOL_32 pass,
				UNS_32 width,
				UNS_32 addr,
				UNS_32 actual,
				UNS_32 expected)
{
	UNS_8 str[16];

	if (pass == FALSE)
	{
		term_dat_out(failed1_msg);
		str_makehex(str, addr, (width * 2));
		term_dat_out_crlf(str);

		term_dat_out(failed2_msg);
		str_makehex(str, expected, (width * 2));
		term_dat_out_crlf(str);
		
		term_dat_out(failed3_msg);
		str_makehex(str, actual, (width * 2));
		term_dat_out_crlf(str);
	}
	else
	{
		term_dat_out(passed_msg);
	}
}

/***********************************************************************
 *
 * Function: memtest_w10
 *
 * Purpose: Walking 1/0 memory test
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     w1      : TRUE for walking 1 test, FALSE for walking 0
 *     hexaddr : Starting address of test
 *     bytes   : Number of bytes to test
 *     width   : Data width (1, 2, or 4)
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void memtest_w10(BOOL_32 w1,
				 UNS_32 hexaddr,
			 	 UNS_32 bytes,
				 UNS_32 width)
{
	UNS_8 tmp8, *p8;
	UNS_16 tmp16, *p16;
	UNS_32 tmp32, tmp32a, tmp32b, lastaddr, *p32;
	int windex;

	/* Display message */
	if (w1 == TRUE)
	{
		if (memtest_disp_addr(w1s_msg, &hexaddr, &bytes, &width) == FALSE)
		{
			return;
		}
	}
	else
	{
		if (memtest_disp_addr(w0s_msg, &hexaddr, &bytes, &width) == FALSE)
		{
			return;
		}
	}

	/* Setup test */
	lastaddr = hexaddr + bytes;
	windex = 0;

	/* Fill test data into memory */
	term_dat_out(start_msg);
	for (tmp32 = hexaddr; tmp32 < lastaddr; tmp32 += width)
	{
		switch (width)
		{
			case 1:
				p8 = (UNS_8 *) tmp32;
				if (w1 == TRUE)
				{
					*p8 = (UNS_8) _BIT(windex);
				}
				else
				{
					*p8 = 0xFF - (UNS_8) _BIT(windex);
				}
				break;

			case 2:
				p16 = (UNS_16 *) tmp32;
				if (w1 == TRUE)
				{
					*p16 = (UNS_16) _BIT(windex);
				}
				else
				{
					*p16 = 0xFFFF - (UNS_16) _BIT(windex);
				}
				break;

			case 4:
				p32 = (UNS_32 *) tmp32;
				if (w1 == TRUE)
				{
					*p32 = (UNS_32) _BIT(windex);
				}
				else
				{
					*p32 = 0xFFFFFFFF - (UNS_32) _BIT(windex);
				}
				break;
		}

		windex++;
		if (windex >= (width * 8))
		{
			windex = 0;
		}
	}

	/* Setup verify */
	windex = 0;

	/* Fill test data into memory */
	term_dat_out(verify_msg);
	for (tmp32 = hexaddr; tmp32 < lastaddr; tmp32 += width)
	{
		switch (width)
		{
			case 1:
				p8 = (UNS_8 *) tmp32;
				if (w1 == TRUE)
				{
					if (*p8 != (UNS_8) _BIT(windex))
					{
						memtest_pf(FALSE, 1, (UNS_32) p8, (UNS_32) *p8,
							(UNS_32) _BIT(windex));
						return;
					}
				}
				else
				{
					tmp8 = 0xFF - (UNS_8) _BIT(windex);
					if (*p8 != tmp8)
					{
						memtest_pf(FALSE, 1, (UNS_32) p8, (UNS_32) *p8,
							(UNS_32) tmp8);
						return;
					}
				}
				break;

			case 2:
				p16 = (UNS_16 *) tmp32;
				if (w1 == TRUE)
				{
					if (*p16 != (UNS_16) _BIT(windex))
					{
						memtest_pf(FALSE, 2, (UNS_32) p16, (UNS_32) *p16,
							(UNS_32) _BIT(windex));
						return;
					}
				}
				else
				{
					tmp16 = 0xFFFF - (UNS_16) _BIT(windex);
					if (*p16 != tmp16)
					{
						memtest_pf(FALSE, 2, (UNS_32) p16, (UNS_32) *p16,
							(UNS_32) tmp16);
						return;
					}
				}
				break;

			case 4:
				p32 = (UNS_32 *) tmp32;
				if (w1 == TRUE)
				{
					tmp32a = (UNS_32) _BIT(windex);
					tmp32b = *p32;
					if (tmp32b != tmp32a)
					{
						memtest_pf(FALSE, 4, (UNS_32) p32, (UNS_32) tmp32b,
							tmp32a);
						return;
					}
				}
				else
				{
					tmp32a = 0xFFFFFFFF - (UNS_32) _BIT(windex);
					tmp32b = *p32;
					if (tmp32b != tmp32a)
					{
						memtest_pf(FALSE, 4, (UNS_32) p32, tmp32b, tmp32a);
						return;
					}
				}
				break;
		}

		windex++;
		if (windex >= (width * 8))
		{
			windex = 0;
		}
	}

	/* Display passed message */
	memtest_pf(TRUE, width, 0, 0, 0);
}

/***********************************************************************
 *
 * Function: memtest_ia
 *
 * Purpose: Inverse address test
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     hexaddr : Starting address of test
 *     bytes   : Number of bytes to test
 *     width   : Data width (1, 2, or 4)
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void memtest_ia(UNS_32 hexaddr,
			 	UNS_32 bytes,
				UNS_32 width)
{
	UNS_8 tmp8, *p8;
	UNS_16 tmp16, *p16;
	UNS_32 tmp32, tmp32a, tmp32b, lastaddr, *p32;

	/* Display message */
	if (memtest_disp_addr(invtst_msg, &hexaddr, &bytes, &width) == FALSE)
	{
		return;
	}

	/* Setup test */
	lastaddr = hexaddr + bytes;

	/* Fill test data into memory */
	term_dat_out(start_msg);
	for (tmp32 = hexaddr; tmp32 < lastaddr; tmp32 += width)
	{
		switch (width)
		{
			case 1:
				p8 = (UNS_8 *) tmp32;
				*p8 = ~((UNS_8) (tmp32 & 0xFF));
				break;

			case 2:
				p16 = (UNS_16 *) tmp32;
				*p16 = ~((UNS_16) (tmp32 & 0xFFFF));
				break;

			case 4:
				p32 = (UNS_32 *) tmp32;
				*p32 = ~tmp32;
				break;
		}
	}

	/* Verify test data */
	term_dat_out(verify_msg);
	for (tmp32 = hexaddr; tmp32 < lastaddr; tmp32 += width)
	{
		switch (width)
		{
			case 1:
				p8 = (UNS_8 *) tmp32;
				tmp8 = ~((UNS_8) (tmp32 & 0xFF));
				if (*p8 != tmp8)
				{
					memtest_pf(FALSE, 1, (UNS_32) p8, *p8, tmp8);
					return;
				}
				break;

			case 2:
				p16 = (UNS_16 *) tmp32;
				tmp16 = ~((UNS_16) (tmp32 & 0xFFFF));
				if (*p16 != tmp16)
				{
					memtest_pf(FALSE, 2, (UNS_32) p16, *p16, tmp16);
					return;
				}
				break;

			case 4:
				p32 = (UNS_32 *) tmp32;
				tmp32a = ~tmp32;
				tmp32b = *p32;
				if (tmp32b != tmp32a)
				{
					memtest_pf(FALSE, 4, (UNS_32) p32, tmp32b, tmp32a);
					return;
				}
				break;
		}
	}

	/* Display passed message */
	memtest_pf(TRUE, width, 0, 0, 0);
}

/***********************************************************************
 *
 * Function: memtest_pt
 *
 * Purpose: Pattern tests
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     hexaddr  : Starting address of test
 *     bytes    : Number of bytes to test
 *     width    : Data width (1, 2, or 4)
 *     patterns : Array of check patterns
 *     numpatts : Number of check patterns
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void memtest_pt(UNS_32 hexaddr,
			 	UNS_32 bytes,
				UNS_32 width,
				UNS_32 *patterns,
				int numpatts)
{
	UNS_8 tmp8, *p8;
	UNS_16 tmp16, *p16;
	UNS_32 tmp32, tmp32a, tmp32b, lastaddr, *p32;
	int idx;

	/* Display message */
	if (memtest_disp_addr(pattst_msg, &hexaddr, &bytes, &width) == FALSE)
	{
		return;
	}

	/* Setup test */
	lastaddr = hexaddr + bytes;
	idx = 0;

	while (idx < numpatts)
	{
		/* Fill pattern into memory */
		term_dat_out(start_msg);
		for (tmp32 = hexaddr; tmp32 < lastaddr; tmp32 += width)
		{
			switch (width)
			{
				case 1:
					p8 = (UNS_8 *) tmp32;
					*p8 = (UNS_8) (patterns [idx] & 0xFF);
					break;

				case 2:
					p16 = (UNS_16 *) tmp32;
					*p16 = (UNS_16) (patterns [idx] & 0xFFFF);
					break;

				case 4:
					p32 = (UNS_32 *) tmp32;
					*p32 = patterns [idx];
					break;
			}
		}

		/* Verify test data */
		term_dat_out(verify_msg);
		for (tmp32 = hexaddr; tmp32 < lastaddr; tmp32 += width)
		{
			switch (width)
			{
				case 1:
					p8 = (UNS_8 *) tmp32;
					tmp8 = (UNS_8) (patterns [idx] & 0xFF);
					if (*p8 != tmp8)
					{
						memtest_pf(FALSE, 1, (UNS_32) p8, *p8, tmp8);
						return;
					}
					break;

				case 2:
					p16 = (UNS_16 *) tmp32;
					tmp16 = (UNS_16) (patterns [idx] & 0xFFFF);
					if (*p16 != tmp16)
					{
						memtest_pf(FALSE, 2, (UNS_32) p16, *p16, tmp16);
						return;
					}
					break;

				case 4:
					p32 = (UNS_32 *) tmp32;
					tmp32a = patterns [idx];
					tmp32b = *p32;
					if (tmp32b != tmp32a)
					{
						memtest_pf(FALSE, 4, (UNS_32) p32, tmp32b, tmp32a);
						return;
					}
					break;
			}
		}

		idx++;
	}

	/* Display passed message */
	memtest_pf(TRUE, width, 0, 0, 0);
}

/***********************************************************************
 *
 * Function: memory_test
 *
 * Purpose: High level memory test start function
 *
 * Processing:
 *     Perform a memory test based on the pased values.
 *
 * Parameters:
 *     hexaddr : Starting address of test
 *     bytes  : Number of bytes to test
 *     width  : Data width (1, 2, or 4)
 *     tstnum : Test number
 *     iters  : Number of test iterations
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void memory_test(UNS_32 hexaddr,
				 UNS_32 bytes,
				 UNS_32 width,
				 UNS_32 tstnum,
				 UNS_32 iters)
{
	while (iters > 0)
	{
		/* Perform test based on passed testnum */
		if (tstnum == (UNS_32) MTST_ALL)
		{
			/* Do all tests */
			memtest_w10(TRUE, hexaddr, bytes, width);
			memtest_w10(FALSE, hexaddr, bytes, width);
			memtest_ia(hexaddr, bytes, width);
			memtest_pt(hexaddr, bytes, width, patts_55aa, 4);
			memtest_pt(hexaddr, bytes, width, patts_00ff, 2);
		}
		else
		{
			switch ((MEMTEST_TEST_T) tstnum)
			{
				case MTST_WALKING1:
					memtest_w10(TRUE, hexaddr, bytes, width);
					break;

				case MTST_WALKING0:
					memtest_w10(FALSE, hexaddr, bytes, width);
					break;

				case MTST_INVADDR:
					memtest_ia(hexaddr, bytes, width);
					break;

				case MTST_PATTERN:
					memtest_pt(hexaddr, bytes, width, patts_55aa, 4);
					break;

				case MTST_SET_CLR:
					memtest_pt(hexaddr, bytes, width, patts_00ff, 2);
					break;

				default:
					break;
			}
		}

		iters--;
	}
}

/***********************************************************************
 *
 * Function: bw_test
 *
 * Purpose: Bandwidth test
 *
 * Processing:
 *     Perform a bandwidth test based on the pased values.
 *
 * Parameters:
 *     hexaddr1 : Source address of test
 *     hexaddr2 : Destination address of test
 *     bytes    : Number of bytes to test (must be dividable by 64)
 *     loops    : Number of times to run the test
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void bw_test(UNS_32 hexaddr1,
			 UNS_32 hexaddr2,
			 UNS_32 bytes,
			 UNS_32 loops)
{
	UNS_64 ticks, base;
	UNS_32 tbytes;
	UNS_8 str[16];

	/* Adjust values if necessary */
	hexaddr1 = hexaddr1 & ~0x3;
	hexaddr2 = hexaddr2 & ~0x3;
	bytes = bytes & ~0x3F;

	if (loops == 0)
	{
		loops = 1;
	}
	if (bytes < 64)
	{
		bytes = 64;
	}
	tbytes = 2 * bytes * loops;

	/* Reset and enable timer */
	time_init();
	time_reset();
	time_start();

	/* Start test */
	while (loops > 0)
	{
		mmove(hexaddr1, hexaddr2, bytes);
		loops--;
	}

	/* Get time for test */
	ticks = time_get();
	time_stop();

	/* Get timer clock rate */
	base = time_get_rate();

	/* Show results */
	term_dat_out(bwc_msg);
	term_dat_out(bwc1_msg);
	str_makedec(str, tbytes);
	term_dat_out_crlf(str);

	term_dat_out(bwc2_msg);
	str_makedec(str, ticks);
	term_dat_out_crlf(str);

	term_dat_out(bwc3_msg);
	str_makedec(str, base);
	term_dat_out_crlf(str);
}
