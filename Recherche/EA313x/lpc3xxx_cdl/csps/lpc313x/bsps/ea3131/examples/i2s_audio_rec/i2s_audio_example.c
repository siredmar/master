/***********************************************************************
 * $Id:: i2s_audio_example.c 8087 2011-09-14 04:13:00Z ing02124        $
 *
 * Project: I2S audio driver recording example
 *
 * Description:
 *     This example uses I2C to initialize the audio CODEC, sets up
 *     correct I2S clocking, and then uses DMA to capture and playback
 *     an audio stream.
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
#include "lpc_irq_fiq.h"
#include "lpc_arm922t_cp15_driver.h"
#include "ea3131_board.h"
#include "lpc313x_cgu_driver.h"
#include "lpc313x_i2s_driver.h"
#include "lpc313x_mstr_i2c_driver.h"
#include "lpc313x_dma_driver.h"
#include "uda1380_codec_driver.h"
#include "lpc313x_i2s_clock_driver.h"
#include "lpc31xx_vmem_driver.h"
#include "uda1380.h"
#include "lpc313x_debug.h"

#define LPC_AUDIO_SRATE  44100
#define LPC_CAPT_PERIOD      8 /* In seconds */

#define AUDIOBUFFSIZE (LPC_AUDIO_SRATE * 4 * LPC_CAPT_PERIOD)
static UNS_8 * dma_buff = (UNS_8 *) 0x31200000;
static volatile INT_32 dmadoneflag, dmach;
static DMAC_REGS_T *dmaregs;
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
 * Returns: Always returns 1
 *
 * Notes: None
 *
 **********************************************************************/
void dma_rec_callback(INT_32 ch, DMA_IRQ_TYPE_T itype, 
    void *pdmaregs)
{
  UNS_32 irqmask = dmaregs->irq_status_clear & (0x3 << ch);

#if 0
  if (irqmask == (2 << dmach))
  {
    uart_puts("Rec 50\% complete\r\n");
  }
#endif
  //if (irqmask == (1 << dmach))
  if (itype == DMA_IRQ_FINISHED)
  {
    /*uart_puts("Rec 100\% complete\r\n");*/
    dmadoneflag = 1;
  }

  dmaregs->irq_status_clear = irqmask;
}

/***********************************************************************
 *
 * Function: dma_pb_callback
 *
 * Purpose: Audio DMA playback handler callback
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
 * Returns: Always returns 1
 *
 * Notes: None
 *
 **********************************************************************/
