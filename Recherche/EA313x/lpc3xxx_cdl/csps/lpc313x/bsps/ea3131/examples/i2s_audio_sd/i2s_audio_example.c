/***********************************************************************
 * $Id:: i2s_audio_example.c 8087 2011-09-14 04:13:00Z ing02124        $
 *
 * Project: I2S audio driver example using SD card
 *
 * Description:
 *     This example uses I2C to initialize the audio CODEC, sets up
 *     correct I2S clocking. This example uses circular mode DMA to
 *      - Play audio PCM wave file from SD card
 *      - Record audio stream & store in SD card as PCM wave file
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
#include <stdio.h>
#include <string.h>

#include "lpc_types.h"
#include "lpc_irq_fiq.h"
#include "lpc_arm922t_cp15_driver.h"
#include "ea3131_board.h"
#include "lpc313x_cgu_driver.h"
#include "lpc313x_i2s_driver.h"
#include "lpc313x_mstr_i2c_driver.h"
#include "lpc313x_dma_driver.h"
#include "uda1380_codec_driver.h"
#include "lpc313x_i2s_clock_driver.h"
#include "lpc313x_uart_driver.h"
#include "lpc313x_mci_driver.h"
#include "lpc31xx_vmem_driver.h"
#include "ff.h"
#include "diskio.h"
#include "lpc_wave.h"
#include "string.h"

/* Audio device structure */
typedef struct audio_dev
{
  UNS_8 *buffer[2];
  int buf_sz;
  int freq;
  int bitspersample;
  int numchan;
  volatile int active;
  volatile int xfer_cnt;
  volatile int done;
  FIL fptr;
  INT_32 dmach;
  volatile int buf_rdy;
} AUDIO_DEV_T;

/* Audio Buffer Size */
#define DMA_BUFFER_REC_HALF_SZ  (8 * 1024) /* 8K buffer */
#define DMA_BUFFER_PLAY_HALF_SZ (8 * 1024) /* 8K buffer */

/* UDA1380 CODEC I2C address */
#define CODEC_I2C_ADDR (0x1A)

/* Initial Sample rate */
#define LPC_PACMANVA_SRATE 11025
#define LPC_RECORDING_RATE 44100
#define WAVE_HEADER_SIZE (sizeof(RIFF_CHUNK_T) + \
                          sizeof(FORMAT_CHUNK_T) + sizeof(DATA_CHUNK_T))

/* Audio buffer Array */
static DMA_BUFFER UNS_8 dma_buff_play[DMA_BUFFER_PLAY_HALF_SZ * 2];
static DMA_BUFFER UNS_8 dma_buff_rec[DMA_BUFFER_REC_HALF_SZ * 2];

/* FATFS File system structure */
static FATFS Fatfs[1];

/* Directory structure */
static DIR dir;

/* File Info structure */
static FILINFO Finfo;

/* Audio device structures */
static AUDIO_DEV_T recording;
static AUDIO_DEV_T playback;

/* External functions */
extern DSTATUS disk_initialize(UNS_8);

/* UART data */
static INT_32 g_uartdev;
char uart_buff[512];

/***********************************************************************
 *
 * Function: uart_string_write
 *
 * Purpose: Write the string to UART
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *    @pbuff: Pointer to string buffer
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 **********************************************************************/
void uart_string_write(char *pbuff)
{
  UNS_32 len = strlen(pbuff);
  UNS_32 written;

  while (len > 0)
  {
    written = uart_write(g_uartdev, pbuff, len);
    len -= written;
    pbuff += written;
  }
}

/***********************************************************************
 *
 * Function: uart_string_read
 *
 * Purpose: Read from UART & store in string buffer
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *    @buff: Pointer to string buffer
 *    @size: Size of string buffer
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 **********************************************************************/
static void uart_string_read(char *buff, int len)
{
  char c;
  int i = 0;

  for (;;)
  {
    while (!(uart_read(g_uartdev, &c, 1)));

    if (c == '\r')
    {
      uart_write(g_uartdev, (void*)&c, 1);
      uart_string_write("\r\n");
      break;
    }

    if ((c == '\b') && i)
    {
      i--;
      uart_write(g_uartdev, (void*)&c, 1);
      continue;
    }

    if (c >= ' ' && i < (len - 1))
    {
      /* Visible chars */
      uart_write(g_uartdev, (void*)&c, 1);
      buff[i++] = c;
    }
  }

  buff[i] = 0;
}

/***********************************************************************
 *
 * Function: uart_getchar
 *
 * Purpose: Reads a single character from UART
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: -1 - If no characters available from UART
 *        : int converted ASCII value of received character
 *
 * Notes: This is a non blocking routine.
 *
 ***********************************************************************/
static int uart_getchar(void)
{
  UNS_8 ch;
  if (uart_read(g_uartdev, &ch, 1) > 0)
    return (int)ch;
  else
    return -1;
}

