#include "../inc/std_types.h"
#include "uart/uart.h"
#include "twi/twimaster.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>

uint8 eeprom[131] = {0};

enum{
   EEPROM_ADDR_2_BYTES  = 2U,
   EEPROM_ADDR_1_BYTES
};

#define EEPROM_WIDTH EEPROM_ADDR_2_BYTES


void write_eeprom_byte(uint16 address, uint8 data)
{
   uint16 status = 1;
   i2c_start(EDID_EEPROM_ADDRESS + I2C_WRITE);
#if  EEPROM_WIDTH == EEPROM_ADDR_2_BYTES
   i2c_write(address >> 8);
#endif
   i2c_write(address);
   i2c_write(data);
   i2c_stop();
   while(status == 1)
   {
      status = i2c_start(EDID_EEPROM_ADDRESS);
   }
   i2c_stop();
}

uint8 read_eeprom_byte(uint16 address) {
   uint8 data;
   i2c_start(EDID_EEPROM_ADDRESS + I2C_WRITE);
#if  EEPROM_WIDTH == EEPROM_ADDR_2_BYTES
   i2c_write(address >> 8);
#endif
   i2c_write(address);
   i2c_start(EDID_EEPROM_ADDRESS + I2C_READ);
   data = i2c_readNak();
   i2c_stop();
   return(data);
}


uint8 main()
{
   int i;

   uart_init(RECEPTION_DISABLED, TRANSMISSION_ENABLED, INTERRUPT_DISABLED);

   uart_puts("\n\ri2c_init()\n\r");
   i2c_init();
   sei(); /* Enable the interrupts */
   uart_puts("aaaaaaaa");
   for(i = 0; i < 128; i++)
   {
      write_eeprom_byte(i, i * 3);
      uart_putc(i *3 );
   }
   uart_puts("bbbbbbbb");
   _delay_ms(1000);

   for(i = 0; i < 128; i++)
   {
      eeprom[i] =  read_eeprom_byte(i);
      uart_putc(eeprom[i]);

   }

   //   i2c_start_wait(EDID_EEPROM_ADDRESS + I2C_WRITE);      // set device address and write mode
   //   i2c_write(0x00);
   //   i2c_write(0x00);
   //
   //   uart_puts("i2c_rep_start()\n\r");
   //   i2c_rep_start(EDID_EEPROM_ADDRESS + I2C_READ);        // set device address and read mode
   //   //uart_puts("drs");
   //   for(i = 0; i < 128; i++)
   //   {
   //      //uart_puts("i2c_readAck()\n\r");
   //
   //      eeprom[i] = i2c_readAck();                       // read one byte form address 0
   //      uart_putc(eeprom[i]);
   //      //_delay_ms(1000);
   //   }
   //   i2c_stop();


   sei(); /* Enable the interrupts */

   while(1)
   {

   }

   return 0;
}
