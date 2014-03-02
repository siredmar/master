/***********************************************************************
 * $Id:: image.h 6363 2011-02-03 07:36:50Z ing02124                     $
 *
 * Project: Image data header file
 *
 * Description:
 *     Image data header file.
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

#ifndef IMAGE_H
#define IMAGE_H
#include "lpc_types.h"

#define IMAGE_BPP 16
extern const UNS_16 image_w;
extern const UNS_16 image_h;
extern const UNS_16 image[];
#define image_palette NULL
#endif /* #ifndef IMAGE_H */
