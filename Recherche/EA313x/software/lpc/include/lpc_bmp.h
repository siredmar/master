/***********************************************************************
 * $Id:: lpc_bmp.h 745 2008-05-13 19:59:29Z pdurgesh                   $
 *
 * Project: BMP file structures
 *
 * Description:
 *     This package contains the structure of the BMP file format.
 *
 * Notes:
 *     Data in the BMP header (as read from a file) is not stored word
 *     aligned after the identifier. If the structure is read from a
 *     file, the header information may need to be realigned to the
 *     structure alignment.
 *
 *     It is the intention of this package to support the most common
 *     BMP image formats in use. Not all BMP formats are supported.
 *
 * Unsupported BMP formats:
 *     RLE compression is not supported
 *     16-bit and 32-bit color images are not supported
 *     Masks stored in the color table are not supported
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

#ifndef LPC_BMP_H
#define LPC_BMP_H

#include "lpc_types.h"
#include "lpc_colors.h"

#if defined (__cplusplus)
extern "C"
{
#endif

/***********************************************************************
 * BMP structure defines
 **********************************************************************/

#define BMP_ID0   'B'        /* BMP file identifier character 1 */
#define BMP_ID1   'M'        /* BMP file identifier character 2 */
#define BI_RGB    0x00000000 /* Uncompressed image identifier */
#define BI_RGBA   0x32424752 /* Uncompressed image identifier alias for
BI_RGB */

/* The following defines are provided, but are not supported in this
   package */
#define BI_RLE4   0x00000002 /* 4-bit RLE compression */
#define BI_RLE8   0x00000001 /* 8-bit RLE compression */
#define BI_RLE8A  0x38454C52 /* 8-bit RLE compression for BI_RLE8 */
#define BI_BITFIELDS 0x00000003 /* Uncomp RGB with sample packing */
#define RGBA      0x41424752 /* Raw RGB with alpha */
#define RGBT      0x54424752 /* Raw RGB with a transparency field */

/***********************************************************************
 * BMP structures
 **********************************************************************/

/* Supported BMP file formats (no compressed or masked color modes are
   supported) */
typedef enum
{
  INVALID_BMP = -1,
  BPP1 = 0,                // 1 bit per pixel with color table
  BPP4,                    // 4 bits per pixel with color table
  BPP8,                    // 8 bits per pixel with color table
  BPP24                    // 24 bits per pixel
} BMP_STORAGE_T;

/* Color table entry format (used with BPP1, BPP4, and BPP8) */
typedef struct
{
  UNS_8    blue;
  UNS_8    green;
  UNS_8    red;
  UNS_8    unused;
} BMP_COLOR_TABLE_T;

/* Color table entry format used with BPP24 */
typedef struct
{
  UNS_8    blue;
  UNS_8    green;
  UNS_8    red;
} BMP24_COLOR_TABLE_T;

/* BMP header structure, not used with files */
typedef struct
{
  CHAR     bftype [2];     /* Always ("BM") for BMP files */
  UNS_32   bfsize;         /* Size of file in bytes */
  UNS_32   rsv1;           /* Reserved */
  UNS_32   dataoffset;     /* Offset from file to start to data */
  UNS_32   bisize;         /* Size of this structure */
  UNS_32   biwidth;        /* Pixel width image size */
  UNS_32   biheight;       /* Pixel height image size */
  UNS_16   biplanes;       /* color planes */
  UNS_16   bibitcount;     /* Bits per pixel */
  UNS_32   bicompressn;    /* Compression type, 0 = BMP */
  UNS_32   bisizeimage;    /* Size of image in bytes */
  UNS_32   rsv3;           /* Normally used for metrics */
  UNS_32   rsv4;           /* Normally used for metrics */
  UNS_32   buclrused;      /* Colors used in the bitmap */
  UNS_32   biclrimp;       /* Number of important colors */
  INT_32   ct_data;        /* Start of color table or data */
} BMP_T;

/***********************************************************************
 * BMP processing and color support functions
 **********************************************************************/

/* Determine if the structure is a BMP structure */
BMP_STORAGE_T bmp_is_header_valid(BMP_T *bmp_data);

/* Returns a pointer to the color table */
BMP_COLOR_TABLE_T *bmp_get_color_table(BMP_T *bmp_data);

/* Returns a pointer to the BMP image data */
void *bmp_get_image_data(BMP_T *bmp_data);

/* Converts a BMP color table entry to a color_type color */
COLOR_T bmp_convert_color(BMP_COLOR_TABLE_T *color_entry);

/* Convert a BMP image to a color_type image */
BMP_STORAGE_T bmp_convert_image(BMP_T *bmp_data,
                                INT_16 *xsize,
                                INT_16 *ysize,
                                COLOR_T *bufout);

/* Allocates storage for a new BMP file */
BMP_T *bmp_allocate_structure(INT_32 xsize,
                              INT_32 ysize,
                              BMP_STORAGE_T bits_per_pixel);

#if defined (__cplusplus)
}
#endif /*__cplusplus */

#endif /* LPC_BMP_H */
