/*
  ITDB02_Graph16.h - Arduino library support for 16bit (W)QVGA LCD Boards
  Copyright (C)2010-2011 Henning Karlsen. All right reserved
  
  Basic functionality of this library are based on the demo-code provided by
  ITead studio. You can find the latest version of the library at
  http://www.henningkarlsen.com/electronics

  This library was originally made especially for the 3.2" TFT LCD Screen Module: 
  ITDB02-3.2 by ITead studio, but has later been expanded to support multiple
  modules. This library has been designed to use 16bit mode, so it will not 
  work with 8bit modules.

  Supported controllers:
		-	HX8347-A
		-	ILI9325D
		-	ILI9327
		-	SSD1289

  If you make any modifications or improvements to the code, I would appreciate
  that you share the code with me so that I might include it in the next release.
  I can be contacted through http://www.henningkarlsen.com/electronics/contact.php

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/


#ifndef ITDB02_Graph16_h
#define ITDB02_Graph16_h

#define LEFT 0
#define RIGHT 9999
#define CENTER 9998

#define PORTRAIT 0
#define LANDSCAPE 1

#define ASPECT_4x3	0	// Legacy
#define ASPECT_16x9	1	// Legacy
#define ITDB32		0	// HX8347-A
#define ITDB32WC	1	// ILI9327
#define TFT01_32W	1	// ILI9327	
#define ITDB32S		2	// SSD1289
#define TFT01_32	2	// SSD1289
#define TFT01_24	3	// ILI9325D

#include "Arduino.h"

struct _current_font
{
	uint8_t* font;
	uint8_t x_size;
	uint8_t y_size;
	uint8_t offset;
	uint8_t numchars;
};

class ITDB02
{
	public:
		ITDB02();
		ITDB02(int RS, int WR,int CS, int RST, int ALE, byte model=ITDB32);
		void InitLCD(byte orientation=PORTRAIT);
		void clrScr();
		void drawPixel(int x, int y);
		void drawLine(int x1, int y1, int x2, int y2);
		void fillScr(byte r, byte g, byte b);
		void drawRect(int x1, int y1, int x2, int y2);
		void drawRoundRect(int x1, int y1, int x2, int y2);
		void fillRect(int x1, int y1, int x2, int y2);
		void fillRoundRect(int x1, int y1, int x2, int y2);
		void drawCircle(int x, int y, int radius);
		void fillCircle(int x, int y, int radius);
		void setColor(byte r, byte g, byte b);
		void setBackColor(byte r, byte g, byte b);
		void print(char *st, int x, int y, int deg=0);
		void printNumI(long num, int x, int y);
		void printNumF(double num, byte dec, int x, int y);
		void setFont(uint8_t* font);
		void drawBitmap(int x, int y, int sx, int sy, unsigned int* data, int scale=1);
		void drawBitmap(int x, int y, int sx, int sy, unsigned int* data, int deg, int rox, int roy);
		void LCD_Writ_Bus(char VH,char VL);
	protected:
		byte fcolorr,fcolorg,fcolorb;
		byte bcolorr,bcolorg,bcolorb;
		byte orient;
		long disp_y_size;
		byte display_model;
		volatile uint8_t *P_RS, *P_WR, *P_CS, *P_RST, *P_ALE;
		uint8_t B_RS, B_WR, B_CS, B_RST, B_ALE;
		_current_font	cfont;

		
		void LCD_Write_COM(char VL);
		void LCD_Write_DATA(char VH,char VL);
		void main_W_com_data(char com1,int dat1);
		void setPixel(byte r,byte g,byte b);
		void drawHLine(int x, int y, int l);
		void drawVLine(int x, int y, int l);
		void printChar(byte c, int x, int y);
		void setXY(word x1, word y1, word x2, word y2);
		void rotateChar(byte c, int x, int y, int pos, int deg);
};

#endif