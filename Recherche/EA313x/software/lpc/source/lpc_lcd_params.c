/***********************************************************************
 * $Id:: lpc_lcd_params.c 2271 2009-09-17 17:46:56Z wellsk             $
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

#include "lpc_lcd_params.h"

/* Hitachi TX09D71VM1CCA portrait mode display parameters */
const LCD_PARAM_T hitachi_tx09d71 =
{
  22,       /* Horizontal back porch */
  11,       /* Horizontal front porch */
  5,        /* HSYNC pulse width */
  240,      /* Pixels per line */
  2,        /* Vertical back porch */
  1,        /* Vertical front porch */
  2,        /* VSYNC pulse width */
  320,      /* Lines per panel */
  0,        /* Do not invert output enable */
  0,        /* Invert panel clock */
  1,        /* Invert HSYNC */
  1,        /* Do not invert VSYNC */
  1,        /* AC bias frequency (not used) */
  18,       /* Bits per pixel */
  5213000,  /* Optimal clock rate (Hz) */
  TFT,   	  /* LCD panel type */
  0,        /* Single panel display */
  0,        /* HRTFT CLS enable flag */
  0,        /* HRTFT SPS enable flag */
  0,        /* HRTFT LP to PS delay */
  0,        /* HRTFT polarity delay */
  0,        /* HRTFT LP delay */
  0,        /* HRTFT SPL delay */
  0         /* HRTFT SPL to CLKS delay */
};

/* Sharp LQ035 portrait mode ADTFT display parameters */
const LCD_PARAM_T sharp_lq035 =
{
  21,       /* Horizontal back porch */
  11,       /* Horizontal front porch */
  13,       /* HSYNC pulse width */
  240,      /* Pixels per line */
  3,        /* Vertical back porch */
  4,        /* Vertical front porch */
  2,        /* VSYNC pulse width */
  320,      /* Lines per panel */
  0,        /* Do not invert output enable */
  1,        /* Invert panel clock */
  1,        /* Invert HSYNC */
  0,        /* Do not invert VSYNC */
  1,        /* AC bias frequency (not used) */
  18,       /* Bits per pixel */
  5213000,  /* Optimal clock rate (Hz) */
  ADTFT,    /* LCD panel type */
  0,        /* Single panel display */
  1,        /* HRTFT CLS enable flag */
  1,        /* HRTFT SPS enable flag */
  9,        /* HRTFT LP to PS delay */
  3,        /* HRTFT polarity delay */
  14,       /* HRTFT LP delay */
  34,       /* HRTFT SPL delay */
  209       /* HRTFT SPL to CLKS delay */
};

/* Sharp LQ039 HRTFT display parameters */
const LCD_PARAM_T sharp_lq039 =
{
  21,       /* Horizontal back porch */
  11,       /* Horizontal front porch */
  13,       /* HSYNC pulse width */
  320,      /* Pixels per line */
  3,        /* Vertical back porch */
  4,        /* Vertical front porch */
  2,        /* VSYNC pulse width */
  240,      /* Lines per panel */
  0,        /* Do not invert output enable */
  1,        /* Invert panel clock */
  1,        /* Invert HSYNC */
  0,        /* Do not invert VSYNC */
  1,        /* AC bias frequency (not used) */
  18,       /* Bits per pixel */
  5213000,  /* Optimal clock rate (Hz) */
  HRTFT,    /* LCD panel type */
  0,        /* Single panel display */
  1,        /* HRTFT CLS enable flag */
  1,        /* HRTFT SPS enable flag */
  9,        /* HRTFT LP to PS delay */
  3,        /* HRTFT polarity delay */
  14,       /* HRTFT LP delay */
  34,       /* HRTFT SPL delay */
  209       /* HRTFT SPL to CLKS delay */
};

/* Sharp LQ050 TFT display parameters */
const LCD_PARAM_T sharp_lq050 =
{
  21,       /* Horizontal back porch */
  11,       /* Horizontal front porch */
  13,       /* HSYNC pulse width */
  320,      /* Pixels per line */
  8,        /* Vertical back porch */
  5,        /* Vertical front porch */
  2,        /* VSYNC pulse width */
  240,      /* Lines per panel */
  0,        /* Do not invert output enable */
  0,        /* Do not invert panel clock */
  1,        /* Invert HSYNC */
  0,        /* Do not invert VSYNC */
  1,        /* AC bias frequency (not used) */
  18,       /* Bits per pixel */
  5213000,  /* Optimal clock rate (Hz) */
  TFT,      /* LCD panel type (NA in TFT mode) */
  0,        /* Single panel display */
  0,        /* HRTFT CLS enable flag (NA in TFT mode) */
  0,        /* HRTFT SPS enable flag (NA in TFT mode) */
  0,        /* HRTFT LP to PS delay (NA in TFT mode) */
  0,        /* HRTFT polarity delay (NA in TFT mode) */
  0,        /* HRTFT LP delay (NA in TFT mode) */
  0,        /* HRTFT SPL delay (NA in TFT mode) */
  0         /* HRTFT SPL to CLKS delay (NA in TFT mode) */
};

