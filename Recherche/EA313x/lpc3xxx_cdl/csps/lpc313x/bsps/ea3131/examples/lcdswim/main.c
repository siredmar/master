/***********************************************************************
 * $Id:: main.c 6363 2011-02-03 07:36:50Z ing02124                     $
 *
 * Project: LCD example using SWIM library functions
 *
 * Description:
 *     A simple LCD display example using SWIM library functions.
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
#include <stdio.h>
#include <string.h>

#include "lpc_types.h"
#include "lpc_string.h"
#include "lpc_swim.h"
#include "lpc_fonts.h"
#include "lpc_swim_font.h"
#include "lpc_colors.h"
#include "lpc_irq_fiq.h"
#include "lpc_arm922t_cp15_driver.h"
#include "ea3131_board.h"
#include "lpc313x_timer_driver.h"
#include "lpc313x_intc_driver.h"
#include "lpc313x_ioconf_driver.h"
#include "lpc313x_uart_driver.h"
#include "lpc313x_pwm_driver.h"
#include "lpc313x_cgu_driver.h"
#include "lpc313x_cgu_driver.h"
#include "lpc313x_lcdc.h"
#include "lpc313x_spi_driver.h"

/* Fonts */
#include "lpc_helvr10.h"
#include "lpc_winfreesystem14x16.h"
#include "lpc_rom8x8.h"
#include "lpc_rom8x16.h"
#include "lpc_x5x7.h"
#include "lpc_x6x13.h"

#ifdef TGT_SDRAM
#include "lpc_swim_image.h"
#include "image.h"
#endif

/* LCD Frame buffer
	 Using SDRAM buffer, since the contents can not be
	 directly written onto display buffer
	 */
#define LCD_FRAME_BUF (0x32000000)

/* Display info */
#define DISPLAY_HEIGHT  320
#define DISPLAY_WIDTH   240

/* Color values */
#define   BLACKC        0x0000    /* 0, 0, 0 */
#define   NAVYC         0x000F    /*   0,   0, 128 */
#define   DARK_GREENC   0x03E0    /*   0, 128,   0 */
#define   DARK_CYANC    0x03EF    /*   0, 128, 128 */
#define   MAROONC       0x7800    /* 128,   0,   0 */
#define   PURPLEC       0x780F    /* 128,   0, 128 */
#define   OLIVEC        0x7BE0    /* 128, 128,   0 */
#define   LIGHT_GRAYC   0xC618    /* 192, 192, 192 */
#define   DARK_GRAYC    0x7BEF    /* 128, 128, 128 */
#define   BLUEC         0x001F    /*   0,   0, 255 */
#define   GREENC        0x07E0    /*   0, 255,   0 */
#define   CYANC         0x07FF    /*   0, 255, 255 */
#define   REDC          0xF800    /* 255,   0,   0 */
#define   MAGENTAC      0xF81F    /* 255,   0, 255 */
#define   YELLOWC       0xFFE0    /* 255, 255, 0   */
#define   WHITEC        0xFFFF    /* 255, 255, 255 */

/* Debug UART buffer */
static char buff[512];

/* UART device ID */
static INT_32 g_uart_id;
const CHAR title[] =  "LPC313X LCD Demo \r\n";
const CHAR title1[] = "SWIM Window1 \r\n";
const CHAR title2[] = "SWIM Window2 \r\n";
const CHAR title3[] = "SWIM Window3 \r\n";
static COLOR_T bkg_color = YELLOWC, frg_color = WHITEC;

/***********************************************************************
 *
 * Function: uart_string_write
 *
 * Purpose: UART string output helper function
 *
 * Processing:
 *     See function.
 *
 * Parameters: @pbuff: Pointer to data buffer
 *             @len: Size of data buffer
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 ***********************************************************************/
void uart_string_write(char *pbuff, UNS_32 len)
{
  UNS_32 written;

  if (g_uart_id != 0)
  {
    if (len == 0)
      len = str_size(pbuff);

    while (len > 0)
    {
      written = uart_write(g_uart_id, pbuff, len);
      len -= written;
      pbuff += written;
    }
  }
}

