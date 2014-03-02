/***********************************************************************
 * $Id:: s1l_image_mngt.c 4931 2010-09-16 00:38:12Z usb10132           $
 *
 * Project: Image management functions
 *
 * Description:
 *     Image management functions
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
#include "lpc_line_parser.h"
#include "s1l_sys.h"
#include "s1l_sys_inf.h"
#include "lpc32xx_clkpwr_driver.h"
#include "lpc_arm922t_cp15_driver.h"
#include "lpc_string.h"
#include "s1l_sys_inf.h"
#include "s1l_sys.h"
#include "s1l_fat.h"

static int bytestoread, cindex, lefttoread;
static UNS_32 curblock, curpage;
static BOOL_32 checkblk;

static UNS_8 rawdl_msg[] =
	"Starting terminal download, send break to stop";
static UNS_8 rawnanddlns_msg[] = "Raw image NAND load is not supported";
static UNS_8 nofilmsg[] = "File in SD root directory doesn't exist";
static UNS_8 noiif_msg[] = "No image in FLASH to load";
static UNS_8 nsaeerr_msg[] = "Error saving image to FLASH";
static UNS_8 nloeerr_msg[] = "Error loading image from FLASH";
static UNS_8 bskip_msg[] = "Skipping bad block ";
static UNS_8 readerr_msg[] = "Error reading NAND sector ";
static UNS_8 writeerr_msg[] = "Error writing NAND sector ";
UNS_8 noflash_msg[] = "No FLASH detected on this board";
UNS_8 blkdeverr_msg[] = "Error opening block device";

/***********************************************************************
 *
 * Function: srec_get_hex
 *
 * Purpose: Gets a hex value from a s-record hex field
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     data     : Pointer to start of hex field
 *     numchars : Number of hex characters in field
 *
 * Outputs: None
 *
 * Returns: Converted hex value
 *
 * Notes: None
 *
 **********************************************************************/
static UNS_32 srec_get_hex(UNS_8 *data,
						   int numchars) {
    UNS_32 hval;
    UNS_8 hexval [32];
    int offset;

    offset = 0;
    hexval [0] = '0';
    hexval [1] = 'x';
    while (numchars > offset) {
        hexval [offset + 2] = data [offset];
        offset++;
    }
    hexval [offset + 2] = '\0';
    str_hex_to_val(hexval, &hval);

    return hval;
}

/***********************************************************************
 *
 * Function: lpc_srec_cksm
 *
 * Purpose: Generates a S-record field checksum
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     data      : Hex string
 *     offset    : Offset in hex string
 *     hex_pairs : Number of hex pairs
 *
 * Outputs: None
 *
 * Returns: Converted hex value
 *
 * Notes: None
 *
 **********************************************************************/
static UNS_8 lpc_srec_cksm(UNS_8 *data,
						   INT_32 offset,
						   INT_32 hex_pairs) {
    int lastoffs;
    UNS_8 cksm = 0;

    lastoffs = ((hex_pairs + 1) * 2) + offset;
    while (offset < lastoffs) {
        cksm = cksm + (UNS_8) srec_get_hex(&data [offset], 2);
        offset += 2;
    }

    return (0xFF - cksm);
}

/***********************************************************************
 *
 * Function: srec_parse_line
 *
 * Purpose: Parses an S-record line
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     data : S-record string
 *     ft   : Pointer to file load type structure to populate
 *
 * Outputs: None
 *
 * Returns: Converted hex value
 *
 * Notes: None
 *
 **********************************************************************/
