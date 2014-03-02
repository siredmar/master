/***********************************************************************
 * $Id:: ea3131_board.c 1388 2008-12-01 19:20:06Z pdurgesh             $
 *
 * Project: EA3131 board functions
 *
 * Description:
 *     This file contains driver support for memory based LCD modules
 *  supported by EA3131 board.
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

/******************************************************************************
 * Includes
 *****************************************************************************/

#include "ea3131_board.h"
#include "lpc313x_timer_driver.h"

/******************************************************************************
 * Local variables
 *****************************************************************************/

static UNS_16 activeDisplay = EA_DISP_QVGA_LCD_V2_ID;

/******************************************************************************
 * Local functions
 *****************************************************************************/


/***********************************************************************
 *
 * Function: lcd_write_cmd_reg
 *
 * Purpose: Write command and argument or data to a register
 *
 * Processing:
 *     Does nothing
 *
 * Parameters: 
 *    [in] cmd_addr - command or register address
 *    [in] data - data to write
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
static void lcd_write_cmd_reg(UNS_16 cmd_addr, UNS_16 data)
{
  EA31_LCD_CMD = cmd_addr;
  EA31_LCD_DATA = data;
}

/***********************************************************************
 *
 * Function: lcd_read_reg
 *
 * Purpose: Read data from a register
 *
 * Processing:
 *     Does nothing
 *
 * Parameters: 
 *    [in] addr - register address
 *
 * Outputs: None
 *
 * Returns: 
 *    Register value
 *
 * Notes: None
 *
 **********************************************************************/
static UNS_16 lcd_read_reg(UNS_16 addr)
{
  EA31_LCD_CMD = addr;
  return EA31_LCD_DATA;
}

/***********************************************************************
 *
 * Function: lcd_eav2_init
 *
 * Purpose: 
 *    Initialization routine for QVGA LCD v2.x display
 *
 * Processing:
 *     Does nothing
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: 
 *    _NO_ERROR if initialization successful
 *
 * Notes: None
 *
 **********************************************************************/
static STATUS lcd_eav2_init(void)
{
  if (lcd_read_reg(0) != EA_DISP_QVGA_LCD_V2_ID)
    return _ERROR;
  
  /*Display Control Setting*/
  
  lcd_write_cmd_reg (0x00,0x0001);    //Driver output control register
  timer_wait_ms(NULL, 2);
  lcd_write_cmd_reg (0x03,0xA2A4);    //LCD Driving waveform control register
  lcd_write_cmd_reg (0x0C,0x0004);    //BGR=0,
  lcd_write_cmd_reg (0x0D,0x0308);    //16-bit compare register
  lcd_write_cmd_reg (0x0E,0x3000);    //display control register 2
  timer_wait_ms(NULL, 10);
  
  lcd_write_cmd_reg (0x1E,0x00EA);
  
  lcd_write_cmd_reg (0x01,0x2B3F);    //Frame cycle control register
  lcd_write_cmd_reg (0x02,0x0600);    //gate scan position register
  lcd_write_cmd_reg (0x10,0x0000);    //CP/WM 18/16-bit selection register 16bit
  lcd_write_cmd_reg (0x11,0x6020);    //vertical scroll control
  lcd_write_cmd_reg (0x07,0x0233);
  lcd_write_cmd_reg (0x0B,0x0039);
  lcd_write_cmd_reg (0x0F,0x0000);    //HEA=128,HSA=0
  
  timer_wait_ms(NULL, 5);
  lcd_write_cmd_reg (0x30,0x0707);    //VEA=160,VSA=0
  lcd_write_cmd_reg (0x31,0x0204);
  lcd_write_cmd_reg (0x32,0x0204);
  lcd_write_cmd_reg (0x33,0x0502);
  lcd_write_cmd_reg (0x34,0x0507);
  lcd_write_cmd_reg (0x35,0x0204);
  lcd_write_cmd_reg (0x36,0x0204);
  lcd_write_cmd_reg (0x37,0x0502);
  lcd_write_cmd_reg (0x3A,0x0302);
  lcd_write_cmd_reg (0x3B,0x0302);
  
  lcd_write_cmd_reg (0x23,0x0000);    //start oscillation
  lcd_write_cmd_reg (0x24,0x0000);    //
  lcd_write_cmd_reg (0x25,0xA000);
  
  lcd_write_cmd_reg (0x48,0x0000);    //VCOM,VCOMH set
  lcd_write_cmd_reg (0x49,0x013F);    //REGP=vci
  lcd_write_cmd_reg (0x4A,0x0000);    //
  lcd_write_cmd_reg (0x4B,0x0000);    //
  
  lcd_write_cmd_reg (0x41,0x0000);    //VCOM,VCOMH set
  lcd_write_cmd_reg (0x42,0x0000);    //VREG1OUT=1.9*REGP
  
  lcd_write_cmd_reg (0x44,0xEF00);    //
  lcd_write_cmd_reg (0x45,0x0000);    //fosc/8,
  lcd_write_cmd_reg (0x46,0x013F);    //VGH=6*VCI1,VGL=-4*VCI1,fosc/8,
  lcd_write_cmd_reg (0x22,0x0000);	//GON=1 DTE=1 D1-0=11
  
  timer_wait_ms(NULL, 5);
  lcd_write_cmd_reg (0x44,0xEF00);	//GON=0 DTE=0 D1-0=01
  lcd_write_cmd_reg (0x45,0x0000);	//GON=1 DTE=0 D1-0=01
  lcd_write_cmd_reg (0x4E,0x0000);	//GON=1 DTE=0 D1-0=11
  lcd_write_cmd_reg (0x4F,0x0000);	//GON=1 DTE=1 D1-0=11
  lcd_write_cmd_reg (0x46,0x013F);	//GON=1 DTE=1 D1-0=11
  lcd_write_cmd_reg (0x22,0x0000);	//GON=1 DTE=1 D1-0=11

  activeDisplay = EA_DISP_QVGA_LCD_V2_ID;

  return _NO_ERROR;  
}

