/* drv-mmc.c

written by Durgesh Pattamatta
19 Oct 2005

Copyright (C) 2008 NXP Semiconductors

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
version 2 as published by the Free Software Foundation.
Please refer to the file debian/copyright for further details.

NOTES
-----

o Detection should be continuous.
o If the card doesn't respond to the known address, we can
perform acqurire again.
o This can be done when the report is generated as well.
o Perhaps we acquire every time?
o The only way to know we have the right card is to randomize the
ID we set in MMC cards or...we have to acquire every time we
start a transaction.
o IO should really be quite easy, check for boundaries and read
blocks as we do in the CF driver.
o Enable SD mode when available.  There are two pieces, the
controller and the card.  
o Check for the best speed.  

-----------
DESCRIPTION
-----------

Clocking
--------

The specification limits the clock frequency during identification
to 400KHz.  During I/O, the limit is set based on card type 
20/25/26/52MHz for MMC/SD,SDHC/new  MMC/HighMMC cards respectively.

Block Caching
-------------

The driver caches one block from the card and copied data from the
cached block to the caller's buffer.  This is done because the
callers aren't expected to be efficient about reading large blocks
of data.  This is a convenience for the upper layers as the command
routines are best when they can handle data in the manner most
efficient to their structure.

Card Acquisition
----------------

The present state of the card acqusition logic is weak.  We acquire
a card with the system initializes.  If the card goes offline, we
have no way to detect and correct it.  What should happen is the
read code ought to detect that the card has changed, probably
because the select fails, and perform the acquire at that time.


*/

#include <config.h>
#include <driver.h>
#include <service.h>
#include <linux/string.h>
#include <apex.h>
#include <command.h>
#include <mach/hardware.h>
#include <console.h>
#include <error.h>

#include <mmc.h>

#define USE_SD      /* Allow SD cards */
#define USE_WIDE    /* Allow WIDE mode */
//#define USE_MULTIBLOCK_READ /* Use multiblock read implementation */


#if defined USE_MMC_BOOTSTRAP
# define SECTION __section (.bootstrap)
# define SECTIOND __section (.bss_bootstrap)
#else
# define SECTION
# define SECTIOND
#endif


extern char* strcat (char*, const char*);

//#define TALK 1
//#undef TALK 

#if defined (USE_MMC_BOOTSTRAP)
# undef TALK
#endif

#if defined (TALK)
#define PRINTF(f...)    printf (f)
#define ENTRY(l)    printf ("%s\n", __FUNCTION__)
#define DBG(l,f...)   do { if (TALK >= l) printf (f); } while (0)
#else
#define PRINTF(f...)    do {} while (0)
#define ENTRY(l)    do {} while (0)
#define DBG(l,f...)   do {} while (0)
#endif

struct mmc_info SECTIOND mmc;
unsigned char SECTIOND mmc_rgb[MMC_SECTOR_SIZE];

#define MS_TIMEOUT 1000

#if defined (USE_MMC_BOOTSTRAP)

static void SECTION _memcpy (void* dest, const void* src, size_t cb)
{
	while (cb--)
		*(unsigned char*) dest++ = *(unsigned char*) src++;
}
#define memcpy _memcpy

static void SECTION _memset (void* pv, int v, size_t cb)
{
	while (cb--)
		*(unsigned char*) pv++ = v;
}
#undef memset
#define memset _memset

#endif

static inline void mmc_clear (void) {
	memset (&mmc, 0, sizeof (mmc));
	mmc.ib = -1; 
}

static void mmc_report (void);

static inline unsigned long response_ocr (void) {
	u32* resp = (u32*)&mmc.response[0];

	return *resp;
}
static inline u32 get_bits(int start, int end, u32* data) 
{
	u32 v;
	u32 i = end >> 5;
	u32 j = start & 0x1f;

	if ( i == (start >> 5) )
		v = (data[i] >> j);
	else
		v = ((data[i] << (32 - j)) | (data[start >> 5] >> j));

	return (v & ((1<<(end - start+1))-1));
}

