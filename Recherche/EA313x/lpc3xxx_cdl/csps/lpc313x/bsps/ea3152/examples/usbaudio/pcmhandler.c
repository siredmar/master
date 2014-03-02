/***********************************************************************
 * $Id:: pcmhandler.c 8087 2011-09-14 04:13:00Z ing02124                     $
 *
 * Project: PCM handler for usb audio device
 *
 * Description:
 *     Handles PCM events from CODEC and I2S.
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
#include <string.h>
#include <stdio.h>
#include "lpc313x_cgu_driver.h"
#include "lpc313x_dma_driver.h"
#include "lpc313x_i2s_clock_driver.h"
#include "lpc_irq_fiq.h"

/* Codec driver */
#include "lpc3152_codec_driver.h"
#include "lpc313x_mstr_i2c_driver.h"

/* Local includes */
#include "pcmhandler.h"
#include "dbg.h"

/* Allocate memory only if it is very small */
#if !defined(TGT_SDRAM) && (PCM_BUFFER_SZ(PCM_MAX_SAMPLING_RATE) > (20 * 1024))
struct pcm_info pcm_info[PCM_DEVICE_CNT] = {{SDRAM_PLAY_BUFFER, }, {SDRAM_REC_BUFFER, }};
#else
static DMA_BUFFER UNS_8 play_buf[PCM_BUFFER_SZ(PCM_MAX_SAMPLING_RATE)];
static DMA_BUFFER UNS_8 rec_buf[PCM_BUFFER_SZ(PCM_MAX_SAMPLING_RATE)];
struct pcm_info pcm_info[PCM_DEVICE_CNT] = {{play_buf, }, {rec_buf, }};
#endif

#if defined(DEBUG) && defined(TGT_SDRAM)
unsigned char *audbuf = (unsigned char *) 0x31200000;
unsigned volatile int acnt;
#endif

/***********************************************************************
 *
 * Function: audio_dma_playback
 *
 * Purpose:  DMA Call back function for Playback Event
 *
 * Processing:
 *     See function.
 *
 * Parameters: @pdmaregs: Pointer to DMA Register structure
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 **********************************************************************/
static void audio_dma_playback(INT_32 ch, DMA_IRQ_TYPE_T itype, void *pdmaregs)
{
  INT_32 dmach = playback->dmach * 2;
  DMAC_REGS_T *dmaregs = dma_get_base();
  UNS_32 irqmask = dmaregs->irq_status_clear & (0x3 << dmach);
  int len = dmaregs->channel[playback->dmach].length & 0x1FFFFF;

  len = (len + 1) * sizeof(UNS_32);
  if (irqmask == (2 << dmach))
  {
    if (!playback->outcnt)
      playback->incnt = len / 2;
    playback->outcnt += len / 2;
  }
  if ((irqmask == (1 << dmach)))
  {
    if (!playback->outcnt)
      playback->incnt = len / 2;
    playback->outcnt += len / 2;
  }
  if (!playback->active)
    playback->done = 1;

  dmaregs->irq_status_clear = irqmask;
}

/***********************************************************************
 *
 * Function: audio_dma_recording
 *
 * Purpose:  DMA Call back function for Recording Event
 *
 * Processing:
 *     See function.
 *
 * Parameters: @pdmaregs: Pointer to DMA Register structure
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 **********************************************************************/
static void audio_dma_recording(INT_32 ch, DMA_IRQ_TYPE_T itype, 
    void *pdmaregs)
{
  int bno = 0;
  INT_32 dmach = recording->dmach * 2;
  DMAC_REGS_T *dmaregs = dma_get_base();
  UNS_32 irqmask = dmaregs->irq_status_clear & (0x3 << dmach);
  int len = dmaregs->channel[recording->dmach].length & 0x1FFFFF;

  len = (len + 1) * sizeof(UNS_32);
  if (irqmask == (2 << dmach))
  {
    bno = 1;
  }
  if ((irqmask == (1 << dmach)))
  {
    bno = 2;
  }
  if (bno)
  {
    recording->incnt += len / 2;
    if (!recording->active)
    {
      recording->active = 1;
      recording->outcnt = recording->incnt = 0;
    }
    if (recording->done)
      recording->tail = (bno - 1) * (len / 2);
  }

  dmaregs->irq_status_clear = irqmask;
}