/***********************************************************************
 *
 * Function: uart_string_read
 *
 * Purpose: UART string input helper function
 *
 * Processing:
 *     See function.
 *
 * Parameters: @buff: Pointer to data buffer
 *             @len: Size of data buffer
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 ***********************************************************************/
static void uart_string_read(char *buff, int len)
{
  char c;
  int i = 0;

  for (;;)
  {
    while (!(uart_read(g_uart_id, &c, 1)));

    if (c == '\r')
    {
      uart_write(g_uart_id, (void*)&c, 1);
      uart_string_write("\r\n", 0);
      break;
    }

    if ((c == '\b') && i)
    {
      i--;
      uart_write(g_uart_id, (void*)&c, 1);
      continue;
    }

    if (c >= ' ' && i < (len - 1))
    {
      /* Visible chars */
      uart_write(g_uart_id, (void*)&c, 1);
      buff[i++] = c;
    }
  }

  buff[i] = 0;
}


/***********************************************************************
 *
 * Function: dump_fb_to_display
 *
 * Purpose: Dump the Frame buffer contents into LCD Display
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 ***********************************************************************/
void dump_fb_to_display(void)
{
  int i = 0, size;
  UNS_16 *buf_ptr;

  /* Dump buffer onto display */
  ea3131_lcd_set_pos(0, 0);
  size = (DISPLAY_HEIGHT * DISPLAY_WIDTH);
  buf_ptr = (UNS_16 *) LCD_FRAME_BUF;
  while (size--)
  {
    EA31_LCD_DATA = buf_ptr[i++];
  }
}

/***********************************************************************
 *
 * Function: get_color_option
 *
 * Purpose: Read the color option from the user
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: User selected color
 *
 * Notes: None
 *
 ***********************************************************************/
COLOR_T get_color_option(void)
{
  char buf[8];
  COLOR_T color;

  uart_string_write("---------------------------------------\r\n", 0);
  uart_string_write("Color options available                \r\n", 0);
  uart_string_write("'B' : Black      ,   'N' : Navy        \r\n", 0);
  uart_string_write("'G' : Dark Green ,   'C' : Dark Cyan   \r\n", 0);
  uart_string_write("'M' : Maroon     ,   'P' : Purple      \r\n", 0);
  uart_string_write("'O' : Olive      ,   'L' : Light Gray  \r\n", 0);
  uart_string_write("'K' : Dark Gray  ,   'U' : Blue        \r\n", 0);
  uart_string_write("'R' : Green      ,   'Y' : Cyan        \r\n", 0);
  uart_string_write("'E' : Red        ,   'A' : Magenta     \r\n", 0);
  uart_string_write("'W' : Yellow     ,   'H' : White       \r\n", 0);
  uart_string_write("---------------------------------------\r\n", 0);
  uart_string_write("\r\n", 0);
  uart_string_write("Select option: ", 0);
  uart_string_read(buf, sizeof(buf));
  uart_string_write("\r\n", 0);

  switch (buf[0])
  {
    case 'B':
    case 'b':
      color = BLACKC;
      break;
    case 'N':
    case 'n':
      color = NAVYC;
      break;
    case 'G':
    case 'g':
      color = DARK_GREENC;
      break;
    case 'C':
    case 'c':
      color = DARK_CYANC;
      break;
    case 'M':
    case 'm':
      color = MAROONC;
      break;
    case 'P':
    case 'p':
      color = PURPLEC;
      break;
    case 'O':
    case 'o':
      color = OLIVEC;
      break;
    case 'L':
    case 'l':
      color = LIGHT_GRAYC;
      break;
    case 'K':
    case 'k':
      color = DARK_GRAYC;
      break;
    case 'U':
    case 'u':
      color = BLUEC;
      break;
    case 'R':
    case 'r':
      color = GREENC;
      break;
    case 'Y':
    case 'y':
      color = CYANC;
      break;
    case 'E':
    case 'e':
      color = REDC;
      break;
    case 'A':
    case 'a':
      color = MAGENTAC;
      break;
    case 'W':
    case 'w':
      color = YELLOWC;
      break;
    case 'H':
    case 'h':
      color = WHITEC;
      break;
    default:
      color = BLACKC;
      break;
  }

  return color;
}

/***********************************************************************
 *
 * Function: get_font_option
 *
 * Purpose: Read the FONT option from the user
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Pointer to user selected font structure
 *
 * Notes: None
 *
 ***********************************************************************/