/***********************************************************************
 *
 * Function: get_fattime
 *
 * Purpose: Dummy time function for FATFS file system
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *    @pbuff: Pointer to string buffer
 *
 * Outputs: None
 *
 * Returns: Time value
 *
 * Notes: None
 *
 **********************************************************************/
DWORD get_fattime(void)
{
  DWORD tmr = 0;
  return tmr;
}

/***********************************************************************
 *
 * Function: sd_read_data
 *
 * Purpose: Read data from SD card, store in data buffer
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *    @fp: Pointer to FATFS file structure
 *    @buff: Pointer to data buffer
 *    @size: Data size to be read
 *
 * Outputs: None
 *
 * Returns: Time value
 *
 * Notes: None
 *
 **********************************************************************/
static STATUS sd_read_data(FIL *fp, UNS_8 *buf, UNS_32 size)
{
  UNS_8 *buf_ptr = buf;
  STATUS status = _NO_ERROR;
  UNS_32 bytes_read, tot_read = 0;
  int err = 0;

  /* Read data */
  while (tot_read < size)
  {
    err = f_read(fp, buf_ptr, size - tot_read, (UINT *) & bytes_read);
    if (err || (bytes_read == 0))
    {
      sprintf(uart_buff, "f_read error err: %d read: %d \r\n",
              err, (int)bytes_read);
      uart_string_write(uart_buff);
      status = _ERROR;
      break;
    }
    tot_read += bytes_read;
    buf_ptr += bytes_read;
  }

  return status;
}

/***********************************************************************
 *
 * Function: fill_buff
 *
 * Purpose: Fills 1/2 of the audio DMA buffer
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *    @fp: Pointer to FATFS file structure
 *    @index: Offset within buffer
 *
 * Outputs: None
 *
 * Returns: Returns 0
 *
 * Notes: None
 *
 **********************************************************************/
static INT_32 fill_buff(FIL *fp, int index)
{
  int sz = playback.xfer_cnt > playback.buf_sz ? playback.buf_sz :
           playback.xfer_cnt;

  /* If Playback done, return */
  if (playback.done > 0)
  {
    playback.done++;
    return 0;
  }

  /* Read from SD card */
  if (sd_read_data(fp, playback.buffer[index], sz)
      == _ERROR)
  {
    playback.done = -1;
  }

  /* Pad data if required */
  if (sz < playback.buf_sz)
    memset(&playback.buffer[index][sz], 0, playback.buf_sz - sz);

  /* Flush cache */
  lpc31xx_flush_dcache_range((UNS_32 *)playback.buffer[index],
                             (UNS_32 *)((UNS_32)playback.buffer[index] + sz));

  /* Decrement data size */
  playback.xfer_cnt -= sz;
  if (playback.xfer_cnt <= 0)
    playback.done = 1;

  return 0;
}

/***********************************************************************
 *
 * Function: dma_play_callback
 *
 * Purpose: Audio DMA Playback callback function
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *    None
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 **********************************************************************/
static void dma_play_callback(INT_32 ch, DMA_IRQ_TYPE_T itype, 
    void *pdmaregs)
{
  INT_32 dmach = 2 * playback.dmach;
  DMAC_REGS_T *dmaregs = dma_get_base();
  volatile UNS_32 irq_status = dmaregs->irq_status_clear & (0x3 << dmach);

  if (irq_status & (0x3 << dmach))
  {
    if (irq_status & (2 << dmach))
    {
      /* Fill first half of buffer */
      playback.buf_rdy |= _BIT(0);
    }

    if (!playback.done && (irq_status & (1 << dmach)))
    {
      /* Fill 2nd half of buffer */
      playback.buf_rdy |= _BIT(1);
    }

    dmaregs->irq_status_clear = irq_status;
  }

  return;
}

/***********************************************************************
 *
 * Function: lpc3131_start_playback
 *
 * Purpose: Start Audio playback function
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *    None
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 **********************************************************************/
static void lpc3131_start_playback(void)
{
  INT_32 dmach;
  DMAC_REGS_T *dmaregs = dma_get_base();

  /* Allocate a DMA channel for playback */
  playback.dmach = dma_alloc_channel(-1, dma_play_callback, (void *)dmaregs);
  dmach = playback.dmach;

  /* Enable audio CODEC output and max volume */
  uda1380_enable_audio_out(1);

  /* Set CODEC volume */
  uda1380_set_volume(0, 0);

  /* Unmute CODEC */
  uda1380_mute(0);

  /* Set I2S Clocks for Playback */
  lpc313x_main_clk_rate(playback.freq);
  lpc313x_chan_clk_enable(CLK_TX_1, playback.freq,
                          (playback.freq * playback.bitspersample * playback.numchan));
  cgu_clk_en_dis(CGU_SB_I2S_CFG_PCLK_ID, 1);
  cgu_clk_en_dis(CGU_SB_EDGE_DET_PCLK_ID, 1);

  /* Setup DMA channel and circular buffer */
  dmaregs->channel[dmach].source = (UNS_32) lpc31xx_va_to_pa(playback.buffer[0]);
  if (playback.numchan == 2)
    dmaregs->channel[dmach].destination = 0x16000160;
  else
    dmaregs->channel[dmach].destination = 0x16000120;
  dmaregs->channel[dmach].length = ((playback.buf_sz * 2) / sizeof(UNS_32)) - 1;
  dmaregs->channel[dmach].config = (DMA_CFG_CIRC_BUF | DMA_CFG_TX_WORD |
                                    DMA_CFG_RD_SLV_NR(0) | DMA_CFG_WR_SLV_NR(9));

  /* enable DMA channel interrupts and start DMA */
  dmaregs->irq_mask &= ~(0x3 << (2 * dmach));
  playback.done = 0;
  playback.active = 1;
  dmaregs->channel[dmach].enable = 1;

  return;
}

