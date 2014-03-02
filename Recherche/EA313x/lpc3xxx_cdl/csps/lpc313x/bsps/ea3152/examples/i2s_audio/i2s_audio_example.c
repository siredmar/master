/***********************************************************************
 * $Id:: i2s_audio_example.c 8087 2011-09-14 04:13:00Z ing02124        $
 *
 * Project: I2S audio driver example
 *
 * Description:
 *     This example uses I2C to initialize the audio CODEC, sets up
 *     correct I2S clocking, and then uses circular mode DMA to play
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
#include "ea3152_board.h"
#include "lpc313x_cgu_driver.h"
#include "lpc313x_i2s_driver.h"
#include "lpc313x_mstr_i2c_driver.h"
#include "lpc313x_dma_driver.h"
#include "lpc3152_codec_driver.h"
#include "lpc313x_i2s_clock_driver.h"
#include "lpc31xx_vmem_driver.h"
#include "lpc_pacmanva.h"

#define AUDIOBUFFSIZE (4 * 1024)

static DMA_BUFFER UNS_32 dma_buff[AUDIOBUFFSIZE / sizeof(UNS_32)];
static UNS_8 *str_ptr;
static volatile INT_32 dmadoneflag, dmach, str_rem;
static DMAC_REGS_T *dmaregs;
static const UNS_32 test_rates[] =
{
  8000, 11025, 12000, 16000, 22050, 32000, 44100, 48000, 64000,
  88200, 96000
};

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
 *    buff: Pointer to DMA buffer half to fill
 *
 * Outputs: None
 *
 * Returns: Returns 1 when there is no more DMA data to fill
 *
 * Notes: None
 *
 **********************************************************************/
INT_32 fill_buff(UNS_32 *buff)
{
  UNS_8 *p8 = (UNS_8 *) buff;
  INT_32 idx, done = 0;

  if (str_rem == 0)
  {
    done = 1;
  }
  else
  {
    for (idx = 0; idx < (AUDIOBUFFSIZE / 2); idx++)
    {
      if (str_rem > 0)
      {
        *p8 = *str_ptr;
        str_ptr++;
        str_rem--;
      }
      else
      {
        /* Silence */
        *p8 = 0;
      }

      p8++;
    }
  }
  lpc31xx_flush_dcache_range(buff, (UNS_32 *)((UNS_32)buff + AUDIOBUFFSIZE));

  return done;
}

/***********************************************************************
 *
 * Function: dma_callback
 *
 * Purpose: Audio DMA handler callback
 *
 * Processing:
 *     For each call to this handler, the DMA circuilar buffer half
 *     that isn't active is refilled. Once all data is transferred,
 *     a flag is set to end the program.
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
void dma_callback(INT_32 ch, DMA_IRQ_TYPE_T itype, void *pdmaregs)
{
  INT_32 done = 0;
  UNS_32 irqmask = dmaregs->irq_status_clear & (0x3 << dmach);

  //if (irqmask == (2 << dmach))
  if (itype == DMA_IRQ_HALFWAY)
  {
    /* Fill first half of buffer */
    done = fill_buff(&dma_buff[0]);
  }
  //if (!done && (irqmask == (1 << dmach)))
  if (!done && (itype == DMA_IRQ_FINISHED))
  {
    /* 2nd half */
    done = fill_buff(&dma_buff[(AUDIOBUFFSIZE / sizeof(UNS_32)) / 2]);
  }

  dmaregs->irq_status_clear = irqmask;
  dmadoneflag = done;
}