/* Sharp LQ057 TFT display parameters */
const LCD_PARAM_T sharp_lq057 =
{
  21,       /* Horizontal back porch */
  11,       /* Horizontal front porch */
  13,       /* HSYNC pulse width */
  320,      /* Pixels per line */
  8,        /* Vertical back porch */
  5,        /* Vertical front porch */
  2,        /* VSYNC pulse width */
  240,      /* Lines per panel */
  0,        /* Do not invert output enable */
  0,        /* Do not invert panel clock */
  1,        /* Invert HSYNC */
  1,        /* Invert VSYNC */
  1,        /* AC bias frequency (not used) */
  18,       /* Bits per pixel */
  5213000,  /* Optimal clock rate (Hz) */
  TFT,      /* LCD panel type (NA in TFT mode) */
  0,        /* Single panel display */
  0,        /* HRTFT CLS enable flag (NA in TFT mode) */
  0,        /* HRTFT SPS enable flag (NA in TFT mode) */
  0,        /* HRTFT LP to PS delay (NA in TFT mode) */
  0,        /* HRTFT polarity delay (NA in TFT mode) */
  0,        /* HRTFT LP delay (NA in TFT mode) */
  0,        /* HRTFT SPL delay (NA in TFT mode) */
  0         /* HRTFT SPL to CLKS delay (NA in TFT mode) */
};

/* Sharp LQ064 TFT display parameters */
const LCD_PARAM_T sharp_lq064 =
{
  89,       /* Horizontal back porch */
  50,       /* Horizontal front porch */
  14,       /* HSYNC pulse width */
  640,      /* Pixels per line */
  18,       /* Vertical back porch */
  94,       /* Vertical front porch */
  17,       /* VSYNC pulse width */
  480,      /* Lines per panel */
  0,        /* Do not invert output enable */
  0,        /* Do not invert panel clock */
  1,        /* Invert HSYNC */
  1,        /* Invert VSYNC */
  1,        /* AC bias frequency (not used) */
  18,       /* Bits per pixel */
  25180000, /* Optimal clock rate (Hz) */
  TFT,      /* LCD panel type (NA in TFT mode) */
  0,        /* Single panel display */
  0,        /* HRTFT CLS enable flag (NA in TFT mode) */
  0,        /* HRTFT SPS enable flag (NA in TFT mode) */
  0,        /* HRTFT LP to PS delay (NA in TFT mode) */
  0,        /* HRTFT polarity delay (NA in TFT mode) */
  0,        /* HRTFT LP delay (NA in TFT mode) */
  0,        /* HRTFT SPL delay (NA in TFT mode) */
  0         /* HRTFT SPL to CLKS delay (NA in TFT mode) */
};
#if 0
//original values
/* Sharp LQ104 TFT display parameters */
const LCD_PARAM_T sharp_lq104 =
{
  89,       /* Horizontal back porch */
  169,      /* Horizontal front porch */
  129,      /* HSYNC pulse width */
  640,      /* Pixels per line */
  32,       /* Vertical back porch */
  242,      /* Vertical front porch */
  3,        /* VSYNC pulse width */
  480,      /* Lines per panel */
  0,        /* Do not invert output enable */
  0,        /* Do not invert panel clock */
  1,        /* Do not invert HSYNC */
  1,        /* Invert VSYNC */
  1,        /* AC bias frequency (not used) */
  18,       /* Bits per pixel */
  25180000, /* Optimal clock rate (Hz) */
  TFT,      /* LCD panel type (NA in TFT mode) */
  0,        /* Single panel display */
  0,        /* HRTFT CLS enable flag (NA in TFT mode) */
  0,        /* HRTFT SPS enable flag (NA in TFT mode) */
  0,        /* HRTFT LP to PS delay (NA in TFT mode) */
  0,        /* HRTFT polarity delay (NA in TFT mode) */
  0,        /* HRTFT LP delay (NA in TFT mode) */
  0,        /* HRTFT SPL delay (NA in TFT mode) */
  0         /* HRTFT SPL to CLKS delay (NA in TFT mode) */
};
#endif
/* Sharp LQ104 TFT display parameters */
const LCD_PARAM_T sharp_lq104 =
{
  79,       /* Horizontal back porch */
  150,      /* Horizontal front porch */
  129,      /* HSYNC pulse width */
  640,      /* Pixels per line */
  32,       /* Vertical back porch */
  242,      /* Vertical front porch */
  3,        /* VSYNC pulse width */
  480,      /* Lines per panel */
  0,        /* Do not invert output enable */
  0,        /* Do not invert panel clock */
  1,        /* Do not invert HSYNC */
  1,        /* Invert VSYNC */
  1,        /* AC bias frequency (not used) */
  18,       /* Bits per pixel */
  25180000, /* Optimal clock rate (Hz) */
  TFT,      /* LCD panel type (NA in TFT mode) */
  0,        /* Single panel display */
  0,        /* HRTFT CLS enable flag (NA in TFT mode) */
  0,        /* HRTFT SPS enable flag (NA in TFT mode) */
  0,        /* HRTFT LP to PS delay (NA in TFT mode) */
  0,        /* HRTFT polarity delay (NA in TFT mode) */
  0,        /* HRTFT LP delay (NA in TFT mode) */
  0,        /* HRTFT SPL delay (NA in TFT mode) */
  0         /* HRTFT SPL to CLKS delay (NA in TFT mode) */
};