/***********************************************************************
 *
 * Function: lpc3131_stop_playback
 *
 * Purpose: Stop Audio Playback function
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *    None
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 **********************************************************************/
static void lpc3131_stop_playback(void)
{
  DMAC_REGS_T *dmaregs = dma_get_base();

  playback.active = 0;
  playback.done = 0;
  playback.buf_rdy = 0;

  /* Disable DMA channel, DMA interrupts */
  dmaregs->channel[playback.dmach].enable = 0;
  dmaregs->irq_mask |= (0x3 << (2 * playback.dmach));

  /* Mute Audio CODEC */
  uda1380_mute(1);

  /* Disbale Audio output */
  uda1380_enable_audio_out(0);

  /* Disable TX1, I2S clocks */
  lpc313x_chan_clk_enable(CLK_TX_1, 0, 0);
  cgu_clk_en_dis(CGU_SB_I2S_CFG_PCLK_ID, 0);
  cgu_clk_en_dis(CGU_SB_EDGE_DET_PCLK_ID, 0);

  /* Free DMA channel */
  dma_free_channel(playback.dmach);

  return;
}

/***********************************************************************
 *
 * Function: sd_write_data
 *
 * Purpose: Write data to SD card function
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *      @fp: Pointer to FATFS File structure
 *      @buf: Pointer to data buffer
 *      @size: Size of data to be written
 *
 * Outputs: None
 *
 * Returns: _ERROR: Error
 *          _NO_ERROR: Success
 *
 * Notes: None
 *
 **********************************************************************/
static STATUS sd_write_data(FIL *fp, const UNS_8 *buf, UNS_32 size)
{
  int err;
  int to_write = (int)size, written;

  /* Write to SD card */
  while (to_write > 0)
  {
    err = f_write(fp, buf, to_write, (UINT *) & written);
    if (err || !written)
    {
      f_close(fp);
      uart_string_write("File write error!\r\n");
      return _ERROR;
    }

    buf += written;
    to_write -= written;
  }

  return _NO_ERROR;
}

/***********************************************************************
 *
 * Function: list_files
 *
 * Purpose: List files on SD card
 *
 * Processing:
 *     List the files & directories present on SD card
 *
 * Parameters:
 *    pdmaregs: Pointer to Path
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 **********************************************************************/
static void list_files(char* path)
{
  FRESULT res;
  long p1;
  UINT s1, s2;
  FATFS *fs;

  /* Open directory */
  res = f_opendir(&dir, path);
  if (res != FR_OK)
  {
    uart_string_write("Unable to open directory \r\n");
    return;
  }

  /* Parse directory  & files */
  p1 = s1 = s2 = 0;
  for (;;)
  {
    res = f_readdir(&dir, &Finfo);
    if ((res != FR_OK) || !Finfo.fname[0])
    {
      break;
    }

    if (Finfo.fattrib & AM_DIR)
    {
      s2++;
    }
    else
    {
      s1++;
      p1 += Finfo.fsize;
    }

    sprintf(uart_buff, "\r%c%c%c%c%c  %u/%02u/%02u  %02u:%02u %9lu  %s \r\n",
            (Finfo.fattrib & AM_DIR) ? 'D' : '-',
            (Finfo.fattrib & AM_RDO) ? 'R' : '-',
            (Finfo.fattrib & AM_HID) ? 'H' : '-',
            (Finfo.fattrib & AM_SYS) ? 'S' : '-',
            (Finfo.fattrib & AM_ARC) ? 'A' : '-',
            (Finfo.fdate >> 9) + 1980, (Finfo.fdate >> 5) & 15, Finfo.fdate & 31,
            (Finfo.ftime >> 11), (Finfo.ftime >> 5) & 63,
            Finfo.fsize, &(Finfo.fname[0]));
    uart_string_write(uart_buff);
    uart_string_write("\n");
  }

  sprintf(uart_buff, "\r%4u File(s),%10lu bytes total\n\r%4u Dir(s)", s1, p1, s2);
  uart_string_write(uart_buff);

  if (f_getfree("/", (DWORD*)&p1, &fs) == FR_OK)
  {
    sprintf(uart_buff, ",%10luK bytes free \r\n", p1 * fs->csize / 2);
    uart_string_write(uart_buff);
  }
}