FONT_T *get_font_option(void)
{
  char buf[8];
  FONT_T *pfont;

  uart_string_write("---------------------------------------\r\n", 0);
  uart_string_write("Font options available                 \r\n", 0);
  uart_string_write("'H' : Adobe Helvetica Medium Normal    \r\n", 0);
  uart_string_write("'E' : ROM 8x8 Font BIOS Mode 10        \r\n", 0);
  uart_string_write("'S' : ROM 8x16 Font BIOS Mode 12       \r\n", 0);
  uart_string_write("'W' : Windows Free System 14x16 Font   \r\n", 0);
  uart_string_write("'M' : Misc Fixed Medium x5x7 Font      \r\n", 0);
  uart_string_write("'C' : Misc Fixed Medium x6x13 Font     \r\n", 0);
  uart_string_write("---------------------------------------\r\n", 0);
  uart_string_write("\r\n", 0);
  uart_string_write("Select option: ", 0);
  uart_string_read(buf, sizeof(buf));
  uart_string_write("\r\n", 0);

  switch (buf[0])
  {
    case 'H':
    case 'h':
      pfont = (FONT_T *) & font_helvr10;
      break;
    case 'E':
    case 'e':
      pfont = (FONT_T *) & font_rom8x8;
      break;
    case 'S':
    case 's':
      pfont = (FONT_T *) & font_rom8x16;
      break;
    case 'W':
    case 'w':
      pfont = (FONT_T *) & font_winfreesys14x16;
      break;
    case 'M':
    case 'm':
      pfont = (FONT_T *) & font_x5x7;
      break;
    case 'C':
    case 'c':
      pfont = (FONT_T *) & font_x6x13;
      break;
    default:
      pfont = (FONT_T *) & font_winfreesys14x16;
      break;
  }

  return pfont;
}

/***********************************************************************
 *
 * Function: conv_ascii_to_dec
 *
 * Purpose: Convert the ASCII string to decimal value
 *
 * Processing:
 *     See function.
 *
 * Parameters: @buf: Pointer to buffer
 *
 * Outputs: None
 *
 * Returns: Decimal value
 *
 * Notes: None
 *
 ***********************************************************************/
unsigned int conv_ascii_to_dec(char *buf)
{
  char ch, *start;
  UNS_32 i = 0, mult = 1;

  start = buf + strlen(buf);
  while (--start >= buf)
  {
    ch = *start;
    if ((ch == '\r') || (ch == '\0'))
      break;

    if (ch >= '0' && ch <= '9')
    {
      i += (mult * (ch - '0'));
      mult *= 10;
    }
  }

  return i;
}

/***********************************************************************
 *
 * Function: get_value
 *
 * Purpose: Read the value from user
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: user value
 *
 * Notes: None
 *
 ***********************************************************************/
INT_32 get_value(void)
{
  char buf[8];
  INT_32 x;

  /* Read Values */
  uart_string_write("Enter Value (In Decimal format): ", 0);
  uart_string_read(buf, sizeof(buf));
  uart_string_write("\r\n", 0);

  /* Convert Decimal value */
  x = conv_ascii_to_dec(buf);

  return x;
}

/***********************************************************************
 *
 * Function: lcd_clear_sceen
 *
 * Purpose: Clear the LCD display
 *
 * Processing:
 *     See function.
 *
 * Parameters: @win: Pointer to SWIM window structure
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 ***********************************************************************/
void lcd_clear_sceen(SWIM_WINDOW_T *win)
{
  COLOR_T colr;
  colr = BLACKC;
  swim_clear_screen(win, colr);
  dump_fb_to_display();
  return;
}

/***********************************************************************
 *
 * Function: lcd_set_colors
 *
 * Purpose: Set the Background & Foreground colors of display
 *
 * Processing:
 *     See function.
 *
 * Parameters: @win: Pointer to SWIM window structure
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 ***********************************************************************/
void lcd_set_colors(SWIM_WINDOW_T *win)
{
  uart_string_write("---------------------------------------\r\n", 0);
  uart_string_write("Choose Background color:               \r\n", 0);
  uart_string_write("---------------------------------------\r\n", 0);
  bkg_color = get_color_option();

  uart_string_write("---------------------------------------\r\n", 0);
  uart_string_write("Choose Foreground color:               \r\n", 0);
  uart_string_write("---------------------------------------\r\n", 0);
  frg_color = get_color_option();

  swim_set_bkg_color(win, bkg_color);
  swim_set_fill_color(win, frg_color);
  swim_clear_screen(win, bkg_color);
  dump_fb_to_display();

  return;
}