static BOOL_32 srec_parse_line(UNS_8 *data,
                               FILE_DATA_T *ft) {
    UNS_8 cksm, cksmcmp, *addr8;
    UNS_32 addr, rectype, addrbytes;
    static UNS_32 lastaddr;
    int numpairs, offset;
    BOOL_32 parsed = FALSE;

    /* Verify S-record marker */
    if ((data [0] == 'S') || (data [0] == 's')) {
        /* Get record type */
        parsed = dec_char_to_val(data [1], &rectype);

        /* Get number of hex pairs */
        numpairs = srec_get_hex(&data [2], 2);

		/* Compute checksum for the line and get line checksum */
        cksmcmp = lpc_srec_cksm(data, 2, (numpairs - 1));
        cksm = srec_get_hex(&data [(4 + ((numpairs - 1) * 2))], 2);
        if (cksmcmp == cksm) {
            /* Handle record */
            switch (rectype) {
                case 1: /* 2 byte address */
                case 2: /* 3 byte address */
                case 3: /* 4 byte address */
                    /* Data block */
                    addrbytes = rectype + 1;
                    addr = srec_get_hex(&data [4], (addrbytes * 2));

					if (ft->loadaddr == 0xFFFFFFFF) 
					{
						lastaddr = addr;
						ft->loadaddr = addr;
					}
					else if (addr != lastaddr) 
					{
						ft->contiguous = FALSE;
					}

                    /* Move hex pairs to memory */
                    addr8 = (UNS_8 *) addr;
                    numpairs = numpairs - (1 + addrbytes);
                    offset = 4 + (addrbytes * 2);
                    ft->num_bytes += numpairs;
                    while (numpairs > 0) {
                        *addr8 = (UNS_8) srec_get_hex(&data [offset], 2);
                        offset += 2;
                        numpairs -= 1;
                        addr8++;
                    }
                    lastaddr = (UNS_32) addr8;
                    break;

                case 7: /* 4 byte address */
                case 8: /* 3 byte address */
                case 9: /* 2 byte address */
                    /* Execution address */
                    addrbytes = 11 - rectype;
                    addr = srec_get_hex(&data [4], (addrbytes * 2));
                    ft->startaddr = (PFV) addr;
                    ft->loaded = TRUE;
                    parsed = TRUE;
                    break;

                case 0: /* Block header, ignore */
                case 5: /* Record count, ignore */
                    parsed = TRUE;
                    break;

                default:
                	/* Unknown field type, generate an error */
                    break;
            }
        }
        else 
        {
        	parsed = FALSE;
        }
    }

    return parsed;
}

/***********************************************************************
 *
 * Function: stream_flash_read
 *
 * Purpose: Read data from FLASH
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     fdata : Pointer to file data to fill
 *     bytes : BYtes to read
 *
 * Outputs: None
 *
 * Returns: Number of bytes read
 *
 * Notes: None
 *
 **********************************************************************/
static INT_32 stream_flash_read(UNS_8 *data,
                                int bytes) 
{
	UNS_32 sector;
	INT_32 btoread, bread = 0;

	/* Limit read size */
	if (bytes > bytestoread) 
	{
		bytes = bytestoread;
	}

	while (bytes > 0) 
	{
		/* Is a bad block check needed? */
		if (checkblk == TRUE)
		{
			/* Start of block, read bad block marker */
			if (flash_is_bad_block(curblock) == FALSE)
			{
				/* Block is good */
				checkblk = FALSE;
			}
			else
			{
				/* Block is bad, go to next block */
				curblock++;
			}
		}

		/* Read more data if needed */
		if (checkblk == FALSE)
		{
			/* Is a data read needed? */
			if (lefttoread == 0) 
			{
				/* Out of pages in the block? */
				if (curpage >= sysinfo.nandgeom->pages_per_block) 
				{
					/* Reset block and page marker */
					curblock++;
					curpage = 0;
					checkblk = TRUE;
				}
				else
				{
					/* Time to read in a new sector */
					sector = conv_to_sector(curblock, curpage);
					if (flash_read_sector(sector, secdat, NULL) <= 0)
						term_dat_out((UNS_8 *)
							"Read error: data may be corrupt\r\n");
					lefttoread = sysinfo.nandgeom->data_bytes_per_page;
					cindex = 0;
					curpage++;
				}
			}

			/* Move data into user buffer */
			btoread = lefttoread;
			if (btoread > bytes) 
			{
				btoread = bytes;
			}
			while (btoread > 0) 
			{
				bread++;
				*data = secdat [cindex];
				cindex++;
				btoread--;
				lefttoread--;
				bytes--;
			}
		}
	}

	return bread;
}