/***********************************************************************
 *
 * Function: sd_list_files
 *
 * Purpose: List files on SD card
 *
 * Processing:
 *     Initialise SD card, mount the disk, & list the files &
 *		 directories present on SD card
 *
 * Parameters:
 *    None
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 **********************************************************************/
static void sd_list_files(void)
{
  int err = 0;

  /* Initialise the Disk */
  err = disk_initialize(0);
  if (err & STA_NODISK)
  {
    uart_string_write("SD card not found \r\n");
    uart_string_write("Insert SD card \r\n");
    return;
  }

  /* Mount the disk */
  if (FR_OK != f_mount(0, &Fatfs[0]))
  {
    sprintf(uart_buff, "Could not mount volume !!! error = %d \r\n", err);
    uart_string_write(uart_buff);
    return;
  }

  /* List Files in SD */
  list_files("");

  return;
}

/***********************************************************************
 *
 * Function: sd_read_riff_header
 *
 * Purpose: Read RIFF header in WAVE file
 *
 * Processing:
 *     Read RIFF header from WAVE file & check if valid WAVE file
 *
 * Parameters:
 *    @fp: Pointer to FATFS File structure
 *    @filesize: Size of the file
 *
 * Outputs: None
 *
 * Returns:
 *    _ERROR: Not valid WAVE PCM file
 *    _NO_ERROR: Valid WAVE PCM file
 *
 * Notes: None
 *
 **********************************************************************/
STATUS sd_read_riff_header(FIL *fp, UNS_32 *filesize)
{
  UNS_8 *buf;
  BOOL_32 found;
  RIFF_CHUNK_T riff;
  STATUS status = _NO_ERROR;

  /* Read RIFF header from file */
  buf = playback.buffer[0];
  status = sd_read_data(fp, buf, sizeof(RIFF_CHUNK_T));
  if (status == _ERROR)
  {
    uart_string_write("Wrror in reading RIFF chunk \r\n");
    status = _ERROR;
  }

  /* Copy to riff structure */
  memcpy((void *)&riff, buf, sizeof(RIFF_CHUNK_T));

  /* Validate RIFF header */
  if (status == _NO_ERROR)
  {
    /* Check RIFF header is OK */
    found = lpc_check_riff_chunk(&riff);
    if (found == FALSE)
    {
      status = _ERROR;
    }
    else
    {
      /* Find Total size of File */
      *filesize = riff.chunksize;

      /* Reduce RIFF header from Total File size */
      *filesize -= 4;
    }
  }

  return status;
}

/***********************************************************************
 *
 * Function: sd_play_file
 *
 * Purpose: Playback file from SD card
 *
 * Processing:
 *     Read Format chunk(s) from WAVE file & check if valid. Read Data
 *     chunk(s) from WAVE file & stream onto Audio interface
 *
 * Parameters:
 *    @fp: Pointer to FATFS File structure
 *    @fsize: Size of the file
 *
 * Outputs: None
 *
 * Returns:
 *    _ERROR: Not successful
 *    _NO_ERROR: Successfull
 *
 * Notes: None
 *
 **********************************************************************/