/***********************************************************************
 *
 * Function: lcd_display_text
 *
 * Purpose: Display the Text on LCD display
 *
 * Processing:
 *     See function.
 *
 * Parameters: @win: Pointer to SWIM window structure
 *             @x: X position
 *             @y: Y position
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 ***********************************************************************/
void lcd_display_text(SWIM_WINDOW_T *win, INT_32 x, INT_32 y)
{
  char buf[32];
  COLOR_T pen;
  FONT_T *font;

  uart_string_write("---------------------------------------\r\n", 0);
  uart_string_write("Choose Font:                           \r\n", 0);
  uart_string_write("---------------------------------------\r\n", 0);
  font = get_font_option();

  uart_string_write("---------------------------------------\r\n", 0);
  uart_string_write("Choose Font color:                      \r\n", 0);
  uart_string_write("---------------------------------------\r\n", 0);
  pen = get_color_option();

  uart_string_write("---------------------------------------\r\n", 0);
  uart_string_write("Enter Text (Max of 32 characters):     \r\n", 0);
  uart_string_write("---------------------------------------\r\n", 0);
  uart_string_write("Type Text: ", 0);
  uart_string_read(buf, sizeof(buf));
  uart_string_write("\r\n", 0);

  swim_set_font(win, font);
  swim_set_pen_color(win, pen);
  swim_put_text_xy(win, (CHAR *)buf, x, y);
  dump_fb_to_display();

  return;
}

/***********************************************************************
 *
 * Function: lcd_display_box
 *
 * Purpose: Draw Box structure on LCD display
 *
 * Processing:
 *     See function.
 *
 * Parameters: @win: Pointer to SWIM window structure
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 ***********************************************************************/
void lcd_display_box(SWIM_WINDOW_T *win)
{
  COLOR_T fill, pen;
  INT_32 x1, y1, x2, y2;

  uart_string_write("---------------------------------------\r\n", 0);
  sprintf(buff, "X1 value (Min: %d Max: %d) \r\n",
          (int)(win->xpvmin - win->xpvmin), (int)(win->xpvmax - win->xpvmin));
  uart_string_write(buff, 0);
  uart_string_write("---------------------------------------\r\n", 0);
  x1 = get_value();

  uart_string_write("---------------------------------------\r\n", 0);
  sprintf(buff, "Y1 value (Min: %d Max: %d) \r\n",
          (int)(win->ypvmin - win->ypvmin), (int)(win->ypvmax - win->ypvmin));
  uart_string_write(buff, 0);
  uart_string_write("---------------------------------------\r\n", 0);
  y1 = get_value();

  uart_string_write("---------------------------------------\r\n", 0);
  sprintf(buff, "X2 value (Min: %d Max: %d) \r\n",
          (int)(win->xpvmin - win->xpvmin), (int)(win->xpvmax - win->xpvmin));
  uart_string_write(buff, 0);
  uart_string_write("---------------------------------------\r\n", 0);
  x2 = get_value();

  uart_string_write("---------------------------------------\r\n", 0);
  sprintf(buff, "Y2 value (Min: %d Max: %d) \r\n",
          (int)(win->ypvmin - win->ypvmin), (int)(win->ypvmax - win->ypvmin));
  uart_string_write(buff, 0);
  uart_string_write("---------------------------------------\r\n", 0);
  y2 = get_value();

  uart_string_write("---------------------------------------\r\n", 0);
  uart_string_write("Choose Edge color:                     \r\n", 0);
  uart_string_write("---------------------------------------\r\n", 0);
  pen = get_color_option();

  uart_string_write("---------------------------------------\r\n", 0);
  uart_string_write("Choose Fill color:                     \r\n", 0);
  uart_string_write("---------------------------------------\r\n", 0);
  fill = get_color_option();

  swim_set_pen_color(win, pen);
  swim_set_fill_color(win, fill);
  swim_put_box(win, x1, y1, x2, y2);
  dump_fb_to_display();

  return;
}

