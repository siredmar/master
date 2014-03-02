/***********************************************************************
 * $Id:: lpc3152_codec_driver.c 6651 2011-03-02 06:51:24Z ing03005     $
 *
 * Project: LPC3152 CODEC read/write driver
 *
 * Description:
 *     Driver to read and write the CODEC registers via I2C on the
 *     LPC3152 analog die. Also provides some CODEC helper functions.
 *     All functions are blocking.
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
#include "lpc313x_chip.h"
#include "lpc313x_mstr_i2c_driver.h"
#include "lpc3152_codec_driver.h"
#include "lpc313x_timer_driver.h"
#include "lpc315x_analog.h"

/***********************************************************************
 * Driver data
 **********************************************************************/

typedef struct
{
  INT_32 i2cdev;
  UNS_8 i2caddr;
} CODEC3152_DAT_T;
static CODEC3152_DAT_T drvdata;

static INT_32 i2chandle;

/***********************************************************************
 * Location functions
 **********************************************************************/

/***********************************************************************
 * Global functions
 **********************************************************************/

/***********************************************************************
 *
 * Function: lpc3152_codec_driver_init
 *
 * Purpose: Initializes driver and data
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     i2cdev  : Handle to an opened I2C device
 *     i2caddr : I2C address of the LPC3152 analog die
 *
 * Outputs: None
 *
 * Returns: 0 if the CODEC was detected, otherwise <0
 *
 * Notes: None
 *
 **********************************************************************/
INT_32 lpc3152_codec_driver_init(INT_32 i2cdev, UNS_8 i2caddr)
{
  UNS_32 curval;
  drvdata.i2cdev = i2cdev;
  drvdata.i2caddr = i2caddr << 1;

  i2chandle = i2c_mstr_open((void *) I2C1, 0);
  ;
  if (i2chandle == 0)
    return _ERROR;

  /* Select ADC & DAC to operate @128fs */
  curval = lpc3152_codec_read(AD_REG_CGU_RW);
  curval &= ~(AD_CGU_AIN_SEL_ADCSYS_256FS |
              AD_CGU_AIN_SEL_ADC1_OFF_1 |
              AD_CGU_AIN_SEL_ADC2_128FS |
              AD_CGU_I2C_CLK_256FS |
              AD_CGU_INT_SEL_CLKNS_256FS |
              AD_CGU_AOUT_SEL_CLKDAC_256FS);
  lpc3152_codec_write(AD_REG_CGU_RW, curval);

  return _NO_ERROR;
}

/***********************************************************************
 *
 * Function: lpc3152_codec_shutdown
 *
 * Purpose: Shutdown CPDEC
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void lpc3152_codec_shutdown(void)
{
  lpc3152_codec_enable_audio_out(0);
  lpc3152_codec_enable_audio_in(0);
}

/***********************************************************************
 *
 * Function: lpc3152_codec_reset
 *
 * Purpose: Reset the CODEC
 *
 * Processing:
 *     Does nothing.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void lpc3152_codec_reset(void)
{
}

/***********************************************************************
 *
 * Function: lpc3152_codec_write
 *
 * Purpose: Write data to a CODEC register
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     reg  : register to write
 *     data : data to write
 *
 * Outputs: None
 *
 * Returns: Value written to register
 *
 * Notes: None
 *
 **********************************************************************/
UNS_32 lpc3152_codec_write(UNS_16 reg, UNS_32 data)
{
  UNS_32 rxdata[8], txdata[8];
  I2C_MTXRX_SETUP_T xfer;
  UNS_32 status;

  txdata[0] = drvdata.i2caddr | I2C_TXFF_START_CND | 0;
  txdata[1] = ((UNS_32) reg & 0xFF00) >> 8;
  txdata[2] = ((UNS_32) reg & 0x00FF) >> 0;
  txdata[3] = ((UNS_32) data & 0xFF000000) >> 24;
  txdata[4] = ((UNS_32) data & 0x00FF0000) >> 16;
  txdata[5] = ((UNS_32) data & 0x0000FF00) >> 8;
  txdata[6] = I2C_TXFF_STOP_CND | ((UNS_32) data & 0x000000FF) >> 0;
  xfer.tx_data = txdata;
  xfer.rx_data = rxdata;
  xfer.tx_length = 7;
  xfer.clock_rate = 100000;
  xfer.cb = NULL;

  if (i2c_mstr_ioctl(i2chandle, I2C_MSTR_TRANSFER, (INT_32) &xfer) ==
      _NO_ERROR)
  {
    status = i2c_mstr_ioctl(i2chandle, I2C_MSTR_GET_STATUS, 0);
    while ((status & (I2C_MSTR_STATUS_ARBF |
                      I2C_MSTR_STATUS_NOACKF | I2C_MSTR_STATUS_DONE)) == 0)
    {
      status = i2c_mstr_ioctl(i2chandle,
                              I2C_MSTR_GET_STATUS, 0);
    }
  }

  return data;
}

