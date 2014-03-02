/***********************************************************************
 * $Id:: lpc_swim.c 7684 2011-06-23 20:37:49Z usb10132                 $
 *
 * Project: Simple Windowing Interface Manager (SWIM)
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

#include "lpc_swim.h"
#include "lpc_fonts.h"
#include "lpc_helvr10.h"

/***********************************************************************
 * Private functions
 **********************************************************************/

static INT_32 swim_abs(INT_32 v1)
{
   if (v1 > 0)
     return v1;

  return -v1;
}
 
/***********************************************************************
 *
 * Function: swim_put_line_raw
 *
 * Purpose: Draw a line on the physical display
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     win : Window identifier
 *     x1  : Physical X position of X line start
 *     y1  : Physical Y position of Y line start
 *     x2  : Physical X position of X line end
 *     y2  : Physical Y position of Y line end
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void swim_put_line_raw(SWIM_WINDOW_T *win,
                       INT_32 x1,
                       INT_32 y1,
                       INT_32 x2,
                       INT_32 y2)
{
  INT_32 e2, sx, sy, dx, dy, err;
  
  /* calculate delta_x and delta_y */
  dx = swim_abs(x2 - x1);
  dy = swim_abs(y2 - y1);
 
  /* set the direction for the step for both x and y, and
     initialize the error */
  if (x1 < x2)
    sx = 1;
  else
    sx = -1;

  if (y1 < y2)
    sy = 1;
  else
    sy = -1;

  err = dx - dy;
   
  while (1)
  {
    if ((x1 >= 0) && (x1 <= win->xpsize) && 
      (y1 >= 0) && (y1 <= win->ypsize))
        *(win->fb + x1 + (y1 * win->xpsize)) = win->pen;

    if ((x1 == x2) && (y1 == y2))
       return;

    e2 = 2 * err;
    if (e2 > -dy)
    {
      err -= dy;
      x1 += sx;
    }
    if (e2 < dx)
    {
      err += dx;
      y1 += sy;
    }
  }
}

/***********************************************************************
 *
 * Function: swim_window_open_p
 *
 * Purpose: Initializes a window and the default values for the window
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     win          : Preallocated windows structure to fill
 *     xsize        : Physical horizontal dimension of the display
 *     ysize        : Physical vertical dimension of the display
 *     fbaddr       : Address of the display's frame buffer
 *     xwin_min     : Physical window left coordinate
 *     ywin_min     : Physical window top coordinate
 *     xwin_max     : Physical window right coordinate
 *     ywin_max     : Physical window bottom coordinate
 *     border_width : Width of the window border in pixels
 *     pcolor       : Pen color
 *     bkcolor      : Background color
 *     fcolor       : Fill color
 *     clear        : Clear window flag
 *
 * Outputs: None
 *
 * Returns:
 *  TRUE if the window was initialized correctly, otherwise FALSE
 *
 * Notes:
 *     This function must be called prior to any other window function
 *
 **********************************************************************/
