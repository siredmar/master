#include "../inc/std_types.h"
#include "uart/uart.h"
#include "twi/twimaster.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include "gpio/gpio.h"

#define CHECKSUM_TIMEOUT 50

typedef enum
{
   NO_ERROR = 0U,
   ERROR
}transmissionStatusType;

typedef enum
{
   CLOSED = 0U,
   OPEN
}connectionStatusType;

/** Global variable for incrementing received data counter */
uint8 cnt = 0;

/** Storage array for received data */
uint8 cmds[131];
uint8 eeprom[131] = {0};
uint8 address_counter = 0;
volatile uint8 continue_send_Checksum = 0;

/** Global variable for calculating the checksum */
volatile uint8 checksum = 0x00;

/** Global flag for indicating the reception of the handshake */
volatile uint8 handshake_received = 0;

/** Global flag for indicating an error during the last transmission */
volatile connectionStatusType transmission_status = NO_ERROR;

volatile connectionStatusType connection_status = CLOSED;

/**
 * @brief Function to be called when command #s* is received. Starts the i2c communication.
 *
 */
void w_start()
{
   cnt = 0;
   /* Check if an error ocurred during the last transmission and stop i2c connection if true */
   if(connection_status == OPEN) //&& transmission_status == ERROR)
   {
      i2c_stop();
      connection_status = CLOSED;
   }
   //   else /* transmission_status == TRANSMISSION_COMPLETE */
   //   {
   //      connection_status = OPEN;
   //   }

   /* Set transmission_status to Error. In case it won't be reset by w_stop(), an error occurred */


   //handshake_received = 0;
   //   i2c_start(EDID_EEPROM_ADDRESS + I2C_WRITE);     /* select EDID eeprom with address 0x50 in write mode */
   //   i2c_write(0x00);                                /* select register address 0x00 to start the write process */

   address_counter = 0;
   /* Leave connection open */
   connection_status = OPEN;

   //transmission_status = ERROR;

   _delay_ms(100);
   uart_puts("#1*");
}

/**
 * @brief Function to be called when command #wX* is received. Sends a data byte via i2c.
 *
 */
void w_data(uint8 data)
{

   cmds[address_counter] = data;
   write_eeprom_byte(address_counter, data);
   address_counter++;
   _delay_ms(100);
   uart_puts("#2*");
}

/**
 * @brief Function to be called when command #x* is received. Exits the i2c communication.
 *
 */
void w_stop()
{

   address_counter = 0;
   //_stop();
   //transmission_status = NO_ERROR;
   connection_status = CLOSED;
   _delay_ms(100);
   uart_puts("#3*");
   handshake_received = 0;
}

/**
 * @brief Function to be called when command #d* is received. Sends the received data bytes via uart.
 *
 */
void dbg_output()
{
   uint8 i;

   for(i = 0; i < 128; i++)
   {
      eeprom[i] = read_eeprom_byte(i);
      uart_putc(eeprom[i]);
   }
}

/**
 * @brief Function to be called when command #c* is received. Sends the checksum of the received bytes via uart.
 *
 */
void send_checksum()
{
   uint8 i, byte;
   uint8 timeoutCounter = 0;
   checksum = 0;
   for(i = 0; i < 128; i++)
   {
      //      byte =
      eeprom[i] = read_eeprom_byte(i);
      checksum ^= eeprom[i];
      //checksum = checksum ^ byte;
      //uart_putc(eeprom[i]);
   }

   //   for(i = 0; i < 128; i++)
   //   {
   //
   //      //uart_putc(checksum);
   //   }
   continue_send_Checksum = 1;
   while(continue_send_Checksum == 1 && timeoutCounter < CHECKSUM_TIMEOUT)
   {
      //   uart_putc('#');
      //   uart_putc('c');
      uart_putc(checksum);
      timeoutCounter++;
      _delay_ms(100);

      //   uart_putc('*');
   }
   //continue_send_Checksum = 0;
}
/**
 * @brief Function to call callbacks when valid commands are received.
 *
 */
void command_ready(uart_i2cCommandType cmd, uint8 data)
{
   switch(cmd)
   {
   case CMD_WRITE_START:
      w_start();
      uart_puts("s");
      break;

   case CMD_WRITE_DATA:
      w_data(data);
      break;

   case CMD_WRITE_STOP:
      w_stop();
      handshake_received = 0;
      uart_puts("x");
      break;

   case CMD_DBG:
      handshake_received = 1;
      dbg_output();
      handshake_received = 0;
      break;

   case CMD_CHECKSUM:
      send_checksum();
      break;

   case CMD_HANDSHAKE:
      //uart_puts("handshake received");
      // handshake_received = 1;
      break;

   case CMD_STOP_CHECKSUM:
      continue_send_Checksum = 0;
      break;

   case CMD_ERROR:
      uart_puts("CMD_ERROR: please send correct commands\n\r");
      break;

   default:
      uart_puts("CMD_ERROR: please send correct commands\n\r");
      break;
   }
}

/**
 * @brief Main function.
 *
 */
int main()
{
   uint8 c, i;
   handshake_received = 0;
   uart_init(RECEPTION_ENABLED, TRANSMISSION_ENABLED, INTERRUPT_ENABLED);
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

      if(handshake_received == 0)
      {
         uart_puts("h");
         _delay_ms(2000);
      }

   }
   return 0;
}