STATUS sd_play_file(FIL *fp, UNS_32 fsize)
{
  UNS_8 *buf;
  DATA_CHUNK_T dat;
  FORMAT_CHUNK_T cur_fmt, format;
  CHUNK_TYPE type;
  STATUS status = _NO_ERROR;
  int ch;

  /* Store current Format */
  memset((void *)&cur_fmt, 0xFF, sizeof(FORMAT_CHUNK_T));

  while (fsize)
  {
    /* Read Chunk data from file */
    buf = playback.buffer[0];
    status = sd_read_data(fp, buf, sizeof(DATA_CHUNK_T));
    if (status == _ERROR)
    {
      uart_string_write("Error in reading chunk \r\n");
      return _ERROR;
    }

    fsize -= sizeof(DATA_CHUNK_T);
    memcpy((void *)&dat, buf, sizeof(DATA_CHUNK_T));

    /* Check Chunk Type */
    status = lpc_check_chunk_type(&dat, &type);
    if (status == _ERROR)
    {
      uart_string_write("Invalid Chunk Type \r\n");
      return _ERROR;
    }

    /* If Format chunk, Read Audio file info */
    if (type == FORMAT_CHUNK)
    {
      /* Read Format Chunk from file */
      status = sd_read_data(fp, buf, dat.chunksize);
      if (status == _ERROR)
      {
        uart_string_write("Error in reading Format chunk \r\n");
        return _ERROR;
      }
      fsize -= dat.chunksize;
      memcpy((void *)&format.wformattag, buf, dat.chunksize);

      /* Check if WAVE PCM file */
      status = lpc_check_format_chunk(&format);
      if (status == _ERROR)
      {
        uart_string_write("Invalid Format! \r\n");
        return _ERROR;
      }
    }
    else if (type == DATA_CHUNK)
    {
      int need_start = 0;

      /* Change the Audio settings if changed */
      if ((cur_fmt.wchannels != format.wchannels) ||
          (cur_fmt.samplessec != format.samplessec) ||
          (cur_fmt.bytespersec != format.bytespersec) ||
          (cur_fmt.blockalign != format.blockalign) ||
          (cur_fmt.bitssample != format.bitssample))
      {
        /* Copy Format */
        memcpy((void *)&cur_fmt, (void *)&format,
               sizeof(FORMAT_CHUNK_T));

        /* Print WAVE File Info */
        uart_string_write("----------------------\r\n");
        uart_string_write("WAVE FILE INFO: \r\n");
        if (cur_fmt.wchannels == 1)
        {
          uart_string_write(" MONO \r\n");
        }
        else
        {
          uart_string_write(" STEREO \r\n");
        }
        sprintf(uart_buff, "Sample Rate: %lu \r\n", cur_fmt.samplessec);
        uart_string_write(uart_buff);
        sprintf(uart_buff, "Bits Per Sample:  %d \r\n", cur_fmt.bitssample);
        uart_string_write(uart_buff);
        sprintf(uart_buff, "Bytes Per Sec:  %lu \r\n", cur_fmt.bytespersec);
        uart_string_write(uart_buff);
        sprintf(uart_buff, "Block align:  %d \r\n", cur_fmt.blockalign);
        uart_string_write(uart_buff);
        sprintf(uart_buff, "File size:  %d \r\n", (int) dat.chunksize);
        uart_string_write(uart_buff);
        uart_string_write("----------------------\r\n");

        playback.freq = cur_fmt.samplessec;
        playback.bitspersample = cur_fmt.bitssample;
        playback.numchan = cur_fmt.wchannels;
        need_start = 1;
      }

      /* DMA size = Current Chunk size */
      playback.xfer_cnt = dat.chunksize;

      /* Pre-fill audio DMA buffers */
      fill_buff(&playback.fptr, 0);
      fill_buff(&playback.fptr, 1);

      /* Start Audio playback */
      if (need_start)
      {
        lpc3131_start_playback();
      }

      uart_string_write("Starting playback\r\n");
      uart_string_write("Press 's' to stop playing\r\n");
      do
      {
        if (playback.buf_rdy & _BIT(0))
        {
          playback.buf_rdy &= ~_BIT(0);
          fill_buff(&playback.fptr, 0);
        }
        else if (playback.buf_rdy & _BIT(1))
        {
          playback.buf_rdy &= ~_BIT(1);
          fill_buff(&playback.fptr, 1);
        }

        ch = uart_getchar();
      }
      while (!playback.done && (ch != 'S' && ch != 's'));

      if (playback.done < 0)
      {
        uart_string_write("Read error\r\n");
        break;
      }

      if (!playback.done)
      {
        break;
      }

      fsize -= dat.chunksize;
    }
  }

  lpc3131_stop_playback();

  return status;
}

/***********************************************************************
 *
 * Function: sd_write_riff_header
 *
 * Purpose: Write RIFF header in WAVE file
 *
 * Processing:
 *     Write RIFF header from WAVE file & check if valid WAVE file
 *
 * Parameters:
 *    @fp: Pointer to FATFS File structure
 *
 * Outputs: None
 *
 * Returns:
 *    _ERROR: Not valid WAVE PCM file
 *    _NO_ERROR: Valid WAVE PCM file
 *
 * Notes: None
 *
 **********************************************************************/
static STATUS sd_write_riff_header(FIL *fp)
{
  RIFF_CHUNK_T rc;
  FORMAT_CHUNK_T fc;
  DATA_CHUNK_T dc;

  if (f_lseek(fp, 0))
  {
    uart_string_write("Unable to seek to file header area\r\n");
    return _ERROR;
  }

  memcpy(rc.id, "RIFF", 4);
  memcpy(rc.waveid, "WAVE", 4);
  rc.chunksize = recording.xfer_cnt + WAVE_HEADER_SIZE - 8;
  if (sd_write_data(fp, (UNS_8 *)&rc, sizeof(RIFF_CHUNK_T))
      == _ERROR)
  {
    uart_string_write("Unable to write RIFF header\r\n");
    return _ERROR;
  }

  memcpy(fc.id, "fmt ", 4);
  fc.chunksize = 16;
  fc.wformattag = 1;
  fc.wchannels = 2;
  fc.samplessec = recording.freq;
  fc.bytespersec = recording.freq * 4;
  fc.blockalign = 4;
  fc.bitssample = 16;
  if (sd_write_data(fp, (UNS_8 *)&fc, sizeof(FORMAT_CHUNK_T)) == _ERROR)
  {
    uart_string_write("Unable to write Format header\r\n");
    return _ERROR;
  }
  memcpy(dc.id, "data", 4);
  dc.chunksize = recording.xfer_cnt;
  if (sd_write_data(fp, (UNS_8 *)&dc, sizeof(DATA_CHUNK_T)) == _ERROR)
  {
    uart_string_write("Unable to write data header\r\n");
    return _ERROR;
  }
  return _NO_ERROR;
}

