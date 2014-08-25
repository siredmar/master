/*
 * This EEPROM library is a modified version of the one
 * provided by Avinash Gupta from www.eXtremeElectronics.co.in
 *
 * It is modified to fit the 24C01 128 Byte EEPROM from Atmel.
 * (c) 2014 Armin Schlegel
 */
#include <avr/io.h>
#include <util/delay.h>
#include "eeprom.h"

#define INITIATE_TRANSFER()   TWCR = (1 << TWINT) | (1 << TWEN)
#define POLL_TILL_DONE()      while(!(TWCR & (1 << TWINT)))
#define CHECK_STATUS()        if((TWSR & 0xF8) != 0x28)\ return FALSE;
#define START_CONDITION()     TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN)
#define STOP_CONDITION()      TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO)
#define WAIT_FOR_STOP()       while(TWCR & (1 << TWSTO))
#define WRITE_TWDR(x)         TWDR = x
#define READ_TWDR(x)          x = TWDR

void eeprom_init()
{
   //Set up TWI Module
   TWBR = 5;
   TWSR &= (~((1 << TWPS1) | (1 << TWPS0)));
}

uint8 write_eeprom_byte(uint16 address,uint8 data)
{
   do
   {
      START_CONDITION();
      POLL_TILL_DONE();
      if((TWSR & 0xF8) != 0x08)
         return FALSE;
      //Now write SLA+W
      //EEPROM @ 00h
      WRITE_TWDR(0b10100000);
      INITIATE_TRANSFER();
      POLL_TILL_DONE();
   }while((TWSR & 0xF8) != 0x18);

   WRITE_TWDR(address);
   INITIATE_TRANSFER();
   POLL_TILL_DONE();
   CHECK_STATUS();
   WRITE_TWDR(data);
   INITIATE_TRANSFER();
   POLL_TILL_DONE();
   CHECK_STATUS();
   STOP_CONDITION();
   WAIT_FOR_STOP();
   _delay_ms(12);
   return TRUE;
}

uint8 read_eeprom_byte(uint16 address)
{
   uint8 data;
   //Initiate a Dummy Write Sequence to start Random Read
   do
   {
      START_CONDITION();
      POLL_TILL_DONE();
      //Check status
      if((TWSR & 0xF8) != 0x08)
         return FALSE;
      //Now write SLA+W
      //EEPROM @ 00h
      WRITE_TWDR(0b10100000);
      INITIATE_TRANSFER();
      POLL_TILL_DONE();
   }while((TWSR & 0xF8) != 0x18);

   TWDR = (address);
   INITIATE_TRANSFER();
   POLL_TILL_DONE();
   CHECK_STATUS();
   //*************************DUMMY WRITE SEQUENCE END **********************
   START_CONDITION();
   POLL_TILL_DONE();
   if((TWSR & 0xF8) != 0x10)
      return FALSE;
   //Now write SLA+R
   //EEPROM @ 00h
   WRITE_TWDR(0b10100001);
   INITIATE_TRANSFER();
   POLL_TILL_DONE();
   if((TWSR & 0xF8) != 0x40)
      return FALSE;
   INITIATE_TRANSFER();
   POLL_TILL_DONE();
   if((TWSR & 0xF8) != 0x58)
      return FALSE;
   READ_TWDR(data);
   STOP_CONDITION();
   WAIT_FOR_STOP();
   return data;
}