#if defined (TALK)
static const char* report_status (unsigned long l)
{
	static char sz[256];

	sprintf (sz, "[%04lx", l);
	if (l & SDMMC_INT_RESP_ERR)
		strcat (sz, " RESP_ERR");
	if (l & SDMMC_INT_CMD_DONE)
		strcat (sz, " CMD_DONE");
	if (l & SDMMC_INT_TXDR)
		strcat (sz, " TXDR");
	if (l & SDMMC_INT_RXDR)
		strcat (sz, " RXDR");
	if (l & SDMMC_INT_RCRC)
		strcat (sz, " RCRC");
	if (l & SDMMC_INT_DCRC)
		strcat (sz, " DCRC");
	if (l & SDMMC_INT_RTO)
		strcat (sz, " RTO");
	if (l & SDMMC_INT_DTO)
		strcat (sz, " DTO");
	if (l & SDMMC_INT_HTO)
		strcat (sz, " HTO");
	if (l & SDMMC_INT_FRUN)
		strcat (sz, " FRUN");
	if (l & SDMMC_INT_HLE)
		strcat (sz, " HLE");
	if (l & SDMMC_INT_SBE)
		strcat (sz, " SBE");
	if (l & SDMMC_INT_ACD)
		strcat (sz, " ACD");
	if (l & SDMMC_INT_EBE)
		strcat (sz, " EBE");
	strcat (sz, "]");
	return sz;
}
#endif



/* clear_all

clears the FIFOs, response and data, and the interrupt status.
*/

static void SECTION clear_all (void)
{
	/* reset all blocks */
	SDMMC_CTRL |= SDMMC_CTRL_FIFO_RESET;
	/* wait till resets clear */
	while (SDMMC_CTRL & SDMMC_CTRL_FIFO_RESET);

	/* Clear interrupt status */
	SDMMC_RINTSTS = 0xFFFFFFFF;
}

static int SECTION send_cmd (u32 cmd, u32 arg)
{
	volatile int tmo = 50;
	volatile int delay;

	SDMMC_CMDARG = arg;
	SDMMC_CMD = SDMMC_CMD_START | cmd;

	while (--tmo && (SDMMC_CMD & SDMMC_CMD_START)) {
		if (tmo & 1)
			delay = 50;
		else
			delay = 18000;

		while (--delay > 1);
	}

	return (tmo < 1)? 1: 0;
}

static void SECTION set_clock (int speed)
{
	/* compute SD/MMC clock dividers */
	u32 mmc_clk = cgu_get_clk_freq(CGU_SB_SD_MMC_CCLK_IN_ID);
	u32 div = ((mmc_clk / speed) + 2) >> 1;

	if ((div == SDMMC_CLKDIV) && SDMMC_CLKENA)
		return; /* requested speed is already set */

	/* disable clock */
	SDMMC_CLKENA = 0;
	SDMMC_CLKSRC = 0;

	/* inform CIU */
	if ( send_cmd( SDMMC_CMD_UPD_CLK | SDMMC_CMD_PRV_DAT_WAIT, 0) )   
		DBG (2, "MMC clock 1tmo\n");

	/* set clock to desired speed */
	SDMMC_CLKDIV = div;
	/* inform CIU */
	if ( send_cmd( SDMMC_CMD_UPD_CLK | SDMMC_CMD_PRV_DAT_WAIT, 0) )   
		DBG (2, "MMC clock 2tmo\n");

	/* enable clock */
	SDMMC_CLKENA = SDMMC_CLKEN_ENABLE;
	/* inform CIU */
	if ( send_cmd( SDMMC_CMD_UPD_CLK | SDMMC_CMD_PRV_DAT_WAIT, 0) )   
		DBG (2, "MMC clock 3tmo\n");


	DBG (2, "MMC clock %ld req: %d main:%d\n", mmc_clk/(SDMMC_CLKDIV << 1) , speed, mmc_clk);
}

/* pull_response

retrieves a command response.  The length is the length of the
expected response, in bits.

*/

static void SECTION pull_response (int length)
{
	u32* resp = (u32*)&mmc.response[0];

	*resp++ = SDMMC_RESP0;
	if (length > 31)
		*resp++ = SDMMC_RESP1;

	if (length > 63)
		*resp++ = SDMMC_RESP2;

	if (length > 95)
		*resp++ = SDMMC_RESP3;

	DBG (3, "resp: 0x%lx 0x%lx 0x%lx 0x%lx(%d)\n", 
		SDMMC_RESP0, SDMMC_RESP1, SDMMC_RESP2, SDMMC_RESP3, length);
}