BOOL_32 swim_window_open_p(SWIM_WINDOW_T *win,
                           INT_32 xsize,
                           INT_32 ysize,
                           COLOR_T *fbaddr,
                           INT_32 xwin_min,
                           INT_32 ywin_min,
                           INT_32 xwin_max,
                           INT_32 ywin_max,
                           INT_32 border_width,
                           COLOR_T pcolor,
                           COLOR_T bkcolor,
                           COLOR_T fcolor,
                           BOOL_32 clear)
{
  INT_32 i;
  BOOL_32 init = FALSE;

  /* Before continuing, check to see that the window size is
     in the physical dimensions of the display */
  if ((xwin_min >= 0) && (ywin_min >= 0) &&
      (xwin_max < xsize) && (ywin_max < ysize))
  {
    init = TRUE;
  }
  else
  {
    /* Window size is out of the physical display size, so it
       should be invalidated */
    win->winused = 0x0;
  }

  if (init == TRUE)
  {
    /* Save physical display dimensions */
    win->xpsize = xsize;
    win->ypsize = ysize;

    /* Save frame buffer address */
    win->fb = fbaddr;

    /* Save physical window dimensions and default colors */
    win->xpmin = xwin_min;
    win->ypmin = ywin_min;
    win->xpmax = xwin_max;
    win->ypmax = ywin_max;
    win->pen = pcolor;
    win->bkg = bkcolor;
    win->fill = fcolor;

    /* Compute physical window dimensions of draw area only */
    win->xpvmin = xwin_min + border_width;
    win->ypvmin = ywin_min + border_width;
    win->xpvmax = xwin_max - border_width;
    win->ypvmax = ywin_max - border_width;

    /* Compute virtual window size of draw area */
    win->xvsize = xwin_max - xwin_min - 2 * border_width;
    win->yvsize = ywin_max - ywin_min - 2 * border_width;

    /* Fill in any unused border padding between draw area and border
       will fill color */
    for (i = 0; i < border_width; i++)
    {
      swim_put_line_raw(win, (xwin_min + i),
                        (ywin_min + i), (xwin_max - i), (ywin_min + i));
      swim_put_line_raw(win, (xwin_max - i),
                        (ywin_min + i), (xwin_max - i), (ywin_max - i));
      swim_put_line_raw(win, (xwin_max - i),
                        (ywin_max - i), (xwin_min + i), (ywin_max - i));
      swim_put_line_raw(win, (xwin_min + i),
                        (ywin_max - i), (xwin_min + i), (ywin_min + i));
    }

    /* Clear draw area with background color */
    if (clear == TRUE)
    {
      swim_clear_screen(win, bkcolor);
    }

    /* Use the default font and make background transparent */
    win->font = (FONT_T *) & font_helvr10;

    /* Set starting text position in upper left of window */
    win->xvpos = win->xpvmin;
    win->yvpos = win->ypvmin;
  }

  return init;
}

/***********************************************************************
 * Public functions
 **********************************************************************/

/***********************************************************************
 *
 * Function: swim_put_pixel
 *
 * Purpose: Puts a pixel at the virtual X, Y coordinate in the window
 *
 * Processing:
 *     Convert the virtual pixel position to a physical position.
 *     If the pixel is inside the window draw area, update the pixel
 *     on the display.
 *
 * Parameters:
 *     win : Window identifier
 *     x1  : Virtual X position of pixel
 *     y1  : Virtual Y position of pixel
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes:
 *     The pixel will not be displayed if the pixel exceeds the window
 *     virtual size. Pixel positions below 0 should not be used with
 *     this function.
 *
 **********************************************************************/
void swim_put_pixel(SWIM_WINDOW_T *win,
                    INT_32 x1,
                    INT_32 y1)
{
  INT_16 realx, realy;

  /* Convert virtual coordinate to physical coordinate taking into
     consideration the border size of the window */
  realx = win->xpvmin + x1;
  realy = win->ypvmin + y1;

  /* Only put the pixel in the window if it fits in the window */
  if ((realx <= win->xpvmax) &&
      (realy <= win->ypvmax))
  {
    *(win->fb + realx + (realy * win->xpsize)) = win->pen;
  }
}

/***********************************************************************
 *
 * Function: swim_put_line
 *
 * Purpose: Draw a line in the virtual window with clipping.
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     win : Window identifier
 *     x1  : Virtual X position of X line start
 *     y1  : Virtual Y position of Y line start
 *     x2  : Virtual X position of X line end
 *     y2  : Virtual Y position of Y line end
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes:
 *     This function supports clipping.
 *
 **********************************************************************/
void swim_put_line(SWIM_WINDOW_T *win,
                   INT_32 x1,
                   INT_32 y1,
                   INT_32 x2,
                   INT_32 y2)
{
  INT_32 e2, sx, sy, dx, dy, err;

  /* Convert virtual coordinates to physical coordinates */
  x1 = x1 + win->xpvmin;
  x2 = x2 + win->xpvmin;
  y1 = y1 + win->ypvmin;
  y2 = y2 + win->ypvmin;
   
  /* calculate delta_x and delta_y */
  dx = swim_abs(x2 - x1);
  dy = swim_abs(y2 - y1);
 
  /* set the direction for the step for both x and y, and
     initialize the error */
  if (x1 < x2)
    sx = 1;
  else
    sx = -1;

  if (y1 < y2)
    sy = 1;
  else
    sy = -1;

  err = dx - dy;
   
  while (1)
  {
    if ((x1 >= win->xpvmin) && (x1 <= win->xpvmax) && 
      (y1 >= win->ypvmin) && (y1 <= win->ypvmax))
        *(win->fb + x1 + (y1 * win->xpsize)) = win->pen;

    if ((x1 == x2) && (y1 == y2))
       return;

    e2 = 2 * err;
    if (e2 > -dy)
    {
      err -= dy;
      x1 += sx;
    }
    if (e2 < dx)
    {
      err += dx;
      y1 += sy;
    }
  }
}

