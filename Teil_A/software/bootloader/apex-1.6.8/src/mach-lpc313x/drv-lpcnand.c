/* drv-lpcnand.c

written by Durgesh Pattamatta
19 Oct 2005

Copyright (C) 2008 NXP Semiconductors

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
version 2 as published by the Free Software Foundation.
Please refer to the file debian/copyright for further details.

-----------
DESCRIPTION
-----------

Divide NAND into multiple sections:

1. BootROM paramas: Block 0
- page 0, 16, 32, 64, 128, 256 contains param structure (lnand:0x00+512)
- if param page has to be updates corrupt 0 and then write new
one in 16 and on. This allows only 6 updates to param page.
- page 1-15, 17-31, 33-63, 65 -127, 129-255 contain bad block lists (lnand:2k+512)
- if bad block list has to be updates corrupt 1-15 and then write new
list from page 17 and on. This allows only 6 updates to bad block list page.
- This release only supports writing once
2. Apex image: Block 1, 2 & 3 (lnand:128k+384k)
3. Apex Environment: Block 4 & 5 (lnand:512k+256k)
- Restrict env size to 2K (nand page size) so that we could wear-level the env 
page accros block 4 & 5. 
- provide "lnand cleanenv" to erase both block 4 & 5
4. Kernel image: Two 2MB worth blocks. so that we could store 1 current and back-up image (lnand:768k+4m)
5. Ramdisk image: 16MB (lnand: +16m)
6. JFFS2 partition (not managed by Apex) 

*/

#include <config.h>
#include <apex.h>
#include <driver.h>
#include <service.h>
#include <linux/string.h>
#include <spinner.h>
#include <error.h>
#include <command.h>

#include <mach/drv-nand.h>

//#define TALK 1
#undef TALK 

#if defined (TALK)
#define PRINTF(f...)    printf (f)
#define ENTRY(l)    printf ("%s\n", __FUNCTION__)
#define DBG(l,f...)   do { if (TALK >= l) printf (f); } while (0)
#else
#define PRINTF(f...)    do {} while (0)
#define ENTRY(l)    do {} while (0)
#define DBG(l,f...)   do {} while (0)
#endif


/* NAND command defines */
#define NAND_Reset			(0xff)
#define NAND_ReadID			(0x90)
#define NAND_Status			(0x70)
#define NAND_ReadSetup			(0x00)
#define NAND_Read			(0x30)
#define NAND_Erase			(0x60)
#define NAND_EraseConfirm		(0xd0)
#define NAND_PageProgram		(0x80)
#define NAND_PageProgramConfirm		(0x10)
#define NAND_RandomDataInput		(0x85)

#define NAND_Fail			(1<<0)
#define NAND_CacheErr			(1<<1)
#define NAND_CacheReady			(1<<5)
#define NAND_Ready			(1<<6)
#define NAND_Writable			(1<<7)

/* define to implement wear leveling of Apex environment section */
#define APEX_NAND_ENV_START_PAGE	(4 * chip->pages_per_block)
#define APEX_NAND_ENV_END_PAGE		((6 * chip->pages_per_block) - 1)

/* chip seclect to which NAND dvice is connected 0, 1, 2, or 3 */
#define CURR_CS		0

/* Timeouts for various NAND operations in msec */
#define TIMEOUT_WAIT_RBY           10
#define TIMEOUT_WAIT_RESET         100
#define TIMEOUT_WAIT_READ          2
#define TIMEOUT_WAIT_ERASE         50
#define TIMEOUT_WAIT_PROGRAM       5
#define TIMEOUT_WAIT_INTERRUPT     5

/* extern functions */
extern u32 crc32_compute(u8 *data, int length);

