/*
 * tft_lcfg.c
 *
 *  Created on: Nov 10, 2012
 *      Author: armin
 */
#include "tft_lcfg.h"

const tft_initSequenceType tft_initialConfig5_s[] =
{

};

const tft_configType tft_initialConfigTft_s =
{
   tft_initialConfig5_s,		/* config struct for tft */
   0      							/* number of config entries is 44 */
};

const void *tft_getLcfgData
(
   void
)
{
   return ((void*) &tft_initialConfigTft_s);
}