/***********************************************************************
 *
 * Function: lcd_display_diamond
 *
 * Purpose: Draw Diamond structure on LCD display
 *
 * Processing:
 *     See function.
 *
 * Parameters: @win: Pointer to SWIM window structure
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 ***********************************************************************/
void lcd_display_diamond(SWIM_WINDOW_T *win)
{
  COLOR_T fill, pen;
  INT_32 x = ((win->xpvmax + win->xpvmin) / 2);
  INT_32 y = ((win->ypvmax + win->ypvmin) / 2);
  INT_32 x_radius, y_radius;

  uart_string_write("---------------------------------------\r\n", 0);
  uart_string_write("Choose X radius Value:                 \r\n", 0);
  uart_string_write("---------------------------------------\r\n", 0);
  x_radius = get_value();

  uart_string_write("---------------------------------------\r\n", 0);
  uart_string_write("Choose Y radius Value:                 \r\n", 0);
  uart_string_write("---------------------------------------\r\n", 0);
  y_radius = get_value();

  uart_string_write("---------------------------------------\r\n", 0);
  uart_string_write("Choose Edge color:                     \r\n", 0);
  uart_string_write("---------------------------------------\r\n", 0);
  pen = get_color_option();

  uart_string_write("---------------------------------------\r\n", 0);
  uart_string_write("Choose Fill color:                     \r\n", 0);
  uart_string_write("---------------------------------------\r\n", 0);
  fill = get_color_option();

  swim_set_pen_color(win, pen);
  swim_set_fill_color(win, fill);
  swim_put_diamond(win, x, y, x_radius, y_radius);
  dump_fb_to_display();

  return;
}

#ifdef TGT_SDRAM
/***********************************************************************
 *
 * Function: lcd_display_image
 *
 * Purpose: Image display tests (Embedded Image is used)
 *
 * Processing:
 *     See function.
 *
 * Parameters: @win: Pointer to SWIM window structure
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 ***********************************************************************/
void lcd_display_image(SWIM_WINDOW_T *win)
{
  char input[8];
  volatile int bout = 0;
  INT_32 x = image_w, y = image_h;
  const COLOR_T *image_ptr =	(const COLOR_T *)image;

  while (!bout)
  {
    uart_string_write("----------------------------------------------\r\n", 0);
    uart_string_write("Image Display Options                         \r\n", 0);
    uart_string_write("----------------------------------------------\r\n", 0);
    uart_string_write("'C' : Clear the LCD screen                    \r\n", 0);
    uart_string_write("'D' : Display the Image on LCD screen         \r\n", 0);
    uart_string_write("'I' : Invert the Image on LCD screen          \r\n", 0);
    uart_string_write("'R' : Rotate the Image to Right               \r\n", 0);
    uart_string_write("'L' : Rotate the Image to Left                \r\n", 0);
    uart_string_write("'A' : Scale & Display the Image               \r\n", 0);
    uart_string_write("'B' : Scale & Invert the Image                \r\n", 0);
    uart_string_write("'F' : Scale & Rotate the Image to Right       \r\n", 0);
    uart_string_write("'E' : Scale & Rotate the Image to Left        \r\n", 0);
    uart_string_write("'X' : Exit Image Display Tests                \r\n", 0);
    uart_string_write("----------------------------------------------\r\n", 0);
    uart_string_write("\r\n", 0);
    uart_string_write("Select option: ", 0);
    uart_string_read(input, sizeof(input));
    uart_string_write("\r\n", 0);

    switch (input[0])
    {
      case 'C':
      case 'c':
        /* Clear the screen */
        lcd_clear_sceen(win);
        break;

      case 'D':
      case 'd':
        swim_clear_screen(win, BLACKC);
        swim_put_win_image(win, image_ptr, x, y, 0, NOROTATION);
        dump_fb_to_display();
        break;

      case 'I':
      case 'i':
        swim_clear_screen(win, BLACKC);
        swim_put_win_image(win, image_ptr, x, y, 0, INVERT);
        dump_fb_to_display();
        break;

      case 'R':
      case 'r':
        swim_clear_screen(win, BLACKC);
        swim_put_win_image(win, image_ptr, x, y, 0, RIGHT);
        dump_fb_to_display();
        break;

      case 'L':
      case 'l':
        swim_clear_screen(win, BLACKC);
        swim_put_win_image(win, image_ptr, x, y, 0, LEFT);
        dump_fb_to_display();
        break;

      case 'A':
      case 'a':
        swim_clear_screen(win, BLACKC);
        swim_put_win_image(win, image_ptr, x,
                           y, 1, NOROTATION);
        dump_fb_to_display();
        break;

      case 'B':
      case 'b':
        swim_clear_screen(win, BLACKC);
        swim_put_win_image(win, image_ptr, x,
                           y, 1, INVERT);
        dump_fb_to_display();
        break;

      case 'F':
      case 'f':
        swim_clear_screen(win, BLACKC);
        swim_put_win_image(win, image_ptr, x,
                           y, 1, RIGHT);
        dump_fb_to_display();
        break;

      case 'E':
      case 'e':
        swim_clear_screen(win, BLACKC);
        swim_put_win_image(win, image_ptr, x,
                           y, 1, LEFT);
        dump_fb_to_display();
        break;

      case 'x':
      case 'X':
        bout = 1;
        break;
    }

    uart_string_write("\r\n", 0);
  }

  return;
}
#endif