/***********************************************************************
 *
 * Function: swim_put_diamond
 *
 * Purpose:
 * Purpose: Draw a diamond in the virtual window
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     win : Window identifier
 *     x   : Virtual X position of the diamond
 *     y   : Virtual Y position of the diamond
 *     rx  : Radius for horizontal
 *     ry  : Radius for vertical
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes:
 *     This function supports clipping.
 *
 **********************************************************************/
void swim_put_diamond(SWIM_WINDOW_T *win,
                      INT_32 x,
                      INT_32 y,
                      INT_32 rx,
                      INT_32 ry)
{
  INT_32 xleft, xright, xleft1, xleft2, xright1, idy, ypmid;
  INT_32 ypmin, ypmax, dlta, err, e2;

  /* Use line draw functions to draw border in pen color in virtual
     coordinates */
  swim_put_line(win, x - rx, y, x, y - ry);
  swim_put_line(win, x + rx, y, x, y - ry);
  swim_put_line(win, x - rx, y, x, y + ry);
  swim_put_line(win, x + rx, y, x, y + ry);

  /* Adjust rx and rx for interior fill region minus border */
  rx--;
  ry--;
  if ((rx <= 0) || (ry <= 0))
    return;

  /* Y limits in physical coordinates minus border line */
  ypmin = y - ry + win->ypvmin;
  ypmid = y + win->ypvmin;
  ypmax = y + ry + win->ypvmin;

  /* X starts draw from center line */
  xleft = xright = x + win->xpvmin;

  err = rx - ry;
  dlta = 1 + rx / ry;

  for (idy = ypmin; idy <= ypmid; idy++)
  {
    xleft1 = xleft2 = xleft;
	xright1 = xright;

    /* Clip left and right to virtual window size */
	if (xleft1 < win->xpvmin)
	  xleft2 = xleft1 = win->xpvmin;
	if (xright1 > win->xpvmax)
	  xright1 = win->xpvmax;

    /* Is top half visible? */
    if ((idy >= win->ypvmin) && (idy <= win->ypvmax))
	{
	  while (xleft1 <= xright1)
	  {
	    * (win->fb + xleft1 + (idy * win->xpsize)) = win->fill;
		xleft1++;
	  }
	}

    /* Draw bottom half if visible */
    if ((ypmax >= ypmid) && (ypmax <= win->ypvmax))
	{
	  /* Mirror bottom */
	  while (xleft2 <= xright1)
	  {
	    * (win->fb + xleft2 + (ypmax * win->xpsize)) = win->fill;
		xleft2++;
	  }
	}
	ypmax--;

    e2 = 2 * err;
    if (e2 > -ry)
    {
      err -= ry;
      xleft -= dlta;
      xright += dlta;
    }
    if (e2 < rx)
    {
      err += rx;
    }
  }
}