/* Sharp LQ121 TFT display parameters */
const LCD_PARAM_T sharp_lq121 =
{
  89,       /* Horizontal back porch */
  169,      /* Horizontal front porch */
  129,      /* HSYNC pulse width */
  800,      /* Pixels per line */
  24,       /* Vertical back porch */
  44,       /* Vertical front porch */
  5,        /* VSYNC pulse width */
  600,      /* Lines per panel */
  0,        /* Do not invert output enable */
  0,        /* Do not invert panel clock */
  1,        /* Invert HSYNC */
  1,        /* Invert VSYNC */
  1,        /* AC bias frequency (not used) */
  18,       /* Bits per pixel */
  31680000, /* Optimal clock rate (Hz) */
  TFT,      /* LCD panel type (NA in TFT mode) */
  0,        /* Single panel display */
  0,        /* HRTFT CLS enable flag (NA in TFT mode) */
  0,        /* HRTFT SPS enable flag (NA in TFT mode) */
  0,        /* HRTFT LP to PS delay (NA in TFT mode) */
  0,        /* HRTFT polarity delay (NA in TFT mode) */
  0,        /* HRTFT LP delay (NA in TFT mode) */
  0,        /* HRTFT SPL delay (NA in TFT mode) */
  0         /* HRTFT SPL to CLKS delay (NA in TFT mode) */
};

/* Sharp LM10V DSTN display */
const LCD_PARAM_T sharp_lm10v =
{
  1,        /* Horizontal back porch */
  1,        /* Horizontal front porch */
  6,        /* HSYNC pulse width */
  640,      /* Pixels per line */
  1,        /* Vertical back porch */
  1,        /* Vertical front porch */
  2,        /* VSYNC pulse width */
  480,      /* Lines per panel */
  0,        /* Do not invert output enable (NA) */
  0,        /* Do not invert panel clock */
  0,        /* Do not invert HSYNC */
  0,        /* Do not invert VSYNC */
  21,       /* AC bias frequency */
  16,       /* Bits per pixel */
  12500000, /* Optimal clock rate (Hz) */
  CSTN,     /* LCD panel type */
  1,        /* Dual panel display */
  0,        /* HRTFT CLS enable flag (NA) */
  0,        /* HRTFT SPS enable flag (NA) */
  0,        /* HRTFT LP to PS delay (NA) */
  0,        /* HRTFT polarity delay (NA) */
  0,        /* HRTFT LP delay (NA) */
  0,        /* HRTFT SPL delay (NA) */
  0         /* HRTFT SPL to CLKS delay (NA) */
};

/* Sharp LM057QB STN display */
const LCD_PARAM_T sharp_lm057qb =
{
  5,          /* Horizontal back porch */
  5,          /* Horizontal front porch */
  3,          /* HSYNC pulse width */
  320,        /* Pixels per line */
  1,          /* Vertical back porch */
  1,          /* Vertical front porch */
  1,          /* VSYNC pulse width */
  240,        /* Lines per panel */
  0,          /* Do not invert output enable (NA) */
  0,          /* Do not invert panel clock */
  0,          /* Do not invert HSYNC */
  0,          /* Do not invert VSYNC */
  1,          /* AC bias frequency */
  4,          /* Bits per pixel */
  4500000,    /* Optimal clock rate (Hz) */
  MONO_4BIT,  /* LCD panel type */
  0,          /* Dual panel display */
  0,          /* HRTFT CLS enable flag (NA) */
  0,          /* HRTFT SPS enable flag (NA) */
  0,          /* HRTFT LP to PS delay (NA) */
  0,          /* HRTFT polarity delay (NA) */
  0,          /* HRTFT LP delay (NA) */
  0,          /* HRTFT SPL delay (NA) */
  0           /* HRTFT SPL to CLKS delay (NA) */
};