static const NAND_BOOT_CFG_PARAMS_T chips[] = 
{
	/* Micron MT*/
#if defined (CONFIG_MICRON_MT29F2G08)
	{
		.tag = "NANDflsh", //{'N', 'A', 'N', 'D', 'f', 'l', 's', 'h',},
		.interface_width = 8,
		.page_size_in_bytes = 2048,
		.page_size_in_32bit_words = 512,
		.pages_per_block = 64,
		.nbr_of_blocks = 2048,
		.amount_of_address_bytes = 5,
		.amount_of_erase_address_bytes = 3,
		.support_read_terminate = 1,
		.page_increment_byte_nr = 2,
		.device_name = { 0x2C, 0xAA, 0xFF, 0x15, 0x20, 'M', 'T', '2', '9', 'F', '2', 'G', '0', '8',}, /* first 4bytes to fill with readid response */
		/* assuming 90MHz clock (1clock = 11ns)to NAND block */
		/* Note, timing macros tcopy naake clk +1 values. */
		/* tsrd=3, tals=3, talh=1, tcls=3, tclh=1, */
		.timing1 = NAND_TIM1_TSRD(3) | NAND_TIM1_TALS(3) | NAND_TIM1_TALH(1) | NAND_TIM1_TCLS(3) | NAND_TIM1_TCLH(1),
		/* tdrd=3, tebi=1, tch=1, tcs=4, treh=2, trp=4, twh=2, twp = 3*/
		.timing2 = NAND_TIM2_TDRD(3) | NAND_TIM2_TEBI(1) | NAND_TIM2_TCH(1) | NAND_TIM2_TCS(4) | NAND_TIM2_TRH(2) |
			NAND_TIM2_TRP(4) | NAND_TIM2_TWH(2) | NAND_TIM2_TWP(3),
		.ecc_mode = 5,
		.id_mask = 0x6, /*except 2nd & 3rd byte check remains id bytes */
	},
#endif
#if defined (CONFIG_MICRON_MT29F4G08)
	{
		.tag = "NANDflsh", //{'N', 'A', 'N', 'D', 'f', 'l', 's', 'h',},
		.interface_width = 8,
		.page_size_in_bytes = 2048,
		.page_size_in_32bit_words = 512,
		.pages_per_block = 64,
		.nbr_of_blocks = 4096,
		.amount_of_address_bytes = 5,
		.amount_of_erase_address_bytes = 3,
		.support_read_terminate = 1,
		.page_increment_byte_nr = 2,
		.device_name = { 0x2C, 0xAC, 0x90, 0x15, 0x54, 'M', 'T', '2', '9', 'F', '4', 'G', '0', '8',}, /* first 4bytes to fill with readid response */
		/* assuming 90MHz clock (1clock = 11ns)to NAND block */
		/* Note, timing macros tcopy naake clk +1 values. */
		/* tsrd=3, tals=3, talh=1, tcls=3, tclh=1, */
		.timing1 = NAND_TIM1_TSRD(3) | NAND_TIM1_TALS(3) | NAND_TIM1_TALH(1) | NAND_TIM1_TCLS(3) | NAND_TIM1_TCLH(1),
		/* tdrd=3, tebi=1, tch=1, tcs=4, treh=2, trp=4, twh=2, twp = 3*/
		.timing2 = NAND_TIM2_TDRD(3) | NAND_TIM2_TEBI(1) | NAND_TIM2_TCH(1) | NAND_TIM2_TCS(4) | NAND_TIM2_TRH(2) |
			NAND_TIM2_TRP(4) | NAND_TIM2_TWH(2) | NAND_TIM2_TWP(3),
		.ecc_mode = 5,
		.id_mask = 0x6, /*except 2nd & 3rd byte check remains id bytes */
	},
#endif
#if defined (CONFIG_SAMSUNG_K9K8G)
	{
		.tag = "NANDflsh", //{'N', 'A', 'N', 'D', 'f', 'l', 's', 'h',},
		.interface_width = 8,
		.page_size_in_bytes = 0x0800,
		.page_size_in_32bit_words = 0x0200,
		.pages_per_block = 0x0040,
		.nbr_of_blocks = 0x0800,
		.amount_of_address_bytes = 5,
		.amount_of_erase_address_bytes = 3,
		.support_read_terminate = 1,
		.page_increment_byte_nr = 2,
		.device_name = { 0xEC, 0xD3, 0x51, 0x95, 0x20, 'K', '9', 'K', '8', 'G', '0', '8', }, /* first 4bytes to fill with readid response */
		/* assuming 90MHz clock (1clock = 11ns)to NAND block */
		/* Note, timing macros take clk +1 values. */
		/* tsrd=2, tals=3, talh=1, tcls=3, tclh=2, */
		.timing1 = NAND_TIM1_TSRD(2) | NAND_TIM1_TALS(3) | NAND_TIM1_TALH(1) | NAND_TIM1_TCLS(3) | NAND_TIM1_TCLH(2),
		/* tdrd=2, tebi=1, tch=1, tcs=4, treh=2, trp=4, twh=2, twp = 3*/
		.timing2 = NAND_TIM2_TDRD(2) | NAND_TIM2_TEBI(1) | NAND_TIM2_TCH(1) | NAND_TIM2_TCS(4) | NAND_TIM2_TRH(2) |
			NAND_TIM2_TRP(4) | NAND_TIM2_TWH(2) | NAND_TIM2_TWP(3),
		.ecc_mode = 5,
		.id_mask = 0x6, /*except 2nd & 3rd byte check remains id bytes */
	},
#endif
#if defined (CONFIG_TOSHIBA_TC58DVM82A)
#define USE8BITCOLADDR
	{
		.tag = "NANDflsh", //{'N', 'A', 'N', 'D', 'f', 'l', 's', 'h',},
		.interface_width = 8,
		.page_size_in_bytes = 512,
		.page_size_in_32bit_words = 128,
		.pages_per_block = 32,
		.nbr_of_blocks = 2048,
		.amount_of_address_bytes = 3,
		.amount_of_erase_address_bytes = 2,
		.support_read_terminate = 0,
		.page_increment_byte_nr = 2,
		.device_name = { 0x98, 0x73, 0xA5, 0xBA, 0xFF, 'T', 'C', '5', '8', 'D', 'V', 'M', '8', '2', 'A' }, /* first 4 bytes to fill with readid response */
		/* assuming 90MHz clock (1clock = 11ns)to NAND block */
		/* Note, timing macros take clk +1 values. */
		/* tsrd=2, tals=3, talh=1, tcls=3, tclh=2, */
		.timing1 = NAND_TIM1_TSRD(2+1) | NAND_TIM1_TALS(3+1) | NAND_TIM1_TALH(1+1) | NAND_TIM1_TCLS(3+1) | NAND_TIM1_TCLH(2+1),
		/* tdrd=2, tebi=1, tch=1, tcs=4, treh=2, trp=4, twh=2, twp = 3*/
		.timing2 = NAND_TIM2_TDRD(2+1) | NAND_TIM2_TEBI(1+1) | NAND_TIM2_TCH(1+1) | NAND_TIM2_TCS(4+1) | NAND_TIM2_TRH(2+1) |
			NAND_TIM2_TRP(4+1) | NAND_TIM2_TWH(2+1) | NAND_TIM2_TWP(3+1),
		.ecc_mode = 5,
		.id_mask = 0x6, /*except 2nd & 3rd byte check remains id bytes */
	},
#endif
};

/* forward declarations */
static void nand_reset (void);
static ssize_t lpcnand_write (struct descriptor_d* d, const void* pv, size_t cb);

/*global variables */
static NAND_BOOT_CFG_PARAMS_T* chip;
static int g_curr_buf = 0;
static int nand_modify;
static int blk0enab;
static u32 nand_bbl_array [(NANDFLASH_BADBLOCK_LIST_LENGTH + 4 / 32)];

