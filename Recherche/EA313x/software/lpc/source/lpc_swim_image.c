/***********************************************************************
 * $Id:: lpc_swim_image.c 745 2008-05-13 19:59:29Z pdurgesh            $
 *
 * Project: Image management for SWIM
 *
 * Description:
 *     See the swim.h header file for a description of this package.
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

#include "lpc_types.h"
#include "lpc_swim_image.h"

/***********************************************************************
 * Public functions
 **********************************************************************/

/***********************************************************************
 *
 * Function: swim_put_image
 *
 * Purpose: Puts an raw image in a window unscaled, clips off edges
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     win   : Window identifier
 *     image : Pointer to image data, must be in display color format
 *     xsize : Size of the image in horizontal pixels
 *     ysize : Size of the image in vertical pixels
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes:
 *     Pixels should be organized in the image from left to right, top
 *     to bottom. (BMP images are not stored like this.)
 *
 **********************************************************************/
void swim_put_image(SWIM_WINDOW_T *win,
                    const COLOR_T *image,
                    INT_32 xsize,
                    INT_32 ysize)
{
  COLOR_T *fb;
  INT_32 x, y;

  /* Unknown values of rtype will do no rotation */
  y = win->ypvmin;

  xsize = xsize + win->xpvmin;
  ysize = ysize + win->ypvmin;

  /* Move image to window pixel by pixel */
  while ((y <= win->ypvmax) && (y < ysize))
  {
    /* Set physical frame buffer address */
    x = win->xpvmin;
    fb = win->fb + x + (y * win->xpsize);

    /* Render a single line */
    while ((x <= win->xpvmax) && (x < xsize))
    {
      *fb = *image;
      fb++;
      image++;
      x++;
    }

    /* Adjust to end of line if the image was clipped */
    image = image + (xsize - x);

    y++;
  }
}

/***********************************************************************
 *
 * Function: swim_put_invert_image
 *
 * Purpose: Puts an raw image in a window unscaled, inverted, with
 *          clipped edges.
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     win   : Window identifier
 *     image : Pointer to image data, must be in display color format
 *     xsize : Size of the image in horizontal pixels
 *     ysize : Size of the image in vertical pixels
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes:
 *     Pixels should be organized in the image from left to right, top
 *     to bottom. (BMP images are not stored like this.)
 *
 **********************************************************************/
void swim_put_invert_image(SWIM_WINDOW_T *win,
                           const COLOR_T *image,
                           INT_32 xsize,
                           INT_32 ysize)
{
  COLOR_T *fb;
  INT_32 x, y, xr, yr;

  y = win->ypvmin;
  yr = ysize - 1;

  /* Move image to window pixel by pixel */
  while ((y <= win->ypvmax) && (yr >= 0))
  {
    /* Set physical frame buffer address */
    x = win->xpvmin;
    xr = xsize - 1;
    fb = win->fb + x + (y * win->xpsize);

    /* Render a single line */
    while ((x <= win->xpvmax) && (xr >= 0))
    {
      *fb = image [xr + yr * xsize];
      fb++;
      x++;
      xr--;
    }

    y++;
    yr--;
  }
}

/***********************************************************************
 *
 * Function: swim_put_left_image
 *
 * Purpose: Puts an raw image in a window unscaled, rotated left, with
 *          clipped edges.
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     win   : Window identifier
 *     image : Pointer to image data, must be in display color format
 *     xsize : Size of the image in horizontal pixels
 *     ysize : Size of the image in vertical pixels
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes:
 *     Pixels should be organized in the image from left to right, top
 *     to bottom. (BMP images are not stored like this.)
 *
 **********************************************************************/
void swim_put_left_image(SWIM_WINDOW_T *win,
                         const COLOR_T *image,
                         INT_32 xsize,
                         INT_32 ysize)
{
  COLOR_T *fb;
  INT_32 x, y, xr, yr;

  x = win->xpvmin;
  yr = ysize - 1;

  /* Move image to window pixel by pixel */
  while ((x <= win->xpvmax) && (yr >= 0))
  {
    /* Set physical frame buffer address to start drawing at
       bottom */
    y = win->ypvmin;
    fb = win->fb + x + (y * win->xpsize);
    xr = 0;

    /* Render a single line */
    while ((y <= win->ypvmax) && (xr < xsize))
    {
      /* Go to next line (y) */
      *fb = image [(xsize - xr - 1) + (ysize - yr - 1) * xsize];
      fb = fb + win->xpsize;

      /* Update picture to next x coordinate */
      y++;
      xr++;
    }

    x++;
    yr--;
  }
}