/***********************************************************************
 *
 * Function: stream_flash_init
 *
 * Purpose: Initialize NAND streamer
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: TRUE if the streamer was initialized, FALSE otherwise
 *
 * Notes: None
 *
 **********************************************************************/
static BOOL_32 stream_flash_init(void)
{
	BOOL_32 good = FALSE;

	if (syscfg.fsave.valid == TRUE) 
	{
		/* Setup read stream */
		bytestoread = syscfg.fsave.num_bytes;
		curblock = syscfg.fsave.block_first;
		curpage = 0;
		cindex = 0;
		checkblk = TRUE;
		lefttoread = 0;
		good = TRUE;
	}

	return good;
}

/***********************************************************************
 *
 * Function: readline
 *
 * Purpose: Read line with line feed
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     data : Pointer to line data to fill
 *     src  : Data source
 *
 * Outputs: None
 *
 * Returns: TRUE if the line was loaded, otherwise FALSE
 *
 * Notes: None
 *
 **********************************************************************/
static BOOL_32 readline(UNS_8 *data,
                        SRC_LOAD_T src) 
{
	UNS_32 rb;
	int lastidx, idx;
	BOOL_32 linedone = FALSE, linegood = FALSE;

	lastidx = idx = 0;
	while (linedone == FALSE) 
	{
		switch (src) 
		{
			case SRC_TERM:
				/* Has a break occurred? */
				if (term_break() == TRUE) 
				{
					/* Exit now */
					linedone = TRUE;
				}
				else if (term_dat_in((data + idx), 1) > 0) 
				{
					idx++;
				}
				break;

			case SRC_NAND:
				/* Get a byte from the FLASH */
				/* Get a byte from the SD/MMC card */
				rb = stream_flash_read(&data [idx], 1);
				if (rb == 0) 
				{
					linedone = TRUE;
				}
				idx++;
				break;

			case SRC_BLKDEV:
				/* Get a byte from the SD/MMC card */
				rb = fat_file_read(&data [idx], 1);
				if (rb == 0) 
				{
					linedone = TRUE;
				}
				idx++;
				break;

			case SRC_NONE:
			default:
				return FALSE;
		}

		if (lastidx != idx) 
		{
			if (data [lastidx] == '\r') 
			{
				linedone = TRUE;
				linegood = TRUE;
				data [lastidx] = '\0';
			}

			lastidx = idx;
		}
	}

	data [idx] = '\0';
	str_upper_to_lower(data);
	return linegood;
}

/***********************************************************************
 *
 * Function: flash_image_save
 *
 * Purpose: Moves image in memory to FLASH application load region
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Always returns TRUE.
 *
 * Notes: None
 *
 **********************************************************************/
