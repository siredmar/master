/***********************************************************************
 * $Id:: lpc_swim_font.c 745 2008-05-13 19:59:29Z pdurgesh             $
 *
 * Project: Font management for SWIM
 *
 * Description:
 *     See the sma_swim_font.h header file for a description of this
 *     package.
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

#include "lpc_swim_font.h"

/***********************************************************************
 * Public functions
 **********************************************************************/

/***********************************************************************
 *
 * Function: swim_get_word_len
 *
 * Purpose: Determines the length of the word (in pixels) up to the
 *          first whitespace character.
 *
 * Processing:
 *     The string is searched for the first whitespace occurance. For
 *     each non-whitespace character, the required length of that
 *     character in the window is computed. For the entire string up
 *     to the first whitespace character, the total length is returned.
 *
 * Parameters:
 *     win  : Window identifier
 *     text : Text string to output in window
 *
 * Outputs: None
 *
 * Returns:
 *     The length of the word (with a stop at the first whitespace) in
 *     the passed string.
 *
 * Notes: None
 *
 **********************************************************************/
INT_16 swim_get_word_len(SWIM_WINDOW_T *win,
                         const CHAR *text)
{
  INT_32 i;
  INT_16 wlen = 0;

  /* Find the length in pixels of the next word (separated by
     whitespace) */
  i = 0;
  while (((UNS_8) text [i] > ' ') && ((UNS_8) text [i] <= 0x7E))
  {
    wlen = wlen + win->font->font_width_table
           [(UNS_8) text [i] - win->font->first_char];
    i++;
  }

  return wlen;
}

/***********************************************************************
 *
 * Function: swim_put_word
 *
 * Purpose: Puts a word in the window, but adds a newline to keep the
 *          word contiguous (without an edge break) if necessary.
 *
 * Processing:
 *     The first word in the string (up to the first whitespace
 *     character) has its total length (based on font) computed and
 *     compared to the right window edge. If the length exceeds the
 *     window edge, a newline occurs and then the word is output.
 *
 * Parameters:
 *     win  : Window identifier
 *     text : Text string to output in window
 *
 * Outputs:  None
 *
 * Returns: The number of characters output to the display.
 *
 * Notes: None
 *
 **********************************************************************/
INT_32 swim_put_word(SWIM_WINDOW_T *win,
                     const CHAR *text)
{
  INT_32 i;

  /* Will the length of the next word exceed the window margin? */
  if ((swim_get_word_len(win, text) + win->xvpos) > win->xpvmax)
  {
    /* Do a newline */
    swim_put_newline(win);
  }

  /* Put just the word characters on the display up to the next
     non-whitespace character or the end of the string */
  i = 0;
  while (((UNS_8) text [i] > ' ') && ((UNS_8) text [i] <= 0x7E))
  {
    swim_put_char(win, text [i]);
    i++;
  }

  return i;
}

/***********************************************************************
 * Public functions
 **********************************************************************/

/***********************************************************************
 *
 * Function: swim_put_text_xy
 *
 * Purpose: Put text at x, y (char) position on screen
 *
 * Processing:
 *     Set the virtual (upper left) text position in the window and
 *     render the text string at this position.
 *
 * Parameters:
 *     win  : Window identifier
 *     text : Text string to output in window
 *     x    : Virtual X position of start of text
 *     y    : Virtual Y position of start of text
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes:
 *     X, Y coords are in virtual pixels!
 *
 **********************************************************************/
void swim_put_text_xy(SWIM_WINDOW_T *win,
                      const CHAR *text,
                      INT_32 x,
                      INT_32 y)
{
  /* Convert virtual x, y positon to physical position */
  swim_set_xy(win, x, y);

  /* Display text string */
  swim_put_text(win, text);
}

/***********************************************************************
 *
 * Function: swim_set_xy
 *
 * Purpose: Sets the X, Y pixel coordinates for the next text operation
 *
 * Processing:
 *     Update the X, Y text position pointers, limiting the position to
 *     the window dimensions.
 *
 * Parameters:
 *     win  : Window identifier
 *     x    : Virtual X position of start of text
 *     y    : Virtual Y position of start of text
 *
 * Outputs:  None
 *
 * Returns: Nothing
 *
 * Notes:
 *     X, Y coords are in virtual pixels!
 *
 **********************************************************************/
