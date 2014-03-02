/***********************************************************************
 * $Id:: lpc313x_i2s_clock_driver.h 3557 2010-05-20 00:31:30Z usb10131 $
 *
 * Project: LPC313X I2S clock control driver
 *
 * Description:
 *     This file provides the necessary clock rate setup functions for
 *     The I2S signals on the LPC313X. These functions include clock
 *     enable and disable, and clock frequency setup on the WS, BCLK,
 *     and SYSCLK pins. All standard audio rates (8KHz, 11.025KHz,
 *     16KHz, 22.05KHz, 32KHz, 44.1KHz, 48KHz) and some substandard
 *     rates (12KHz, 24KHz) are supported with FS clock multipliers
 *     of 128, 256, 384, 512, 768, and 1024. All standard bit clock
 *     rates are supported (8 bits x 2 channels, 16-bits x 2 channels,
 *     24-bits x 2 channels).
 *
 * Notes:
 *     See the driver for more information on how to use these
 *     functions.
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
 *********************************************************************/

#ifndef LPC313X_I2S_CLOCK_DRIVER_H
#define LPC313X_I2S_CLOCK_DRIVER_H

#include "lpc_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* I2S supported clock groupings */
enum i2s_supp_clks {CLK_RX_0, CLK_TX_0, CLK_RX_1, CLK_TX_1};

/* Enabled main clock defines */
#define CLK_USE_256FS  1
#define CLK_USE_TXCLKO 2

/* Setup the FS clock rate of the audio system. This must be called
   prior to any other functions of this driver. It sets up the divider
   ratio between the WS rate and the CODEC clock rate (FS256 or
   SYSCLK). Valid rates are 128, 256, 384, 512, 768, and 1024. Also
   selects which clocks (CLK_USE_256FS or CLK_USE_CLKO or both or
   none) to use for the main CODEC clock pin. Returns the currently
   selected rate or 0 if the function fails. */
UNS_32 lpc313x_set_fsmult_rate(UNS_32 fsmult, UNS_32 main_clks);

/* Set the audio CODEC clock rate or 0 to disable the clock. This value
   is the desired audio sample rate in Hz. It is used with fsmult to
   use to generate the FS256 or SYSCLK values, which are computed from
   (fsmult * freq) */
UNS_32 lpc313x_main_clk_rate(UNS_32 freq);

/* Set a specific channel's bit clock and word select rates. This
 * applies to the channel's WS and BCLK signals. Returns the actual
 * programmed WS clock rate. Use 0 to disable. */
UNS_32 lpc313x_chan_clk_enable(enum i2s_supp_clks chclk,
                               UNS_32 ws_freq, UNS_32 bit_freq);

#ifdef __cplusplus
}
#endif

#endif /* LPC313X_I2S_CLOCK_DRIVER_H */