/***********************************************************************
 *
 * Function: lpc3152_codec_read
 *
 * Purpose: Read a CODEC register
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     reg  : register to read
 *
 * Outputs: None
 *
 * Returns: Value read from register
 *
 * Notes: None
 *
 **********************************************************************/
UNS_32 lpc3152_codec_read(UNS_16 reg)
{
  UNS_32 data;
  UNS_32  rxdata[8], txdata[8];
  I2C_MTXRX_SETUP_T xfer;
  UNS_32 status;

  txdata[0] = drvdata.i2caddr | I2C_TXFF_START_CND | 0;
  txdata[1] = ((UNS_32) reg & 0xFF00) >> 8;
  txdata[2] = ((UNS_32) reg & 0x00FF) >> 0;
  txdata[3] = drvdata.i2caddr | I2C_TXFF_START_CND | 1;
  txdata[4] = 0;
  txdata[5] = 0;
  txdata[6] = 0;
  txdata[7] = I2C_TXFF_STOP_CND | 0;
  xfer.tx_data = txdata;
  xfer.rx_data = rxdata;
  xfer.tx_length = 8;
  xfer.clock_rate = 100000;
  xfer.cb = NULL;

  if (i2c_mstr_ioctl(i2chandle, I2C_MSTR_TRANSFER, (INT_32) &xfer) ==
      _NO_ERROR)
  {
    status = i2c_mstr_ioctl(i2chandle, I2C_MSTR_GET_STATUS, 0);
    while ((status & (I2C_MSTR_STATUS_ARBF |
                      I2C_MSTR_STATUS_NOACKF | I2C_MSTR_STATUS_DONE)) == 0)
    {
      status = i2c_mstr_ioctl(i2chandle,
                              I2C_MSTR_GET_STATUS, 0);
    }
  }

  data = ((rxdata[3] & 0xFF) | ((rxdata[2] & 0xFF) << 8) |
          ((rxdata[1] & 0xFF) << 16) | ((rxdata[0] & 0xFF) << 24));

  return data;
}

