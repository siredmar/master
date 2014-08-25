/*
***************************************************************************
* This is a platform independend RS232 library and was modified by Armin Schlegel
* to fit for the project of his master thesis.
*
* Original Author: Teunis van Beelen
*
* Copyright (C) 2005, 2006, 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014 Teunis van Beelen
*
* teuniz@gmail.com
*
***************************************************************************
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation version 2 of the License.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*
***************************************************************************
*
* This version of GPL is at http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
*
***************************************************************************
*/

/* last revision: Januari 31, 2014 */

/* For more info and how to use this libray, visit: http://www.teuniz.net/RS-232/ */


#ifndef RS232_H_
#define RS232_H_


#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>

void rs232_receive_handler (int status);
int rs232_open_port(const char *comport, int baudrate);
int rs232_sendByte(int comport_number, unsigned char byte);
int rs232_sendBuf(int comport_number, unsigned char *buf, int size);
int rs232_close_port(int comport);
void rs232_puts(int comport, const char *text, int length);


#endif



