/***********************************************************************
 * $Id:: lpc3152_codec_driver.h 2083 2009-07-27 16:22:29Z wellsk       $
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

#ifndef LPC3152_CODEC_DRIVER_H
#define LPC3152_CODEC_DRIVER_H

#include "lpc_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

/***********************************************************************
 * Global functions
 **********************************************************************/

/* Driver initialization - this must be called prior to any other
   function in this driver. An opened I2C driver handle must be
   passed as well as the I2C address */
INT_32 lpc3152_codec_driver_init(INT_32 i2cdev, UNS_8 i2caddr);

/* Shutdown UDA1380 */
void lpc3152_codec_shutdown(void);

/* Reset the CODEC */
void lpc3152_codec_reset(void);

/* Write data to a UDA1380 CODEC register */
UNS_32 lpc3152_codec_write(UNS_16 reg, UNS_32 data);

/* Read a CODEC register */
UNS_32 lpc3152_codec_read(UNS_16 reg);

/* Mute or unmute audio output */
void lpc3152_codec_mute(INT_32 mute);

/* Set volume level */
void lpc3152_codec_set_volume(UNS_8 left_vol, UNS_8 right_vol);

/* Enable or disable audio output */
void lpc3152_codec_enable_audio_out(INT_32 enable);

/* Enable or disable audio input */
void lpc3152_codec_enable_audio_in(INT_32 enable);

#ifdef __cplusplus
}
#endif

#endif /* LPC3152_CODEC_DRIVER_H */