/***********************************************************************
 *
 * Function: pcm_start_dma
 *
 * Purpose:  Start the Circular buffer DMA function
 *
 * Processing:
 *     See function.
 *
 * Parameters: @buffer: Pointer to data buffer
 *             @size: Size of data
 *             @rec: Recording flag
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 **********************************************************************/
static void pcm_start_dma(const UNS_8 *buffer, int size, int rec)
{
  INT_32 dmach;
  DMAC_REGS_T *dmaregs = dma_get_base();
  UNS_32 source, dest, config;
  if (!rec)
  {
    dmach = playback->dmach;
    source = (UNS_32) lpc31xx_va_to_pa((void*)buffer);
    dest = (UNS_32) & playback->i2s_ch->interleaved[0];
    config = (DMA_CFG_CIRC_BUF | DMA_CFG_TX_WORD |
              DMA_CFG_RD_SLV_NR(0) | DMA_CFG_WR_SLV_NR(9));
  }
  else
  {
    dmach = recording->dmach;
    source = (UNS_32) & recording->i2s_ch->interleaved[0];
    dest = (UNS_32) lpc31xx_va_to_pa((void*)buffer);
    config = (DMA_CFG_CIRC_BUF | DMA_CFG_TX_WORD |
              DMA_CFG_RD_SLV_NR(13) | DMA_CFG_WR_SLV_NR(0));
  }

  dmaregs->channel[dmach].enable = 0;
  dmaregs->irq_status_clear |= (0x3 << (dmach * 2));
  dmaregs->channel[dmach].counter = 0;
  dmaregs->channel[dmach].source = source;
  dmaregs->channel[dmach].destination = dest;
  dmaregs->channel[dmach].length = (size / sizeof(UNS_32)) - 1;
  dmaregs->channel[dmach].config = config;

  /* enable DMA channel interrupts and start DMA */
  dmaregs->irq_mask &= ~(0x3 << (2 * dmach));
  dmaregs->channel[dmach].enable = 1;
  return;
}

/***********************************************************************
 *
 * Function: pcm_buffer_playback
 *
 * Purpose:  Hook-up function that receives data from
 *           USB ISO pipe (See isohandler.c).
 *
 * Processing:
 *     See function.
 *
 * Parameters: @buff: Pointer to data buffer
 *             @size: Size of data
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 **********************************************************************/
int pcm_buffer_playback(const UNS_8 * buff, int size)
{
  if (!playback->active || !playback->freq)
    return 0;

  /* Wait for playback to be ready */
  if (!playback->buflen)
  {
    playback->buflen = PCM_BUFFER_SZ(playback->freq);
  }

  if (playback->head + size > playback->buflen)
    size = playback->buflen - playback->head;
  memcpy(playback->buffer + playback->head, buff, size);
#if (defined(DEBUG) && defined(TGT_SDRAM))
  if (acnt < (5 * 1024 * 1204))
  {
    memcpy(audbuf + acnt, buff, size);
    acnt += size;
  }
#endif
  playback->sof_cnt ++;
  playback->head += size;
  if (playback->head >= playback->buflen)
    playback->head = 0;
  playback->incnt += size;
  if (playback->done && playback->incnt >= ((playback->freq / 100) * 4))
  {
    playback->done = 0;
    pcm_start_dma(playback->buffer, playback->buflen, 0);
    playback->offset = playback->incnt;
    playback->pskew = 0;
  }

  /* Calculate the rate at which the playback is lagging/leading
  	 the input samples */
  if (playback->sof_cnt % 100 == 0)
  {
    int cnt, skew;
    cnt = dma_get_base()->channel[playback->dmach].counter;
    cnt = (cnt + 1) * sizeof(UNS_32);
    if (cnt > playback->buflen / 2)
      cnt -= playback->buflen / 2;

    cnt += playback->outcnt;
    skew = cnt - (playback->incnt - playback->offset);
    if (!playback->pskew)
    {
      playback->pskew = skew;
      playback->skew_cnt --;
    }
    playback->skew_sum += playback->pskew - skew;
    playback->skew_cnt ++;
    playback->pskew = skew;
    if (playback->sof_cnt % 1000 == 0)
    {
      playback->skew = (playback->skew_sum / playback->skew_cnt) * 10;
      playback->skew_sum = playback->skew_cnt = 0;
    }
  }
  return 0;
}

/***********************************************************************
 *
 * Function: pcm_buffer_record
 *
 * Purpose:  Hook-up function that sends data to USB ISO pipe
 *
 * Processing:
 *     See function.
 *
 * Parameters: @buff: Pointer to data buffer
 *             @size: Size of data
 *
 * Outputs: None
 *
 * Returns: Return with 0
 *
 * Notes: None
 *
 **********************************************************************/