#if 0
void codec_reg_dump(void)
{
  int i = 0;
  const UNS_32 creg[] =
  {
    0x00, 0x01, 0x02, 0x10, 0x11, 0x12,
    0x13, 0x14, 0x15, 0x16, 0x17, 0xFF
  };

  while (creg[i] != 0xFF)
  {
    printf("CODEC reg 0x%02x = 0x%08x\n", creg[i],
           lpc3152_codec_read(creg[i]));
    i++;
  }
}
#endif

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
  INT_32 i2sTxdev;
  int rateidx, ret = _ERROR;

  /* Disable interrupts in ARM core */
  disable_irq_fiq();

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
  /* Setup the I2S clocking as follows:
     WS (sample rate)          = LPC_PACMANVA_SRATE
     bitclock rate (bck)       = LPC_PACMANVA_SRATE*32
     CODEC clock (SYSCLK) rate = LPC_PACMANVA_SRATE*128 */
  if (lpc313x_set_fsmult_rate(128, CLK_USE_256FS) <= 0)
    goto exit1;
  if (lpc313x_main_clk_rate(LPC_PACMANVA_SRATE) <= 0)
    goto exit1;
  if (lpc313x_chan_clk_enable(CLK_TX_1, LPC_PACMANVA_SRATE,
                              (LPC_PACMANVA_SRATE * 32)) <= 0)
    goto exit1;

  /* Note: The SYSCLK clock must be enabled as per the previous
     step prior to I2C CODEC access. It is asumed the sysclk is
     already on */

  /* Enable I2C1 interrupt used by the CODEC driver */
  int_enable(IRQ_I2C1);

  /* Setup CODEC driver */
  if (lpc3152_codec_driver_init(0, 0x0c) < 0)
    goto exit3;

  /* open the I2S TX1 device  */
  i2sTxdev = i2s_open(I2S_TX1, (PFV) NULL);
  if (i2sTxdev == 0)
    goto exit4;

  /* set I2S format for I2S TX - default */
  i2s_ioctl(i2sTxdev, I2S_FORMAT_SETTINGS, I2S);

  /* Setup DMA - a single circular buffer is used for audio. This buffer
     is 2KBytes in size and is seperated into 2 1K halves. The DMA will
     generate an interrupt when it's halfway and completely done with a
     transfer. These interrupts are used to refill the unused half of
     the buffer with new audio data. */
  dma_init();
  dmaregs = dma_get_base();
  dmach = dma_alloc_channel(0, dma_callback, (void *)dmaregs);

  /* Enable audio CODEC output and max volume */
  lpc3152_codec_enable_audio_out(1);
  lpc3152_codec_set_volume(0, 0);
  lpc3152_codec_mute(0);

#if 0
  /* Dump CODEC registers */
  codec_reg_dump();
#endif

  rateidx = 0;
  while (test_rates[rateidx] != 0)
  {
    lpc313x_main_clk_rate(test_rates[rateidx]);
    lpc313x_chan_clk_enable(CLK_TX_1, test_rates[rateidx],
                            (test_rates[rateidx] * 32));

    /* DMA size is number of audio samples times 2 channels times
       16-bits per sample */
    str_ptr = (UNS_8 *) sma_pacmanva;
    str_rem = LPC_PACMANVA_NUM_SAMPLES * 2 * sizeof(UNS_16);

    /* Pre-fill audio DMA buffers */
    fill_buff(&dma_buff[0]);
    fill_buff(&dma_buff[(AUDIOBUFFSIZE / sizeof(UNS_32)) / 2]);

    /* Setup DMA channel and circular buffer */
    dmaregs->channel[dmach].source = (UNS_32) lpc31xx_va_to_pa(dma_buff);
    dmaregs->channel[dmach].destination = 0x16000160;
    dmaregs->channel[dmach].length = (AUDIOBUFFSIZE / sizeof(UNS_32)) - 1;
    dmaregs->channel[dmach].config = (DMA_CFG_CIRC_BUF | DMA_CFG_TX_WORD |
                                      DMA_CFG_RD_SLV_NR(0) | DMA_CFG_WR_SLV_NR(9));

    /* enable DMA channel interrupts and start DMA */
    dmaregs->irq_mask &= ~(0x3 << (2 * dmach));
    dmadoneflag = 0;
    dmaregs->channel[dmach].enable = 1;

    /* Let DMA handle audio */
    while (dmadoneflag == 0);

    /* Disable channel */
    dmaregs->irq_mask |= (0x3 << (2 * dmach));
    dmaregs->channel[dmach].enable = 0;

    /* Next rate */
    rateidx++;
  }

  /* Audio CODEC will turn off */
  lpc3152_codec_mute(1);
  lpc3152_codec_enable_audio_out(0);

  ret = 0;

  dma_free_channel(dmach);
exit4:
  lpc3152_codec_shutdown();
exit3:
exit1:
  lpc313x_main_clk_rate(0);
  lpc313x_chan_clk_enable(CLK_TX_1, 0, 0);

  return ret;
}
