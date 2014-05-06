/*
  ITDB02_Touch.cpp - Arduino library support for ITDB02 Touch function
  Copyright (C)2010 Henning Karlsen. All right reserved
  
  Basic functionality of this library are based on the demo-code provided by
  ITead studio. You can find the latest version of the library at
  http://www.henningkarlsen.com/electronics

  This library has been made for the 2.4" TFT LCD Screen Module: �ITDB02-2.4�, 
  the 3.2" TFT LCD Screen Module: �ITDB02-3.2� and the 3.2" Wide-screen module 
  �ITDB02-3.2WC� by ITead studio.

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

  Version:   1.0  - Sep  13 2010  - initial release
			 1.01 - Sep  18 2010  - Added example: ITDB02_Touch_ButtonTest
			 1.1  - Oct   7 2010  - Fixed incompatibility with ITDB02-3.2
			 1.2  - Oct  12 2010  - Added support for ITDB02-3.2WC
			 1.21 - Mar  27 2011  - Updated some of the examples to be 
									compatible with ITDB02_Graph(16) v4.0

*/

#define PixSizeX	13.78
#define PixOffsX	411
#define PixSizeY	11.01
#define PixOffsY	378

#include "Arduino.h"
#include "ITDB02_Touch.h"
#include <avr/pgmspace.h>

ITDB02_Touch::ITDB02_Touch(byte tclk, byte tcs, byte din, byte dout, byte irq)
{
    T_CLK = tclk;
    T_CS  = tcs;
    T_DIN = din;
    T_DOUT= dout;
    T_IRQ = irq;
}

void ITDB02_Touch::InitTouch(byte orientation)
{
	orient = orientation;

    pinMode(T_CLK,  OUTPUT);
    pinMode(T_CS,   OUTPUT);
    pinMode(T_DIN,  OUTPUT);
    pinMode(T_DOUT, INPUT);
    pinMode(T_IRQ,  INPUT);

	digitalWrite(T_CS,  HIGH);
	digitalWrite(T_CLK, HIGH);
	digitalWrite(T_DIN, HIGH);
	digitalWrite(T_CLK, HIGH);
}

void ITDB02_Touch::touch_WriteData(byte data)
{
	byte temp;
	byte nop;

	temp=data;
	digitalWrite(T_CLK,LOW);

	for(byte count=0; count<8; count++)
	{
		if(temp & 0x80)
			digitalWrite(T_DIN, HIGH);
		else
			digitalWrite(T_DIN, LOW);
		temp = temp << 1; 
		digitalWrite(T_CLK, LOW);                
		nop++;
		digitalWrite(T_CLK, HIGH);
		nop++;
	}
}

word ITDB02_Touch::touch_ReadData()
{
	byte nop;
	word data = 0;
	for(byte count=0; count<12; count++)
	{
		data <<= 1;
		digitalWrite(T_CLK, HIGH);               
		nop++;
		digitalWrite(T_CLK, LOW);
		nop++;
		if (digitalRead(T_DOUT))
			data++;
	}
	return(data);
}

void ITDB02_Touch::read()
{
	unsigned long tx=0;
	unsigned long ty=0;

	digitalWrite(T_CS,LOW);                    

	for (int i=0; i<prec; i++)
	{
		touch_WriteData(0x90);        
		digitalWrite(T_CLK,HIGH);
		digitalWrite(T_CLK,LOW); 
		ty+=touch_ReadData();

		touch_WriteData(0xD0);      
		digitalWrite(T_CLK,HIGH);
		digitalWrite(T_CLK,LOW);
		tx+=touch_ReadData();
	}

	digitalWrite(T_CS,HIGH);

	TP_X=tx/prec;
	TP_Y=ty/prec;
}

bool ITDB02_Touch::dataAvailable()
{
  bool avail;
  avail = !digitalRead(T_IRQ);
  return avail;
}

int ITDB02_Touch::getX()
{
	int value;

	if (orient == PORTRAIT)
	{
		if (PixSizeX>=0)
		{
			value = 240-((TP_X-PixOffsX)/PixSizeX);
		}
		else
		{
			value = (TP_X-PixOffsX)/-(PixSizeX);
		}
	}
	else
	{
		if (PixSizeY<0)
			value = 400-((TP_Y-PixOffsY)/-PixSizeY);
		else
			value = ((TP_Y-PixOffsY)/PixSizeY);
	}

	if (value < 0)
		value = 0;
	return value;
}

int ITDB02_Touch::getY()
{
	int value;

	if (orient == PORTRAIT)
	{
		if (PixSizeY<0)
			value = ((TP_Y-PixOffsY)/-PixSizeY);
		else
			value = 320-((TP_Y-PixOffsY)/PixSizeY);
	}
	else
	{
		if (PixSizeX>=0)
		{
			value = 240-((TP_X-PixOffsX)/PixSizeX);
		}
		else
		{
			value = (TP_X-PixOffsX)/-(PixSizeX);
		}
	}

	if (value < 0)
		value = 0;
	return value;  
}

void ITDB02_Touch::setPrecision(byte precision)
{
	switch (precision)
	{
		case PREC_LOW:
			prec=1;
			break;
		case PREC_MEDIUM:
			prec=10;
			break;
		case PREC_HI:
			prec=25;
			break;
		case PREC_EXTREME:
			prec=100;
			break;
		default:
			prec=10;
			break;
	}
}