/*
 * tft_lcfg.c
 *
 *  Created on: Nov 10, 2012
 *      Author: armin
 */
#include "tft_lcfg.h"

const tft_initSequenceType tft_initialConfig4d3_s[] =
{
//      {0x0001, 0x0000,                 200, NOPARAM},   // software reset
//      // {0x0001, 0x0000,                 500, NOPARAM},   // software reset
//      {0x00E0, 0x0001,                 1, PARAM},      // PLL config - continued
//      {0x0000, 0x0000,                 5, NOPARAM},   // NOP
//      {0x00E2, 0x001D, 				      1, PARAM},		// PLL config
//		{NOCMD,  0x0002, 				      1, PARAM},
//		{NOCMD,  0x0054, 				      1, PARAM},
//
//		{0x00E0, 0x0003, 				      600, PARAM},		// PLL config - continued
//
//		{0x00E6, 0x0000, 				      0, PARAM},		// LSHIFT freq
//		{NOCMD,  0x00FF, 				      0, PARAM},
//		{NOCMD,  0x00BE, 				      0, PARAM},
//
//		{0x00B0, 0x0020, 				      0, PARAM},		// set lcd mode
//		{NOCMD,  0x0000, 				      0, PARAM},
//		{NOCMD,  ((HDP >> 8) & 0x00FF),  0, PARAM},
//		{NOCMD,  (HDP & 0x00FF), 		   0, PARAM},
//		{NOCMD,  ((HDP >> 8) & 0x00FF),  0, PARAM},
//		{NOCMD,  (VDP & 0x00FF), 		   0, PARAM},
//		{NOCMD,  0x0000, 				      0, PARAM},
//
//		{0x00B4, ((HT >> 8) & 0x00FF),   0, PARAM},		// horizontal period
//		{NOCMD,  (HT & 0x00FF), 		   0, PARAM},
//		{NOCMD,  ((HPS >> 8) & 0x00FF),  0, PARAM},
//		{NOCMD,  (HPS & 0x00FF), 		   0, PARAM},
//		{NOCMD,  HPW, 					      0, PARAM},
//		{NOCMD,  ((LPS >> 8) & 0x00FF),  0, PARAM},
//		{NOCMD,  (LPS & 0x00FF), 		   0, PARAM},
//		{NOCMD,  0x0000, 				      0, PARAM},
//
//		{0x00B6, ((VT >> 8) & 0x00FF),   0, PARAM},		// vertical period
//		{NOCMD,  (VT & 0x00FF), 		   0, PARAM},
//		{NOCMD,  (VPS >> 8) & 0x00FF, 	0, PARAM},
//		{NOCMD,  (VPS & 0x00FF), 		   0, PARAM},
//		{NOCMD,  VPW, 					      0, PARAM},
//		{NOCMD,  ((FPS >> 8) & 0x00FF),  0, PARAM},
//		{NOCMD,  (FPS & 0x00FF), 		   0, PARAM},
//
//		{0x0036, 0x0000, 				      0, PARAM},		// rotation - landscape
//
//		{0x00F0, 0x0003, 				      0, PARAM},		// pixel data interface 16 bit    0b011 is 16 Bit
//
//		{0x0029, 0x0000, 				      0, NOPARAM},	// set display on
//
//		{0x00BE, 0x0006, 				      0, PARAM},		// pwm config
//		{NOCMD,  0x00F0, 				      0, PARAM},
//		{NOCMD,  0x0001, 				      0, PARAM},
//		{NOCMD,  0x00F0, 				      0, PARAM},
//		{NOCMD,  0x0000, 				      0, PARAM},
//		{NOCMD,  0x0000, 				      0, PARAM},
//
//		{0x00D0, 0x000D, 				      0, PARAM},		// pwm coffig continued
//
//		{0x00B8, 0x0000, 				      0, PARAM},		// gpio config
//		{NOCMD,  0x0000, 				      0, PARAM},
//
//		{0x00BA, 0x0000, 				      0, PARAM}		// gpio to 0x00
};

const tft_configType tft_initialConfigTft_s =
{
   tft_initialConfig4d3_s,		/* config struct for tft */
  0// 45 							/* number of config entries is 44 */
};

const void *tft_getLcfgData
(
   void
)
{
   return ((void*) &tft_initialConfigTft_s);
}