/* Mark a block as bad in the memory based bad block table (from block 0) */
static void nand_setbb_array_bit(u32 pagenum) {
	u32 blocknum = pagenum / chip->pages_per_block;
	u32 idx = blocknum / 32;
	u32 offs = 1 << (blocknum - (idx * 32));

	nand_bbl_array[idx] &= ~offs;
}

/* Check if a block in the page is good (!0) or bad (0) in the memory
   based bad block table */
static int nand_checkbb_array_bit(u32 pagenum) {
	u32 blocknum = pagenum / chip->pages_per_block;
	u32 idx = blocknum / 32;
	u32 offs = 1 << (blocknum - (idx * 32));

	/* Returns '!0' if the block is good */
	return (int) (nand_bbl_array[idx] & offs);
}

/* Modify access check */
static int nand_modfiy_ok(void) {
	if (!nand_modify) {
		printf("NAND block 0 has not been formatted yet!. Modify\n");
		printf("functions have been disabled. Use the lpcnand f\n");
		printf("command to prepare NAND FLASH prior to use\n");
		return 0;
	}

	return 1;
}

/* wait_on_busy

checks the status of the device and returns when it is no longer
busy.  
*/
static void nand_reset (void)
{
	unsigned long time_start = timer_read ();
	/* clear edge status */
	volatile u32 status = NANDC_CHECK_STS;

	/* following line is just avoid warning. status gets cleaed by reading
	the register. */
	if (status) 
		status = 0;

	NANDC_SET_CMD = NAND_Reset;
	/* Wait for MLC NAND ready */
	while (!(NANDC_CHECK_STS & NAND_CHK_STS_RB_EDGE(CURR_CS))) {
		if (timer_delta (time_start, timer_read ()) >= TIMEOUT_WAIT_RBY) {
			printf ("\nbailing at timeout status 0x%lx \n", NANDC_CHECK_STS);
			break;
		}
	}
}

/* wait_on_int
Waits for one of the requested interrupts to get set.  
*/
static void wait_on_int (u32 int_mask, u32 tmo)
{
	unsigned long time_start = timer_read ();
	/* Wait for MLC NAND ready */
	while (!(NANDC_IRQ_STS_RAW & int_mask)) {
		if (timer_delta (time_start, timer_read ()) >= tmo) {
			printf ("\nbailing at timeout status 0x%08lx bits 0x%08x\n",
				NANDC_IRQ_STS_RAW, int_mask);
			break;
		}
	}
}

inline void lpcnand_address (unsigned long page, int index)
{
#ifdef USE8BITCOLADDR
	/* Modify address for index value */
	if (index >= 256)
		index -= 256;
	if (index >= 256)
		index -= 256;

	NANDC_SET_ADDR = (index & 0xff);
	NANDC_SET_ADDR = ( page        & 0xff);
	NANDC_SET_ADDR = ((page >>  8) & 0xff);
printf("ADDR 0x%02x 0x%02x 0x%02x", (index & 0xff),
	( page        & 0xff), ((page >>  8) & 0xff));

	if (chip->amount_of_address_bytes > 3) {
		NANDC_SET_ADDR = ((page >> 16) & 0xff);
printf(" 0x%02x", ((page >>  16) & 0xff));
}
printf("\n");

#else
	NANDC_SET_ADDR = (index & 0xff);
	NANDC_SET_ADDR = ((index >> 8) & 0xff);
	NANDC_SET_ADDR = ( page        & 0xff);
	NANDC_SET_ADDR = ((page >>  8) & 0xff);

	if (chip->amount_of_address_bytes > 4)
		NANDC_SET_ADDR = ((page >> 16) & 0xff);
#endif
}

inline void lpcnand_read_setup (unsigned long page, int index)
{
	/* clear edge status */
	NANDC_IRQ_STS_RAW = NAND_IRQ_RB_POS_EDGE(CURR_CS);

#ifdef USE8BITCOLADDR
	if (index < 256) {
		NANDC_SET_CMD = NAND_ReadSetup;
printf("0x00-read index %d", index);
	}
	else if (index < 512) {
		NANDC_SET_CMD = 0x01;
printf("0x01-read index %d", index);
	}
	else {
		NANDC_SET_CMD = 0x50;
printf("0x50-read index %d", index);
	}

#else
	NANDC_SET_CMD = NAND_ReadSetup;
#endif

	lpcnand_address (page, index);
	if (chip->support_read_terminate) {
		NANDC_SET_CMD = NAND_Read;
}
	wait_on_int (NAND_IRQ_RB_POS_EDGE(CURR_CS), TIMEOUT_WAIT_READ);
}


/* Check block 0 for boot ROM header and build saved bad block list */
static int lpcnand_block0_good(void)
{
	char tag [9];
	int i, val;
	unsigned long idx = 0xFFFFFFFF, page = 1;
	int offs = 0, entries;

	/* Read block 0, page 0, offset 0 */
	lpcnand_read_setup(0, 0);

	for (i = 0; i < 9; i++)
		tag[i] = (char) NANDC_READ_DATA;
	tag[8] = '\0';

	if (strcmp(tag, "NANDflsh") != 0)
		return 0;

	memset((void*)nand_bbl_array, 0xFF, sizeof(nand_bbl_array));

	/* Read bad block data */
	lpcnand_read_setup(page, offs);

	/* Get the number of bad blocks */
	if (idx == 0xFFFFFFFF) {
		idx = (NANDC_READ_DATA & 0xFF) << 0;
		idx |= (NANDC_READ_DATA & 0xFF) << 8;
		idx |= (NANDC_READ_DATA & 0xFF) << 16;
		idx |= (NANDC_READ_DATA & 0xFF) << 24;
	}
	if (idx == 0xFFFFFFFF) {
		/* Something is wrong */
		nand_modify = 0;

		return 0;
	}

	/* Generate list from entries */
	entries = (int) idx;
	while (entries > 0) {
		val = ((NANDC_READ_DATA & 0xFF) << 0) | ((NANDC_READ_DATA & 0xFF) << 8) |
			((NANDC_READ_DATA & 0xFF) << 16) | ((NANDC_READ_DATA & 0xFF) << 24);
		if ((val & 0x00FFFFFF) == ((((u32) 'B') << 0) | (((u32) 'A') << 8) |
			(((u32) 'D') << 16))) {
			/* Go to the next page and continue */
			offs += 512;
			if (offs >= chip->page_size_in_bytes) {
				page++;
				offs = 0;
			}

			/* Read bad block data */
			lpcnand_read_setup(page, offs);
		}
		else {
			nand_setbb_array_bit(val * chip->pages_per_block);
			entries--;
		}
	}

	return 1;
}