/***********************************************************************
 *
 * Function: lpc3152_codec_mute
 *
 * Purpose: Mute or unmute audio output
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     mute : !0 to muye, 0 to unmute
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void lpc3152_codec_mute(INT_32 mute)
{
  UNS_32 curval = lpc3152_codec_read(AD_REG_INT_1_RW);

  if (mute)
  {
    curval |= AD_INT1_CTRL_MUTE;
  }
  else
  {
    curval &= ~AD_INT1_CTRL_MUTE;
  }

  lpc3152_codec_write(AD_REG_INT_1_RW, curval);
}

/***********************************************************************
 *
 * Function: lpc3152_codec_input_mute
 *
 * Purpose: Mute or unmute audio input
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     mute : !0 to muye, 0 to unmute
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void lpc3152_codec_input_mute(INT_32 mute)
{
  UNS_32 curval = lpc3152_codec_read(AD_REG_DEC_RW);

  if (mute)
  {
    curval |= (AD_DEC_CTRL_EN_MUTE);
  }
  else
  {
    curval &= ~(AD_DEC_CTRL_EN_MUTE);
  }

  lpc3152_codec_write(AD_REG_DEC_RW, curval);
}

/***********************************************************************
 *
 * Function: lpc3152_codec_set_volume
 *
 * Purpose: Set volume level
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     left_vol  : Left volume value, 0 (max) to 252 (min)
 *     right_vol : Right volume value, 0 (max) to 252 (min)
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void lpc3152_codec_set_volume(UNS_8 left_vol, UNS_8 right_vol)
{
  UNS_32 vol = lpc3152_codec_read(AD_REG_INT_1_RW) & ~0xFFFF;

  vol |= (UNS_32) right_vol | (((UNS_32) left_vol) << 8);
  lpc3152_codec_write(AD_REG_INT_1_RW, vol);
}

/***********************************************************************
 *
 * Function: lpc3152_codec_set_input_volume
 *
 * Purpose: Set volume level for Audio input
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     left_vol  : Left volume value, 0 (max) to 252 (min)
 *     right_vol : Right volume value, 0 (max) to 252 (min)
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void lpc3152_codec_set_input_volume(UNS_8 left_vol, UNS_8 right_vol)
{
  UNS_32 vol = lpc3152_codec_read(AD_REG_DEC_RW) & ~0xFFFF;

  vol |= (UNS_32) right_vol | (((UNS_32) left_vol) << 8);
  lpc3152_codec_write(AD_REG_DEC_RW, vol);
}

/***********************************************************************
 *
 * Function: lpc3152_codec_enable_audio_out
 *
 * Purpose: Enable or disable audio output
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *    enable : !0 to enable audio output, 0 to disable
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void lpc3152_codec_enable_audio_out(INT_32 enable)
{
  UNS_32 curval;

  /* Analog output uses the interpolator and analog out blocks */
  if (enable)
  {
    /* Power up Clocks in Analog CODEC
     * CLK_256FS, DSP Clock, Interpolator Clock,
     * I2SRX_BITCLK, I2SRX_SYSCLK, DAC_CLK
     * Set clock config as shown below:
     * Interpolator clock - 128fs
     * NS clock - 128fs
     * I2C Clock - SYSCLK
     * DAC polarity normal
     * */
    curval = lpc3152_codec_read(AD_REG_CGU_RW);
    curval &= ~((AD_CGU_I2C_CLK_256FS_PD) |
                (AD_CGU_INT_CLK_DSP_PD) |
                (AD_CGU_INT_CLK_NS_PD) |
                (AD_CGU_AOUT_CLK_DAC_PD) |
                (AD_CGU_I2SRX_BCK_PD) |
                (AD_CGU_I2SRX_SYSCLK_PD));
    lpc3152_codec_write(AD_REG_CGU_RW, curval);

    /* Switch on HeadPhone Amplifiers */
    curval = lpc3152_codec_read(AD_REG_AOUT_RW);
    curval &= ~((AD_AOUT_CTRL_HP_L_PD) |
                (AD_AOUT_CTRL_HP_R_PD));
    lpc3152_codec_write(AD_REG_AOUT_RW, curval);

    /* Enable VREF SLOW */
    curval = lpc3152_codec_read(AD_REG_AOUT_RW);
    curval &= ~(AD_AOUT_CTRL_VREF_SLOW_PD);
    lpc3152_codec_write(AD_REG_AOUT_RW, curval);

    /* Power up VCOM_VREF1, BIAS,  */
    curval = lpc3152_codec_read(AD_REG_AIN_1_RW);
    curval &= ~((AD_AIN1_CTRL_VCOM_REF1_PD) |
                (AD_AIN1_CTRL_BIAS1_PD));
    lpc3152_codec_write(AD_REG_AIN_1_RW, curval);

    /* Power down PD_DAC */
    curval = lpc3152_codec_read(AD_REG_INT_0_RW);
    curval |= (AD_INT0_CTRL_DAC_PD);
    lpc3152_codec_write(AD_REG_INT_0_RW, curval);

    /* Connect DAC output to HP */
    curval = lpc3152_codec_read(AD_REG_AOUT_RW);
    curval |= (AD_AOUT_CTRL_SWDAC_ON);
    lpc3152_codec_write(AD_REG_AOUT_RW, curval);

    /* Enable DACs */
    curval = lpc3152_codec_read(AD_REG_AOUT_RW);
    curval &= ~((AD_AOUT_CTRL_SDAC_R_PD) |
                (AD_AOUT_CTRL_SDAC_L_PD));
    lpc3152_codec_write(AD_REG_AOUT_RW, curval);

    /* Power up PD_DAC */
    curval = lpc3152_codec_read(AD_REG_INT_0_RW);
    curval &= ~(AD_INT0_CTRL_DAC_PD);
    lpc3152_codec_write(AD_REG_INT_0_RW, curval);
    timer_wait_ms(NULL, 30);

    /* Ramp up VREF SLOW */
    curval = lpc3152_codec_read(AD_REG_AOUT_RW);
    curval |= (AD_AOUT_CTRL_VREF_SLOW_UP);
    lpc3152_codec_write(AD_REG_AOUT_RW, curval);
    timer_wait_ms(NULL, 500);

    /* Unmute Interpolator */
    curval = lpc3152_codec_read(AD_REG_INT_1_RW);
    curval &= ~(AD_INT1_CTRL_MUTE | 0x0000FFFF);
    lpc3152_codec_write(AD_REG_INT_1_RW, curval);

    /* Unmute AVC, set AVC volume */
    curval = lpc3152_codec_read(AD_REG_AOUT_RW);
    curval &= ~(AD_AOUT_CTRL_GAIN_AVC_COA_MSK |
                AD_AOUT_CTRL_GAIN_AVC_FIN_MSK);
    curval |= ((0x7C0 << AD_AOUT_CTRL_GAIN_AVC_COA_POS) |
               (0x1 << AD_AOUT_CTRL_GAIN_AVC_FIN_POS));
    lpc3152_codec_write(AD_REG_AOUT_RW, curval);
  }
  else
  {
    /* Power down seuqence */
    /* Mute Interpolator */
    curval = lpc3152_codec_read(AD_REG_INT_1_RW);
    curval |= (AD_INT1_CTRL_MUTE);
    lpc3152_codec_write(AD_REG_INT_1_RW, curval);

    /* Mute AVC */
    curval = lpc3152_codec_read(AD_REG_AOUT_RW);
    curval &= ~(AD_AOUT_CTRL_GAIN_AVC_COA_MSK |
                AD_AOUT_CTRL_GAIN_AVC_FIN_MSK);
    lpc3152_codec_write(AD_REG_AOUT_RW, curval);

    /* Ramp down VREF SLOW */
    curval = lpc3152_codec_read(AD_REG_AOUT_RW);
    curval &= ~(AD_AOUT_CTRL_VREF_SLOW_UP);
    lpc3152_codec_write(AD_REG_AOUT_RW, curval);
    timer_wait_ms(NULL, 500);

    /* Power down PD_DAC */
    curval = lpc3152_codec_read(AD_REG_INT_0_RW);
    curval |= (AD_INT0_CTRL_DAC_PD);
    lpc3152_codec_write(AD_REG_INT_0_RW, curval);
    timer_wait_ms(NULL, 30);

    /* Disconnect DAC output from HP */
    curval = lpc3152_codec_read(AD_REG_AOUT_RW);
    curval &= ~(AD_AOUT_CTRL_SWDAC_ON);
    lpc3152_codec_write(AD_REG_AOUT_RW, curval);

    /* Disable DACs */
    curval = lpc3152_codec_read(AD_REG_AOUT_RW);
    curval |= ((AD_AOUT_CTRL_SDAC_R_PD) |
               (AD_AOUT_CTRL_SDAC_L_PD));
    lpc3152_codec_write(AD_REG_AOUT_RW, curval);

    /* Power down VCOM_VREF1 & BIAS */
    curval = lpc3152_codec_read(AD_REG_AIN_1_RW);
    curval |= ((AD_AIN1_CTRL_VCOM_REF1_PD) |
               (AD_AIN1_CTRL_BIAS1_PD));
    lpc3152_codec_write(AD_REG_AIN_1_RW, curval);

    /* Disable VREF SLOW */
    curval = lpc3152_codec_read(AD_REG_AOUT_RW);
    curval |= (AD_AOUT_CTRL_VREF_SLOW_PD);
    lpc3152_codec_write(AD_REG_AOUT_RW, curval);

    /* Switch off HeadPhone Amplifiers */
    curval = lpc3152_codec_read(AD_REG_AOUT_RW);
    curval |= ((AD_AOUT_CTRL_HP_R_PD) |
               //(AD_AOUT_CTRL_HP_C_PD) |
               (AD_AOUT_CTRL_HP_L_PD));
    lpc3152_codec_write(AD_REG_AOUT_RW, curval);

    /* Power down Clocks */
    curval = lpc3152_codec_read(AD_REG_CGU_RW);
    curval |= ((AD_CGU_I2C_CLK_256FS_PD) |
               (AD_CGU_INT_CLK_DSP_PD) |
               (AD_CGU_INT_CLK_NS_PD) |
               (AD_CGU_AOUT_CLK_DAC_PD) |
               (AD_CGU_I2SRX_BCK_PD) |
               (AD_CGU_I2SRX_SYSCLK_PD));
    lpc3152_codec_write(AD_REG_CGU_RW, curval);
  }
}