/***********************************************************************
 *
 * Function: swim_put_right_image
 *
 * Purpose: Puts an raw image in a window unscaled, rotated right, with
 *          clipped edges.
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     win   : Window identifier
 *     image : Pointer to image data, must be in display color format
 *     xsize : Size of the image in horizontal pixels
 *     ysize : Size of the image in vertical pixels
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes:
 *     Pixels should be organized in the image from left to right, top
 *     to bottom. (BMP images are not stored like this.)
 *
 **********************************************************************/
void swim_put_right_image(SWIM_WINDOW_T *win,
                          const COLOR_T *image,
                          INT_32 xsize,
                          INT_32 ysize)
{
  COLOR_T *fb;
  INT_32 x, y, xr, yr;

  x = win->xpvmin;
  yr = ysize - 1;

  /* Move image to window pixel by pixel */
  while ((x <= win->xpvmax) && (yr >= 0))
  {
    /* Set physical frame buffer address to start drawing at bottom */
    y = win->ypvmin;
    fb = win->fb + x + (y * win->xpsize);
    xr = 0;

    /* Render a single line */
    while ((y <= win->ypvmax) && (xr < xsize))
    {
      /* Go to next line (y) */
      *fb = image [xr + yr * xsize];
      fb = fb + win->xpsize;

      /* Update picture to next x coordinate */
      y++;
      xr++;
    }

    x++;
    yr--;
  }
}

/***********************************************************************
 *
 * Function: swim_put_scale_image
 *
 * Purpose: Puts an raw image in a window scaled.
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     win   : Window identifier
 *     image : pointer to image data, must be in display color format
 *     xsize : Size of the image in horizontal pixels
 *     ysize : Size of the image in vertical pixels
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes:
 *     Pixels should be organized in the image from left to right, top
 *     to bottom. (BMP images are not stored like this.)
 *
 **********************************************************************/
void swim_put_scale_image(SWIM_WINDOW_T *win,
                          const COLOR_T *image,
                          INT_32 xsize,
                          INT_32 ysize)
{
  INT_32 xsc, ysc;
  INT_32 x, y;
  COLOR_T *fb;

  /* Top of window */
  y = win->ypvmin;

  /* Rescale image into window */
  while (y <= win->ypvmax)
  {
    x = win->xpvmin;

    /* Scale he display size to the image size */
    ysc = ((ysize - 1) * (y - win->ypvmin)) / win->yvsize;
    fb = win->fb + x + (y * win->xpsize);

    /* Render a single line */
    while (x <= win->xpvmax)
    {
      /* Get x pixel in image */
      xsc = ((xsize - 1) * (x - win->xpvmin)) / win->xvsize;
      *fb = image [xsc + ysc * xsize];
      fb++;
      x++;
    }

    y++;
  }
}

/***********************************************************************
 *
 * Function: swim_put_scale_invert_image
 *
 * Purpose: Puts an raw image in a window scaled and inverted.
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     win   : Window identifier
 *     image : pointer to image data, must be in display color format
 *     xsize : Size of the image in horizontal pixels
 *     ysize : Size of the image in vertical pixels
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes:
 *     Pixels should be organized in the image from left to right, top
 *     to bottom. (BMP images are not stored like this.)
 *
 **********************************************************************/
void swim_put_scale_invert_image(SWIM_WINDOW_T *win,
                                 const COLOR_T *image,
                                 INT_32 xsize,
                                 INT_32 ysize)
{
  INT_32 xsc, ysc;
  INT_32 x, y;
  COLOR_T *fb;

  /* Top of window */
  y = win->ypvmin;

  /* Rescale image into window */
  while (y <= win->ypvmax)
  {
    x = win->xpvmin;

    /* Scale he display size to the image size */
    ysc = ((ysize - 1) * (y - win->ypvmin)) / win->yvsize;
    fb = win->fb + x + (y * win->xpsize);

    /* Render a single line */
    while (x <= win->xpvmax)
    {
      /* Get x pixel in image */
      xsc = ((xsize - 1) * (x - win->xpvmin)) / win->xvsize;
      *fb = image [(xsize - 1 - xsc) + (ysize - 1 - ysc) * xsize];
      fb++;
      x++;
    }

    y++;
  }
}