static void write_badblock_list(void)
{
	u32* bad_list_buf = (u32*)EXT_SDRAM_PHYS;
	char* buff_ptr;
	u32 temp_buff[2];
	int curr_pos = 0;
	int magic_word_pos;
	int page_nr;
	int cBad = 0;
	struct descriptor_d desc;

	DBG (1, "before memset buf:0x%08x len:%d\n", (u32)bad_list_buf, NANDFLASH_BADBLOCK_LIST_LENGTH * 4);

	/* fill buffer with 0xFF */
	memset((void*)bad_list_buf, 0xFF, NANDFLASH_BADBLOCK_LIST_LENGTH * 4);
	memset((void*)nand_bbl_array, 0xFF, sizeof(nand_bbl_array));
	
	DBG (1, "memset done page:%d / %d\n", chip->pages_per_block, chip->nbr_of_blocks);
	cBad = 0;
	/* build bad block list page */
	for (page_nr = chip->pages_per_block; 
		page_nr < (chip->nbr_of_blocks * chip->pages_per_block); 
		page_nr += chip->pages_per_block) {

		DBG (1, "page_nr:%d \r", page_nr);
		/* reset nand device */
		nand_reset ();
		/* index to spare area */
		lpcnand_read_setup(page_nr, chip->page_size_in_bytes); 
		if (NANDC_READ_DATA != 0xFF) {
			printf ("Block %d bad\n", page_nr/chip->pages_per_block);
			++cBad;
			nand_setbb_array_bit(page_nr);
			*((u32*)(bad_list_buf + cBad)) = page_nr/chip->pages_per_block;
			if (cBad > NANDFLASH_BADBLOCK_LIST_LENGTH)
				break;
		}
	}
	/* update bad block count */
	*bad_list_buf = cBad;

	curr_pos = 0;
	page_nr = 1;
	while (curr_pos < (cBad + 1)) {

		if ( (cBad + 1 - curr_pos) < (chip->page_size_in_32bit_words - 2)) {
			magic_word_pos = cBad + 1;
		}
		else {
			magic_word_pos = curr_pos + chip->page_size_in_32bit_words - 2;
		}
		DBG (1, "curr_pos:%d, cBad:%d, magic_pos:%d\n", curr_pos, cBad, magic_word_pos);

		/* save last 2 words in current page to write with magic id & CRC.
		Not needed if CBad < (chip->page_size_in_32bit_words - 2)*/
		temp_buff[0] = bad_list_buf[magic_word_pos];
		temp_buff[1] = bad_list_buf[magic_word_pos + 1];

		/* insert magic word */
		buff_ptr = (char*)&bad_list_buf[magic_word_pos];
		buff_ptr[0] = 'B';
		buff_ptr[1] = 'A';
		buff_ptr[2] = 'D';
		buff_ptr[3] = (char)(page_nr & 0xFF);

		/* compute CRC32 */
		bad_list_buf[magic_word_pos + 1] = crc32_compute((u8*)&bad_list_buf[curr_pos], 
			((u32)&buff_ptr[4] - (u32)&bad_list_buf[curr_pos]));

		/* create the nand descriptor for write routine */
		desc.index = 0;
		desc.start = page_nr * chip->page_size_in_bytes;
		desc.length = chip->page_size_in_bytes;

		DBG (1, "buff_ptr:0x%08x, start:0x%08x, page_nr:%d\n", (u32)buff_ptr, desc.start, page_nr);

		/* write the bad page */
		if (lpcnand_write (&desc, (void*)(bad_list_buf + curr_pos), 
			chip->page_size_in_bytes) < chip->page_size_in_bytes) {
			printf ("Write bad block page %d failed\n", page_nr);
			return;
		}
		
		/*put back last 2 words*/
		bad_list_buf[magic_word_pos] = temp_buff[0];
		bad_list_buf[magic_word_pos + 1] = temp_buff[1];
		/* move to next page */
		page_nr++;
		/* update current position in bad block list */
		curr_pos = magic_word_pos;
	}
}

