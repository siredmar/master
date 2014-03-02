/***********************************************************************
 * $Id:: lpc_lcd_params.h 2272 2009-09-17 17:47:12Z wellsk             $
 *
 * Project: Sharp LCD parameters
 *
 * Description:
 *     This file contains common LCD parameters used on all Sharp
 *     evaluation boards.
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

#ifndef LPC_SHARP_LCD_PARAM_H
#define LPC_SHARP_LCD_PARAM_H

#include "lpc_types.h"

#if defined (__cplusplus)
extern "C"
{
#endif

/* LCD display types */
typedef enum
{
  TFT = 0,      /* Panel type is standard TFT */
  ADTFT,        /* Panel type is advanced TFT */
  HRTFT,        /* Panel type is highly reflective TFT */
  MONO_4BIT,    /* Panel type is 4-bit mono */
  MONO_8BIT,    /* Panel type is 8-bit mono */
  CSTN          /* Panel type is color STN */
} LCD_PANEL_T;

/* Structure containing the parameters for the LCD panel */
typedef struct
{
  UNS_8           h_back_porch;         /* Horizontal back porch in
                                             clocks (minimum of 1) */
  UNS_8           h_front_porch;        /* Horizontal front porch in
                                             clocks (minimum of 1) */
  UNS_8           h_sync_pulse_width;   /* HSYNC pulse width in
                                             clocks (minimum of 1) */
  UNS_16          pixels_per_line;      /* Pixels per line (horizontal
                                             resolution) */
  UNS_8           v_back_porch;         /* Vertical back porch in
                                             clocks */
  UNS_8           v_front_porch;        /* Vertical front porch in
                                             clocks */
  UNS_8           v_sync_pulse_width;   /* VSYNC pulse width in
                                             clocks (minimum 1 clock) */
  UNS_16          lines_per_panel;      /* Lines per panel (vertical
                                             resolution) */
  UNS_8           invert_output_enable; /* Invert output enable, 1 =
                                             invert*/
  UNS_8           invert_panel_clock;   /* Invert panel clock, 1 =
                                             invert*/
  UNS_8           invert_hsync;         /* Invert HSYNC, 1 = invert */
  UNS_8           invert_vsync;         /* Invert VSYNC, 1 = invert */
  UNS_8           ac_bias_frequency;    /* AC bias frequency in
                                             clocks (minimum 1) */
  UNS_8           bits_per_pixel;       /* Maximum bits per pixel the
                                             display supports */
  UNS_32          optimal_clock;        /* Optimal clock rate (Hz) */
  LCD_PANEL_T     lcd_panel_type;       /* LCD panel type */
  UNS_8           dual_panel;           /* Dual panel, 1 = dual panel
                                             display */

  /* The following parameters are needed for ADTFT and HRTFT panels
     only. For all other panels, these should be programmed to 0 */
  UNS_8           hrtft_cls_enable;     /* HRTFT CLS enable flag, 1 =
                                             enable */
  UNS_8           hrtft_sps_enable;     /* HRTFT SPS enable flag, 1 =
                                             enable */
  UNS_8           hrtft_lp_to_ps_delay; /* HRTFT LP to PS delay in
                                             clocks */
  UNS_8           hrtft_polarity_delay; /* HRTFT polarity delay in
                                             clocks */
  UNS_8           hrtft_lp_delay;       /* HRTFT LP delay in clocks */
  UNS_8           hrtft_spl_delay;      /* HRTFT SPL delay in
                                             clocks */
  /* HRTFT SPL to CLKS delay */
  UNS_16          hrtft_spl_to_cls_delay;
} LCD_PARAM_T;

/* Hitachi TX09D71VM1CCA portrait mode display parameters */
extern const LCD_PARAM_T hitachi_tx09d71;

/* Sharp LQ035 portrait mode ADTFT display */
extern const LCD_PARAM_T sharp_lq035;

/* Sharp LQ039 HRTFT display */
extern const LCD_PARAM_T sharp_lq039;

/* Sharp LQ050 TFT display - also works for the LQ036 and LQ038
   LCDs */
extern const LCD_PARAM_T sharp_lq050;

/* Sharp LQ057 TFT display */
extern const LCD_PARAM_T sharp_lq057;

/* Sharp LQ064 TFT display */
extern const LCD_PARAM_T sharp_lq064;

/* Sharp LQ104 TFT display */
extern const LCD_PARAM_T sharp_lq104;

/* Sharp LQ121 TFT display */
extern const LCD_PARAM_T sharp_lq121;

/* Sharp LM10V DSTN display */
extern const LCD_PARAM_T sharp_lm10v;

/* Sharp LM057QB STN display */
extern const LCD_PARAM_T sharp_lm057qb;

/* Sharp LM057QC STN display */
extern const LCD_PARAM_T sharp_lm057qc;

/* Sharp LM64K11 STN display */
extern const LCD_PARAM_T sharp_lm64k11;

extern const LCD_PARAM_T toshiba;
extern const LCD_PARAM_T okaya;

#if defined (__cplusplus)
}
#endif /*__cplusplus */

#endif /* LPC_SHARP_LCD_PARAM_H */
