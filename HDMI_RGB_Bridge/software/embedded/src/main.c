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
uint8 cmds[130];

volatile uint8 checksum = 0x00;

uint8 w_start()
{
   Std_ReturnType ret = E_NOT_OK;
   //i2c_start(EDID_EEPROM_ADDRESS + I2C_WRITE);     /* select EDID eeprom with address 0x50 in write mode */

   //i2c_write(0x00);                                /* select register address 0x00 to start the write process */
   //uart_puts(0x00);
   _delay_ms(100);
   uart_puts("#1*");
   ret = E_OK;
   return ret;
}

uint8 w_data(uint8 data)
{
   Std_ReturnType ret = E_NOT_OK;
   //i2c_write(data);
   //uart_puts(data);
   _delay_ms(100);
   uart_puts("#2*");
   ret = E_OK;
   return ret;
}

uint8 w_stop()
{
   Std_ReturnType ret = E_NOT_OK;
   //i2c_stop();
   //uart_puts(0xFF);
   _delay_ms(100);
   uart_puts("#3*");
   ret = E_OK;
   return ret;
}

uint8 dbg_output()
{
//   uint8 i;
//   for(i = 0; i < 127; i++)
//      uart_putc(cmds[i]);
}

void send_checksum()
{
//   char buf[10];
//   sprintf(buf, "%c", checksum);
   uart_putc('#');
   uart_putc('c');
   uart_putc(checksum);
   uart_putc('*');
}
void command_ready(uart_i2cCommandType cmd, uint8 data)
{
   //cmds[cnt++] = data;
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
      break;

   case CMD_DBG:
      dbg_output();
      break;

   case CMD_CHECKSUM:
      send_checksum();
      break;

   case CMD_ERROR:
      uart_puts("CMD_ERROR: please send correct commands\n\r");
      break;

   default:
      uart_puts("CMD_ERROR: please send correct commands\n\r");
      break;
   }
}


int main()
{
   //checksum = 0;
   uart_init(RECEPTION_ENABLED, TRANSMISSION_ENABLED, INTERRUPT_ENABLED);
   //gpio_init();
   i2c_init();
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

   }

   return 0;
}
