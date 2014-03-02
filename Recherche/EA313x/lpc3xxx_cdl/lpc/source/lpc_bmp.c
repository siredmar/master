/***********************************************************************
 * $Id:: lpc_bmp.c 745 2008-05-13 19:59:29Z pdurgesh                   $
 *
 * Project: BMP file structures
 *
 * Description:
 *     See the bmp.h header file for a description of this package.
 *
 *     This package uses *malloc*. If you want to use this package, you
 *     should replace malloc with your own dynamic allocation call if
 *     malloc is an invalid function.
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

#include "lpc_bmp.h"
#include "lpc_heap.h"

/***********************************************************************
 * Private functions
 **********************************************************************/

/***********************************************************************
 *
 * Function: bmp_render_w_palette
 *
 * Purpose: Converts a pelletized (color table) BMP image into a COLOR_T
 *          image in raw format.
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     bmp_data : pointer to BMP data structure
 *     buf      : pointer of where to place converted image
 *     pmask    : Color table entry mask
 *     md_shift : incremental mask shift value
 *     pshift   : initial mask shift value
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void bmp_render_w_palette(BMP_T *bmp_data,
                          COLOR_T *buf,
                          UNS_8 pmask,
                          UNS_8 md_shift,
                          UNS_8 pshift)
{
  BMP_COLOR_TABLE_T *color_table;
  UNS_8 *image_data;
  INT_32 x, xdif, y;
  UNS_8 pixel_data = 0, pixel_mask, pixel_shift = 0;
  INT_32 table_index, image_start;
  UNS_8 mask_shift = 0;
  COLOR_T *fbuf;

  /* Get pointers to color table and image data */
  color_table = bmp_get_color_table(bmp_data);
  image_start = (INT_32) bmp_get_image_data(bmp_data);
  image_data = (UNS_8 *) image_start;

  /* Clear initial pixel mask */
  pixel_mask = 0x0;

  /* Convert Y axis first */
  y = bmp_data->biheight - 1;
  while (y >= 0)
  {
    /* Reset buf to start from left to right */
    fbuf = buf + (bmp_data->biwidth * y);

    /* Render a single line */
    x = bmp_data->biwidth - 1;
    while (x >= 0)
    {
      /* A byte contains data for up to 8 pixels, once the
         pixel mask has reached 0x0, load a new byte and reset
         the mask */
      if (pixel_mask == 0x0)
      {
        /* Reset pixel mask and shift value */
        pixel_mask = pmask;
        pixel_shift = pshift;
        mask_shift = md_shift;

        /* get a new byte and increment image pointer */
        pixel_data = *image_data;
        image_data++;
      }

      /* Determine index into color table */
      table_index = (INT_32)
                    ((pixel_data & pixel_mask) >> mask_shift);

      /* Use the appropriate index in the color table */
      *fbuf = bmp_convert_color(&color_table [table_index]);
      fbuf++;

      /* Update x coordinate */
      x--;

      /* Shift pixel mask to next bit */
      pixel_mask = pixel_mask >> pixel_shift;
      mask_shift = mask_shift - pshift;
    }

    y--;

    /* Since a horizontal line is 'padded' to end on a 32-bit
       boundary, the image pointer may need to be adjusted for the
       next line */
    x = ((INT_32) image_data - image_start) / 4;
    xdif = x * 4;
    image_data = image_data + (((INT_32) image_data -
                                image_start) - xdif);

    /* Clear pixel_mask to reset pixel mask and shift values */
    pixel_mask = 0x0;
  }
}

/***********************************************************************
 *
 * Function: bmp_render24
 *
 * Purpose: Converts a raw 24-bit RGB image to a COLOR_T raw image.
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     bmp_data : pointer to BMP data structure
 *     buf      : pointer of where to place converted image
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void bmp_render24(BMP_T *bmp_data,
                  COLOR_T *buf)
{
  BMP24_COLOR_TABLE_T *image_data;
  INT_32 x, y;
  COLOR_T *fbuf;

  /* Get pointer to image data - each pixel entry can be considered
     a color table / palette entry */
  image_data =
    (BMP24_COLOR_TABLE_T *) bmp_get_image_data(bmp_data);

  /* Convert Y axis first */
  y = bmp_data->biheight - 1;
  while (y >= 0)
  {
    /* Reset buf to start from left to right */
    fbuf = buf + (bmp_data->biwidth * y);

    /* Render a single line */
    x = bmp_data->biwidth - 1;
    while (x >= 0)
    {
      /* Use the appropriate index in the color table */
      *fbuf = bmp_convert_color(
                (BMP_COLOR_TABLE_T *) image_data);

      /* Update source to next pixel data set */
      image_data++;
      fbuf++;

      /* Update x coordinate */
      x--;
    }

    /* Since a horizontal line is 'padded' to end on a 32-bit
       boundary, the image pointer may need to be adjusted for the
       next line */
    x = bmp_data->biwidth / 4;
    x = x * 4;
    image_data = (BMP24_COLOR_TABLE_T *)((INT_32) image_data +
                                         (bmp_data->biwidth - x));

    y--;
  }
}

