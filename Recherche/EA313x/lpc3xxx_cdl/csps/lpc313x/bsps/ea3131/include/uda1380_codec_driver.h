/***********************************************************************
 * $Id:: uda1380_codec_driver.h 6632 2011-02-28 17:38:12Z ing03005     $
 *
 * Project: UDA1380 CODEC read/write driver
 *
 * Description:
 *     Driver to read and write CODEC registers. Also provides some
 *     CODEC helper functions. All functions are blocking.
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

#ifndef UDA1380_CODEC_DRIVER_H
#define UDA1380_CODEC_DRIVER_H

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
INT_32 uda1380_driver_init(INT_32 i2cdev, UNS_8 i2caddr);

/* Shutdown UDA1380 */
void uda1380_shutdown(void);

/* Reset the CODEC */
void uda1380_reset(void);

/* Write data to a UDA1380 CODEC register */
UNS_16 uda1380_write(UNS_8 reg, UNS_16 data);

/* Set only selected bits in a UDA1380 CODEC register */
UNS_16 uda1380_set(UNS_8 reg, UNS_16 setmask);

/* Set only selected bits in a UDA1380 CODEC register */
UNS_16 uda1380_clear(UNS_8 reg, UNS_16 clearmask);

/* Read a CODEC register */
INT_32 uda1380_read(UNS_8 reg);

/* Mute or unmute audio output */
void uda1380_mute(INT_32 mute);

/* Set bass and treble boot levels */
void uda1380_set_boost(UNS_8 bass, UNS_8 treble);

/* Set volume level */
void uda1380_set_volume(UNS_8 left_vol, UNS_8 right_vol);

/* Enable or disable audio output */
int uda1380_enable_audio_out(INT_32 enable);

/* Enable or disable audio input */
int uda1380_enable_audio_in(INT_32 enable);

/* Enable or disable audio input and output */
int uda1380_enable_audio(INT_32 enable);

#ifdef __cplusplus
}
#endif

#endif /* UDA1380_CODEC_DRIVER_H */