void dma_pb_callback(INT_32 ch, DMA_IRQ_TYPE_T itype, 
    void *pdmaregs)
{
  UNS_32 irqmask = dmaregs->irq_status_clear & (0x3 << dmach);
#if 0
  if (irqmask == (2 << dmach))
  {
    uart_puts("Playback 50\% complete\r\n");
  }
#endif
  //if (irqmask == (1 << dmach))
  if (itype == DMA_IRQ_FINISHED)
  {
    /*uart_puts("Playback 100\% complete\r\n");*/
    dmadoneflag = 1;
  }

  dmaregs->irq_status_clear = irqmask;
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
  INT_32 i2cdev = 0, i2sTxdev, i2sRxdev;
  int tout, j;

  /* Disable interrupts in ARM core */
  disable_irq_fiq();

  /* Setup miscellaneous board functions */
  ea3131_board_init();

  /* Set virtual address of MMU table */
  cp15_set_vmmu_addr((void *)ISROM_MMU_TTB_BASE);

  /* Initialize interrupt system */
  int_initialize(0xFFFFFFFF);

  /* install default I2C0 & I2C1 interrupt handlers */
  int_install_irq_handler(IRQ_I2C1, (PFV)i2c1_mstr_int_hanlder);

  /* Enable IRQ interrupts in the ARM core */
  enable_irq();

  // HACK! required to connect 256FS fractional clock
  * (UNS_32 *) 0x130044dc = 5;

  /* I2S requires various clocks set at specific rates to get the
     target frequency. The I2S clocking driver will generate the
     correct rates for each clock group */
  lpc313x_set_fsmult_rate(256, CLK_USE_256FS);
  lpc313x_main_clk_rate(LPC_AUDIO_SRATE);
  lpc313x_chan_clk_enable(CLK_RX_1, LPC_AUDIO_SRATE,
                          (LPC_AUDIO_SRATE * 32));

  /* Enable I2C system clock */
  cgu_clk_en_dis(CGU_SB_I2C1_PCLK_ID, TRUE);
  
	/* Enable I2C1 interrupt */
	int_enable(IRQ_I2C1);

	/* DMA init */
  dma_init();
  dmaregs = dma_get_base();

  /* Setup CODEC driver */
  if (uda1380_driver_init(i2cdev, 0x1a) < 0)
  {
    uart_puts("UDA1380 driver init failed! Exiting...\r\n");
    return 1;
  }
	
	/* TX1 I2S */
  i2sTxdev = i2s_open(I2S_TX1, (PFV) NULL);
  if (i2sTxdev == 0)
  {
    uart_puts("I2S_TX1 open error!\r\n");
		return 2;
  }

  /* set I2S format for I2S TX - default */
  i2s_ioctl(i2sTxdev, I2S_FORMAT_SETTINGS, I2S);

  /* Open I2S RX1 device  */
  i2sRxdev = i2s_open(I2S_RX1, (PFV) NULL);
  if (i2sRxdev == 0)
  {
    uda1380_shutdown();
    uart_puts("Unable to open RX1 for audio capture! Exiting...\r\n");
    return 3;
  }

  /* set I2S format for I2S TX - default */
  i2s_ioctl(i2sRxdev, I2S_FORMAT_SETTINGS, I2S);

  for (j = 0; j < 2; j ++)
  {
    dmach = dma_alloc_channel(-1, dma_rec_callback, (void *)dmaregs);

    *(UNS_32 *) 0x16000004 = 4;
    (void) uda1380_enable_audio_in(1);
    lpc313x_main_clk_rate(LPC_AUDIO_SRATE);
    lpc313x_chan_clk_enable(CLK_RX_1, LPC_AUDIO_SRATE,
                            (LPC_AUDIO_SRATE * 32));

    /* Setup DMA channel and circular buffer */
    dmaregs->channel[dmach].destination = (UNS_32) lpc31xx_va_to_pa(dma_buff);
    dmaregs->channel[dmach].source = 0x16000260;
    dmaregs->channel[dmach].length = (AUDIOBUFFSIZE / sizeof(UNS_32)) - 1;
    dmaregs->channel[dmach].config = (DMA_CFG_TX_WORD |
                                      DMA_CFG_RD_SLV_NR(13) | DMA_CFG_WR_SLV_NR(0));

    /* enable DMA channel interrupts and start DMA */
    dmaregs->irq_mask &= ~(0x3 << (2 * dmach));
    dmadoneflag = 0;
    dmaregs->channel[dmach].enable = 1;

    tout = 0xFF;
    uart_puts("Recording [Time Remaining in Seconds]: xx");
    /* Let DMA handle audio */
    while (dmadoneflag == 0)
    {
      UNS_32 counter = dmaregs->channel[dmach].counter / LPC_AUDIO_SRATE;
      if (tout != counter)
      {
        if (counter == 0 && tout <= LPC_CAPT_PERIOD)
          counter = LPC_CAPT_PERIOD;
        tout = counter;
        sprintf(buftmp, "\033[2D%02u", LPC_CAPT_PERIOD - tout);
        uart_puts(buftmp);
      }
    }
    uart_puts("\r\nRecording Completed!\r\n");

    /* Disable channel */
    dmaregs->channel[dmach].enable = 0;
    dmaregs->irq_mask |= (0x3 << (2 * dmach));
    /* Audio CODEC will turn off */
    lpc313x_chan_clk_enable(CLK_RX_1, 0, 0);
    if (uda1380_enable_audio_in(0))
    {
      uart_puts("Audio in deinit error \r\n");
    }
    dma_free_channel(dmach);

    /********* Playback area ****************/
    /* Allocate a DMA channel for playback */
    dmach = dma_alloc_channel(-1, dma_pb_callback, NULL);

    /* Enable audio CODEC output and max volume */
    if (uda1380_enable_audio_out(1))
    {
      uart_puts("Audio out init error \r\n");
    }
    uda1380_set_volume(0, 0);
    uda1380_mute(0);
    lpc313x_main_clk_rate(LPC_AUDIO_SRATE);
    lpc313x_chan_clk_enable(CLK_TX_1, LPC_AUDIO_SRATE,
                            (LPC_AUDIO_SRATE * 32));
    cgu_clk_en_dis(CGU_SB_I2S_CFG_PCLK_ID, 1);
    cgu_clk_en_dis(CGU_SB_EDGE_DET_PCLK_ID, 1);


    /* Setup DMA channel and circular buffer */
    dmaregs->channel[dmach].source = (UNS_32) lpc31xx_va_to_pa(dma_buff);
    dmaregs->channel[dmach].destination = 0x16000160;
    dmaregs->channel[dmach].length = (AUDIOBUFFSIZE / sizeof(UNS_32)) - 1;
    dmaregs->channel[dmach].config = (DMA_CFG_TX_WORD |
                                      DMA_CFG_RD_SLV_NR(0) | DMA_CFG_WR_SLV_NR(9));

    /* enable DMA channel interrupts and start DMA */
    dmaregs->irq_mask &= ~(0x3 << (2 * dmach));
    dmadoneflag = 0;
    dmaregs->channel[dmach].enable = 1;
    tout = 0xFF;
    uart_puts("Playback [Time Remaining in Seconds]: xx");
    /* Let DMA handle audio */
    while (dmadoneflag == 0)
    {
      UNS_32 counter = dmaregs->channel[dmach].counter / LPC_AUDIO_SRATE;
      if (tout != counter)
      {
        if (counter == 0 && tout <= LPC_CAPT_PERIOD)
          counter = LPC_CAPT_PERIOD;
        tout = counter;
        sprintf(buftmp, "\033[2D%02u", LPC_CAPT_PERIOD - tout);
        uart_puts(buftmp);
      }
    }
    uart_puts("\r\nPlayback Completed!\r\n");

    /* Disable channel */
    dmaregs->channel[dmach].enable = 0;
    dmaregs->irq_mask |= (0x3 << (2 * dmach));

    /* Audio CODEC will turn off */
    uda1380_mute(1);
    cgu_clk_en_dis(CGU_SB_I2S_CFG_PCLK_ID, 0);
    cgu_clk_en_dis(CGU_SB_EDGE_DET_PCLK_ID, 0);
    lpc313x_chan_clk_enable(CLK_TX_1, 0, 0);
    if (uda1380_enable_audio_out(0))
    {
      uart_puts("Audio out deinit error \r\n");
    }
    dma_free_channel(dmach);
  } /* End of for j */
	
  i2s_close(i2sRxdev);
  i2s_close(i2sTxdev);

  return 0;
}