/***********************************************************************
 *
 * Function: dma_rec_callback
 *
 * Purpose: Audio DMA recording handler callback
 *
 * Processing:
 *              This function is called from the DMA ISR to notify
 *              the application about half done and done.
 *
 * Parameters:
 *    pdmaregs: Pointer to DMA registers
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 **********************************************************************/
static void dma_rec_callback(INT_32 ch, DMA_IRQ_TYPE_T itype, 
    void *pdmaregs)
{
  DMAC_REGS_T *dmaregs = dma_get_base();
  INT_32 dmach = 2 * recording.dmach;
  UNS_32 irqmask = dmaregs->irq_status_clear & (0x3 << dmach);

  //if (irqmask == (2 << dmach))
  if (itype == DMA_IRQ_HALFWAY)
  {
    recording.buf_rdy |= _BIT(0);
    recording.xfer_cnt += recording.buf_sz;
  }
  //if (irqmask == (1 << dmach))
  if (itype == DMA_IRQ_FINISHED)
  {
    recording.buf_rdy |= _BIT(1);
    recording.xfer_cnt += recording.buf_sz;
  }

  dmaregs->irq_status_clear = irqmask;
}

/***********************************************************************
 *
 * Function: lpc3131_start_recording
 *
 * Purpose: Start Audio recording function
 *
 * Processing:
 *    See function
 *
 * Parameters:
 *     None
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 **********************************************************************/
static void lpc3131_start_recording(void)
{
  INT_32 dmach;
  DMAC_REGS_T *dmaregs = dma_get_base();
  recording.dmach = dma_alloc_channel(-1, dma_rec_callback, (void *)dmaregs);

  dmach = recording.dmach;
  *(UNS_32 *) 0x16000004 = 4;

  /* Enable RX1 clocks */
  lpc313x_main_clk_rate(recording.freq);
  lpc313x_chan_clk_enable(CLK_RX_1, recording.freq,
                          (recording.freq * 32));

  /* Enable CODEC Audio input */
  uda1380_enable_audio_in(1);

  /* Set Audio input volume */
  uda1380_set_volume(0x00, 0x00);

  /* Unmute Audio input */
  uda1380_mute(0);

  /* Setup DMA channel and circular buffer */
  dmaregs->channel[dmach].destination = (UNS_32) lpc31xx_va_to_pa(recording.buffer[0]);
  dmaregs->channel[dmach].source = 0x16000260;
  dmaregs->channel[dmach].length = ((recording.buf_sz * 2) / sizeof(UNS_32)) - 1;
  dmaregs->channel[dmach].config = (DMA_CFG_CIRC_BUF | DMA_CFG_TX_WORD |
                                    DMA_CFG_RD_SLV_NR(13) | DMA_CFG_WR_SLV_NR(0));

  /* Enable DMA channel interrupts and start DMA */
  dmaregs->irq_mask &= ~(0x3 << (2 * dmach));
  recording.active = 1;
  recording.done = 0;
  recording.xfer_cnt = 0;

  /* Start DMA */
  dmaregs->channel[dmach].enable = 1;

  return;
}

/***********************************************************************
 *
 * Function: lpc3131_stop_recording
 *
 * Purpose: Stop Audio recording function
 *
 * Processing:
 *    See function
 *
 * Parameters:
 *     None
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 **********************************************************************/
static void lpc3131_stop_recording(void)
{
  DMAC_REGS_T *dmaregs = dma_get_base();
  recording.active = 0;

  /* Disable DMA channel, DMA interrupts */
  dmaregs->channel[recording.dmach].enable = 0;
  dmaregs->irq_mask |= (0x3 << (2 * recording.dmach));

  /* Disbale Audio CODEC input */
  uda1380_enable_audio_in(0);

  /* Disable RX1 clocks */
  lpc313x_chan_clk_enable(CLK_RX_1, 0, 0);

  /* Free the DMA channel */
  dma_free_channel(recording.dmach);

  return;
}

/***********************************************************************
 *
 * Function: sd_audio_record
 *
 * Purpose: Record audio to file on SD card
 *
 * Processing:
 *     Create a WAVE file and record audio into it
 *
 * Parameters:
 *    @filename: File name array
 *
 * Outputs: None
 *
 * Returns:
 *    None
 *
 * Notes: None
 *
 **********************************************************************/
