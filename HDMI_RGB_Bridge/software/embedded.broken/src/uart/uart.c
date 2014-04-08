/**
 * \file uart.c
 * \brief This file contains the implementation of the UART module.
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

#include "uart.h"
#include <avr/interrupt.h>
#include <util/delay.h>

/*--- Macros ---------------------------------------------------------*/
//#define F_CPU 7372800UL

#define BAUD 9600UL    /**< the high baudrate */

/** Calculated UBRR value for high baud rate */
#define UBRR_VAL ((F_CPU+BAUD*8)/(BAUD*16)-1)


/** Counter variable used for determining the type and the end of the received data set */
uint8_t uart_str_cnt = 0;

/** Status variable, indicating whether a new data set is incoming (1) or not (0) */
uint8_t new_data = 0;

/** Buffer holding the received data set */
char uart_str[MAXUARTSTR] = "";

/** Status variable, indicating whether the end of a data set has been found (\n\r) */
uint8_t block_finished = 0;

/** Index of the beginning of a new data set within the UART buffer */
uint8_t new_data_index = 0;

/*--- External Function Definitions ----------------------------------*/
extern void command_ready(uart_i2cCommandType cmd, uint8 data);
extern uint8 checksum;

/**
 * \brief Initialize UART communication with given parameters rxen, txen, rxcie and BAUD_VAL_HIGH
 *
 * \param rxen   enable receiving
 * \param txen   enable transmitting
 * \param rxcie  enable receiving interrupts
 */
void uart_init(uart_rxenType rxen, uart_txenType txen, uart_rxieType rxcie)
{
   UCSR0B = ((uint8)rxen << RXEN0) | ((uint8)rxcie << RXCIE0) | ((uint8)txen << TXEN0);
   UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
   UBRR0H = UBRR_VAL >> 8;
   UBRR0L = UBRR_VAL & 0xFF;
}

/**
 * \brief Send a single character via UART
 *
 * \param c byte to send
 */
void uart_putc(uint8 byte)
{
   while (!(UCSR0A & (1 << UDRE0)));
   UDR0 = byte;         /* sende Zeichen */

}

/**
 * \brief Transmit string
 *
 * \param s pointer to string to send
 */
void uart_puts(const uint8 *s) {
   while (*s ) {
      uart_putc(*s);
      _delay_us(10);
      s++;
   }
}

//uint8 convert_uart_str_to_int(sint8 *str)
//{
//   uint8 retval = 0;
//   retval += 100*(str[1]-48);
//   retval += 10*(str[2]-48);
//   retval += (str[3]-48);
//   return retval;
//}


ISR(USART0_RX_vect)
{
   //__asm("nop");
   sint8 next_char;

   // uint8 timer_val = 0;
   next_char = UDR0;

   checksum = checksum ^ next_char;

   if(next_char == '#')
   {
      uart_str_cnt = 0;
      block_finished = 0;
   }
   if(next_char == '*' && !block_finished)
   {
      block_finished = 1;
      //uart_str[uart_str_cnt + 1] = '\0';
      //uart_puts(uart_str);
      //uart_puts("command received\n\r");
      switch(uart_str[1])
      {
      case 's':
         command_ready(CMD_WRITE_START, 0xFF);
         break;

      case 'w':
         command_ready(CMD_WRITE_DATA, uart_str[2]);
         break;

      case 'x':
         command_ready(CMD_WRITE_STOP, 0xFF);
         break;
      case 'd':
         command_ready(CMD_DBG, 0xFF);
         break;

      case 'c':
         command_ready(CMD_CHECKSUM, 0xFF);
         break;

      default:
         command_ready(CMD_ERROR, 0xFF);
         break;
      }
   }

   if(!block_finished)
   {
      uart_str[uart_str_cnt] = next_char;
      uart_str_cnt++;
   }
}



