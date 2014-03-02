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

#include <string.h>
#include <stdio.h>

#include "lpc_types.h"
#include "lpc_string.h"
#include "lpc_irq_fiq.h"
#include "lpc_arm922t_cp15_driver.h"
#include "ea3152_board.h"
#include "lpc313x_cgu_driver.h"
#include "lpc313x_i2s_driver.h"
#include "lpc313x_mstr_i2c_driver.h"
#include "lpc313x_dma_driver.h"
#include "lpc3152_codec_driver.h"
#include "lpc313x_i2s_clock_driver.h"
#include "lpc31xx_vmem_driver.h"
#include "lpc313x_uart_driver.h"

#define LPC_ANA_CODEC_I2C_ADDR (0x0C) /* Analog CODEC I2C address */

#define LPC_AUDIO_SRATE  44100
#define LPC_CAPT_PERIOD      8 /* In seconds */

#define AUDIOBUFFSIZE (LPC_AUDIO_SRATE * 4 * LPC_CAPT_PERIOD)

static UNS_8 * dma_buff = (UNS_8 *) 0x31200000;
static volatile INT_32 dmadoneflag, dmach_p, dmach_r;
static DMAC_REGS_T *dmaregs;

static INT_32 uartdev;

/* UART buffer */
char buftmp[512];

/***********************************************************************
 *
 * Function: uart_string_write
 *
 * Purpose: UART string output helper function
 *
 * Processing:
 *     See function.
 *
 * Parameters: @pbuff - Pointer to char buffer
 *             @len   - Length of buffer
 *
 * Outputs: Character stored in @pbuff printed to uart
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 ***********************************************************************/
static void uart_string_write(const char *pbuff, UNS_32 len)
{
  UNS_32 written;
  UNS_32 devid;

  uartdev = uartdev ? uartdev : uart_open((void *) UART, 0);
  devid = uartdev;

  if (devid != 0)
  {
    if (len == 0)
      len = strlen(pbuff);

    while (len > 0)
    {
      written = uart_write(devid, (UNS_8 *)pbuff, len);
      len -= written;
      pbuff += written;
    }
  }
}