/***********************************************************************
 *
 * Function: swim_clear_screen
 *
 * Purpose:
 *     Fills the draw area of the display with the selected color
 *
 * Processing:
 *     Loop through all virtual window (draw area) locations and
 *     updates them with the passed color value.
 *
 * Parameters:
 *     win  : Window identifier
 *     colr : Color to place in the window
 *
 * Outputs:  None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void swim_clear_screen(SWIM_WINDOW_T *win,
                       COLOR_T colr)
{
  INT_32 x, y;

  for (y = win->ypvmin; y <= win->ypvmax; y++)
  {
    for (x = win->xpvmin; x <= win->xpvmax; x++)
    {
      * (win->fb + x + (y * win->xpsize)) = colr;
    }
  }
}

/***********************************************************************
 *
 * Function: swim_put_box
 *
 * Purpose: Place a box with corners (X1, Y1) and (X2, Y2)
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     win : Window identifier
 *     x1  : Virtual left position of box
 *     y1  : Virtual upper position of box
 *     x2  : Virtual right position of box
 *     y2  : Virtual lower position of box
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void swim_put_box(SWIM_WINDOW_T *win,
                  INT_32 x1,
                  INT_32 y1,
                  INT_32 x2,
                  INT_32 y2)
{
  INT_32 xinc, yinc;
  INT_32 ysave;

  if (x1 > x2)
  {
    xinc = x1;
    x1 = x2;
    x2 = xinc;
  }

  /* Swap y1 and y2 if y1 is larger than y2 */
  if (y1 > y2)
  {
    yinc = y1;
    y1 = y2;
    y2 = yinc;
  }

  /* Convert virtual coordinates to physical coordinates */
  x1 = x1 + win->xpvmin;
  x2 = x2 + win->xpvmin;
  y1 = y1 + win->ypvmin;
  y2 = y2 + win->ypvmin;

  /* Clip boxes to window sizes */
  if (x1 < win->xpvmin)
  {
    x1 = win->xpvmin;
  }
  if (y1 < win->ypvmin)
  {
    y1 = win->ypvmin;
  }
  if (x2 > win->xpvmax)
  {
    x2 = win->xpvmax;
  }
  if (y2 > win->ypvmax)
  {
    y2 = win->ypvmax;
  }

  /* Get X and Y differences */
  xinc = x2 - x1;
  yinc = y2 - y1;

  /* Make outer edge of box in pen color */
  swim_put_line_raw(win, x1, y1, x2, y1);
  swim_put_line_raw(win, x2, y1, x2, y2);
  swim_put_line_raw(win, x2, y2, x1, y2);
  swim_put_line_raw(win, x1, y2, x1, y1);

  /* Increment X, Y values so they won't overwrite the edge */
  x1++;
  y1++;

  /* Draw the box inside with the fill color */
  ysave = y1;
  while (x1 < x2)
  {
    y1 = ysave;
    while (y1 < y2)
    {
      * (win->fb + x1 + (y1 * win->xpsize)) = win->fill;
      y1++;
    }

    x1++;
  }
}

/***********************************************************************
 *
 * Function: swim_window_open
 *
 * Purpose: Initializes a window and the default values for the window
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     win          : Preallocated windows structure to fill
 *     xsize        : Physical horizontal dimension of the display
 *     ysize        : Physical vertical dimension of the display
 *     fbaddr       : Address of the display's frame buffer
 *     xwin_min     : Physical window left coordinate
 *     ywin_min     : Physical window top coordinate
 *     xwin_max     : Physical window right coordinate
 *     ywin_max     : Physical window bottom coordinate
 *     border_width : Width of the window border in pixels
 *     pcolor       : Pen color
 *     bkcolor      : Background color
 *     fcolor       : Fill color
 *
 * Outputs: None
 *
 * Returns:
 *  TRUE if the window was initialized correctly, otherwise FALSE
 *
 * Notes:
 *     This function must be called prior to any other window function
 *
 **********************************************************************/
BOOL_32 swim_window_open(SWIM_WINDOW_T *win,
                         INT_32 xsize,
                         INT_32 ysize,
                         COLOR_T *fbaddr,
                         INT_32 xwin_min,
                         INT_32 ywin_min,
                         INT_32 xwin_max,
                         INT_32 ywin_max,
                         INT_32 border_width,
                         COLOR_T pcolor,
                         COLOR_T bkcolor,
                         COLOR_T fcolor)
{
  BOOL_32 init;

  init = swim_window_open_p(win, xsize, ysize, fbaddr, xwin_min,
                            ywin_min, xwin_max, ywin_max, border_width, pcolor, bkcolor,
                            fcolor, TRUE);

  /* Default font background is not transparent */
  win->tfont = 1;

  return init;
}