static void SECTION pull_data (void* pv, int cnt)
{
	int i = 0;
	int fcnt;

	while ( i < cnt) {
		fcnt = SDMMC_GET_FCNT(SDMMC_STATUS) << 2;
		/* LPC313x maps address above 0x18000100 to FIFO to support
		burst reading. So let's use memcpy to speedup read. Since we
		always pass 4byte aligned address and also multiple of 4 count
		let's hope memcpy of C runtime does LDMs.*/
		if (fcnt)
			memcpy(pv + i, (void*)SDMMC_DATA_ADR, fcnt);
		i += fcnt;
	}
}

static void SECTION push_data (const void* pv, size_t cb)
{
	int i = 0;
	int fcnt;

	while (i < cb)
	{
		fcnt = SDMMC_FIFO_SZ - (SDMMC_GET_FCNT(SDMMC_STATUS) << 2);
		/* LPC313x maps address above 0x18000100 to FIFO to support
		burst writing. So let's use memcpy to speedup write. Since we
		always pass 4byte aligned address and also multiple of 4 count
		let's hope memcpy of C runtime does STMs (store multiples).*/
		memcpy((void*)SDMMC_DATA_ADR, pv + i, fcnt);
		i += fcnt;
	}
}

static unsigned long SECTION wait_for_completion (short bits)
{
	unsigned short status = 0;
	unsigned long time_start = timer_read ();
	int timed_out = 0;

	/* also check error conditions */
	bits |= SDMMC_INT_EBE | SDMMC_INT_SBE | SDMMC_INT_HLE
		| SDMMC_INT_RTO | SDMMC_INT_RCRC | SDMMC_INT_RESP_ERR;

	if (bits & SDMMC_INT_DATA_OVER)
		bits |= SDMMC_INT_FRUN | SDMMC_INT_HTO | SDMMC_INT_DTO
		| SDMMC_INT_DCRC;

	DBG (3, " (%x) ", bits);
	do {
		udelay (1);
		status = SDMMC_RINTSTS;

		if (timer_delta (time_start, timer_read ()) >= MS_TIMEOUT) {
			//printf ("\nbailing at timeout status 0x%x bits 0x%x\n", status, bits);
			timed_out = 1;
		}
	} while (((status  & bits) == 0) && !timed_out);

	DBG (3, " => %s 0x%lx\n", report_status (status), SDMMC_STATUS);

	if (timed_out)
		DBG (1, " timedout => cmd:0x%lx %s 0x%lx\n", SDMMC_CMD, report_status (status), SDMMC_STATUS);

	return status | (timed_out ? SDMMC_INT_RTO : 0);
}

#if defined (TALK) && ! defined (USE_MMC_BOOTSTRAP)
static void report_command (u32 cmd, u32 cmd_reg, u32 arg)
{
	DBG (3, "cmd (%03ld) 0x%08lx (0x%x)/(0x%x)  arg 0x%08lx(0x%x)"
		" ctrl 0x%04lxrate 0x%02lx\n",
		(SDMMC_CMD & 0x3F), SDMMC_CMD, cmd_reg, cmd, SDMMC_CMDARG, 
		arg, SDMMC_CTRL, SDMMC_CLKDIV);
}
#else
# define report_command(cmd, cmd_reg, arg)
#endif

