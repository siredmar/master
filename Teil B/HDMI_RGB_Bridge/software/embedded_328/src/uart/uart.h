/**
 * \file uart.h
 * \brief This file contains the interface of the UART module.
 *
 * Copyright (C) 2011  Armin Schlegel, Christian Eismann
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _UART_H_
#define _UART_H_

/*--- Includes -------------------------------------------------------*/

#include <inttypes.h>
#include <avr/io.h>
#include "../inc/std_types.h"

/** The maximum size of a string that is sent via UART */
#define MAXUARTSTR 250

typedef enum
{
   CMD_WRITE_START = 0U,
   CMD_WRITE_DATA,
   CMD_WRITE_STOP,
   CMD_ERROR,
   CMD_DBG,
   CMD_CHECKSUM,
   CMD_STOP_CHECKSUM,
   CMD_HANDSHAKE
}uart_i2cCommandType;

typedef enum
{
   RECEPTION_DISABLED = 0U,
   RECEPTION_ENABLED
}uart_rxenType;

typedef enum
{
   TRANSMISSION_DISABLED = 0U,
   TRANSMISSION_ENABLED
}uart_txenType;

typedef enum
{
   INTERRUPT_DISABLED = 0U,
   INTERRUPT_ENABLED
}uart_rxieType;


/*--- Function Prototypes --------------------------------------------*/

void uart_init(uart_rxenType rxen, uart_txenType txen, uart_rxieType rxcie);
void uart_putc(uint8 byte);
void uart_puts(const uint8 *s);


#endif /* #ifndef _UART_H_ */