/***********************************************************************
 *
 * Function: swim_window_open_noclear
 *
 * Purpose: Initializes a window and the default values for the window
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     win          : Preallocated windows structure to fill
 *     xsize        : Physical horizontal dimension of the display
 *     ysize        : Physical vertical dimension of the display
 *     fbaddr       : Address of the display's frame buffer
 *     xwin_min     : Physical window left coordinate
 *     ywin_min     : Physical window top coordinate
 *     xwin_max     : Physical window right coordinate
 *     ywin_max     : Physical window bottom coordinate
 *     border_width : Width of the window border in pixels
 *     pcolor       : Pen color
 *     bkcolor      : Background color
 *     fcolor       : Fill color
 *
 * Outputs: None
 *
 * Returns:
 *  TRUE if the window was initialized correctly, otherwise FALSE
 *
 * Notes:
 *     This function must be called prior to any other window function
 *
 **********************************************************************/
BOOL_32 swim_window_open_noclear(SWIM_WINDOW_T *win,
                                 INT_32 xsize,
                                 INT_32 ysize,
                                 COLOR_T *fbaddr,
                                 INT_32 xwin_min,
                                 INT_32 ywin_min,
                                 INT_32 xwin_max,
                                 INT_32 ywin_max,
                                 INT_32 border_width,
                                 COLOR_T pcolor,
                                 COLOR_T bkcolor,
                                 COLOR_T fcolor)
{
  BOOL_32 init;

  init = swim_window_open_p(win, xsize, ysize, fbaddr, xwin_min,
                            ywin_min, xwin_max, ywin_max, border_width, pcolor, bkcolor,
                            fcolor, FALSE);

  /* Default font background is transparent */
  win->tfont = 0;

  return init;
}

/***********************************************************************
 *
 * Function: swim_window_close
 *
 * Purpose: Reallocates a window for use
 *
 * Processing:
 *     For the passed window ID, clear the window used flag.
 *
 * Parameters:
 *     win : Window identifier
 *
 * Outputs:  None
 *
 * Returns:  Nothing
 *
 * Notes:
 *     This is a defunct function and is not needed.
 *
 **********************************************************************/
void swim_window_close(SWIM_WINDOW_T *win)
{
  win->winused = 0x0;
}

/***********************************************************************
 *
 * Function: swim_set_pen_color
 *
 * Purpose: Sets the pen color
 *
 * Processing:
 *     For the passed window ID, update to the passed pen color.
 *
 * Parameters:
 *     win       : Window identifier
 *     pen_color : New pen color
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void swim_set_pen_color(SWIM_WINDOW_T *win,
                        COLOR_T pen_color)
{
  win->pen = pen_color;
}

/***********************************************************************
 *
 * Function: swim_set_fill_color
 *
 * Purpose: Sets the fill color
 *
 * Processing:
 *     For the passed window ID, update to the passed fill color.
 *
 * Parameters:
 *     win        : Window identifier
 *     fill_color : New fill color
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void swim_set_fill_color(SWIM_WINDOW_T *win,
                         COLOR_T fill_color)
{
  win->fill = fill_color;
}

/***********************************************************************
 *
 * Function: swim_set_bkg_color
 *
 * Purpose: Sets the color used for backgrounds
 *
 * Processing:
 *     For the passed window ID, update to the passed background color.
 *
 * Parameters:
 *     win        : Window identifier
 *     tbkg_color : New background color
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void swim_set_bkg_color(SWIM_WINDOW_T *win,
                        COLOR_T bkg_color)
{
  win->bkg = bkg_color;
}

/***********************************************************************
 *
 * Function: swim_get_horizontal_size
 *
 * Purpose: Get the virtual window horizontal size
 *
 * Processing:
 *     For the passed window ID, return the x size of the window.
 *
 * Parameters:
 *     win        : Window identifier
 *
 * Outputs: None
 *
 * Returns: The virtual window horizontal size
 *
 * Notes: None
 *
 **********************************************************************/
INT_32 swim_get_horizontal_size(SWIM_WINDOW_T *win)
{
  return win->xvsize;
}

/***********************************************************************
 *
 * Function: swim_get_vertical_size
 *
 * Purpose: Get the virtual window vertical size
 *
 * Processing:
 *     For the passed window ID, return the x size of the window.
 *
 * Parameters:
 *     win        : Window identifier
 *
 * Outputs: None
 *
 * Returns: The virtual window horizontal size
 *
 * Notes: None
 *
 **********************************************************************/
INT_32 swim_get_vertical_size(SWIM_WINDOW_T *win)
{
  return win->yvsize;
}