int pcm_buffer_record(UNS_8 *buff, int size)
{
  if (recording->done)
  {
    recording->done = 0;
  }
  if (!recording->active)
  {
    memset(buff, 0, size);
    return size;
  }
  memcpy(buff, recording->buffer + recording->tail, size);
  recording->tail += size;
  if (recording->tail >= recording->buflen)
    recording->tail = 0;
  recording->outcnt += size;
  return 0;
}

#if (defined(DEBUG) && defined(TGT_SDRAM))
/***********************************************************************
 *
 * Function: play_buffer
 *
 * Purpose:  Playback Audio buffer (used for debugging only)
 *
 * Processing:
 *     See function.
 *
 * Parameters: @buff: Pointer to data buffer
 *             @size: Size of data
 *
 * Outputs: None
 *
 * Returns: Return with 0
 *
 * Notes: None
 *
 **********************************************************************/
int play_buffer(const UNS_8 * buffer, int size)
{
  DMAC_REGS_T *dmaregs = dma_get_base();
  playback->buflen = size;
  playback->incnt = size;
  playback->outcnt = 0;
  pcm_start_dma(buffer, size, 0);
  while (playback->incnt > playback->outcnt);
  dmaregs->irq_mask |= (0x3 << (2 * playback->dmach));
  dmaregs->channel[playback->dmach].enable = 0;
  audbuf = (unsigned char *) 0x31200000;
  acnt = 0;
  return 0;
}

int record_buffer(UNS_8 * buffer, int size)
{
	 return 0;
}
#endif

/***********************************************************************
 *
 * Function: pcm_setup_playback
 *
 * Purpose:  Set the playback of PCM data in audio buffer function
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Return with 0
 *
 * Notes: None
 *
 **********************************************************************/
int pcm_setup_playback(void)
{
  DMAC_REGS_T *dmaregs = dma_get_base();
  playback->dmach = dma_alloc_channel(-1, audio_dma_playback, (void *)dmaregs);

  /* Enable audio CODEC output and max volume */
  lpc3152_codec_set_volume(playback->vol, playback->vol);
  lpc3152_codec_mute(playback->mute);

  /* Book Keeping */
  playback->active = 1;
  playback->incnt = playback->outcnt = 0;
  playback->buflen = 0;
  playback->head = 0;
  playback->skew_cnt = playback->skew = playback->skew_sum = 0;
  playback->done = 1;

  /* Return OK */
  return 0;
}

/***********************************************************************
 *
 * Function: pcm_cleanup_playback
 *
 * Purpose: Clean up the playback function
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Return with 0
 *
 * Notes: None
 *
 **********************************************************************/
int pcm_cleanup_playback(void)
{
  DMAC_REGS_T *dmaregs = dma_get_base();
  int tout = 0xffffff;

  /* TODO See for recording check necessity */
  while (!playback->done && tout--)
    ;

  /* Disable DMA channel, interrupts */
  dmaregs->channel[playback->dmach].enable = 0;
  dmaregs->irq_mask |= (0x3 << (2 * playback->dmach));

  /* Mute CODEC, disable Audio out */
  lpc3152_codec_mute(1);
  lpc313x_chan_clk_enable(CLK_TX_1, 0, 0);
  cgu_clk_en_dis(CGU_SB_I2S_CFG_PCLK_ID, 0);
  cgu_clk_en_dis(CGU_SB_EDGE_DET_PCLK_ID, 0);

  /* Free DMA channel */
  dma_free_channel(playback->dmach);

  playback->active = 0;
  playback->sof_cnt = 0;

  return 0;
}

/***********************************************************************
 *
 * Function: pcm_setup_recording
 *
 * Purpose: Set up PCM for recording function
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Return with 0
 *
 * Notes: None
 *
 **********************************************************************/
int pcm_setup_recording(void)
{
  DMAC_REGS_T *dmaregs = dma_get_base();
  recording->dmach = dma_alloc_channel(-1, audio_dma_recording, (void *)dmaregs);

  recording->incnt = recording->outcnt = 0;
  recording->tail = 0;
  recording->done = 1;

  /* I2S RX1 as master */
  *(UNS_32 *) 0x16000004 = 4;

  /* Audio CODEC Unmute */
  lpc3152_codec_input_mute(0);

  while (!recording->freq)
    ;

  pcm_set_recording_rate(recording->freq);
  recording->buflen = PCM_BUFFER_SZ(recording->freq);
  pcm_start_dma(recording->buffer, recording->buflen, 1);

  return 0;
}

/***********************************************************************
 *
 * Function: pcm_cleanup_recording
 *
 * Purpose: Clean up recording function
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Return with 0
 *
 * Notes: None
 *
 **********************************************************************/