static int SECTION execute_command (unsigned long cmd,
				    unsigned long arg,
				    unsigned short wait_status)
{
	/* if APP command there are 2 stages */
	int step = (cmd & CMD_BIT_APP) ? 2 : 1;
	int status = 0;
	u32 cmd_reg = 0;

	if (!wait_status)
		wait_status = ((cmd >> CMD_SHIFT_RESP) & CMD_MASK_RESP)
		? SDMMC_INT_CMD_DONE : SDMMC_INT_DATA_OVER;

	/* Clear the interrupts & FIFOs*/
	if (cmd & CMD_BIT_DATA)
		clear_all();
	while (step)
	{
		set_clock ((cmd & CMD_BIT_LS) ? 400000 : mmc.speed);

		if (status)
			DBG (3, "%s: step %d s 0x%x\n", __FUNCTION__, step, status);

		SDMMC_RINTSTS = 0xFFFFFFFF;

		switch (step) {

		case 1:     /* Execute command */
			cmd_reg = ((cmd & CMD_MASK_CMD) >> CMD_SHIFT_CMD)
				| ((cmd & CMD_BIT_INIT)  ? SDMMC_CMD_INIT : 0)
				| ((cmd & CMD_BIT_DATA)  ? (SDMMC_CMD_DAT_EXP | SDMMC_CMD_PRV_DAT_WAIT) : 0)
				| ((((cmd >> CMD_SHIFT_RESP) & CMD_MASK_RESP) == 2) ? SDMMC_CMD_RESP_LONG : 0)
				| ((cmd & (CMD_MASK_RESP << CMD_SHIFT_RESP)) ? SDMMC_CMD_RESP_EXP : 0)
				| ((cmd & CMD_BIT_WRITE)  ? SDMMC_CMD_DAT_WR : 0)
				| ((cmd & CMD_BIT_STREAM) ? SDMMC_CMD_STRM_MODE: 0)
				| ((cmd & CMD_BIT_BUSY) ? SDMMC_CMD_STOP: 0)
				| ((cmd & CMD_BIT_AUTO_STOP)  ? SDMMC_CMD_SEND_STOP: 0)
				| SDMMC_CMD_START 
				;
			/* wait for previos data finsh for select/deselect commands */
			if (((cmd & CMD_MASK_CMD) >> CMD_SHIFT_CMD) == MMC_SELECT_CARD)
			{
			  cmd_reg |= SDMMC_CMD_PRV_DAT_WAIT;
			}
			/* wait for command to be accepted by CIU */
			if ( send_cmd (cmd_reg, arg) == 0)
			  --step;
			break;

		case 0:
			return 0;

		case 2:      /* APP prefix */
			cmd_reg = MMC_APP_CMD 
				| SDMMC_CMD_RESP_EXP /* Response is status */
				| ((cmd & CMD_BIT_INIT)  ? SDMMC_CMD_INIT : 0)
				| SDMMC_CMD_START 
				;
			if ( send_cmd (cmd_reg, mmc.rca<<16) == 0)
			  --step;
			break;
		}


		report_command (cmd, cmd_reg, arg);


		/* wait for command response*/
		status = wait_for_completion (wait_status);

		/* We return an error if there is a timeout, even if we've fetched a
		response */
		if (status & SDMMC_INT_ERROR) {

			if ((SDMMC_CMD & 0x3F) != 0x07)
				DBG (1, "cmd (%03ld) 0x%08lx arg 0x%08lx st:0x%08x\n", 
				(SDMMC_CMD & 0x3F), SDMMC_CMD, SDMMC_CMDARG, status);

			return status;
		}

		if (status & SDMMC_INT_CMD_DONE)
			switch ((cmd >> CMD_SHIFT_RESP) & CMD_MASK_RESP) {
			case 0:
				break;
			case 1:
			case 3:
				pull_response (31);
				break;
			case 2:
				pull_response (128);
				break;
			}

	}
	return 0;
}
/** Purpose: Puts current selected card in trans state **/
static int set_trans_state(void)
{
	u32 status;
	u32* resp = (u32*)&mmc.response[0];

	/* get current state of the card */
	status = execute_command(CMD_SEND_STATUS, mmc.rca<<16, 0);
	if (status & SDMMC_INT_RTO)
	{
		/* unable to get the card state. So return immediatly. */
		return -1;
	}

	DBG (3, "mmc status 0x%x 0x%x 0x%x 0x%x\n", status, *resp, mmc.rca, CMD_SEND_STATUS);
	/* check card state in response */
	status = R1_CURRENT_STATE(*resp);
	switch (status)
	{
	case MMC_STBY_ST:
		/* put card in 'Trans' state */
		status = execute_command (CMD_SELECT_CARD, mmc.rca<<16, 0);
		if (status != 0)
		{
			/* unable to put the card in Trans state. So return immediatly. */
			return -1;
		}
		break;
	case MMC_TRAN_ST:
		/*do nothing */
		break;
	default:
		/* card shouldn't be in other states so return */
		return -1;
	}

#if defined (USE_WIDE)
	if (mmc.card_type & CARD_TYPE_SD) {
		execute_command (CMD_SD_SET_WIDTH, 2, 0); /* SD, 4 bit width */
		/* if positive response */
		SDMMC_CTYPE = SDMMC_CTYPE_4BIT;
	}
#endif
	/* set block length */
	SDMMC_BLKSIZ = MMC_SECTOR_SIZE;
	status = execute_command (CMD_SET_BLOCKLEN, MMC_SECTOR_SIZE, 0);


	return 0;
}