static void format_device(void) {
	void* ptr;
	int count;
	int tmp;

	tmp = blk0enab;
	blk0enab = 1;
	nand_modify = 1;

	/* erase block 0 */
	NANDC_IRQ_STS_RAW = NAND_IRQ_RB_POS_EDGE(CURR_CS);
	NANDC_SET_CMD = NAND_Erase;
	for (count=0; count< chip->amount_of_erase_address_bytes; count++ )
		NANDC_SET_ADDR = 0x00;

	NANDC_SET_CMD = NAND_EraseConfirm;
	wait_on_int(NAND_IRQ_RB_POS_EDGE(CURR_CS), TIMEOUT_WAIT_ERASE);
	/* Send page program command  */
	NANDC_SET_CMD = NAND_PageProgram;
	lpcnand_address (0, 0);
	/* set HW flow control to write the page and do ECC */
	NANDC_IRQ_STS_RAW = 0xFFFFFFFF;
	/* build param page */
	ptr = (g_curr_buf)?(void*)NANDC_RAM1_PHYS:(void*)NANDC_RAM0_PHYS;
	memcpy(ptr, (void*)chip, sizeof(NAND_BOOT_CFG_PARAMS_T));
	memset(ptr + sizeof(NAND_BOOT_CFG_PARAMS_T), 
		0, (LPC313x_NAND_BPARAM_PAGE_SZ - sizeof(NAND_BOOT_CFG_PARAMS_T) - 4));
	/* compute CRC32. */
	*((u32*)(ptr + (LPC313x_NAND_BPARAM_PAGE_SZ - 4))) = 
		crc32_compute((g_curr_buf)?(u8*)NANDC_RAM1_PHYS:(u8*)NANDC_RAM0_PHYS, 
		LPC313x_NAND_BPARAM_PAGE_SZ - 4);

	DBG (1, "crc 0x%08x / 0x%08x\n",  
		*((u32*)(ptr + (NANDC_BUF_SZ - 4))), (u32)(ptr + (NANDC_BUF_SZ - 4)));
	/* fill rest of the sub-page.*/ 
	memset(ptr + LPC313x_NAND_BPARAM_PAGE_SZ, 
		0xFF, (NANDC_BUF_SZ - LPC313x_NAND_BPARAM_PAGE_SZ));
	/*When 512-516 bytes are written it will trigger ECC computation.*/
	*((u32*)(ptr + NANDC_BUF_SZ)) = 0xFFFFFFFF;

	/* see if ECC check is enabled */
	if (NANDC_CFG & NAND_CFG_EC)
		wait_on_int((g_curr_buf)?NAND_IRQ_ECC_ENC_RAM1:NAND_IRQ_ECC_ENC_RAM0, TIMEOUT_WAIT_PROGRAM);
	/* write param page */
	NANDC_CTRL_FLOW = (g_curr_buf)?NAND_CTRLFLW_WRITE_RAM1:NAND_CTRLFLW_WRITE_RAM0;
	wait_on_int((g_curr_buf)?NAND_IRQ_WR_RAM1:NAND_IRQ_WR_RAM0, TIMEOUT_WAIT_PROGRAM);
	/*toggle buff */
	g_curr_buf ^= 1;
	NANDC_IRQ_STS_RAW = NAND_IRQ_RB_POS_EDGE(CURR_CS);
	NANDC_SET_CMD = NAND_PageProgramConfirm;
	wait_on_int(NAND_IRQ_RB_POS_EDGE(CURR_CS), TIMEOUT_WAIT_PROGRAM);
	/* check status */
	NANDC_SET_CMD = NAND_Status;
	udelay(5);// Wait for tWHR
	if (NANDC_READ_DATA & NAND_Fail) {
		printf ("Write param page failed\n");
		nand_modify = 0;
		blk0enab = tmp;
		return;
	}

	/* now write bad block list page */
	write_badblock_list();

	blk0enab = tmp;
}

/* nand_init

probes the NAND flash device.

Note that the status check redundantly sends the Status command
when we are not using the CONFIG_NAND_LPD mode.  It's left in for
now.
*/
static void lpcnand_init (void)
{
	unsigned short id[4];
	u32 config = NAND_CFG_DC;//NAND_CFG_ECGC; 

	/* enable NAND clocks */
	cgu_clk_en_dis(CGU_SB_NANDFLASH_S0_CLK_ID, 1);
	cgu_clk_en_dis(CGU_SB_NANDFLASH_NAND_CLK_ID, 1);
	cgu_clk_en_dis(CGU_SB_NANDFLASH_PCLK_ID, 1);
	cgu_clk_en_dis(CGU_SB_NANDFLASH_AES_CLK_ID, 0);

	/* reset NAND module through CGU */
	cgu_soft_reset_module(NANDFLASH_NAND_RESET_N_SOFT);
	cgu_soft_reset_module(NANDFLASH_ECC_RESET_N_SOFT);
	cgu_soft_reset_module(NANDFLASH_AES_RESET_N_SOFT); /* needed for LPC3153 & 54 parts only */
	
	cgu_clk_en_dis(CGU_SB_NANDFLASH_ECC_CLK_ID, 1);

	/* NAND muxing */
	SYS_MUX_NAND_MCI = 0;

	/* select bank0*/
	NANDC_SET_CE = NAND_SETCE_WP | NAND_SETCE_CV(CURR_CS);

	/* reset nand device */
	nand_reset ();

	NANDC_SET_CMD = NAND_Status;
	udelay(5);
	if ((NANDC_READ_DATA & NAND_Ready) == 0)
		goto exit;

	NANDC_SET_CMD = NAND_ReadID;
	NANDC_SET_ADDR = 0;

	id[0] = NANDC_READ_DATA;
	id[1] = NANDC_READ_DATA;
	id[2] = NANDC_READ_DATA;
	id[3] = NANDC_READ_DATA;

	for (chip = (NAND_BOOT_CFG_PARAMS_T*)&chips[0];
		chip < (chips + sizeof(chips)/sizeof (NAND_BOOT_CFG_PARAMS_T));
		++chip) {
#define _M(i)\
	((chip->id_mask & (1<<(i))) || (id[i] == chip->device_name[i]))
		
			DBG(1, "ID 0x%02x/0x%02x/0x%02x/0x%02x\n",
				chip->device_name[0], chip->device_name[1],
				chip->device_name[2], chip->device_name[3]);
			if (_M (0) && _M (1) && _M (2) && _M (3)) {
				DBG(1, "ID matchded\n");
				break;
			}
#undef _M
	}

	if (chip > (chips + sizeof(chips)/sizeof (chips[0])))
		chip = NULL;

	if (chip) {
		/* init timings */
		NANDC_IRQ_TIMING1 = chip->timing1;
		NANDC_IRQ_TIMING2 = chip->timing2;
		/* build config value for the device */
		/* set ecc type */
		switch (chip->ecc_mode)
		{
		case 8:
			config |= NAND_CFG_8BIT_ECC;
		case 5:
			config |= NAND_CFG_EC;
			break;
		default:
			break;
		}

		/* write the final config value */
		NANDC_CFG = config;
	}
	printf ("\n* LPC-NAND flash");

	if (chip)
		printf (" %d MiB total, %d KiB erase, %d B page"
		" (0x%02x/0x%02x/0x%02x/0x%02x)\n",
		(chip->nbr_of_blocks * chip->page_size_in_bytes * chip->pages_per_block)/(1024*1024), 
		(chip->page_size_in_bytes * chip->pages_per_block)/1024,
		chip->page_size_in_bytes,
		id[0], id[1], id[2], id[3]);
	else
		printf (" unknown 0x%02x/0x%02x/0x%02x/0x%02x\n",
		id[0], id[1], id[2], id[3]);

 	/* Does block 0 have a valid structure yet? */
	blk0enab = 0;
	nand_modify = 0;

	if (lpcnand_block0_good() != 0) {
		/* Populate bad block list array from block 0 */
		nand_modify = 1;
	}
	else {
#if defined (CONFIG_AUTOFORMAT_BLOCK0)
		printf("NAND Block 0 is being formatted.\n");
		format_device();

#else
		printf("NAND Block 0 has not yet been formatted for operation. NAND modify\n"
			"operations have been temporarily suspended. Use the 'lpcnand format\n"
			"command to setup block 0.\n");
#endif
	}

exit:
	NAND_CS_DISABLE;
}