/***********************************************************************
 *
 * Function: lpc3152_codec_enable_audio_in
 *
 * Purpose: Enable or disable audio input
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *    enable : !0 to enable audio input, 0 to disable
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void lpc3152_codec_enable_audio_in(INT_32 enable)
{
  UNS_32 curval;

  /* Analog Input uses Decimator and analog out blocks */
  if (enable)
  {
    /* Power up Clocks in Analog CODEC
     * CLK_256FS, DSP Clock, Interpolator Clock,
     * I2STX_BITCLK, I2STX_SYSCLK, ADC_CLK
     * Set clock config as shown below:
     * I2C Clock - SYSCLK
     * */
    curval = lpc3152_codec_read(AD_REG_CGU_RW);
    curval &= ~((AD_CGU_I2C_CLK_256FS_PD) |
                (AD_CGU_I2STX_BCK_PD) |
                (AD_CGU_I2STX_SYSCLK_PD) |
                (AD_CGU_AIN_CLK_ADC1_PD) |
                (AD_CGU_AIN_CLK_ADC2_PD) |
                (AD_CGU_AIN_CLK_ADCSYS_PD) |
                (AD_CGU_DEC_CLK_PD));
    lpc3152_codec_write(AD_REG_CGU_RW, curval);

    /* Disbale VREF SLOW */
    curval = lpc3152_codec_read(AD_REG_AOUT_RW);
    curval &= ~(AD_AOUT_CTRL_VREF_SLOW_PD);
    lpc3152_codec_write(AD_REG_AOUT_RW, curval);

    /* Power up: VCOM_REF1 */
    curval = lpc3152_codec_read(AD_REG_AIN_1_RW);
    curval &= ~((AD_AIN1_CTRL_VCOM_REF1_PD) |
                (AD_AIN1_CTRL_BIAS1_PD));
    lpc3152_codec_write(AD_REG_AIN_1_RW, curval);

    /* Ramp up VREF SLOW */
    curval = lpc3152_codec_read(AD_REG_AOUT_RW);
    curval |= (AD_AOUT_CTRL_VREF_SLOW_UP);
    lpc3152_codec_write(AD_REG_AOUT_RW, curval);
    timer_wait_ms(NULL, 500);

    /* Unmute Decimator, Set Decimator gain & Enable AGC */
    curval = lpc3152_codec_read(AD_REG_DEC_RW);
    curval &= ~((0x0000FFFF) | AD_DEC_CTRL_EN_MUTE);
    curval |= ((AD_DEC_CTRL_GAIN_R(0x40)) |
               (AD_DEC_CTRL_GAIN_L(0x40)) |
               (AD_DEC_CTRL_EN_DCFILTO) |
               (AD_DEC_CTRL_EN_DCFILTI));
    lpc3152_codec_write(AD_REG_DEC_RW, curval);

    /* Power up:
    	 ADC Left channel, SDC Mic,
       PGA Mic, LNA Mic
    	 Enable Left & Right Mux to Mic Input
    	 */
    curval = lpc3152_codec_read(AD_REG_AIN_1_RW);
    curval &=	~((AD_AIN1_CTRL_ADCR_PD) |
                (AD_AIN1_CTRL_ADCL_PD) |
                (AD_AIN1_CTRL_SDCM_PD) |
                (AD_AIN1_CTRL_PGAM_PD) |
                (AD_AIN1_CTRL_LNA_PD));
    curval |= ((AD_AIN1_CTRL_MUX_R(0x3)) |
               (AD_AIN1_CTRL_MUX_L(0x3)));
    lpc3152_codec_write(AD_REG_AIN_1_RW, curval);
  }
  else
  {
    /* Power down seuqence */
    /* Mute Decimator & Disable AGC */
    curval = lpc3152_codec_read(AD_REG_DEC_RW);
    curval &= ~((0x0000FFFF) | AD_DEC_CTRL_EN_AGC |
                AD_DEC_CTRL_EN_DCFILTO |
                AD_DEC_CTRL_EN_DCFILTI);
    curval |= AD_DEC_CTRL_EN_MUTE;
    lpc3152_codec_write(AD_REG_DEC_RW, curval);

    /* Ramp down VREF SLOW */
    curval = lpc3152_codec_read(AD_REG_AOUT_RW);
    curval &= ~(AD_AOUT_CTRL_VREF_SLOW_UP);
    lpc3152_codec_write(AD_REG_AOUT_RW, curval);
    timer_wait_ms(NULL, 500);

    /* Power down: VCOM_REF1 */
    curval = lpc3152_codec_read(AD_REG_AIN_1_RW);
    curval |= ((AD_AIN1_CTRL_VCOM_REF1_PD) |
               (AD_AIN1_CTRL_BIAS1_PD));
    lpc3152_codec_write(AD_REG_AIN_1_RW, curval);

    /* Ramp down VREF SLOW */
    curval = lpc3152_codec_read(AD_REG_AOUT_RW);
    curval |= (AD_AOUT_CTRL_VREF_SLOW_PD);
    lpc3152_codec_write(AD_REG_AOUT_RW, curval);

    /* Power down:
    	 ADC Left channel, SDC Mic,
       PGA Mic, LNA Mic
    	 Enable Left & Right Mux to Mic Input
    	 */
    curval = lpc3152_codec_read(AD_REG_AIN_1_RW);
    curval |=	((AD_AIN1_CTRL_ADCR_PD) |
               (AD_AIN1_CTRL_ADCL_PD) |
               (AD_AIN1_CTRL_SDCM_PD) |
               (AD_AIN1_CTRL_PGAM_PD) |
               (AD_AIN1_CTRL_LNA_PD));
    curval &= ~((AD_AIN1_CTRL_MUX_R(0x3)) |
                (AD_AIN1_CTRL_MUX_L(0x3)));
    curval |= ((AD_AIN1_CTRL_MUX_R(0x1)) |
               (AD_AIN1_CTRL_MUX_L(0x1)));
    lpc3152_codec_write(AD_REG_AIN_1_RW, curval);

    /* Power down Clocks in Analog CODEC
     * CLK_256FS, DSP Clock, Interpolator Clock,
     * I2STX_BITCLK, I2STX_SYSCLK, ADC_CLK
     * Set clock config as shown below:
     * I2C Clock - SYSCLK
     * */
    curval = lpc3152_codec_read(AD_REG_CGU_RW);
    curval |= ((AD_CGU_I2C_CLK_256FS_PD) |
               (AD_CGU_I2STX_BCK_PD) |
               (AD_CGU_I2STX_SYSCLK_PD) |
               (AD_CGU_AIN_CLK_ADC1_PD) |
               (AD_CGU_AIN_CLK_ADC2_PD) |
               (AD_CGU_AIN_CLK_ADCSYS_PD) |
               (AD_CGU_DEC_CLK_PD));
    lpc3152_codec_write(AD_REG_CGU_RW, curval);
  }
}