/* mmc_acquire

detects cards on the bus and initializes them for IO.  It can
detect both SD and MMC card types.

It will only detect a single card and the first one will be the one
that is configured with an RCA and will be used by the driver.

*/

void SECTION mmc_acquire (void)
{
	int status;
	int tries = 0;
	unsigned long ocr = 0x00ff8000;
	unsigned long r;
	int state = 0;
	unsigned long command = 0;

	mmc_clear ();

#if defined(CONFIG_MACH_VAL313X)
	/* check if card is inserted in the slot. GPIO12 */
	GPIO_IN(IOCONF_GPIO, _BIT(6));
	if (GPIO_STATE(IOCONF_GPIO) & _BIT(6)) {
		/* card is absent  return */
		DBG (1, "%s: Card not inserted 0x%lx\n", __FUNCTION__, GPIO_STATE(IOCONF_GPIO));
		return;
	}
#elif defined(CONFIG_MACH_EA313x_V1) || defined(CONFIG_MACH_EA313x_V2) || defined(CONFIG_MACH_VAL3154)
	/* check if card is inserted in the slot. mI2STX_BCK0 */
	GPIO_IN(IOCONF_EBI_MCI, _BIT(7));
	if (GPIO_STATE(IOCONF_EBI_MCI) & _BIT(7)) {
		/* card is absent  return */
		DBG (1, "%s: Card not inserted 0x%lx\n", __FUNCTION__, GPIO_STATE(IOCONF_GPIO));
		return;
	}
#elif defined(CONFIG_MACH_EPLPC3131_V1)
   /* FIXME: add card selection code here. Until then 
	    card is always selected.
	  */
#endif

	/* clear card type */
	SDMMC_CTYPE = 0;
	/* set high speed for the card as 20MHz */
	mmc.speed = 20000000;

	status = execute_command (CMD_IDLE, 0, SDMMC_INT_CMD_DONE);

	while (state < 100) {

		switch (state) {

		case 0:     /* Setup for SD */
			mmc.acquired = 0;
			/* check if it is SDHC card */
			status = execute_command (CMD_SD_SEND_IF_COND, 0x1AA, 0);
			if (!(status & SDMMC_INT_RTO)) {

				u32* resp = (u32*)&mmc.response[0];

				/* check response has same echo pattern */
				if ((resp[0] & 0xFF) == 0xAA)
					/* it is SD 2.0 card so indicate we are SDHC capable*/
					ocr |= OCR_HC_CCS;
			}  

			++state;
			command = CMD_SD_OP_COND;
			tries = 10;   
			/* assume SD card */
			mmc.card_type |= CARD_TYPE_SD;
			/* for SD cards high speed is 25MHz */
			mmc.speed = 25000000;

			break;

		case 10:      /* Setup for MMC */
			/* start fresh for MMC crds */
			mmc.card_type &= ~CARD_TYPE_SD;  
			status = execute_command (CMD_IDLE, 0, SDMMC_INT_CMD_DONE);
			command = CMD_MMC_OP_COND;
			tries = 10;
			ocr |= OCR_HC_CCS;
			++state;
			/* for MMC cards high speed is 20MHz */
			mmc.speed = 20000000;
			DBG (1, "Trying MMC sequence\n");
			break;

		case 1:
		case 11:
			status = execute_command (command, 0, 0);
			if (status & SDMMC_INT_RTO)
				state += 9;		/* Mode unavailable */
			else
				++state;
			break;

		case 2:			/* Initial OCR check  */
		case 12:
			ocr = response_ocr () | (ocr & OCR_HC_CCS);
			if (ocr & OCR_ALL_READY)
				++state;
			else
				state += 2;
			break;

		case 3:			/* Initial wait for OCR clear */
		case 13:
			while ((ocr & OCR_ALL_READY) && --tries > 0) {
				mdelay (MS_ACQUIRE_DELAY);
				status = execute_command (command, 0, 0);
				ocr = response_ocr () | (ocr & OCR_HC_CCS);
			}
			if (ocr & OCR_ALL_READY)
				state += 7;
			else
				++state;
			break;

		case 14:
			/* for MMC cards set high capacity bit */
			ocr |= OCR_HC_CCS;
		case 4:     /* Assign OCR */  
			tries = 200;
			ocr &= 0x40ff8000;	/* Mask for the bits we care about */
			do {
				mdelay (MS_ACQUIRE_DELAY);
				mmc.acquire_time += MS_ACQUIRE_DELAY;
				status = execute_command (command, ocr, 0);
				r = response_ocr ();
			} while (!(r & OCR_ALL_READY) && --tries > 0);
			if (r & OCR_ALL_READY) {
				/* is it high capacity card */
				mmc.card_type |= (r & OCR_HC_CCS);  
				++state;
			} else
				state += 6;

			break;

		case 5:     /* CID polling */
		case 15:
			status = execute_command (CMD_ALL_SEND_CID, 0, 0);
			memcpy (mmc.cid, mmc.response + 1, 16);
			++state;
			break;

		case 6:     /* RCA send */
			status = execute_command (CMD_SD_SEND_RCA, 0, 0);
			mmc.rca = (*((u32*)&mmc.response[0])) >> 16;
			++state;
			break;
		case 16:      /* RCA assignment */
			mmc.rca = 1;
			status = execute_command (CMD_MMC_SET_RCA, mmc.rca << 16, 0);
			++state;
			break;

		case 7:
		case 17:
			status = execute_command (CMD_SEND_CSD, mmc.rca << 16, 0);
			memcpy (mmc.csd, mmc.response, 16);
			mmc.acquired = 1;
			++state;
			break;

		default:
			state += 100; /* break from while loop */
			break;
		}
	}

	if (mmc_card_acquired ()) {

		/* change delay gates per card type */
		if (mmc.card_type & CARD_TYPE_SD)
			SYS_SDMMC_DELAYMODES = 0x1B;
		else
			SYS_SDMMC_DELAYMODES = 0x16;

		mmc.read_bl_len = get_bits(80, 83, (u32*)mmc.csd); //mmc.csd[10] & 0xf;
		mmc.block_len = 1 << mmc.read_bl_len;

		if ( (mmc.card_type & CARD_TYPE_HC) && 
			(mmc.card_type & CARD_TYPE_SD) )
		{
			/* See section 5.3.3 CSD Register (CSD Version 2.0) of SD2.0 spec
			an explanation for the calculation of these values
			*/
			mmc.c_size = get_bits(48, 63, (u32*)mmc.csd) + 1; 
			mmc.blocknr = mmc.c_size << 10; /* 512 byte blocks */
		}
		else
		{
			/* See section 5.3 of the 4.1 revision of the MMC specs for
			an explanation for the calculation of these values
			*/
			mmc.c_size = get_bits(62, 73, (u32*)mmc.csd);
			mmc.c_size_mult = get_bits(47, 49, (u32*)mmc.csd); //csd_c_size_mult ();
			mmc.mult = 1 <<  (mmc.c_size_mult + 2);
			mmc.blocknr = (mmc.c_size + 1) * mmc.mult;
			/* adjust blocknr to 512/block */
			if ( mmc.block_len > 512)
				mmc.blocknr = mmc.blocknr * (mmc.block_len >> 9);

			/* get extended CSD for newer MMC cards CSD spec >= 4.0*/
			if ( ((mmc.card_type & CARD_TYPE_SD) == 0) && 
				(get_bits(122, 125, (u32*)mmc.csd) >= 4) ) {

					u32 ext_csd[MMC_SECTOR_SIZE/4];

					/* put card in trans state */
					status = execute_command (CMD_SELECT_CARD, mmc.rca<<16, 0);
					/* set block size and byte count */
					SDMMC_BLKSIZ = MMC_SECTOR_SIZE;
					SDMMC_BYTCNT = MMC_SECTOR_SIZE;
					/* send EXT_CSD command */
					status = execute_command (CMD_SEND_EXT_CSD, 0, SDMMC_INT_CMD_DONE 
						| SDMMC_INT_DATA_OVER | SDMMC_INT_RXDR);
					if ((status & SDMMC_INT_ERROR) == 0) {

						pull_data(ext_csd, MMC_SECTOR_SIZE);

						/* check EXT_CSD_VER is greater than 1.1 */
						if ( (ext_csd[48] & 0xFF) > 1)
							mmc.blocknr = ext_csd[53]; /* bytes 212:215 represent sec count */
						/* switch to 52MHz clock if card type is set to 1 or else set to 26MHz */
						if ( (ext_csd[49] & 0xFF) == 1 ) {
							/* for type 1 MMC cards high speed is 52MHz */
							mmc.speed = 52000000;
						}
						else {
							/* for type 0 MMC cards high speed is 26MHz */
							mmc.speed = 26000000;
						}
					}
			}
		}

		mmc.device_size = mmc.blocknr << 9; /* blocknr * 512 */

	}
}