static int lpcnand_open (struct descriptor_d* d)
{
	if (!chip)
		return -1;

	/* Check if param page is programmed */

	return 0;
}

static int read_buffer(u8 *buffer, int bytes) {
	/* set HW flow control to read the page and do ECC */
	NANDC_IRQ_STS_RAW = 0xFFFFFFFF;

	NANDC_CTRL_FLOW = (g_curr_buf)?NAND_CTRLFLW_READ_RAM1:NAND_CTRLFLW_READ_RAM0;
	wait_on_int((g_curr_buf)?NAND_IRQ_RD_RAM1:NAND_IRQ_RD_RAM0, TIMEOUT_WAIT_RBY);

	/* see if ECC check is enabled */
	if (NANDC_CFG & NAND_CFG_EC)
		wait_on_int((g_curr_buf)?NAND_IRQ_ECC_DEC_RAM1:NAND_IRQ_ECC_DEC_RAM0, TIMEOUT_WAIT_RBY);

	memcpy(buffer, (g_curr_buf)?(void*)NANDC_RAM1_PHYS:(void*)NANDC_RAM0_PHYS, bytes);
	/* toggle buffer */
	g_curr_buf ^= 1;

	return 1;
}

static ssize_t lpcnand_read (struct descriptor_d* d, void* pv, size_t cb)
{
	ssize_t cbRead = 0;
	unsigned long blk;

	if (!chip)
		return cbRead;

	if (nand_modfiy_ok() == 0)
		return 0;

	if (d->index + cb > d->length)
		cb = d->length - d->index;

	while (cb) {
		unsigned long page  = (d->start + d->index)/chip->page_size_in_bytes;
		int index = (d->start + d->index) & (chip->page_size_in_bytes - 1);
		int available = chip->page_size_in_bytes - index;
		int toread;

		/* this driver always reads on page boundaries only */
		if (index & (NANDC_BUF_SZ -1)) {
			printf("unaligned read 0x%08x\n", d->start + d->index);
			return 0;
		}
		/* if index is not 0 then add 16 byte for every 512 block. */
		index += (((index >> 9) & 0x3)) * 16;

		/* If the current block is bad, move to the next block */
		if (nand_checkbb_array_bit(page) == 0) {
			blk = page / chip->pages_per_block;
			printf("Read:Skipping bad block %d\n", (int) blk);

			/* Continue at next block */
			d->start = (blk + 1) * chip->pages_per_block * chip->page_size_in_bytes;
			d->index = 0;
		}
		else {
			if (available > cb)
				available = cb;

			d->index += available;
			cb -= available;
			cbRead += available;

			/* reset nand device */
			nand_reset ();
			/* do read setup */
			lpcnand_read_setup (page, index);

			while (available > 0) {
				toread = (available < NANDC_BUF_SZ)?available:NANDC_BUF_SZ;
				read_buffer((u8 *) pv, toread);

				pv += toread;
				available -= toread;
			}
		}
	}

	return cbRead;
}

static int write_buffer(u8 *buffer, int bytes) {
	void* buf;

	/* set HW flow control to read the page and do ECC */
	NANDC_IRQ_STS_RAW = 0xFFFFFFFF;

	/* Write output buffer to memory */
	buf = (g_curr_buf)?(void*)NANDC_RAM1_PHYS:(void*)NANDC_RAM0_PHYS;
	memcpy(buf, buffer, bytes);
	if (bytes < NANDC_BUF_SZ) {
		memset(buf + bytes, 0xFF, NANDC_BUF_SZ - bytes);
	}
	if (bytes <= NANDC_BUF_SZ) {
		/*When 512-516 bytes are written it will trigger ECC computation.*/
		*((u32*)(buf + NANDC_BUF_SZ)) = 0xFFFFFFFF;
	}

	/* see if ECC check is enabled */
	if (NANDC_CFG & NAND_CFG_EC)
		wait_on_int((g_curr_buf)?NAND_IRQ_ECC_ENC_RAM1:NAND_IRQ_ECC_ENC_RAM0, TIMEOUT_WAIT_RBY);
	NANDC_CTRL_FLOW = (g_curr_buf)?NAND_CTRLFLW_WRITE_RAM1:NAND_CTRLFLW_WRITE_RAM0;
	wait_on_int((g_curr_buf)?NAND_IRQ_WR_RAM1:NAND_IRQ_WR_RAM0, TIMEOUT_WAIT_RBY);

	/* toggle buffer */
	g_curr_buf ^= 1;

	return 1;
}