BOOL_32 flash_image_save(void) 
{
	UNS_32 fblock, ffblock;
	INT_32 sector, numsecs, nblks;
	FLASH_SAVE_T flashsavdat;

	/* Set first block pat boot loader */
	ffblock = fblock = sysinfo.sysrtcfg.bl_num_blks;

	/* Save programmed FLASH data */
	flashsavdat.block_first = fblock;
	flashsavdat.num_bytes = sysinfo.lfile.num_bytes;
	flashsavdat.loadaddr = sysinfo.lfile.loadaddr;
	flashsavdat.startaddr = (UNS_32) sysinfo.lfile.startaddr;
	flashsavdat.valid = TRUE;

	/* Get starting sector and number of sectors to program */
	numsecs = sysinfo.lfile.num_bytes /
		sysinfo.nandgeom->data_bytes_per_page;
	if ((numsecs * sysinfo.nandgeom->data_bytes_per_page) <
		sysinfo.lfile.num_bytes) 
	{
		numsecs++;
	}
	flashsavdat.secs_used = numsecs;
	nblks = numsecs / sysinfo.nandgeom->pages_per_block;
	if ((nblks * sysinfo.nandgeom->pages_per_block) < numsecs) 
	{
		nblks++;
	}
	flashsavdat.blocks_used = nblks;

	/* Erase necessary blocks first */
	while (nblks > 0) 
	{
		if (flash_is_bad_block(ffblock) == FALSE)
		{
			flash_erase_block(ffblock);
			nblks--;
		}
		
		ffblock++;
	}

	/* Burn image into FLASH */
	sector = conv_to_sector(fblock, 0);
	if (mem_to_nand(sector, (UNS_8 *) sysinfo.lfile.loadaddr,
		(numsecs * sysinfo.nandgeom->data_bytes_per_page)) == FALSE)
	{
		term_dat_out_crlf(nsaeerr_msg);
	}
	else
	{
		/* Update saved NAND configuration */
		syscfg.fsave = flashsavdat;
		cfg_save(&syscfg);
	}

	return TRUE;
}

/***********************************************************************
 *
 * Function: flash_image_load
 *
 * Purpose: Moves FLASH application in load region to memory
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Always returns TRUE.
 *
 * Notes: None
 *
 **********************************************************************/
BOOL_32 flash_image_load(void) 
{
	UNS_32 fblock;
	INT_32 sector;

	if (syscfg.fsave.valid == FALSE) 
	{
		term_dat_out_crlf(noiif_msg);
		return TRUE;
	}

	/* Find starting block of burned image */
	fblock = sysinfo.sysrtcfg.bl_num_blks;

	/* Read data into memory */
	sector = conv_to_sector(fblock, 0);
	if (nand_to_mem(sector, (UNS_8 *) syscfg.fsave.loadaddr,
		(syscfg.fsave.secs_used *
		sysinfo.nandgeom->data_bytes_per_page)) == FALSE)
	{
		term_dat_out_crlf(nloeerr_msg);
	}
	else
	{
		/* Load image */
		sysinfo.lfile.num_bytes = syscfg.fsave.num_bytes;
		sysinfo.lfile.startaddr = (PFV) syscfg.fsave.startaddr;
		sysinfo.lfile.loadaddr = syscfg.fsave.loadaddr;
		sysinfo.lfile.flt = FLT_RAW;
		sysinfo.lfile.contiguous = TRUE;
		sysinfo.lfile.loaded = TRUE;
	}

	return TRUE;
}

/***********************************************************************
 *
 * Function: raw_load
 *
 * Purpose: Load a raw file from a source to memory
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     fdata : Pointer to file data to fill
 *     addr  : Address to load data
 *     filename : Filename (sd cards only)
 *     src   : Data source
 *
 * Outputs: None
 *
 * Returns: TRUE if the file was loaded, otherwise FALSE
 *
 * Notes: None
 *
 **********************************************************************/