int pcm_cleanup_recording(void)
{
  DMAC_REGS_T *dmaregs = dma_get_base();

  /* Audio CODEC Mute */
  lpc3152_codec_input_mute(1);

  recording->freq = 0;
  lpc313x_chan_clk_enable(CLK_RX_1, 0, 0);
  dmaregs->channel[recording->dmach].enable = 0;
  dmaregs->irq_mask |= (0x3 << (2 * recording->dmach));
  dma_free_channel(recording->dmach);
  recording->active = 0;
  recording->done = 0;
  recording->sof_cnt = 0;

  return 0;
}

/***********************************************************************
 *
 * Function: pcm_device_init
 *
 * Purpose: Initialise the Audio PCM device function
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Return with 0
 *
 * Notes: None
 *
 **********************************************************************/
int pcm_device_init(void)
{
  INT_32 i2cdev = 1, i2sTxdev, i2sRxdev;

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
  lpc313x_main_clk_rate(44100);
  lpc313x_chan_clk_enable(CLK_TX_1, 44100,
                          (44100 * 32));

  /* Enable SYSCLK_O */
  cgu_clk_en_dis(CGU_SB_SYSCLK_O_ID, TRUE);

  /* Enable I2C system clock */
  cgu_clk_en_dis(CGU_SB_I2C1_PCLK_ID, TRUE);

  /* Enable I2C1 interrupt */
  int_enable(IRQ_I2C1);

  dma_init();

  /* Initialise channel numkbers */
  recording->i2s_ch = PCM_I2S_CH_RECORDING;
  playback->i2s_ch = PCM_I2S_CH_PLAYBACK;

  /* Setup CODEC driver */
  if (lpc3152_codec_driver_init(i2cdev, PCM_LPC3152_I2C_ADDR) < 0)
  {
    uart_puts("LPC3152 Audio CODEC driver init failed! \r\n");
    return 1;
  }

  /* Open I2S TX1 channel */
  i2sTxdev = i2s_open(playback->i2s_ch, (PFV) NULL);
  if (i2sTxdev == 0)
  {
    lpc3152_codec_shutdown();
    uart_puts("Playback I2S channel open error!\r\n");
    return 2;
  }

  /* Set I2S format for I2S TX1 - default */
  i2s_ioctl(i2sTxdev, I2S_FORMAT_SETTINGS, I2S);

  /* Open I2S RX1 channel  */
  i2sRxdev = i2s_open(recording->i2s_ch, (PFV) NULL);
  if (i2sRxdev == 0)
  {
    lpc3152_codec_shutdown();
    uart_puts("Recording I2S channel open error!\r\n");
    return 3;
  }

  /* Set I2S format for I2S RX1 - default */
  i2s_ioctl(i2sRxdev, I2S_FORMAT_SETTINGS, I2S);

  /* Enable Audio paths (Both Input & Output) */
  lpc3152_codec_enable_audio_sys(1);

  return 0;
}

/***********************************************************************
 *
 * Function: pcm_set_playback_rate
 *
 * Purpose: Set up I2S sample rate for Playback function
 *
 * Processing:
 *     See function.
 *
 * Parameters: @rate: Sample rate
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 **********************************************************************/
void pcm_set_playback_rate(int rate)
{
  lpc313x_main_clk_rate(rate);
  lpc313x_chan_clk_enable(CLK_TX_1,
                          rate, (rate * 32));
  cgu_clk_en_dis(CGU_SB_I2S_CFG_PCLK_ID, 1);
  cgu_clk_en_dis(CGU_SB_EDGE_DET_PCLK_ID, 1);

  playback->freq = rate;
}

/***********************************************************************
 *
 * Function: pcm_set_recording_rate
 *
 * Purpose: Set up I2S sample rate for Recording function
 *
 * Processing:
 *     See function.
 *
 * Parameters: @rate: Sample rate
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 **********************************************************************/
void pcm_set_recording_rate(int rate)
{
  recording->freq = rate;
  lpc313x_main_clk_rate(recording->freq);
  lpc313x_chan_clk_enable(CLK_RX_1,
                          recording->freq, (recording->freq * 32));
}

/***********************************************************************
 *
 * Function: pcm_sync_playback
 *
 * Purpose: Calculate Skew function
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 **********************************************************************/
void pcm_sync_playback(void)
{
  static int printed;
  //extern char buftmp[];

  if (!playback->active)
    return;

  if (playback->sof_cnt % 1000 == 0)
  {
    if (!printed)
    {
      //sprintf(buftmp, "Skew = %d\r\n", playback->skew);
      //uart_puts(buftmp);
      printed = 1;
    }
  }
  else
    printed = 0;
}

