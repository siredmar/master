/***********************************************************************
* $Id:: pcmhandler.h 6653 2011-03-02 12:20:06Z ing03005                     $
*
* Project: USB audio device PCM handler functions
*
* Description:
*     This file contains the structure definitions and manifest
*     constants for the LPC313X chip family component:
*        PCM Codec event handlers, and support routines.
*
* Notes:
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
#ifndef _PCMHANDLER_H
#define _PCMHANDLER_H

/* The following includes are required */
#include "lpc31xx_vmem_driver.h"
#include "lpc313x_i2s_driver.h"
#include "usbcfg.h"

/* The pcmhandler will buffer PCM_BUFFER_TIME * 2 milliseconds of data
 * NOTE: Must be a multiple of 10
 **/
#define PCM_BUFFER_TIME          100 /* milliseconds */
#define PCM_MAX_SAMPLING_RATE    48000

/* rate / 100 = (No. of samples per 10 milli seconds) x
 * 4 = 4 bytes per sample (32 bits) x
 * 2 = Dual buffer x
 * PCM_BUFFER_TIME / 10 = (Number of milliseconds to hold)
 **/
#define PCM_BUFFER_SZ(rate)      (((rate) / 100) * 4 * 2 * (PCM_BUFFER_TIME / 10))

/* Playback starts after we have 10ms of sample with us */
#define PCM_PLAYBACK_OFFSET      ((playback->freq / 100) * 4)

#define SDRAM_BUFFER_START           0x31200000
#define SDRAM_PLAY_BUFFER            (UNS_8 *)SDRAM_BUFFER_START
#define SDRAM_REC_BUFFER             (UNS_8 *)(SDRAM_BUFFER_START + PCM_BUFFER_SZ(PCM_MAX_SAMPLING_RATE))

/* I2S Channels used for recording and playback */
#define PCM_I2S_CH_PLAYBACK   I2S_TX1
#define PCM_I2S_CH_RECORDING  I2S_RX1

/* UDA1380 CODEC I2C address */
#define PCM_UDA1380_I2C_ADDR  (0x1A)


typedef enum pcm_info_type
{
  PCM_RECORDING,
  PCM_PLAYBACK,
  PCM_DEVICE_CNT
}pcm_info_type_t;

struct pcm_info
{
  UNS_8 *buffer;
  volatile int active;
  volatile unsigned long sof_cnt;
  volatile int xfer_cnt;
  volatile int head;
  volatile int tail;
  volatile int full;
  volatile int mute;
  volatile int freq;
  volatile int vol;
  volatile int incnt;
  volatile int outcnt;
  volatile int done;
  volatile int buflen;
  volatile int skew_sum;
  volatile int skew_cnt;
  volatile int skew;
  volatile int pskew;
  volatile int tlapsed;
  INT_32 dmach;
  I2S_REGS_T * i2s_ch;
};
extern struct pcm_info DMA_BUFFER pcm_info[PCM_DEVICE_CNT];

#define recording (&pcm_info[PCM_RECORDING])
#define playback  (&pcm_info[PCM_PLAYBACK])
#ifdef __cpluscplus
extern "C"
{
#endif
  int pcm_buffer_playback(const UNS_8 * buff, int size);
  int pcm_buffer_recording(UNS_8 * buff, int size);
  void pcm_buffer_play(void);
  int pcm_buffer_record(UNS_8 *buff, int size);
  int pcm_setup_playback(void);
  int pcm_cleanup_playback(void);
  int pcm_setup_recording(void);
  int pcm_cleanup_recording(void);
  int pcm_device_init(void);
  void pcm_device_deinit(void);
  void pcm_set_playback_rate(int rate);
  void pcm_set_recording_rate(int rate);
  void pcm_sync_playback(void);
  void pcm_sync_recording(void);
#if (defined(DEBUG) && defined(TGT_SDRAM))
  int play_buffer(const UNS_8 * buffer, int size);
#endif
#ifdef __cplusplus
}
#endif
#endif /* _PCMHANDLER_H */