BOOL_32 raw_load(FILE_DATA_T *fdata,
                 UNS_32 addr,
                 UNS_8 *filename,
                 SRC_LOAD_T src) 
{
	UNS_32 rb, bytes = 0;
	UNS_8 ch, *ptr8 = (UNS_8 *) addr;
	BOOL_32 readloop, loaded = FALSE;

	if (src == SRC_TERM) 
	{
		term_dat_out_crlf(rawdl_msg);

		/* Read data from terminal until a break is encountered */
		while (term_break() == FALSE) 
		{
			if (term_dat_in(&ch, 1) > 0) 
			{
				bytes++;
				*ptr8 = ch;
				ptr8++;
			}
		}

		fdata->num_bytes = bytes;
		fdata->contiguous = TRUE;
		loaded = TRUE;
	}
	else if (src == SRC_NAND) 
	{
		/* Move image in NAND to memory */
		term_dat_out_crlf(rawnanddlns_msg);

	}
	else if (src == SRC_BLKDEV) 
	{
		/* Initialize FAT interface first */
		if (fat_init() == FALSE)
		{
			term_dat_out(blkdeverr_msg);
		}
		/* Try to open file */
		else if (fat_file_open(filename) == FALSE) 
		{
			term_dat_out_crlf(nofilmsg);
		}
		else 
		{
			fdata->num_bytes = 0;
			readloop = TRUE;
			while (readloop == TRUE) 
			{
				rb = fat_file_read(ptr8, 8);
				fdata->num_bytes += rb;
				ptr8 += 8;
				if (rb != 8) 
				{
					readloop = FALSE;
				}
			}
			fdata->contiguous = TRUE;
			loaded = TRUE;
		}

		fat_deinit();
	}

	return loaded;
}

/***********************************************************************
 *
 * Function: srec_parse
 *
 * Purpose: Load and parse an S-record file
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     fdata    : Pointer to file data to fill
 *     src      : Data source
 *     filename : Filename (sd cards only)
 *
 * Outputs: None
 *
 * Returns: E if the file was parsed and loaded, otherwise FALSE
 *
 * Notes: None
 *
 **********************************************************************/
BOOL_32 srec_parse(FILE_DATA_T *ft,
                   SRC_LOAD_T src,
                   UNS_8 *filename) {
    UNS_8 line[384];
    int idx;
    BOOL_32 fop = FALSE, parsed = TRUE, done = FALSE;

    ft->loadaddr = 0xFFFFFFFF;
    ft->flt = FLT_RAW; /* Loaded as an S-record, saved as RAW */
    ft->num_bytes = 0;
    ft->startaddr = (PFV) 0xFFFFFFFF;
    ft->contiguous = TRUE;
    ft->loaded = FALSE;

	/* Attempt to open file if from SD source */
	if (src == SRC_BLKDEV) 
	{
		/* Init FAT filesystem and block device */
		if (fat_init() == FALSE)
		{
			term_dat_out(blkdeverr_msg);
			done = TRUE;
		}
		else
		{
			/* Try to open file */
			parsed = fat_file_open(filename);
			fop = TRUE;
			if (parsed == FALSE)
			{
				term_dat_out_crlf(nofilmsg);
				done = TRUE;
			}
		}
	}
	else if (src == SRC_NAND) 
	{
		/* Initialize NAND streamer */
		if (stream_flash_init() == FALSE) 
		{
			term_dat_out_crlf(noiif_msg);
			done = TRUE;
		}
	}
	else 
	{
		term_dat_out_crlf(rawdl_msg);
	}

    while ((done == FALSE) && (ft->loaded == FALSE)) 
    {
        /* Read line from device */
        parsed = readline(line, src);
        if (parsed == TRUE) 
        {
			/* Skip whitespace */
			idx = skip_whitespace(line, 0);

			parsed &= srec_parse_line(&line [idx], ft);
        }
		if (parsed == FALSE) 
		{
			done = TRUE;
		}
    }

	if (fop == TRUE)
	{
		fat_deinit();
	}

	return parsed;
}