/***********************************************************************
 * Public functions
 **********************************************************************/

/***********************************************************************
 *
 * Function: bmp_is_header_valid
 *
 * Purpose: Determine if the structure is a BMP structure
 *
 * Processing:
 *     The header type (bftype) is examined to match 'BM'. If it matches
 *     and the file type is uncompressed, then the color depth is
 *     examined and the return value set to the appropriate color depth
 *     enumeration. If an unsupported type is found, type INVALID_BMP
 *     will be returned.
 *
 * Parameters:
 *     bmp_data : Pointer to a BMP data structure.
 *
 * Outputs: None
 *
 * Returns:
 *     Enumeration that defines the BMP color depth, or INVALID_BMP if
 *     the BMP type is unsupported.
 *
 * Notes: None
 *
 **********************************************************************/
BMP_STORAGE_T bmp_is_header_valid(BMP_T *bmp_data)
{
  BMP_STORAGE_T retval = INVALID_BMP;

  /* Does the structure have the bitmap identifier? */
  if ((bmp_data->bftype [0] == BMP_ID0) &&
      (bmp_data->bftype [1] == BMP_ID1))
  {
    /* Compression field must be empty! */
    if ((bmp_data->bicompressn == BI_RGB) ||
        (bmp_data->bicompressn == BI_RGBA))
    {
      switch (bmp_data->bibitcount)
      {
        case 1:
          /* 1 bit per pixel */
          retval = BPP1;
          break;

        case 4:
          /* 4 bits per pixel */
          retval = BPP4;
          break;

        case 8:
          /* 8 bits per pixel */
          retval = BPP8;
          break;

        case 24:
          /* 24 bits per pixel */
          retval = BPP24;
          break;

        default:
          /* No other modes are valid, keep invalid flag */
          break;
      }
    }
  }

  return retval;
}

/***********************************************************************
 *
 * Function: bmp_get_color_table
 *
 * Purpose: Returns a pointer to the color table
 *
 * Processing:
 *     A call to bmp_is_header_valid is performed to determine the BMP
 *     file type. If the BMP file type is BPP1, BPP4, or BPP8, then the
 *     color table is assigned a pointer after the BMP header
 *     information.
 *
 * Parameters:
 *     bmp_data : Pointer to a BMP data structure.
 *
 * Outputs: Nothing
 *
 * Returns: A pointer to the color table, or 0x0 if one does not exist
 *
 * Notes:
 *     1, 4, and 8 bit per pixel BMP images have color tables.
 *
 **********************************************************************/
BMP_COLOR_TABLE_T *bmp_get_color_table(BMP_T *bmp_data)
{
  BMP_STORAGE_T bmp_colors;
  BMP_COLOR_TABLE_T *table_ptr = (BMP_COLOR_TABLE_T *) 0x0;

  /* Get BMP structure type */
  bmp_colors = bmp_is_header_valid(bmp_data);

  /* Does a color table exist? */
  if ((bmp_colors >= BPP1) && (bmp_colors <= BPP8))
  {
    /* Color table exists, return pointer */
    table_ptr = (BMP_COLOR_TABLE_T *) bmp_data->ct_data;
  }

  return table_ptr;
}

/***********************************************************************
 *
 * Function: bmp_get_image_data
 *
 * Purpose: Returns a pointer to the BMP image data.
 *
 * Processing:
 *     A call to bmp_is_header_valid is performed to determine the BMP
 *     file type. Based on the BMP file type, the number of entries in
 *     the color table is computed. The pointer to the image data is
 *     computed at the end of the header plus an offset for the color
 *     table.
 *
 * Parameters:
 *     bmp_data : Pointer to a BMP data structure.
 *
 * Outputs: Nothing
 *
 * Returns: A pointer to the BMP image data.
 *
 * Notes: None
 *
 **********************************************************************/
void *bmp_get_image_data(BMP_T *bmp_data)
{
  INT_32 ctsize;

  /* Skip past color table */
  switch (bmp_is_header_valid(bmp_data))
  {
    case BPP1:
      /* Color table size is 2 entries */
      ctsize = 2;
      break;

    case BPP4:
      /* Color table size is 16 entries */
      ctsize = 16;
      break;

    case BPP8:
      /* Color table size is 256 entries */
      ctsize = bmp_data->buclrused;
      break;

    default:
      /* No color table */
      ctsize = 0;
  }

  return (void *)((UNS_32) bmp_data->ct_data +
                  ctsize * sizeof(BMP_COLOR_TABLE_T));
}

/***********************************************************************
 *
 * Function: bmp_convert_color
 *
 * Purpose: Converts a BMP color table entry to a COLOR_T color
 *
 * Processing:
 *     A color table entry (or raw 24-bit entry) is converted into the
 *     native (compiled) color type by masking and shifting the red,
 *     green, and blue components of color and computing the closest
 *     color in the native format (either 233, 555, or 565).
 *
 * Parameters:
 *     color_entry : Color table entry pointer
 *
 * Outputs: None
 *
 * Returns: A converted color_type entry from the color data
 *
 * Notes:
 *     Not valid for 16-bit or 32-bit color formats.
 *
 **********************************************************************/