static ssize_t lpcnand_write (struct descriptor_d* d, const void* pv, size_t cb)
{
	int cbWrote = 0;
	unsigned long blk;

	if (!chip)
		return cbWrote;

	if (nand_modfiy_ok() == 0)
		return 0;

	if (d->index + cb > d->length)
		cb = d->length - d->index;

	while (cb) {
		unsigned long page  = (d->start + d->index)/chip->page_size_in_bytes;
		int index = (d->start + d->index) & (chip->page_size_in_bytes - 1);
		int available = chip->page_size_in_bytes - index;
		int towrite;

		if ((blk0enab == 0) && (page == 0)) {
			printf("\nCan't write block 0 (protected), use lpcnand protblk0 to disable\n");
			return 0;
		}

		/* this driver always writes on sub-page (512) boundaries only */
		if (index & (NANDC_BUF_SZ -1)) {
			printf("unaligned write 0x%08x\n", d->start + d->index);
			return 0;
		}
		/* if index is not 0 then add 16 byte for every 512 block. */
		index += (((index >> 9) & 0x3)) * 16;

		/* If the current block is bad, move to the next block */
		if (nand_checkbb_array_bit(page) == 0) {
			blk = page / chip->pages_per_block;
			printf("Skipping write of bad block %d\n", (int) blk);

			/* Continue at next block */
			d->start = (blk + 1) * chip->pages_per_block * chip->page_size_in_bytes;
			d->index = 0;
		}
		else {
			if (available > cb)
				available = cb;

			/* Reset and read to perform I/O on the data region  */
			nand_reset ();

			/* Send page program command  */
			NANDC_SET_CMD = NAND_PageProgram;
			lpcnand_address (page, index);

			DBG(1, "lpcnand write page:0x%08lx index:0x%08x pv:0x%08x\n", page, index, (u32)pv);
			d->index += available;
			cb -= available;
			cbWrote += available;

			while (available > 0) {
				towrite = (available < NANDC_BUF_SZ)? available : NANDC_BUF_SZ;
				write_buffer((u8 *) pv, towrite);
				available -= towrite;
			}
			NANDC_IRQ_STS_RAW = NAND_IRQ_RB_POS_EDGE(CURR_CS);
			NANDC_SET_CMD = NAND_PageProgramConfirm;
			wait_on_int(NAND_IRQ_RB_POS_EDGE(CURR_CS), TIMEOUT_WAIT_PROGRAM);

			NANDC_SET_CMD = NAND_Status;
			udelay(5);// Wait for tWHR
			if (NANDC_READ_DATA & NAND_Fail) {
				printf ("Write failed at page %ld\n", page);
				goto exit;
			}
		}
	}

exit:
	return cbWrote;
}

static int erase_block(unsigned long page) {
	int count;

	NANDC_IRQ_STS_RAW = NAND_IRQ_RB_POS_EDGE(CURR_CS);
	NANDC_SET_CMD = NAND_Erase;

	for (count=0; count< chip->amount_of_erase_address_bytes; count++, page >>= 8 )
		NANDC_SET_ADDR = (u8)( page & 0xff );
	NANDC_SET_CMD = NAND_EraseConfirm;

	wait_on_int(NAND_IRQ_RB_POS_EDGE(CURR_CS), TIMEOUT_WAIT_ERASE);

	NANDC_SET_CMD = NAND_Status;
	udelay(5);
	if (NANDC_READ_DATA & NAND_Fail) {
		return 0;
	}

	return 1;
}

static void lpcnand_erase (struct descriptor_d* d, size_t cb)
{
	u32 erase_size = (chip->page_size_in_bytes * chip->pages_per_block);
	if (!chip)
		return;

	if (nand_modfiy_ok() == 0)
		return;

	if (d->index + cb > d->length)
		cb = d->length - d->index;

	do {
		unsigned long page = (d->start + d->index)/chip->page_size_in_bytes;
		unsigned long available
			= erase_size - ((d->start + d->index) & (erase_size - 1));

		if ((blk0enab == 0) && (page == 0)) {
			printf("Can't erase block 0, use lpcnand protblk0 to disable\n");
		}
		else if (nand_checkbb_array_bit(page) == 0) {
			printf("Bad block %d, skipping...\n", (int) page/chip->pages_per_block);
		}
		else {
			if (erase_block(page) == 0) {
				printf ("Erase failed at block %ld\n", page/chip->pages_per_block);
			}
		}

		if (available < cb) {
			cb -= available;
			d->index += available;
		}
		else {
			cb = 0;
			d->index = d->length;
		}
	} while (cb > 0);
}

#if !defined (CONFIG_SMALL)
static void lpcnand_report (void)
{
	unsigned char status;


	if (!chip)
		return;

	NANDC_SET_CMD = NAND_Status;
	udelay(5);
	status = NANDC_READ_DATA;
	printf ("  lnand:   %d MiB total, %d KiB erase, %d B page %s%s%s\n",
		(chip->nbr_of_blocks * chip->page_size_in_bytes * chip->pages_per_block)/(1024*1024), 
		(chip->page_size_in_bytes * chip->pages_per_block)/1024,
		chip->page_size_in_bytes,
		(status & NAND_Fail) ? " FAIL" : "",
		(status & NAND_Ready) ? " RDY" : "",
		(status & NAND_Writable) ? " R/W" : " R/O"
		);
}
#endif

static __driver_3 struct driver_d lpcnand_driver = {
	.name = "lnand",
	.description = "LPC NAND flash driver (hw ecc)",
	.flags = DRIVER_WRITEPROGRESS(6),
	.open = lpcnand_open,
	.close = close_helper,
	.read = lpcnand_read,
	.write = lpcnand_write,
	.erase = lpcnand_erase,
	.seek = seek_helper,
};