void swim_set_xy(SWIM_WINDOW_T *win,
                 INT_32 x,
                 INT_32 y)
{
  win->xvpos = x + win->xpvmin;
  win->yvpos = y + win->ypvmin;

  /* Limit to window dimensions */
  if (win->xvpos < win->xpvmin)
  {
    win->xvpos = win->xpvmin;
  }
  else if (win->xvpos > win->xpvmax)
  {
    win->xvpos = win->xpvmax;
  }

  if (win->yvpos < win->ypvmin)
  {
    win->yvpos = win->ypvmin;
  }
  else if (win->yvpos > win->ypvmax)
  {
    win->yvpos = win->ypvmax;
  }
}

/***********************************************************************
 *
 * Function: swim_get_xy
 *
 * Purpose: Returns the X, Y pixel coordinates for the next text
 *          operation
 *
 * Processing:
 *     The logical X and Y positions are computed by subtracting the
 *     physical text position values by the physical minimum window
 *     limits.
 *
 * Parameters:
 *     win  : Window identifier
 *     x    : Address of where to return virtual X value
 *     y    : Address of where to return virtual X value
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes:
 *     X, Y coords are in virtual pixels!
 *
 **********************************************************************/
void swim_get_xy(SWIM_WINDOW_T *win,
                 INT_32 *x,
                 INT_32 *y)
{
  *x = win->xvpos - win->xpvmin;
  *y = win->yvpos - win->ypvmin;
}

/***********************************************************************
 *
 * Function: swim_put_text
 *
 * Purpose: Puts a string of text in a window
 *
 * Processing:
 *     Each character will be routed to the swim_put_char function until
 *     a string terminator is reached. For newline characters, a newline
 *     will occur instead of a character output.
 *
 * Parameters:
 *     win  : Window identifier
 *     text : Text string to output in window
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes:  None
 *
 **********************************************************************/
void swim_put_text(SWIM_WINDOW_T *win,
                   const CHAR *text)
{
  INT_32 i = 0;

  /* Continue until the entire string is output */
  while (text [i] != '\0')
  {
    if (text [i] == '\n')
    {
      swim_put_newline(win);
    }
    else if (((UNS_8) text [i] >= win->font->first_char)
             && ((UNS_8) text [i] <= win->font->last_char))
    {
      /* Put character on screen */
      swim_put_char(win, text [i]);
    }

    i++;
  }
}

/***********************************************************************
 *
 * Function: swim_put_ltext
 *
 * Purpose: Puts a string of text in a window, but will adjust the
 *          position of a word if the word length exceeds the edge of
 *          the display.
 *
 * Processing:
 *     While the string has data in it, check for the newline character.
 *     If it exists, output a newline. If the string data is inside the
 *     font character table, output the first word in the string (with
 *     support for generating a newline if the word will exceed the
 *     window edge). Continue until all words/characters are output.
 *
 * Parameters:
 *     win  : Window identifier
 *     text : Text string to output in window
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes:  None
 *
 **********************************************************************/
void swim_put_ltext(SWIM_WINDOW_T *win,
                    const CHAR *text)
{
  INT_32 i = 0;

  /* Continue until the entire string is output */
  while (text [i] != '\0')
  {
    if (text [i] == '\n')
    {
      swim_put_newline(win);
      i++;
    }
    else if (((UNS_8) text [i] >= win->font->first_char)
             && ((UNS_8) text [i] <= win->font->last_char))
    {
      /* Check for entire words first */
      if (((UNS_8) text [i] > ' ') && ((UNS_8) text [i] <= 0x7E))
      {
        /* Put entire word on screen */
        i = i + swim_put_word(win, &text [i]);
      }
      else
      {
        swim_put_char(win, text [i]);
        i++;
      }
    }
    else
    {
      /* Put a space out */
      swim_put_char(win, ' ');
      i++;
    }
  }
}

/***********************************************************************
 *
 * Function: swim_window_scroll
 *
 * Purpose: Scrolls the window up one line
 *
 * Processing:
 *     From the vertical font size and lower window limit, determine the
 *     move indices for the move operation. Move all lines up by the
 *     numbers of pixel in the present font height until the top of the
 *     window has been reached.
 *
 * Parameters:
 *     win   : Window identifier
 *     lines : Number of lines to scroll up
 *
 * Outputs:  None
 *
 * Returns:  Nothing
 *
 * Notes: This function is private to this module.
 *
 **********************************************************************/