/* Sharp LM057QC STN display */
const LCD_PARAM_T sharp_lm057qc =
{
  5,          /* Horizontal back porch */
  5,          /* Horizontal front porch */
  3,          /* HSYNC pulse width */
  320,        /* Pixels per line */
  0,          /* Vertical back porch */
  0,          /* Vertical front porch */
  0,          /* VSYNC pulse width */
  240,        /* Lines per panel */
  0,          /* Do not invert output enable (NA) */
  0,          /* Do not invert panel clock */
  0,          /* Do not invert HSYNC */
  0,          /* Do not invert VSYNC */
  0,//1,          /* AC bias frequency */
  8,          /* Bits per pixel */
  2500000,    /* Optimal clock rate (Hz) */
  CSTN,       /* LCD panel type */
  0,          /* Dual panel display */
  0,          /* HRTFT CLS enable flag (NA) */
  0,          /* HRTFT SPS enable flag (NA) */
  0,          /* HRTFT LP to PS delay (NA) */
  0,          /* HRTFT polarity delay (NA) */
  0,          /* HRTFT LP delay (NA) */
  0,          /* HRTFT SPL delay (NA) */
  0           /* HRTFT SPL to CLKS delay (NA) */
};

/* Sharp LM64K11 STN display */
const LCD_PARAM_T sharp_lm64k11 =
{
  5,          /* Horizontal back porch */
  5,          /* Horizontal front porch */
  3,          /* HSYNC pulse width */
  640,        /* Pixels per line */
  1,          /* Vertical back porch */
  1,          /* Vertical front porch */
  1,          /* VSYNC pulse width */
  240,        /* Lines per panel */
  0,          /* Do not invert output enable (NA) */
  0,          /* Do not invert panel clock */
  0,          /* Do not invert HSYNC */
  0,          /* Do not invert VSYNC */
  1,          /* AC bias frequency */
  4,          /* Bits per pixel */
  4500000,    /* Optimal clock rate (Hz) */
  MONO_4BIT,  /* LCD panel type */
  1,          /* Dual panel display */
  0,          /* HRTFT CLS enable flag (NA) */
  0,          /* HRTFT SPS enable flag (NA) */
  0,          /* HRTFT LP to PS delay (NA) */
  0,          /* HRTFT polarity delay (NA) */
  0,          /* HRTFT LP delay (NA) */
  0,          /* HRTFT SPL delay (NA) */
  0           /* HRTFT SPL to CLKS delay (NA) */
};


const LCD_PARAM_T toshiba =
{
  11,       /* Horizontal back porch */
  0,       /* Horizontal front porch */
  69,        /* HSYNC pulse width */
  320,      /* Pixels per line */

  7,        /* Vertical back porch */
  7,        /* Vertical front porch */
  45,        /* VSYNC pulse width */
  240,      /* Lines per panel */

  0,        /* Do not invert output enable */
  0,        /* Invert panel clock */
  0,        /* Invert HSYNC */
  0,        /* Do not invert VSYNC */

  1,        /* AC bias frequency (not used) */

  18,       /* Bits per pixel */
  6300000,  /* Optimal clock rate (Hz) */
  TFT,   	  /* LCD panel type */

  0,        /* Single panel display */
  0,        /* HRTFT CLS enable flag */
  0,        /* HRTFT SPS enable flag */
  0,        /* HRTFT LP to PS delay */
  0,        /* HRTFT polarity delay */
  0,        /* HRTFT LP delay */
  0,        /* HRTFT SPL delay */
  0         /* HRTFT SPL to CLKS delay */
};

const LCD_PARAM_T okaya =
{
  36,       /* Horizontal back porch */
  12,       /* Horizontal front porch */
  30,        /* HSYNC pulse width */
  320,      /* Pixels per line */

  26,        /* Vertical back porch */
  18,        /* Vertical front porch */
  4,        /* VSYNC pulse width */
  240,      /* Lines per panel */

  0,        /* Do not invert output enable */
  0,        /* Invert panel clock */
  0,        /* Invert HSYNC */
  0,        /* Do not invert VSYNC */

  1,        /* AC bias frequency (not used) */

  18,       /* Bits per pixel */
  5213000,  /* Optimal clock rate (Hz) */
  TFT,   	  /* LCD panel type */

  0,        /* Single panel display */
  0,        /* HRTFT CLS enable flag */
  0,        /* HRTFT SPS enable flag */
  0,        /* HRTFT LP to PS delay */
  0,        /* HRTFT polarity delay */
  0,        /* HRTFT LP delay */
  0,        /* HRTFT SPL delay */
  0         /* HRTFT SPL to CLKS delay */
};