/***********************************************************************
 *
 * Function: swim_put_scale_left_image
 *
 * Purpose: Puts an raw image in a window scaled and rotated left.
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     win   : Window identifier
 *     image : pointer to image data, must be in display color format
 *     xsize : Size of the image in horizontal pixels
 *     ysize : Size of the image in vertical pixels
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes:
 *     Pixels should be organized in the image from left to right, top
 *     to bottom. (BMP images are not stored like this.)
 *
 **********************************************************************/
void swim_put_scale_left_image(SWIM_WINDOW_T *win,
                               const COLOR_T *image,
                               INT_32 xsize,
                               INT_32 ysize)
{
  INT_32 xsc, ysc;
  INT_32 x, y;
  COLOR_T *fb;

  /* Top of window */
  y = win->ypvmin;

  /* Rescale image into window */
  while (y <= win->ypvmax)
  {
    x = win->xpvmin;

    /* Scale y coords of picture into x axis */
    ysc = ((xsize - 1) * (win->ypvmax - y)) / win->yvsize;
    fb = win->fb + x + (y * win->xpsize);

    /* Render a single horizontal line with 'y' data */
    while (x <= win->xpvmax)
    {
      /* Get x pixel in image */
      xsc = ((ysize - 1) * (x - win->xpvmin)) / win->xvsize;
      *fb = image [ysc + xsc * xsize];
      fb++;
      x++;
    }

    y++;
  }
}

/***********************************************************************
 *
 * Function: swim_put_scale_right_image
 *
 * Purpose: Puts an raw image in a window scaled and rotated right.
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     win   : Window identifier
 *     image : pointer to image data, must be in display color format
 *     xsize : Size of the image in horizontal pixels
 *     ysize : Size of the image in vertical pixels
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes:
 *     Pixels should be organized in the image from left to right, top
 *     to bottom. (BMP images are not stored like this.)
 *
 **********************************************************************/
void swim_put_scale_right_image(SWIM_WINDOW_T *win,
                                const COLOR_T *image,
                                INT_32 xsize,
                                INT_32 ysize)
{
  INT_32 xsc, ysc;
  INT_32 x, y;
  COLOR_T *fb;

  /* Top of window */
  y = win->ypvmin;

  /* Rescale image into window */
  while (y <= win->ypvmax)
  {
    x = win->xpvmin;

    /* Scale y coords of picture into x axis */
    ysc = ((xsize - 1) * (y - win->ypvmin)) / win->yvsize;
    fb = win->fb + x + (y * win->xpsize);

    /* Render a single horizontal line with 'y' data */
    while (x <= win->xpvmax)
    {
      /* Get x pixel in image */
      xsc = ((ysize - 1) * (win->xpvmax - x)) / win->xvsize;
      *fb = image [ysc + xsc * xsize];
      fb++;
      x++;
    }

    y++;
  }
}

/***********************************************************************
 *
 * Function: swim_put_win_image
 *
 * Purpose: This function simply provides a single API for all the
 *          image functions.
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     win   : Window identifier
 *     image : pointer to image data, must be in display color format
 *     xsize : Size of the image in horizontal pixels
 *     ysize : Size of the image in vertical pixels
 *     scale : If set, the picture will be scaled to the window size
 *             If not set, the picture will be clipped
 *     rtype : Rotation type flag, either Norotation, Left, Right, or
 *             Invert
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void swim_put_win_image(SWIM_WINDOW_T *win,
                        const COLOR_T *image,
                        INT_32 xsize,
                        INT_32 ysize,
                        INT_32 scale,
                        SWIM_ROTATION_T rtype)
{
  switch (rtype)
  {
    case INVERT:
      if (scale != 0)
      {
        swim_put_scale_invert_image(win, image, xsize, ysize);
      }
      else
      {
        swim_put_invert_image(win, image, xsize, ysize);
      }
      break;

    case LEFT:
      if (scale != 0)
      {
        swim_put_scale_left_image(win, image, xsize, ysize);
      }
      else
      {
        swim_put_left_image(win, image, xsize, ysize);
      }
      break;

    case RIGHT:
      if (scale != 0)
      {
        swim_put_scale_right_image(win, image, xsize, ysize);
      }
      else
      {
        swim_put_right_image(win, image, xsize, ysize);
      }
      break;

    case NOROTATION:
    default:
      if (scale != 0)
      {
        swim_put_scale_image(win, image, xsize, ysize);
      }
      else
      {
        swim_put_image(win, image, xsize, ysize);
      }
      break;
  }
}