/***********************************************************************
 *
 * Function: uart_puts
 *
 * Purpose: Writes a NUL terminated ASCII string to UART
 *
 * Processing:
 *     See function.
 *
 * Parameters: @str - Contains string to be written
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 ***********************************************************************/
void uart_puts(const char *str)
{
  uart_string_write(str, 0);
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
 * Returns: Always returns 1
 *
 * Notes: None
 *
 **********************************************************************/
void dma_rec_callback(INT_32 ch, DMA_IRQ_TYPE_T itype, 
    void *pdmaregs)
{
  UNS_32 irqmask = dmaregs->irq_status_clear & (0x3 << (2 * dmach_r));

  //if (irqmask & (1 << (2 * dmach_r)))
  if (itype == DMA_IRQ_FINISHED)
  {
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
  UNS_32 irqmask = dmaregs->irq_status_clear & (0x3 << (dmach_p * 2));

  if (itype == DMA_IRQ_FINISHED)
  {
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
  INT_32 i2sTxdev, i2sRxdev;
  int tout, j;

  /* Disable interrupts in ARM core */
  disable_irq_fiq();

  /* Set virtual address of MMU table */
  cp15_set_vmmu_addr((void *)ISROM_MMU_TTB_BASE);

  // HACK! required to connect 256FS fractional clock
  * (UNS_32 *) 0x130044dc = 5;

  /* Set RX1 as Master */
  *(UNS_32 *) 0x16000004 = 4;

  /* Initialize interrupt system */
  int_initialize(0xFFFFFFFF);

  /* install default I2C0 & I2C1 interrupt handlers */
  int_install_irq_handler(IRQ_I2C1, (PFV)i2c1_mstr_int_hanlder);

  /* Enable IRQ interrupts in the ARM core */
  enable_irq();

  /* I2S requires various clocks set at specific rates to get the
     target frequency. The I2S clocking driver will generate the
     correct rates for each clock group */
  lpc313x_set_fsmult_rate(128, CLK_USE_256FS);
  lpc313x_main_clk_rate(LPC_AUDIO_SRATE);
  lpc313x_chan_clk_enable(CLK_RX_1, LPC_AUDIO_SRATE,
                          (LPC_AUDIO_SRATE * 32));

  /* Note: The SYSCLK clock (generated on the CLK256FS pin) must be
     enabled as per the previous step prior to I2C CODEC access */
  /* Enable SYSCLK_O */
  cgu_clk_en_dis(CGU_SB_SYSCLK_O_ID, TRUE);
  cgu_clk_en_dis(CGU_SB_I2S_CFG_PCLK_ID, 1);
  cgu_clk_en_dis(CGU_SB_EDGE_DET_PCLK_ID, 1);

  /* Enable I2C system clock */
  cgu_clk_en_dis(CGU_SB_I2C1_PCLK_ID, TRUE);

  /* Enable I2C Interrupt */
  int_enable(IRQ_I2C1);

  /* Initialise DMA */
  dma_init();
  dmaregs = dma_get_base();

  /* Setup CODEC driver */
  if (lpc3152_codec_driver_init(0, LPC_ANA_CODEC_I2C_ADDR) < 0)
  {
    uart_puts("LPC3152 CODEC driver init failed! Exiting...\r\n");
    return 2;
  }

  /* Open Tx1 I2S channel */
  i2sTxdev = i2s_open(I2S_TX1, (PFV) NULL);
  if (i2sTxdev == 0)
  {
    uart_puts("I2S_TX1 open error! \r\n");
    return 3;
  }

  /* set I2S format for I2S TX - default */
  i2s_ioctl(i2sTxdev, I2S_FORMAT_SETTINGS, I2S);

  /* open the I2S RX1 channel */
  i2sRxdev = i2s_open(I2S_RX1, (PFV) NULL);
  if (i2sRxdev == 0)
  {
    uart_puts("Unable to open RX1 for audio capture! Exiting...\r\n");
    return 4;
  }

  /* set I2S format for I2S RX - default */
  i2s_ioctl(i2sRxdev, I2S_FORMAT_SETTINGS, I2S);

  for (j = 0; j < 2; j ++)
  {
    /* Allocate DMA channel */
    dmach_r = dma_alloc_channel(-1, dma_rec_callback, (void *)dmaregs);

    /* Enable Audio Input */
    lpc3152_codec_enable_audio_in(1);

    /* Set Audio input volume */
    lpc3152_codec_set_input_volume(0x40, 0x40);

    /* Unmute Audio input */
    lpc3152_codec_input_mute(0);

    /* Set RX Clocks */
    lpc313x_main_clk_rate(LPC_AUDIO_SRATE);
    lpc313x_chan_clk_enable(CLK_RX_1, LPC_AUDIO_SRATE,
                            (LPC_AUDIO_SRATE * 32));

    /* Setup DMA channel and circular buffer */
    dmaregs->channel[dmach_r].source = 0x16000260;
    dmaregs->channel[dmach_r].destination = (UNS_32) lpc31xx_va_to_pa(dma_buff);
    dmaregs->channel[dmach_r].length = (AUDIOBUFFSIZE / sizeof(UNS_32)) - 1;
    dmaregs->channel[dmach_r].config = (DMA_CFG_TX_WORD |
                                        DMA_CFG_RD_SLV_NR(13) | DMA_CFG_WR_SLV_NR(0));

    /* enable DMA channel interrupts and start DMA */
    dmaregs->irq_mask &= ~(0x3 << (2 * dmach_r));
    dmadoneflag = 0;
    dmaregs->channel[dmach_r].enable = 1;

    tout = 0xFF;
    uart_puts("Recording [Time Remaining in Seconds]: xx");
    /* Let DMA handle audio */
    while (dmadoneflag == 0)
    {
      UNS_32 counter = dmaregs->channel[dmach_r].counter / LPC_AUDIO_SRATE;
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
    dmaregs->channel[dmach_r].enable = 0;
    dmaregs->irq_mask |= (0x3 << (2 * dmach_r));

    /* Mute Audio input */
    lpc3152_codec_input_mute(1);

    /* Audio input shutdown */
    lpc3152_codec_enable_audio_in(0);

    /* Disable RX clocks */
    lpc313x_chan_clk_enable(CLK_RX_1, 0, 0);

    /* Free the DMA channel */
    dma_free_channel(dmach_r);

    /* Allocate a DMA channel for playback */
    dmach_p = dma_alloc_channel(-1, dma_pb_callback, (void *)dmaregs);

    /* Enable audio CODEC output and max volume */
    lpc3152_codec_enable_audio_out(1);

    /* Set Volume to Max */
    lpc3152_codec_set_volume(0x0, 0x0);

    /* Unmute */
    lpc3152_codec_mute(0);

    /* Set I2S Clocks */
    lpc313x_main_clk_rate(LPC_AUDIO_SRATE);
    lpc313x_chan_clk_enable(CLK_TX_1, LPC_AUDIO_SRATE,
                            (LPC_AUDIO_SRATE * 32));

    /* Setup DMA channel and circular buffer */
    dmaregs->channel[dmach_p].source = (UNS_32) lpc31xx_va_to_pa(dma_buff);
    dmaregs->channel[dmach_p].destination = 0x16000160;
    dmaregs->channel[dmach_p].length = (AUDIOBUFFSIZE / sizeof(UNS_32)) - 1;
    dmaregs->channel[dmach_p].config = (DMA_CFG_TX_WORD |
                                        DMA_CFG_RD_SLV_NR(0) | DMA_CFG_WR_SLV_NR(9));

    /* enable DMA channel interrupts and start DMA */
    dmaregs->irq_mask &= ~(0x3 << (2 * dmach_p));
    dmadoneflag = 0;
    dmaregs->channel[dmach_p].enable = 1;
    tout = 0xFF;
    uart_puts("Playback [Time Remaining in Seconds]: xx");

    /* Let DMA handle audio */
    while (dmadoneflag == 0)
    {
      UNS_32 counter = dmaregs->channel[dmach_p].counter / LPC_AUDIO_SRATE;
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
    dmaregs->channel[dmach_p].enable = 0;
    dmaregs->irq_mask |= (0x3 << (2 * dmach_p));

    /* Mute CODEC */
    lpc3152_codec_mute(1);

    /* Audio CODEC will turn off */
    lpc3152_codec_enable_audio_out(0);

    /* Disable TX1 Clocks */
    lpc313x_chan_clk_enable(CLK_TX_1, 0, 0);

    /* Free the DMA channel */
    dma_free_channel(dmach_p);
  } /* End of for j */

  cgu_clk_en_dis(CGU_SB_I2S_CFG_PCLK_ID, 0);
  cgu_clk_en_dis(CGU_SB_EDGE_DET_PCLK_ID, 0);

  i2s_close(i2sTxdev);
  i2s_close(i2sRxdev);

  disable_irq_fiq();

  return 0;
}