/***********************************************************************
 *
 * Function: lcd_multiple_windows_tests
 *
 * Purpose: Multiple SWIM window tests
 *
 * Processing:
 *     See function.
 *
 * Parameters: @win1: Pointer to SWIM window1 structure
 *             @win2: Pointer to SWIM window2 structure
 *             @win3: Pointer to SWIM window3 structure
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 ***********************************************************************/
void lcd_multiple_windows_tests(SWIM_WINDOW_T *win1,
                                SWIM_WINDOW_T *win2, SWIM_WINDOW_T *win3)
{
  char input[8];
  volatile int bout = 0;
  INT_32 win_no = 0;
  SWIM_WINDOW_T *win = NULL;

  while (!bout)
  {
    uart_string_write("-----------------------------------------\r\n", 0);
    uart_string_write("Multiple Windows Tests Options           \r\n", 0);
    uart_string_write("-----------------------------------------\r\n", 0);
    uart_string_write("'S' : Select the window                  \r\n", 0);
    uart_string_write("'C' : Clear the window                   \r\n", 0);
    uart_string_write("'L' : Set Bkg & Frg colors on window     \r\n", 0);
    uart_string_write("'T' : Display Text on window             \r\n", 0);
    uart_string_write("'B' : Draw Box on LCD screen             \r\n", 0);
    uart_string_write("'D' : Draw Diamond on LCD screen         \r\n", 0);
    uart_string_write("'X' : Exit multiple tests                \r\n", 0);
    uart_string_write("---------------------------------------  \r\n", 0);
    uart_string_write("\r\n", 0);
    uart_string_write("Select option: ", 0);
    uart_string_read(input, sizeof(input));
    uart_string_write("\r\n", 0);

    switch (input[0])
    {
      case 'S':
      case 's':
        uart_string_write("Choose Window (Enter 1, 2, or 3) \r\n", 0);
        win_no = get_value();
        if (win_no == 1)
        {
          win = win1;
        }
        else if (win_no == 2)
        {
          win = win2;
        }
        else if (win_no == 3)
        {
          win = win3;
        }
        else
        {
          uart_string_write("Wrong Window No. selected! \r\n", 0);
          win = NULL;
        }
        break;

      case 'C':
      case 'c':
        /* Clear the screen */
        if (win == NULL)
        {
          uart_string_write("Window not selected \r\n", 0);
        }
        else
        {
          lcd_clear_sceen(win);
        }
        break;

      case 'L':
      case 'l':
        if (win == NULL)
        {
          uart_string_write("Window not selected \r\n", 0);
        }
        else
        {
          /* Set Background & Foreground colors */
          lcd_set_colors(win);
        }
        break;

      case 'T':
      case 't':
        if (win == NULL)
        {
          uart_string_write("Window not selected \r\n", 0);
        }
        else
        {
          /* Display Text */
          lcd_display_text(win, (win->xpvmin + (win->xvsize / 3)),
                           (win->ypvmin + (win->yvsize / 2)));
        }
        break;

      case 'B':
      case 'b':
        if (win == NULL)
        {
          uart_string_write("Window not selected \r\n", 0);
        }
        else
        {
          /* Display Box */
          lcd_display_box(win);
        }
        break;

      case 'D':
      case 'd':
        if (win == NULL)
        {
          uart_string_write("Window not selected \r\n", 0);
        }
        else
        {
          /* Display Box */
          lcd_display_diamond(win);
        }
        break;

      case 'x':
      case 'X':
        bout = 1;
        break;
    }

    uart_string_write("\r\n", 0);
  }

  return;
}