static void sd_audio_record(char *filename)
{
  int err;
  FIL *fp = &recording.fptr;

  f_unlink(filename);
  err = f_open(fp, filename, (FA_CREATE_ALWAYS | FA_WRITE));
  if (err)
  {
    uart_string_write("f_open Failed \r\n");
    sprintf(uart_buff, "File name: %s \r\n", filename);
    uart_string_write(uart_buff);
    return ;
  }

  if (f_lseek(fp, WAVE_HEADER_SIZE))
  {
    uart_string_write("lseek failed on record file\r\n");
    f_close(fp);
    return ;
  }

  uart_string_write("Starting Recording...\r\n");
  lpc3131_start_recording();
  uart_string_write("Press 's' to stop recording\r\n");
  do
  {
    if (recording.done < 0)
    {
      uart_string_write("Error recording!\r\n");
      lpc3131_stop_recording();
      f_close(fp);
      f_unlink(filename);
      return;
    }

    if (recording.buf_rdy & _BIT(0))
    {
      recording.buf_rdy &= ~_BIT(0);

      /* Invalidate Data cache */
      lpc31xx_invalidate_dcache_range((UNS_32 *)recording.buffer[0],
                                      (UNS_32 *)((UNS_32)recording.buffer[0] + recording.buf_sz));

      /* Write to SD card */
      err = sd_write_data(&recording.fptr,
                          recording.buffer[0], recording.buf_sz);
      if (err)
        recording.done = -1;
    }
    else if (recording.buf_rdy & _BIT(1))
    {
      recording.buf_rdy &= ~_BIT(1);

      /* Invalidate Data cache */
      lpc31xx_invalidate_dcache_range((UNS_32 *)recording.buffer[1],
                                      (UNS_32 *)((UNS_32)recording.buffer[1] + recording.buf_sz));

      /* Write to SD card */
      err = sd_write_data(&recording.fptr,
                          recording.buffer[1], recording.buf_sz);
      if (err)
        recording.done = -1;
    }

    err = uart_getchar();
  }
  while (err != 'S' && err != 's');

  recording.done = 1;
  lpc3131_stop_recording();

  sd_write_riff_header(fp);
  f_close(fp);

  uart_string_write("Audio recording complete! \r\n");
}

/***********************************************************************
 *
 * Function: sd_playback
 *
 * Purpose: Playback file from SD card
 *
 * Processing:
 *     Open WAVE file on SD card, check if it is valid WAVE file.
 *     Playback file on Audio interface
 *
 * Parameters:
 *    @filename: File name array
 *
 * Outputs: None
 *
 * Returns:
 *    None
 *
 * Notes: None
 *
 **********************************************************************/
static void sd_playback(char *filename)
{
  STATUS status;
  FIL *fp = &playback.fptr;
  UNS_32 fsize;
  int err = 0;

  /* Open the file */
  err = f_open(fp, filename, (FA_OPEN_EXISTING | FA_READ));
  if (err)
  {
    uart_string_write("f_open Failed \r\n");
    sprintf(uart_buff, "File name: %s \r\n", filename);
    uart_string_write(uart_buff);
    goto out2;
  }

  /* Read Wave file headers */
  status = sd_read_riff_header(fp, &fsize);
  if (status == _ERROR)
  {
    uart_string_write("sd_read_wave_header Failed \r\n");
    goto out1;
  }

  /* Playback the file */
  status = sd_play_file(fp, fsize);
  if (status == _ERROR)
  {
    uart_string_write("sd_play_file Failed \r\n");
  }

	uart_string_write("Audio playback complete! \r\n");

out1:
  f_close(fp);
out2:
  return;
}

/***********************************************************************
 *
 * Function: lpc313x_audio_init
 *
 * Purpose: Initialise LPC315X Audio clocks, I2C function
 *
 * Processing:
 *     See function
 *
 * Parameters:
 *    None
 *
 * Outputs: None
 *
 * Returns:
 *           0: Success
 *           !0: Error
 *
 * Notes: None
 *
 **********************************************************************/