void swim_window_scroll(SWIM_WINDOW_T *win,
                        INT_32 lines)
{
  COLOR_T *fb1, *fb2;
  INT_32 yref1 = win->ypvmin;
  INT_32 yref2 = yref1 + lines;
  INT_32 ref;

  while (yref2 <= win->ypvmax)
  {
    /* Line move addresses */
    fb1 = win->fb + win->xpvmin + (yref1 * win->xpsize);
    fb2 = win->fb + win->xpvmin + (yref2 * win->xpsize);

    /* Move a single line at a time */
    ref = win->xpvmax - win->xpvmin + 1;
    while (ref > 0)
    {
      *fb1++ = *fb2++;
      ref--;
    }

    /* Next lines */
    yref1++;
    yref2++;
  }

  /* Clear out bottom lines */
  yref1 = win->yvpos;
  while (yref1 <= win->ypvmax)
  {
    /* Line clear address */
    fb1 = win->fb + win->xpvmin + (yref1 * win->xpsize);

    /* Clear a single line at a time */
    ref = win->xpvmax - win->xpvmin + 1;
    while (ref > 0)
    {
      *fb1++ = win->bkg;
      ref--;
    }

    yref1++;
  }
}

/***********************************************************************
 *
 * Function: swim_put_char
 *
 * Purpose: Puts a character in the window.
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     win      : Window identifier
 *     textchar : Text string to output in window
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void swim_put_char(SWIM_WINDOW_T *win,
                   const CHAR textchar)
{
  INT_32 i, j;
  INT_32 charindex;
  UNS_16 *charfields, chardata;
  COLOR_T *fb;

  /* If this is a carriage return, do a newline */
  if (textchar == '\n')
  {
    swim_put_newline(win);
  }
  else
  {
    /* Determine index to character data */
    charindex = (INT_32) textchar - (INT_32) win->font->first_char;

    /* Will the character fit on the display? */
    if ((win->xvpos +
         (INT_32) win->font->font_width_table [charindex]) >
        win->xpvmax)
    {
      /* Will not fit, do a newline */
      swim_put_newline(win);
    }

    /* Determine the start of the bitfields for the character */
    charfields = win->font->font_table + (charindex *
                                          win->font->font_height);

    /* Map character to the window */
    for (i = 0; i < (INT_32) win->font->font_height; i++)
    {
      /* Get starting pixel location for font mapping in window */
      fb = win->fb + win->xvpos + ((win->yvpos + i) * win->xpsize);

      /* Get character line mapping data */
      chardata = charfields [i];

      /* Convert character line bit data to a pixel line in
         window */
      for (j =
             (INT_32) win->font->font_width_table [charindex];
           j > 0; j--)
      {
        if ((chardata & 0x8000) != 0)
        {
          *fb++ = win->pen;
        }
        else if (win->tfont != 0)
        {
          *fb++ = win->bkg;
        }
        else
        {
          fb++;
        }

        /* Next bit in character line */
        chardata = chardata << 1;
      }
    }

    /* Increment to next text location */
    win->xvpos = win->xvpos +
                 (INT_32) win->font->font_width_table [charindex];
  }
}

/***********************************************************************
 *
 * Function: swim_put_newline
 *
 * Purpose: Performs a newline in a window
 *
 * Processing:
 *     Set the text pointer for the next text character operation to
 *     the beginning of the following line. If the following lin
 *     exceeds the window size, perform a line scroll.
 *
 * Parameters:
 *     win : Window identifier
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void swim_put_newline(SWIM_WINDOW_T *win)
{
  INT_32 diff;

  /* Set text pointer to start of next line */
  win->xvpos = win->xpvmin;
  win->yvpos = win->yvpos + (INT_32) win->font->font_height;

  /* Next character is below bottom of window, scroll the window
     up */
  while ((win->yvpos +
          (INT_32) win->font->font_height) > win->ypvmax)
  {
    /* Scroll just enough for the next line */
    diff = (INT_32) win->font->font_height -
           (win->ypvmax - win->yvpos);
    win->yvpos = win->yvpos - diff;
    swim_window_scroll(win, diff);
  }
}