/***********************************************************************
 *
 * Function: lcd_multiple_windows
 *
 * Purpose: Multiple SWIM window tests
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 ***********************************************************************/
void lcd_multiple_windows(void)
{
  SWIM_WINDOW_T win1, win2, win3;
  UNS_32 regionsize;
  BOOL_32 status;
  COLOR_T *fblog = (COLOR_T *) LCD_FRAME_BUF;

  /* Create 3 windows */
  regionsize = DISPLAY_WIDTH / 3;

  /* Open SWIM windows */
  status = swim_window_open(&win1, DISPLAY_WIDTH,
                            DISPLAY_HEIGHT, fblog, 2,
                            2, (regionsize - 1), (DISPLAY_HEIGHT - 2),
                            1, WHITEC, REDC, DARK_GRAYC);
  if (status == FALSE)
  {
    uart_string_write("SWIM window 1 open failed \r\n", 0);
    return;
  }

  status = swim_window_open(&win2, DISPLAY_WIDTH,
                            DISPLAY_HEIGHT, fblog, regionsize,
                            2, ((2 * regionsize) - 1), (DISPLAY_HEIGHT - 2),
                            1, WHITEC, GREENC, DARK_GRAYC);
  if (status == FALSE)
  {
    uart_string_write("SWIM window 2 open failed \r\n", 0);
    swim_window_close(&win1);
    return;
  }

  status = swim_window_open(&win3, DISPLAY_WIDTH,
                            DISPLAY_HEIGHT, fblog,
                            (2 * regionsize), 2, ((3 * regionsize) - 2),
                            (DISPLAY_HEIGHT - 2), 1, WHITEC, BLUEC, DARK_GRAYC);
  if (status == FALSE)
  {
    uart_string_write("SWIM window 3 open failed \r\n", 0);
    swim_window_close(&win1);
    swim_window_close(&win2);
    return;
  }

  /* fill fram buffer with blue color */
  dump_fb_to_display();

  /* Set Font Type for Windows */
  swim_set_font(&win1, (FONT_T *)&font_winfreesys14x16);
  swim_set_font(&win2, (FONT_T *)&font_winfreesys14x16);
  swim_set_font(&win3, (FONT_T *)&font_winfreesys14x16);

  /* Dump Strings */
  swim_put_text_xy(&win1, title1, 30, 120);
  swim_put_text_xy(&win2, title2, 110, 120);
  swim_put_text_xy(&win3, title3, 190, 120);
  dump_fb_to_display();

  /* Call Multiple Window Tests */
  lcd_multiple_windows_tests(&win1, &win2, &win3);

  swim_window_close(&win1);
  swim_window_close(&win2);
  swim_window_close(&win3);

  return;
}

/***********************************************************************
 *
 * Function: c_entry
 *
 * Purpose: Application entry point from the startup code
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Always returns 1
 *
 * Notes: None
 *
 **********************************************************************/
