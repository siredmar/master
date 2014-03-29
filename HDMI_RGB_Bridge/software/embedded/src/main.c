#include "../inc/std_types.h"
#include "gpio/gpio.h"
#include "uart/uart.h"
#include "twi/twimaster.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>


uint8 w_start()
{

   uart_puts("*ACKS#");
}

uint8 w_data(uint8 data)
{

   uart_puts("*ACKW#");
}

void command_ready(uart_i2cCommandType cmd, uint8 data)
{
   if(cmd == CMD_WRITE_START)
   {
      w_start();
   } else
   {
      if(cmd == CMD_WRITE_DATA)
      {
         w_data(data);
      }
   }


}

int main()
{
   uart_init(RECEPTION_ENABLED, TRANSMISSION_ENABLED, INTERRUPT_ENABLED);
   gpio_init();

   uart_puts("----------------------------------------\n\r");
   uart_puts("HDMI_RGB_LVDS Board EDID Data programmer\n\r");
   uart_puts("(c) Armin Schlegel, 29.03.2014\r\n");
   uart_puts("----------------------------------------\n\r\n\r");
   uart_puts("Command help:\n\r");
   uart_puts("*s#   - indicates the start of a write process   - returns *ACKS#\n\r");
   uart_puts("*d,0xXX# - writes a byte 0xXX                     - returns *ACKW#\n\r");
   uart_puts("Note: for a complete EDID dataset write 128 bytes\n\r");

   sei(); /* Enable the interrupts */

   while(1)
   {

   }

   return 0;
}
