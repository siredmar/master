/**********************************************************************
*
* Project: NAND flash parameters
*
* Description:
*     This file contains NAND flash parameters.
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

#include "lpc_nandflash_params.h"

/* ST NAND256R3A parameters*/
const NANDFLASH_PARAM_T st_nand256r3a =
{
  BUS_WIDTH_8,                   /* Bus width */
  BLOCK_PAGE_SMALL,              /* Block page (small/large) */
  ADDR_CYCLES_3,                 /* Address cycles (3/4/5) */
};

/* MICRON MT29F2G08AACWP parameters */
const NANDFLASH_PARAM_T micron_mt29f2g08aacwp =
{
  BUS_WIDTH_8,                   /* Bus width */
  BLOCK_PAGE_LARGE,              /* Block page (small/large) */
  ADDR_CYCLES_5,                 /* Address cycles (3/4/5) */
};