int c_entry(void)
{
  volatile UNS_32 break_out = 0;
  char input[8];
  INT_32 dev_pwm;
  SWIM_WINDOW_T win;
  BOOL_32 status;
  COLOR_T *fblog;
  INT_32 h_size = 0, v_size = 0;

  /* Disable interrupts in ARM core */
  disable_irq_fiq();

  /* Setup miscellaneous board functions */
  ea3131_board_init();

  /* Set virtual address of MMU table */
  cp15_set_vmmu_addr((void *)ISROM_MMU_TTB_BASE);

  /* Initialize interrupt system */
  int_initialize(0xFFFFFFFF);

  /******************************************************************/
  /* Setup UART for 115.2K, 8 data bits, no parity, 1 stop bit */
  g_uart_id = uart_open((void *) UART_BASE, 0);
  if (!g_uart_id)
  {
    return 1;
  }

  /* Enable IRQ interrupts in the ARM core */
  enable_irq();

  uart_string_write("---------------------------------------\r\n", 0);
  uart_string_write("LPC313x LCD example using SWIM library \r\n", 0);
  uart_string_write("---------------------------------------\r\n", 0);

  /* Init LCD panel */
  ea3131_lcd_init(EA_DISP_QVGA_LCD_V2_ID);

  /* enable back light */
  dev_pwm = pwm_open((void *)PWM, 0);
  pwm_ioctl(dev_pwm, PWM_ENABLE, 1);
  pwm_ioctl(dev_pwm, PWM_DUTY_R, 0);

  /* Open SWIM window */
  fblog = (COLOR_T *) LCD_FRAME_BUF;
  status = swim_window_open(&win, DISPLAY_WIDTH, DISPLAY_HEIGHT,
                            fblog, 2, 2, (DISPLAY_WIDTH - 2),
                            (DISPLAY_HEIGHT - 2), 2, BLACKC,
                            YELLOWC, BLUEC);
  if (status == FALSE)
  {
    uart_string_write("SWIM window open failed \r\n", 0);
    return 1;
  }

  /* fill fram buffer with blue color */
  dump_fb_to_display();

  /* Dump Title */
  swim_set_font(&win, (FONT_T *)&font_winfreesys14x16);
  swim_put_text_xy(&win, title, 60, 150);
  dump_fb_to_display();

  break_out = 0;
  while (!break_out)
  {
    uart_string_write("-----------------------------------------\r\n", 0);
    uart_string_write("LCD Demo using SWIM Library Options      \r\n", 0);
    uart_string_write("-----------------------------------------\r\n", 0);
    uart_string_write("'S' : Get size (H & V) of LCD screen     \r\n", 0);
    uart_string_write("'C' : Clear the LCD screen               \r\n", 0);
    uart_string_write("'L' : Set Bkg & Frg colors on LCD screen \r\n", 0);
    uart_string_write("'T' : Display Text on LCD screen         \r\n", 0);
    uart_string_write("'B' : Draw Box on LCD screen             \r\n", 0);
    uart_string_write("'D' : Draw Diamond on LCD screen         \r\n", 0);
    uart_string_write("'M' : Multiple SWIM windows on LCD screen\r\n", 0);
#ifdef TGT_SDRAM
    uart_string_write("'I' : Image on LCD screen                \r\n", 0);
#endif
    uart_string_write("'X' : Exit LCD Demo                      \r\n", 0);
    uart_string_write("---------------------------------------  \r\n", 0);
    uart_string_write("\r\n", 0);
    uart_string_write("Select option: ", 0);
    uart_string_read(input, sizeof(input));
    uart_string_write("\r\n", 0);

    switch (input[0])
    {
      case 'S':
      case 's':
        h_size = swim_get_horizontal_size(&win);
        v_size = swim_get_vertical_size(&win);
        sprintf(buff, "Horizontal Size: %d \r\n", (int)h_size);
        uart_string_write(buff, 0);
        sprintf(buff, "Vertical Size: %d \r\n", (int)v_size);
        uart_string_write(buff, 0);
        break;

      case 'C':
      case 'c':
        /* Clear the screen */
        lcd_clear_sceen(&win);
        break;

      case 'L':
      case 'l':
        /* Set Background & Foreground colors */
        lcd_set_colors(&win);
        break;

      case 'T':
      case 't':
        /* Display Text */
        lcd_display_text(&win, (win.xpvmin + (win.xvsize / 3)),
                         (win.ypvmin + (win.yvsize / 2)));
        break;

      case 'B':
      case 'b':
        /* Display Box */
        lcd_display_box(&win);
        break;

      case 'D':
      case 'd':
        /* Display Box */
        lcd_display_diamond(&win);
        break;

      case 'M':
      case 'm':
        /* Multiple SWIM Windows */
        lcd_multiple_windows();
        break;

#ifdef TGT_SDRAM
      case 'I':
      case 'i':
        /* Multiple SWIM Windows */
        lcd_display_image(&win);
        break;
#endif

      case 'x':
      case 'X':
        break_out = 1;
        break;
    }

    uart_string_write("\r\n", 0);
  }

  uart_string_write("------------------------------------------\r\n", 0);
  uart_string_write("LPC313x LCD example using SWIM library: Done \r\n", 0);
  uart_string_write("------------------------------------------\r\n", 0);

  /* Close SWIM window */
  swim_window_close(&win);

  /* Close PWM */
  pwm_close(dev_pwm);

  /* Close UART */
  uart_close(g_uart_id);

  /* Disable interrupts in ARM core */
  disable_irq_fiq();

  return 1;
}
