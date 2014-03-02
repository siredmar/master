/*
 * (C) Copyright 2010 NXP Semiconductors
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#define crc32table	((const unsigned int *)0x12015CBC)

unsigned int crc32_compute(unsigned char *data, int length)
{
	unsigned int crc;

	crc = 0xFFFFFFFF;
	
	while (length--) {
		crc = crc32table[(crc ^ *data++) & 0xFF] ^(crc >> 8);
	}

	crc ^= 0xFFFFFFFF;
	return crc;
}