void SECTION mmc_init (void)
{
  volatile u32 i;
	/* enable SD/MMC clock */
	cgu_clk_en_dis(CGU_SB_SD_MMC_HCLK_ID, 1);
	cgu_clk_en_dis(CGU_SB_SD_MMC_CCLK_IN_ID, 1);

	/* reset SD/MMC/MCI modules through CGU */
  /* clear and set the register */
  CGU_CFG->resetn_soft[SD_MMC_PNRES_SOFT] = 0;
  CGU_CFG->resetn_soft[SD_MMC_NRES_CCLK_IN_SOFT] = 0;
  /* introduce some delay */
  for (i = 0;i < 1000;i++);
  CGU_CFG->resetn_soft[SD_MMC_NRES_CCLK_IN_SOFT] = CGU_CONFIG_SOFT_RESET;
  CGU_CFG->resetn_soft[SD_MMC_PNRES_SOFT] = CGU_CONFIG_SOFT_RESET;

	/* Set IOCONF to MCI pins */
	SYS_SDMMC_DELAYMODES = 0;
	SYS_MUX_GPIO_MCI = 1;
	/* set the pins as driven by IP in IOCONF */
	GPIO_DRV_IP(IOCONF_EBI_MCI, 0xF0000003);

	/* power-up / select MMC+ slot */
	GPIO_OUT_LOW(IOCONF_EBI_I2STX_0, _BIT(5));

	/* wait atleast 10ms */
	udelay (10 * 1000);

	/* set delay gates */
	SYS_SDMMC_DELAYMODES = 0x1B;

	/* reset all blocks */
	SDMMC_CTRL = SDMMC_CTRL_RESET | SDMMC_CTRL_FIFO_RESET 
		| SDMMC_CTRL_DMA_RESET;
	/* wait till resets clear */
	while (SDMMC_CTRL & 
		(SDMMC_CTRL_RESET | SDMMC_CTRL_FIFO_RESET | SDMMC_CTRL_DMA_RESET));

	/* Clear the interrupts for the host controller */
	SDMMC_RINTSTS = 0xFFFFFFFF;

	/* Put in max timeout */
	SDMMC_TMOUT = 0xFFFFFF40;

	/* FIFO threshold settings  */
	SDMMC_FIFOTH = (0x2 << 28) | (0x10 << 16) | (0x10 << 0);

	/* disable clock to CIU */
	SDMMC_CLKENA = 0;
	SDMMC_CLKSRC = 0;

	/* clear mmc structure*/  
	mmc_clear ();
	/* start card enumeration */
	mmc_acquire ();
}