static __service_6 struct service_d lpcnand_service = {
	.init = lpcnand_init,
#if !defined (CONFIG_SMALL)
	.report = lpcnand_report,
#endif
};

#define SMALLPAGESIZE (512)
void mark_page_bad(unsigned long page) {
	u8* tmp_buf = (u8 *) EXT_SDRAM_PHYS;

	/* Setup buffer for bad block markers */
	memset((void*)tmp_buf, 0xff, SMALLPAGESIZE);
	tmp_buf[512] = 0;

	nand_reset ();

	NANDC_SET_CMD = NAND_PageProgram;
	if (chip->page_size_in_bytes == 512) {
		/* Small block FLASH, no extra indicing */
		lpcnand_address (page, 0);
	}
	else if (chip->page_size_in_bytes == 2048) {
		/* Large block FLASH, no extra indicing */
		lpcnand_address (page, (3 * SMALLPAGESIZE));
	}
	else {
		/* Huge block FLASH, no extra indicing */
		lpcnand_address (page, (7 * SMALLPAGESIZE));
	}

	write_buffer(tmp_buf, 516);

	NANDC_IRQ_STS_RAW = NAND_IRQ_RB_POS_EDGE(CURR_CS);
	NANDC_SET_CMD = NAND_PageProgramConfirm;
	wait_on_int(NAND_IRQ_RB_POS_EDGE(CURR_CS), TIMEOUT_WAIT_PROGRAM);

	NANDC_SET_CMD = NAND_Status;
	udelay(5);// Wait for tWHR
	if (NANDC_READ_DATA & NAND_Fail) {
		printf ("Failed marking %d block bad\n", (int) (page/chip->pages_per_block));
	}
	else {
		printf("Block %d marked bad\n", (int) (page/chip->pages_per_block));
	}
}

int cmd_lpcnand (int argc, const char** argv)
{
	int page, cBad = 0;

	if (!chip)
		return 0; 

	if (PARTIAL_MATCH (argv[1], "f", "ormat") == 0) {
	 	/* Does block 0 already have a valid structure yet? */
		if (nand_modify) {
			printf("Block 0 already has a valid structure!\n");

			if ((argc > 3) || (argv[2][0] != '1')) {
				printf("CAUTION: The bad block list in block 0 contains the entire bad\n"
					"block list for the device from factory marked bad blocks. If\n"
					"you erase this block now, you may lose these markers forever!\n"
					"You can override this warning message with 'lpcnand format 1'.\n");
				return -1;
			}
		}

		format_device();
	}

	if (PARTIAL_MATCH (argv[1], "s", "can") == 0) {
		
		cBad = 0;

		for (page = chip->pages_per_block; page < (chip->nbr_of_blocks * chip->pages_per_block);
			page += chip->pages_per_block) {
			/* reset nand device */
			nand_reset ();
			lpcnand_read_setup(page, chip->page_size_in_bytes); /* index to spare area */
			{
				if (NANDC_READ_DATA != 0xFF) {
					printf ("Block %d bad\n", page/chip->pages_per_block);
					++cBad;
				}
			}
			DBG(2, "Checking page:0x%08x\n", page);
		}
		printf ("%d of %d blocks are bad\n",
			cBad, chip->nbr_of_blocks);
	}

	if (PARTIAL_MATCH (argv[1], "b", "blist") == 0) {
		if (nand_modfiy_ok() == 0)
			return 0;

		cBad = 0;

		for (page = chip->pages_per_block; page < (chip->nbr_of_blocks * chip->pages_per_block);
			page += chip->pages_per_block) {
			/* reset nand device */
			if (nand_checkbb_array_bit(page) == 0) {
				printf ("Block %d bad\n", page/chip->pages_per_block);
				++cBad;
			}
			DBG(2, "Checking page:0x%08x\n", page);
		}
		printf ("%d of %d blocks are bad\n",
			cBad, chip->nbr_of_blocks);
	}

	if (PARTIAL_MATCH (argv[1], "r", "estore") == 0) {
		if (nand_modfiy_ok() == 0)
			return 0;

		printf("Restoring factory bad block markers\n");

		/* Erase all blocks and mark blocks bad as specified by block 0 table */
		for (page = chip->pages_per_block; page < (chip->nbr_of_blocks * chip->pages_per_block);
			page += chip->pages_per_block) {
			if (nand_checkbb_array_bit(page) == 0) {
				/* Mark this block/page as bad */
				mark_page_bad(page);
			}
			else {
				erase_block(page);
			}
		}
	}

	if (PARTIAL_MATCH (argv[1], "p", "rotblk0") == 0) {
		printf ("Block 0 modify protection is ");
		if (argv[2][0] == '0') {
			blk0enab = 1;
			printf ("off\n");
		}
		else {
			blk0enab = 0;
			printf ("on\n");
		}
	}

	/* This command is not for end-users. Don't use it! */
	if (strcmp (argv[1], "diagmode") == 0) {
		nand_modify = 1;

		if ((argc >= 2) || (argv[2][0] != '1')) {
			/* option 1 - clear bad blocks */
			memset((void*)nand_bbl_array, 0xFF, sizeof(nand_bbl_array));
		}
	}

	return 0;
}

/* Empty function needed for Apex NAND driver */
void nandsys_init(void) {}

static __command struct command_d c_nand = {
	.command = "lpcnand",
	.func = cmd_lpcnand,
	COMMAND_DESCRIPTION ("lpcnand test function")
	COMMAND_HELP (
	"lpcnand [SUBCOMMAND [PARAMETER]]\n"
	" format     - Write boot params and bad blocklist to block 0\n"
	" scan       - Scan for factory marked bad blocks\n"
	" bblist     - Dump bad block list (from block 0)\n"
	" restore    - Restore device to factory default state\n"
	" protblk0   - Enable or disable block 0 modify protection\n"
	"\n")
};