/***********************************************************************
 *
 * Function: swim_set_font
 *
 * Purpose: Sets the active font
 *
 * Processing:
 *     Switch to the selected font by setting the font structure pointer
 *     in the windows structure based on the passed enumeration. If the
 *     next character output in the new font will exceed the window
 *     limit, perform a window text scroll.
 *
 * Parameters:
 *     win  : Window identifier
 *     font : Pointer to the selected font data structure
 *
 * Outputs:  None
 *
 * Returns: Nothing
 *
 * Notes:  None
 *
 **********************************************************************/
void swim_set_font(SWIM_WINDOW_T *win,
                   FONT_T *font)
{
  INT_32 diff;

  win->font = font;

  /* After changing to the new font, determine if there is enough
     room for the font height on the existing line in the window */
  if ((win->yvpos + win->font->font_height) > win->ypvmax)
  {
    diff = (INT_32) win->font->font_height -
           (win->ypvmax - win->yvpos);
    win->yvpos = win->yvpos - diff;
    swim_window_scroll(win, diff);
  }
}

/***********************************************************************
 *
 * Function: swim_get_font_height
 *
 * Purpose: Returns the active font's height in pixels
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     win  : Window identifier
 *
 * Outputs: None
 *
 * Returns: The height of the active font in pixels.
 *
 * Notes:  None
 *
 **********************************************************************/
INT_16 swim_get_font_height(SWIM_WINDOW_T *win)
{
  return win->font->font_height;
}

/***********************************************************************
 *
 * Function: swim_set_title
 *
 * Purpose:  Creates a title bar in the window and adjusts the client
 *           area to be outside the title bar area.
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     win        : Window identifier
 *     title      : title string to use for window
 *     ttlbkcolor : Background color in title area
 *
 * Outputs: None
 *
 * Returns:  Nothing
 *
 * Notes:
 *     Do not call this function more than once for a window or
 *     problems may occur.
 *
 **********************************************************************/
void swim_set_title(SWIM_WINDOW_T *win,
                    const CHAR *title,
                    COLOR_T ttlbkcolor)
{
  COLOR_T savedf, savedp, savedb;
  INT_32 savedt;

  /* Is present font height larger than window client height? */
  if ((swim_get_font_height(win) < (4 + win->yvsize)) &&
      (title != (CHAR *) 0))
  {
    /* There is enough room for title bar, so continue */

    /* Save original colors and font transparentcy flag */
    savedf = win->fill;
    savedp = win->pen;
    savedb = win->bkg;
    savedt = win->tfont;

    /* Set fill color to background color (temporarily)
       used with box function */
    win->fill = ttlbkcolor;
    win->bkg = ttlbkcolor;
    win->pen = win->bkg;

    /* Draw the background for the title bar */
    swim_put_box(win, 0, 0, win->xvsize,
                 (4 + swim_get_font_height(win) - 2));

    /* Reset text starting position for title string */
    win->xvpos = win->xpvmin + 2;
    win->yvpos = win->ypvmin + 1;

    /* Restore original pen color (used for text color) */
    win->pen = savedp;

    /* Restore the original colors */
    win->fill = savedf;
    win->bkg = savedb;

    /* Put string in title bar area (with transparent background) */
    win->tfont = 0;
    swim_put_text(win, title);
    win->tfont = savedt;

    /* Draw a line under the title bar, but before the
       (new) client area */
    swim_put_line(win, 0,
                  (4 + swim_get_font_height(win) - 1),
                  win->xpvmax, (4 + swim_get_font_height(win) - 1));

    /* Adjust client height of window (virtual and physcal) */
    win->ypmin = win->ypmin + swim_get_font_height(win) + 4;
    win->ypvmin = win->ypvmin + swim_get_font_height(win) + 4;

    /* Resize y dimension */
    win->yvsize = win->yvsize - swim_get_font_height(win) + 4;

    /* Reset text starting position to new client area */
    win->xvpos = win->xpvmin;
    win->yvpos = win->ypvmin;
  }
}

/***********************************************************************
 *
 * Function: swim_set_font_trasparency
 *
 * Purpose: Enables and disables font backgrounds. When set, the font
 *          background will not be drawn in the background color (useful
 *          for  painting text over pictures).
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     win    : Window identifier
 *    trans  : When not 0, the font backgrounds will not be drawn
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void swim_set_font_trasparency(SWIM_WINDOW_T *win,
                               INT_32 trans)
{
  win->tfont = trans;
}
