/***********************************************************************
 * $Id:: lpc3152_codec_driver.c 3557 2010-05-20 00:31:30Z usb10131     $
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
  drvdata.i2cdev = i2cdev;
  drvdata.i2caddr = i2caddr << 1;

  i2chandle = i2c_mstr_open((void *) I2C1, 0);
  if (i2chandle == 0)
    return _ERROR;

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
    curval |= (1 << 16);
  }
  else
  {
    curval &= ~(1 << 16);
  }

  lpc3152_codec_write(AD_REG_INT_1_RW, curval);
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
    // FIXME - the exact programming sequence for the LPC3152
    // audio CODEC is unknown. There seem to be some bits that are
    // listed as reserved that are required to change state for
    // the audio CODEC to work.
    /* Select various clocking options */
//		curval = lpc3152_codec_read(AD_REG_CGU_RW);
//		printf("0x%08x\n", curval);
//		curval &= ~((1 << 0) | (1 << 1) | (1 << 2) | (1 << 4) |
//			(1 << 5) | (1 << 11) | (1 << 15) | (1 << 16) |
//			(1 << 17) | (1 << 20));
//		curval |= (1 <<
//		lpc3152_codec_write(AD_REG_CGU_RW, curval);
    lpc3152_codec_write(AD_REG_CGU_RW, 0x03c40600);
//		printf("0x%08x\n", curval);

    /* Analog output options */
    curval = ((1 << 31) | (1 << 30) | (0x3 << 10) |
              (0x3 << 8) | (0x3 << 6) | (1 << 3) | (0x3fff << 16));
    lpc3152_codec_write(AD_REG_AOUT_RW, curval);

    /* Interpolator options (not used) */
    curval = 0x0;
    lpc3152_codec_write(AD_REG_INT_0_RW, curval);
    curval = 0x0;
    lpc3152_codec_write(AD_REG_INT_1_RW, curval);

    /* I2S multiplexer options */
    curval = ((0x3 << 8) | (0x3 << 0));
    lpc3152_codec_write(AD_REG_DAIOMUX_RW, curval);
  }
  else
  {
    // FIXME not implemented yet
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
  /* Develope me! */
}