static int lpc3131_audio_init(void)
{
  INT_32 i2cdev = 0, i2sTxdev, i2sRxdev;

  /* install default I2C0 & I2C1 interrupt handlers */
  int_install_irq_handler(IRQ_I2C1, (PFV)i2c1_mstr_int_hanlder);

  /* Enable IRQ interrupts in the ARM core */
  enable_irq();

  // HACK! required to connect 256FS fractional clock
  * (UNS_32 *) 0x130044dc = 5;

  /* I2S requires various clocks set at specific rates to get the
     target frequency. The I2S clocking driver will generate the
     correct rates for each clock group */
  lpc313x_set_fsmult_rate(128, CLK_USE_256FS);
  lpc313x_main_clk_rate(LPC_RECORDING_RATE);
  lpc313x_chan_clk_enable(CLK_TX_1, LPC_RECORDING_RATE,
                          (LPC_RECORDING_RATE * 32));

  /* Enable SYSCLK_O */
  cgu_clk_en_dis(CGU_SB_SYSCLK_O_ID, TRUE);

  /* Enable I2C system clock */
  cgu_clk_en_dis(CGU_SB_I2C1_PCLK_ID, TRUE);

  /* Enable I2C1 interrupt */
  int_enable(IRQ_I2C1);

  /* Initialise DMAC */
  dma_init();

  /* Setup CODEC driver */
  if (uda1380_driver_init(i2cdev, CODEC_I2C_ADDR) < 0)
  {
    uart_string_write("UDA1380 CODEC init failed!\r\n");
    return 1;
  }

  /* Open I2S TX1, set format */
  i2sTxdev = i2s_open(I2S_TX1, (PFV) NULL);
  if (i2sTxdev == 0)
  {
    uda1380_shutdown();
    uart_string_write("I2S_TX1 open error!\r\n");
    return 2;
  }

  /* set I2S format for I2S TX1 */
  i2s_ioctl(i2sTxdev, I2S_FORMAT_SETTINGS, I2S);

  /* Open the I2S RX1, set format */
  i2sRxdev = i2s_open(I2S_RX1, (PFV) NULL);
  if (i2sRxdev == 0)
  {
    uda1380_shutdown();
    uart_string_write("I2S_RX1 open error \r\n");
    return 3;
  }

  /* set I2S format for I2S TX - default */
  i2s_ioctl(i2sRxdev, I2S_FORMAT_SETTINGS, I2S);

  return 0;
}

/***********************************************************************
 *
 * Function: c_entry
 *
 * Purpose: Application entry point from the startup code
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Always returns 1
 *
 * Notes: None
 *
 **********************************************************************/
int c_entry(void)
{
  int ret = _ERROR;
  volatile UNS_32 break_out = 0;

  /* Disable interrupts in ARM core */
  disable_irq_fiq();

  /* Setup SD card detect */
  mci_init();

  /* Set virtual address of MMU table */
  cp15_set_vmmu_addr((void *)ISROM_MMU_TTB_BASE);

  /* Set I2S MUX Settings - RX1 as Master */
  *(UNS_32 *) 0x16000004 = 4;

  /* Initialize interrupt system */
  int_initialize(0xFFFFFFFF);

  /* Initialise UART */
  g_uartdev = uart_open((void *) UART_BASE, 0);
  if (g_uartdev == 0)
  {
    return 1;
  }

  /* Initialise LPC315X Audio setup */
  if (lpc3131_audio_init())
  {
    uart_string_write("Error initializing audio setup!\r\n");
    return 4;
  }

  /* Initliase Rec & Playback device structure */
  recording.buffer[0] = &dma_buff_rec[0];
  recording.buffer[1] = &dma_buff_rec[DMA_BUFFER_REC_HALF_SZ];
  recording.buf_sz = DMA_BUFFER_REC_HALF_SZ;
  recording.freq = LPC_RECORDING_RATE;
  recording.bitspersample = 16;
  recording.numchan = 2;
  playback.buffer[0] = &dma_buff_play[0];
  playback.buffer[1] = &dma_buff_play[DMA_BUFFER_PLAY_HALF_SZ];
  playback.buf_sz = DMA_BUFFER_PLAY_HALF_SZ;

  /* Demo */
  break_out = 0;
  while (!break_out)
  {
    uart_string_write("------------------------ \r\n");
    uart_string_write("AUDIO DEMO OPTIONS \r\n");
    uart_string_write("------------------------ \r\n");
    uart_string_write("Select 'l': List Files on SD card \r\n");
    uart_string_write("Select 'p': Select File for Playback \r\n");
    uart_string_write("Select 'r': Record to SD card \r\n");
    uart_string_write("Select 'x': Exit Demo \r\n");
    uart_string_write("------------------------ \r\n");
    uart_string_write("Select option: ");
    uart_string_read(uart_buff, sizeof(uart_buff));

    switch (uart_buff[0])
    {
      case 'l':
      case 'L':
        sd_list_files();
        break;
      case 'p':
      case 'P':
        uart_string_write("Type File name:  ");
        uart_string_read(uart_buff, sizeof(uart_buff));
        sd_playback(uart_buff);
        break;
      case 'r':
      case 'R':
        if (1)
        {
          char fname[60];
          uart_string_write("Type File name:  ");
          uart_string_read(fname, sizeof(fname));
          sd_audio_record(fname);
        }
        break;

      case 'x':
      case 'X':
        break_out = 1;
        break;
    }
  }

  ret = 0;

  /* Shutdown CODEC */
  uda1380_shutdown();

  uart_string_write("------------------------ \r\n");
  uart_string_write("End of AUDIO DEMO    \r\n");
  uart_string_write("------------------------ \r\n");

  return ret;
}
