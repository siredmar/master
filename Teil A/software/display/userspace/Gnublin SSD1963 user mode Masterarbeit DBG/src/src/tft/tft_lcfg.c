/*
 * tft_lcfg.c
 *
 *  Created on: Nov 10, 2012
 *      Author: armin
 */
#include "tft_lcfg.h"

const tft_initSequenceType tft_initialConfig4d3_s[] =
{
     // {0x0001, 0x0000,                 50, NOPARAM},   // software reset
      {0x00E2, 0x002D, 				      0, PARAM},		// PLL config
		{NOCMD,  0x0002, 				      0, PARAM},
		{NOCMD,  0x0004, 				      200, PARAM},

		{0x00E0, 0x0001, 				      200, PARAM},		// PLL config - continued

		{0x00E0, 0x0003, 				      200, PARAM},		// PLL config - continued

		{0x0001, 0x0000, 				      5, NOPARAM},	// software reset

		{0x00E6, 0x0000, 				      0, PARAM},		// LSHIFT freq
		{NOCMD,  0x00FF, 				      0, PARAM},
		{NOCMD,  0x00BE, 				      1, PARAM},

		{0x00B0, 0x0020, 				      1, PARAM},		// set lcd mode
		{NOCMD,  0x0000, 				      1, PARAM},
		{NOCMD,  ((HDP >> 8) & 0x00FF),  1, PARAM},
		{NOCMD,  (HDP & 0x00FF), 		   1, PARAM},
		{NOCMD,  ((HDP >> 8) & 0x00FF),  1, PARAM},
		{NOCMD,  (VDP & 0x00FF), 		   1, PARAM},
		{NOCMD,  0x0000, 				      5, PARAM},

		{0x00B4, ((HT >> 8) & 0x00FF),   1, PARAM},		// horizontal period
		{NOCMD,  (HT & 0x00FF), 		   1, PARAM},
		{NOCMD,  ((HPS >> 8) & 0x00FF),  1, PARAM},
		{NOCMD,  (HPS & 0x00FF), 		   1, PARAM},
		{NOCMD,  HPW, 					      1, PARAM},
		{NOCMD,  ((LPS >> 8) & 0x00FF),  1, PARAM},
		{NOCMD,  (LPS & 0x00FF), 		   1, PARAM},
		{NOCMD,  0x0000, 				      1, PARAM},

		{0x00B6, ((VT >> 8) & 0x00FF),   1, PARAM},		// vertical period
		{NOCMD,  (VT & 0x00FF), 		   1, PARAM},
		{NOCMD,  (VPS >> 8) & 0x00FF, 	1, PARAM},
		{NOCMD,  (VPS & 0x00FF), 		   1, PARAM},
		{NOCMD,  VPW, 					      1, PARAM},
		{NOCMD,  ((FPS >> 8) & 0x00FF),  1, PARAM},
		{NOCMD,  (FPS & 0x00FF), 		   1, PARAM},

		{0x0036, 0x0000, 				      1, PARAM},		// rotation - landscape

		{0x00F0, 0x0000, 				      5, PARAM},		// pixel data interface 8 bit    0b000 is 8 Bit, 0b110 is 9 Bit

		{0x0029, 0x0000, 				      1, NOPARAM},	// set display on

		{0x00BE, 0x0006, 				      1, PARAM},		// pwm config
		{NOCMD,  0x00F0, 				      1, PARAM},
		{NOCMD,  0x0001, 				      1, PARAM},
		{NOCMD,  0x00F0, 				      1, PARAM},
		{NOCMD,  0x0000, 				      1, PARAM},
		{NOCMD,  0x0000, 				      1, PARAM},

		{0x00D0, 0x000D, 				      1, PARAM},		// pwm coffig continued

		{0x00B8, 0x0000, 				      1, PARAM},		// gpio config
		{NOCMD,  0x0000, 				      1, PARAM},

		{0x00BA, 0x0000, 				      1, PARAM}		// gpio to 0x00
};

const tft_configType tft_initialConfigTft_s =
{
   tft_initialConfig4d3_s,		/* config struct for tft */
   44 							/* number of config entries is 44 */
};

const void *tft_getLcfgData
(
   void
)
{
   return ((void*) &tft_initialConfigTft_s);
}