COLOR_T bmp_convert_color(BMP_COLOR_TABLE_T *color_entry)
{
  COLOR_T cv_color, r, g, b;

  /* Scale colors to color_type color */
  r = (COLOR_T)
      ((UNS_32) color_entry->red * (REDMASK >> REDSHIFT) / 256);
  g = (COLOR_T)
      ((UNS_32) color_entry->green * (GREENMASK >> GREENSHIFT) / 256);
  b = (COLOR_T)
      ((UNS_32) color_entry->blue * (BLUEMASK >> BLUESHIFT) / 256);
  cv_color = (b << BLUESHIFT) + (g << GREENSHIFT) + (r << REDSHIFT);

  return cv_color;
}

/***********************************************************************
 *
 * Function: bmp_convert_image
 *
 * Purpose: Convert a BMP image to a COLOR_T image
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     bmp_data : pointer to a BMP data structure
 *     xsize    : Pointer to place the horizontal size of the image
 *     ysize    : Pointer to place the vertical size of the image
 *     bufout   : Pointer to where to place the converted image
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes:
 *     Only uncompressed 1, 4, 8, and 24 bit per pixel formats are
 *     supported. Before converting, be sure that the target buffer,
 *     bufout, is large enough for the converted image.
 *
 **********************************************************************/
BMP_STORAGE_T bmp_convert_image(BMP_T *bmp_data,
                                INT_16 *xsize,
                                INT_16 *ysize,
                                COLOR_T *bufout)
{
  BMP_STORAGE_T bmpcolors;

  /* Get BMP image type */
  bmpcolors = bmp_is_header_valid(bmp_data);

  if (bmpcolors != INVALID_BMP)
  {
    /* Save horizontal and vertical sizes */
    *xsize = bmp_data->biwidth;
    *ysize = bmp_data->biheight;

    /* Good table entry */
    switch (bmpcolors)
    {
      case BPP1:
        /* 1 bit per pixel, call function */
        bmp_render_w_palette(bmp_data, bufout, 0x80, 7, 1);
        break;

      case BPP4:
        /* 4 bits per pixel, call function */
        bmp_render_w_palette(bmp_data, bufout, 0xF0, 4, 4);
        break;

      case BPP8:
        /* 8 bits per pixel, call function */
        bmp_render_w_palette(bmp_data, bufout, 0xFF, 0, 8);
        break;

      case BPP24:
        /* 24 bits per pixel, call function */
        bmp_render24(bmp_data, bufout);
        break;

      default:
        /* Invalid or unsupported BMP image */
        *xsize = 0;
        *ysize = 0;
        break;
    }
  }
  else
  {
    /* Invalid or unsupported BMP image */
    *xsize = 0;
    *ysize = 0;
  }

  return bmpcolors;
}

/***********************************************************************
 *
 * Function: bmp_convert_image
 *
 * Purpose: Allocates storage for a new BMP file structure.
 *
 * Processing:
 *     This function computes the required size needed for the BMP
 *     header, color table, and image data, based on the color depth.
 *     Memory for an image (with header and color table) is allocated
 *     and the pointer returned to the caller.
 *
 * Parameters:
 *     xsize          : Horizontal size of the image storage space
 *     ysize          : Vertical size of the image storage space
 *     bits_per_pixel : number of bits per pixel, used to set the size
 *                      of the buffer and color table (enumerator)
 *
 * Outputs: Nothing
 *
 * Returns:
 *     A pointer to a new allocated BMP structure, or NULL if an error
 *     occurred.
 *
 * Notes:
 *     The bits_per_pixel parameter is important for optimal memory
 *     usage. Setting this value will 'adjust' the sizing of the
 *     allocated BMP structure, modifying the sizes of the color table
 *     and data area. If unsure of the bits per pixel, use BPP24,
 *     extra memory will be allocated for BPP24, but no memory
 *     allocation problems will occur.
 *
 **********************************************************************/
BMP_T *bmp_allocate_structure(INT_32 xsize,
                              INT_32 ysize,
                              BMP_STORAGE_T bits_per_pixel)
{
  INT_32 alloc_size, ctable_size, dentry_size;

  /* Need room for the basic structure size */
  alloc_size = sizeof(BMP_T);

  /* Select size of color table and data area based on bits per
     pixel */
  switch (bits_per_pixel)
  {
    case BPP1:
      ctable_size = 2;
      dentry_size = 1;
      break;

    case BPP4:
      ctable_size = 16;
      dentry_size = 4;
      break;

    case BPP8:
      ctable_size = 256;
      dentry_size = 8;
      break;

    case BPP24:
    default:
      ctable_size = 0;     /* No color table */
      dentry_size = 24;
      break;
  }

  /* Add in data area size, - size based on bits per pixel (8 extra
     bytes are included at the end of the allocated memory for
     safety) */
  alloc_size = alloc_size +
               (ctable_size * sizeof(BMP_COLOR_TABLE_T)) +
               (xsize * ysize * dentry_size / 8) + 8;

  /* Return pointer to allocated structure */
  return (BMP_T *) lpc_new(alloc_size);
}