/***********************************************************************
 *
 * Function: mem_to_nand
 *
 * Purpose: Moves data from memory to FLASH
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     starting_sector : Starting sector for write
 *     buff            : Pointer to buffer to write
 *     bytes           : Number of bytes to write
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
BOOL_32 mem_to_nand(UNS_32 starting_sector,
					void *buff,
					UNS_32 bytes)
{
	BOOL_32 blkchk;
	UNS_32 block, page, sector, wbytes;
	UNS_8 str[16], *p8 = (UNS_8 *) buff;

	if (sysinfo.nandgeom == NULL)
	{
		term_dat_out_crlf(noflash_msg);
		return FALSE;
	}

	/* Get block and page for passed sector */
	block = starting_sector / sysinfo.nandgeom->pages_per_block;
	page = starting_sector -
		(block * sysinfo.nandgeom->pages_per_block);

	/* Align bytes to a sector boundary */
	wbytes = bytes & ~(sysinfo.nandgeom->data_bytes_per_page - 1);
	if (wbytes < bytes)
	{
		bytes = wbytes + sysinfo.nandgeom->data_bytes_per_page;
	}

	/* Write data */
	blkchk = TRUE;
	while (bytes > 0)
	{
		/* Is a block check needed? */
		if ((page == 0) && (blkchk == TRUE))
		{
			if (flash_is_bad_block(block) != FALSE)
			{
				/* Goto next block */
				term_dat_out(bskip_msg);
				str_makedec(str, block);
				term_dat_out_crlf(str);
				block++;
				page = 0;
			}
			else
			{
				/* Block is good */
				blkchk = FALSE;
			}
		}
		else
		{
			/* Convert to sector */
			sector = conv_to_sector(block, page);

			/* Write sector data */
			if (flash_write_sector(sector, p8, NULL) == FALSE)
			{
				term_dat_out(writeerr_msg);
				str_makedec(str, sector);
				term_dat_out_crlf(str);
				bytes = 0;
				return FALSE;
			}
			else
			{
				p8 += sysinfo.nandgeom->data_bytes_per_page;
				bytes -= sysinfo.nandgeom->data_bytes_per_page;
			}

			/* Next page and block */
			page++;
			if (page >= sysinfo.nandgeom->pages_per_block)
			{
				page = 0;
				block++;
				blkchk = TRUE;
			}
		}
	}

	return TRUE;
}

/***********************************************************************
 *
 * Function: nand_to_mem
 *
 * Purpose: Moves data from FLASH to memory
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     starting_sector : Starting sector for read operation
 *     buff            : Buffer to place read data
 *     bytes           : Number of bytyes to read
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
BOOL_32 nand_to_mem(UNS_32 starting_sector,
					void *buff,
					UNS_32 bytes)
{
	BOOL_32 blkchk;
	UNS_32 block, page, sector, toread;
	UNS_8 str[16], *p8 = (UNS_8 *) buff;

	if (sysinfo.nandgeom == NULL)
	{
		term_dat_out_crlf(noflash_msg);
		return FALSE;
	}

	/* Get block and page for passed sector */
	block = starting_sector / sysinfo.nandgeom->pages_per_block;
	page = starting_sector -
		(block * sysinfo.nandgeom->pages_per_block);
	/* Read data */
	blkchk = TRUE;
	while (bytes > 0)
	{
		/* Is a block check needed? */
		if ((page == 0) && (blkchk == TRUE))
		{
			if (flash_is_bad_block(block) != FALSE)
			{
				/* Goto next block */
				term_dat_out(bskip_msg);
				str_makedec(str, block);
				term_dat_out_crlf(str);
				block++;
				page = 0;
				blkchk = TRUE;
			}
			else
			{
				blkchk = FALSE;
			}
		}
		else
		{
			/* Convert to sector */
			sector = conv_to_sector(block, page);

			toread = bytes;
			if (toread > sysinfo.nandgeom->data_bytes_per_page)
			{
				toread = sysinfo.nandgeom->data_bytes_per_page;
			}

			/* Read sector data */
			if (flash_read_sector(sector, secdat, NULL) <= 0)
			{
				term_dat_out(readerr_msg);
				str_makedec(str, sector);
				term_dat_out_crlf(str);
			}

			memcpy(p8, secdat, toread);
			p8 += toread;
			bytes -= toread;

			/* Next page and block */
			page++;
			if (page >= sysinfo.nandgeom->pages_per_block)
			{
				page = 0;
				block++;
				blkchk = TRUE;
			}
		}
	}

	return TRUE;
}