static void mmc_report (void)
{
	printf ("  mmc:    %s card acquired",
		mmc_card_acquired () ? ((mmc.card_type & CARD_TYPE_SD)? "sd" : "mmc") : "no");
	if (mmc_card_acquired ()) {
		printf (", rca 0x%x (%d ms)", mmc.rca, mmc.acquire_time);
		printf (", %ld.%02ld MiB\n",
			mmc.device_size/(1024*1024),
			(((mmc.device_size/1024)%1024)*100)/1024);
		dump (mmc.cid, 16, 0);
		dump (mmc.csd, 16, 0);
		printf ("\n");
	}
	else
		printf ("\n");
}

static int mmc_open (struct descriptor_d* d)
{
	DBG (2,"%s: opened %ld %ld\n", __FUNCTION__, d->start, d->length);

	if (!mmc_card_acquired ())
		ERROR_RETURN (ERROR_IOFAILURE, "no card");

	return 0;
}


/* mmc_read

performs the read of data from the SD/MMC card.  It handles
unaligned, and sub-block length I/O.

*/

ssize_t SECTION mmc_read (struct descriptor_d* d, void* pv, size_t cb)
{
	ssize_t cbRead = 0;
	int status;


	if (d->index + cb > d->length)
		cb = d->length - d->index;

	DBG (4, "mmc reading %d\n", cb);
	while (cb) {
		u32 index = d->start + d->index;
		int availableMax = MMC_SECTOR_SIZE - (index & (MMC_SECTOR_SIZE - 1));
		int available = cb;
		int block = index >> 9;

		if (available > availableMax)
			available = availableMax;


		/* Read block into buffer  */
		if (block != mmc.ib) {
			int tmp_index = 0;

			/* if high capacity card use block index */
			if ( mmc.card_type & CARD_TYPE_HC)
				tmp_index = block;
			else
				tmp_index = index & ~(MMC_SECTOR_SIZE - 1); 

			/* if can't put card in trans state return immediately */
			if (set_trans_state())
				return 0;

			DBG (1, "mmc reading %d\n", block, tmp_index);

			/* read single block and cache it */
			SDMMC_BLKSIZ = MMC_SECTOR_SIZE;
			SDMMC_BYTCNT = MMC_SECTOR_SIZE;
			status = execute_command (CMD_READ_SINGLE, tmp_index,
				SDMMC_INT_CMD_DONE | SDMMC_INT_DATA_OVER | SDMMC_INT_RXDR);
			if ((status & SDMMC_INT_ERROR) == 0) {        
				pull_data(mmc_rgb, MMC_SECTOR_SIZE);
				mmc.ib = block;
			}
		}

		DBG (1, "mmc read: 0x%08x %d \n", index, available);
		memcpy (pv, mmc_rgb + (index & (MMC_SECTOR_SIZE - 1)), available);

		d->index += available;
		cb -= available;
		cbRead += available;
		pv += available;
	}


	return cbRead;
}