/***********************************************************************
 *
 * Function: lcd_eav1_init
 *
 * Purpose: 
 *    Initialization routine for QVGA OLED v1.x display
 *
 * Processing:
 *     Does nothing
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: 
 *    _NO_ERROR if initialization successful
 *
 * Notes: None
 *
 **********************************************************************/
static STATUS lcd_eav1_init(void)
{
  UNS_16 result = 0;

  //set 16-bit mode
  EA31_LCD_CMD = (0x23);

  //set cpu interface
  lcd_write_cmd_reg (0x02, 0x0000);

  //set 65k colors, one 16-bit transfer per pixel
  lcd_write_cmd_reg (0x03, 0x0130);

  EA31_LCD_CMD = (0x0f);

  result = EA31_LCD_DATA;
  if (result != EA_DISP_QVGA_OLED_V1_ID)
  {
	  return _ERROR;
  }

  //set standby off
  lcd_write_cmd_reg (0x10, 0x0000);

  //100ms delay
  timer_wait_ms(NULL, 100);

  //32 ms delay
  timer_wait_ms(NULL, 32);
  
  //set display on
  lcd_write_cmd_reg (0x05, 0x0001);

  //enable image data transfer
  EA31_LCD_CMD = (0x22);

  activeDisplay = EA_DISP_QVGA_OLED_V1_ID;

  return _NO_ERROR;  
}

/******************************************************************************
 * Public functions
 *****************************************************************************/

/*****************************************************************************
 *
 * Description:
 *    Initialize the display
 *
 * Params:
 *    [in] display - Selects which display to initialize. Use any of the 
 *                   EA_DISP_* constants
 *
 * Return:
 *    TRUE if the display got correctly initialized; otherwise FALSE
 *
 ****************************************************************************/
STATUS ea3131_lcd_init(UNS_32 disp_id)
{

  /* QVGA LCD v2.x */

  if (disp_id == EA_DISP_QVGA_LCD_V2_ID)
  {
    return lcd_eav2_init();
  }

  /* QVGA OLED v1.x */

  else if (disp_id == EA_DISP_QVGA_OLED_V1_ID)
  {
    return lcd_eav1_init();
  }

  return _ERROR;
}

/*****************************************************************************
 *
 * Description:
 *    Set position of the pen
 *
 * Params:
 *    [in] x - x position
 *    [in] y - y position
 *
 *
 ****************************************************************************/
void ea3131_lcd_set_pos(UNS_16 x, UNS_16 y)
{
  if (activeDisplay == EA_DISP_QVGA_LCD_V2_ID)
  {
    lcd_write_cmd_reg(0x4e, x & 0xff);
    lcd_write_cmd_reg(0x4f, y & 0x1ff);
    EA31_LCD_CMD = (0x22);
  }

  else if (activeDisplay == EA_DISP_QVGA_OLED_V1_ID)
  {
    EA31_LCD_CMD = (0x20);
    EA31_LCD_DATA = (x);
    EA31_LCD_CMD = (0x21);
    EA31_LCD_DATA = (y);
    EA31_LCD_CMD = (0x22);
  }
}


