#include "../inc/std_types.h"
#include "uart/uart.h"
#include "twi/twimaster.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>

#define EDID_EEPROM_ADDRESS 0x50

uint8 cnt = 0;
uint8 cmds[131];

volatile uint8 checksum = 0x00;
volatile uint8 handshake_received = 0;

void w_start()
{
   cnt = 0;
   //handshake_received = 0;
   //i2c_start(EDID_EEPROM_ADDRESS + I2C_WRITE);     /* select EDID eeprom with address 0x50 in write mode */
   //i2c_write(0x00);                                /* select register address 0x00 to start the write process */
   _delay_ms(100);
   uart_puts("#1*");
}

void w_data(uint8 data)
{
   cmds[cnt] = data;
   cnt++;
   //i2c_write(data);
   _delay_ms(100);
   uart_puts("#2*");
}

void w_stop()
{
   //i2c_stop();
   _delay_ms(100);
   uart_puts("#3*");
   handshake_received = 0;
}

void dbg_output()
{
   uint8 i;
   for(i = 0; i < 128; i++)
      uart_putc(cmds[i]);
}

void send_checksum()
{
   uart_putc('#');
   uart_putc('c');
   uart_putc(checksum);
   uart_putc('*');
}
void command_ready(uart_i2cCommandType cmd, uint8 data)
{
   switch(cmd)
   {
   case CMD_WRITE_START:
      w_start();
      break;

   case CMD_WRITE_DATA:
      w_data(data);
      break;

   case CMD_WRITE_STOP:
      w_stop();
      handshake_received = 0;
      break;

   case CMD_DBG:
      dbg_output();
      break;

   case CMD_CHECKSUM:
      send_checksum();
      break;

   case CMD_HANDSHAKE:
      //uart_puts("handshake received");
     // handshake_received = 1;
      break;

   case CMD_ERROR:
      uart_puts("CMD_ERROR: please send correct commands\n\r");
      break;

   default:
      uart_puts("CMD_ERROR: please send correct commands\n\r");
      break;
   }
}


uint8 main()
{
   handshake_received = 0;
   uart_init(RECEPTION_ENABLED, TRANSMISSION_ENABLED, INTERRUPT_ENABLED);
   i2c_init();
//   char buf[255];
//   uint16 second = 0;

//   uart_puts("\n\r");
//   uart_puts("----------------------------------------\n\r");
//   uart_puts("HDMI_RGB_LVDS Board EDID Data programmer\n\r");
//   uart_puts("(c) Armin Schlegel, 30.03.2014\r\n");
//   uart_puts("----------------------------------------\n\r");
//   uart_puts("Command help:\n\r");
//   uart_puts("#s*   - indicates the start of a write process   - returns #1*\n\r");
//   uart_puts("#wX*  - writes a byte X                          - returns #2*\n\r");
//   uart_puts("#x*   - indicates the end of a data transfer     - returns #3*\n\r");
//   uart_puts("#c*   - returns the checksum                     - returns #3*\n\r");
//   uart_puts("Note: for a complete EDID dataset write 128 bytes\n\r\n\r");

   sei(); /* Enable the interrupts */

   while(1)
   {
      if(handshake_received == 0)
      {
         uart_puts("h");
//         itoa(second++, buf, 10);
//
//         uart_puts(buf);
         _delay_ms(2000);
      }
   }

   return 0;
}