/* mmc_write

performs the write of data from the SD/MMC card.

//   It handles unaligned, and sub-block length I/O.

*/

ssize_t SECTION mmc_write (struct descriptor_d* d, const void* pv, size_t cb)
{
	ssize_t cbWrote = 0;
	int status;

	/* partial block wrtites shouldn't be allowed */

	if (d->index + cb > d->length)
		cb = d->length - d->index;

	/* if can't put card in trans state return immediately */
	if (set_trans_state())
		return 0;


	while (cb) {
		unsigned long index = d->start + d->index;
		int availableMax = MMC_SECTOR_SIZE
			- (index & (MMC_SECTOR_SIZE - 1)); /* Must not overlap a block */
		int available = cb;

		if (available > availableMax)
			available = availableMax;

		//    DBG (1, "%ld %ld\n", mmc.ib, index);

		DBG (1, "%s: write av %d  avM %d  ind %ld  cb %d\n", __FUNCTION__,
			available, availableMax, index, cb);

		SDMMC_BYTCNT = available;

		status = execute_command (CMD_WRITE_SINGLE, index, 
			SDMMC_INT_CMD_DONE | SDMMC_INT_TXDR);

		DBG (2, "%s: sending waiting to write %x\n", __FUNCTION__, status);

		push_data (pv, available);
		DBG (2, "%s: restarted %s\n", __FUNCTION__, report_status (SDMMC_RINTSTS));
		//    status = execute_command (CMD_STOP, 0, 0);
		status = wait_for_completion (SDMMC_INT_DATA_OVER);
		DBG (2, "%s: end %s\n", __FUNCTION__, report_status (status));

		d->index += available;
		cb -= available;
		cbWrote += available;
		pv += available;
	}

	return cbWrote;
}

static __driver_5 struct driver_d mmc_driver = {
	.name    = "mmc",
	.description = "MMC/SD card driver",
	.flags = DRIVER_READPROGRESS(2),
	.open    = mmc_open,
	.close = close_helper,
	.read    = mmc_read,
	.write = mmc_write,
	.seek    = seek_helper,
};

static __service_6 struct service_d mmc_service = {
	.init    = mmc_init,
#if !defined (CONFIG_SMALL)
	.report  = mmc_report,
#endif
};

static int cmd_mmc (int argc, const char** argv)
{

	mmc_init ();

	return 0;
}

static __command struct command_d c_mmc = {
	.command = "mmc",
	.func = cmd_mmc,
	COMMAND_DESCRIPTION ("test MMC controller")
};