/***********************************************************************
 *
 * Function: lpc3152_codec_enable_audio_sys
 *
 * Purpose: Enable or disable both audio output & input paths
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *    enable : !0 to enable audio input, 0 to disable
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void lpc3152_codec_enable_audio_sys(INT_32 enable)
{
  UNS_32 curval;

  /* Analog output uses the interpolator and analog out blocks */
  if (enable)
  {
    /* Power up Clocks in Analog CODEC
     * CLK_256FS, DSP Clock, Interpolator Clock,
     * I2SRX_BITCLK, I2SRX_SYSCLK, DAC_CLK
     * Set clock config as shown below:
     * Interpolator clock - 128fs
     * NS clock - 128fs
     * I2C Clock - SYSCLK
     * DAC polarity normal
     * */
    /* Power up Clocks in Analog CODEC
     * CLK_256FS, DSP Clock, Interpolator Clock,
     * I2STX_BITCLK, I2STX_SYSCLK, ADC_CLK
     * Set clock config as shown below:
     * I2C Clock - SYSCLK
     * */
    curval = lpc3152_codec_read(AD_REG_CGU_RW);
    curval &= ~((AD_CGU_I2C_CLK_256FS_PD) |
                (AD_CGU_INT_CLK_DSP_PD) |
                (AD_CGU_INT_CLK_NS_PD) |
                (AD_CGU_AOUT_CLK_DAC_PD) |
                (AD_CGU_I2SRX_BCK_PD) |
                (AD_CGU_I2SRX_SYSCLK_PD) |
                (AD_CGU_I2STX_BCK_PD) |
                (AD_CGU_I2STX_SYSCLK_PD) |
                (AD_CGU_AIN_CLK_ADC1_PD) |
                (AD_CGU_AIN_CLK_ADC2_PD) |
                (AD_CGU_AIN_CLK_ADCSYS_PD) |
                (AD_CGU_DEC_CLK_PD));
    lpc3152_codec_write(AD_REG_CGU_RW, curval);

    /* Switch on HeadPhone Amplifiers */
    curval = lpc3152_codec_read(AD_REG_AOUT_RW);
    curval &= ~((AD_AOUT_CTRL_HP_L_PD) |
                (AD_AOUT_CTRL_HP_R_PD));
    lpc3152_codec_write(AD_REG_AOUT_RW, curval);

    /* Enable VREF SLOW */
    curval = lpc3152_codec_read(AD_REG_AOUT_RW);
    curval &= ~(AD_AOUT_CTRL_VREF_SLOW_PD);
    lpc3152_codec_write(AD_REG_AOUT_RW, curval);

    /* Power up VCOM_VREF1, BIAS,  */
    curval = lpc3152_codec_read(AD_REG_AIN_1_RW);
    curval &= ~((AD_AIN1_CTRL_VCOM_REF1_PD) |
                (AD_AIN1_CTRL_BIAS1_PD));
    lpc3152_codec_write(AD_REG_AIN_1_RW, curval);

    /* Power down PD_DAC */
    curval = lpc3152_codec_read(AD_REG_INT_0_RW);
    curval |= (AD_INT0_CTRL_DAC_PD);
    lpc3152_codec_write(AD_REG_INT_0_RW, curval);

    /* Connect DAC output to HP */
    curval = lpc3152_codec_read(AD_REG_AOUT_RW);
    curval |= (AD_AOUT_CTRL_SWDAC_ON);
    lpc3152_codec_write(AD_REG_AOUT_RW, curval);

    /* Enable DACs */
    curval = lpc3152_codec_read(AD_REG_AOUT_RW);
    curval &= ~((AD_AOUT_CTRL_SDAC_R_PD) |
                (AD_AOUT_CTRL_SDAC_L_PD));
    lpc3152_codec_write(AD_REG_AOUT_RW, curval);

    /* Power up PD_DAC */
    curval = lpc3152_codec_read(AD_REG_INT_0_RW);
    curval &= ~(AD_INT0_CTRL_DAC_PD);
    lpc3152_codec_write(AD_REG_INT_0_RW, curval);
    timer_wait_ms(NULL, 30);

    /* Ramp up VREF SLOW */
    curval = lpc3152_codec_read(AD_REG_AOUT_RW);
    curval |= (AD_AOUT_CTRL_VREF_SLOW_UP);
    lpc3152_codec_write(AD_REG_AOUT_RW, curval);
    timer_wait_ms(NULL, 500);

    /* Unmute AVC, set AVC volume */
    curval = lpc3152_codec_read(AD_REG_AOUT_RW);
    curval &= ~(AD_AOUT_CTRL_GAIN_AVC_COA_MSK |
                AD_AOUT_CTRL_GAIN_AVC_FIN_MSK);
    curval |= ((0x7C0 << AD_AOUT_CTRL_GAIN_AVC_COA_POS) |
               (0x1 << AD_AOUT_CTRL_GAIN_AVC_FIN_POS));
    lpc3152_codec_write(AD_REG_AOUT_RW, curval);

    /* Unmute Decimator, Set Decimator gain & Enable AGC */
    curval = lpc3152_codec_read(AD_REG_DEC_RW);
    curval &= ~((0x0000FFFF) | AD_DEC_CTRL_EN_MUTE);
    curval |= ((AD_DEC_CTRL_GAIN_R(0x40)) |
               (AD_DEC_CTRL_GAIN_L(0x40)) |
               (AD_DEC_CTRL_EN_DCFILTO) |
               (AD_DEC_CTRL_EN_DCFILTI));
    lpc3152_codec_write(AD_REG_DEC_RW, curval);

    /* Power up:
    	 ADC Left channel, SDC Mic,
       PGA Mic, LNA Mic
    	 Enable Left & Right Mux to Mic Input
    	 */
    curval = lpc3152_codec_read(AD_REG_AIN_1_RW);
    curval &=	~((AD_AIN1_CTRL_ADCR_PD) |
                (AD_AIN1_CTRL_ADCL_PD) |
                (AD_AIN1_CTRL_SDCM_PD) |
                (AD_AIN1_CTRL_PGAM_PD) |
                (AD_AIN1_CTRL_LNA_PD));
    curval |= ((AD_AIN1_CTRL_MUX_R(0x3)) |
               (AD_AIN1_CTRL_MUX_L(0x3)));
    lpc3152_codec_write(AD_REG_AIN_1_RW, curval);
  }
  else
  {
    /* Power down seuqence */
    /* Mute Interpolator */
    curval = lpc3152_codec_read(AD_REG_INT_1_RW);
    curval |= (AD_INT1_CTRL_MUTE);
    lpc3152_codec_write(AD_REG_INT_1_RW, curval);

    /* Mute AVC */
    curval = lpc3152_codec_read(AD_REG_AOUT_RW);
    curval &= ~(AD_AOUT_CTRL_GAIN_AVC_COA_MSK |
                AD_AOUT_CTRL_GAIN_AVC_FIN_MSK);
    lpc3152_codec_write(AD_REG_AOUT_RW, curval);

    /* Mute Decimator & Disable AGC */
    curval = lpc3152_codec_read(AD_REG_DEC_RW);
    curval &= ~((0x0000FFFF) | AD_DEC_CTRL_EN_AGC |
                AD_DEC_CTRL_EN_DCFILTO |
                AD_DEC_CTRL_EN_DCFILTI);
    curval |= AD_DEC_CTRL_EN_MUTE;
    lpc3152_codec_write(AD_REG_DEC_RW, curval);

    /* Ramp down VREF SLOW */
    curval = lpc3152_codec_read(AD_REG_AOUT_RW);
    curval &= ~(AD_AOUT_CTRL_VREF_SLOW_UP);
    lpc3152_codec_write(AD_REG_AOUT_RW, curval);
    timer_wait_ms(NULL, 500);

    /* Power down PD_DAC */
    curval = lpc3152_codec_read(AD_REG_INT_0_RW);
    curval |= (AD_INT0_CTRL_DAC_PD);
    lpc3152_codec_write(AD_REG_INT_0_RW, curval);
    timer_wait_ms(NULL, 30);

    /* Disconnect DAC output from HP */
    curval = lpc3152_codec_read(AD_REG_AOUT_RW);
    curval &= ~(AD_AOUT_CTRL_SWDAC_ON);
    lpc3152_codec_write(AD_REG_AOUT_RW, curval);

    /* Disable DACs */
    curval = lpc3152_codec_read(AD_REG_AOUT_RW);
    curval |= ((AD_AOUT_CTRL_SDAC_R_PD) |
               (AD_AOUT_CTRL_SDAC_L_PD));
    lpc3152_codec_write(AD_REG_AOUT_RW, curval);

    /* Power down VCOM_VREF1 & BIAS */
    curval = lpc3152_codec_read(AD_REG_AIN_1_RW);
    curval |= ((AD_AIN1_CTRL_VCOM_REF1_PD) |
               (AD_AIN1_CTRL_BIAS1_PD));
    lpc3152_codec_write(AD_REG_AIN_1_RW, curval);

    /* Disable VREF SLOW */
    curval = lpc3152_codec_read(AD_REG_AOUT_RW);
    curval |= (AD_AOUT_CTRL_VREF_SLOW_PD);
    lpc3152_codec_write(AD_REG_AOUT_RW, curval);

    /* Switch off HeadPhone Amplifiers */
    curval = lpc3152_codec_read(AD_REG_AOUT_RW);
    curval |= ((AD_AOUT_CTRL_HP_R_PD) |
               //(AD_AOUT_CTRL_HP_C_PD) |
               (AD_AOUT_CTRL_HP_L_PD));
    lpc3152_codec_write(AD_REG_AOUT_RW, curval);

    /* Power down:
    	 ADC Left channel, SDC Mic,
       PGA Mic, LNA Mic
    	 Enable Left & Right Mux to Mic Input
    	 */
    curval = lpc3152_codec_read(AD_REG_AIN_1_RW);
    curval |=	((AD_AIN1_CTRL_ADCR_PD) |
               (AD_AIN1_CTRL_ADCL_PD) |
               (AD_AIN1_CTRL_SDCM_PD) |
               (AD_AIN1_CTRL_PGAM_PD) |
               (AD_AIN1_CTRL_LNA_PD));
    curval &= ~((AD_AIN1_CTRL_MUX_R(0x3)) |
                (AD_AIN1_CTRL_MUX_L(0x3)));
    curval |= ((AD_AIN1_CTRL_MUX_R(0x1)) |
               (AD_AIN1_CTRL_MUX_L(0x1)));
    lpc3152_codec_write(AD_REG_AIN_1_RW, curval);

    /* Power down Clocks */
    curval = lpc3152_codec_read(AD_REG_CGU_RW);
    curval |= ((AD_CGU_I2C_CLK_256FS_PD) |
               (AD_CGU_INT_CLK_DSP_PD) |
               (AD_CGU_INT_CLK_NS_PD) |
               (AD_CGU_AOUT_CLK_DAC_PD) |
               (AD_CGU_I2SRX_BCK_PD) |
               (AD_CGU_I2SRX_SYSCLK_PD) |
               (AD_CGU_I2STX_BCK_PD) |
               (AD_CGU_I2STX_SYSCLK_PD) |
               (AD_CGU_AIN_CLK_ADC1_PD) |
               (AD_CGU_AIN_CLK_ADC2_PD) |
               (AD_CGU_AIN_CLK_ADCSYS_PD) |
               (AD_CGU_DEC_CLK_PD));
    lpc3152_codec_write(AD_REG_CGU_RW, curval);
  }
}
