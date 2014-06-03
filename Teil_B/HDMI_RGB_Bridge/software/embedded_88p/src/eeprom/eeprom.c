#include <avr/io.h>
#include <util/delay.h>

#include "eeprom.h"

void eeprom_init()
{
	//Set up TWI Module
	TWBR = 5;
	TWSR &= (~((1<<TWPS1)|(1<<TWPS0)));

}

uint8_t write_eeprom_byte(uint16_t address,uint8_t data)
{
	do
	{
		//Put Start Condition on TWI Bus
		TWCR=(1<<TWINT)|(1<<TWSTA)|(1<<TWEN);

		//Poll Till Done
		while(!(TWCR & (1<<TWINT)));

		//Check status
		if((TWSR & 0xF8) != 0x08)
			return FALSE;

		//Now write SLA+W
		//EEPROM @ 00h
		TWDR=0b10100000;	

		//Initiate Transfer
		TWCR=(1<<TWINT)|(1<<TWEN);

		//Poll Till Done
		while(!(TWCR & (1<<TWINT)));
	
	}while((TWSR & 0xF8) != 0x18);
		

//	//Now write ADDRH
//	TWDR=(address>>8);
//
//	//Initiate Transfer
//	TWCR=(1<<TWINT)|(1<<TWEN);
//
//	//Poll Till Done
//	while(!(TWCR & (1<<TWINT)));
//
//	//Check status
//	if((TWSR & 0xF8) != 0x28)
//		return FALSE;

	//Now write ADDRL
	TWDR=(address);

	//Initiate Transfer
	TWCR=(1<<TWINT)|(1<<TWEN);

	//Poll Till Done
	while(!(TWCR & (1<<TWINT)));

	//Check status
	if((TWSR & 0xF8) != 0x28)
		return FALSE;

	//Now write DATA
	TWDR=(data);

	//Initiate Transfer
	TWCR=(1<<TWINT)|(1<<TWEN);

	//Poll Till Done
	while(!(TWCR & (1<<TWINT)));

	//Check status
	if((TWSR & 0xF8) != 0x28)
		return FALSE;

	//Put Stop Condition on bus
	TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
	
	//Wait for STOP to finish
	while(TWCR & (1<<TWSTO));

	//Wait untill Writing is complete
	_delay_ms(12);

	//Return TRUE
	return TRUE;

}

uint8_t read_eeprom_byte(uint16_t address)
{
	uint8_t data;

	//Initiate a Dummy Write Sequence to start Random Read
	do
	{
		//Put Start Condition on TWI Bus
		TWCR=(1<<TWINT)|(1<<TWSTA)|(1<<TWEN);

		//Poll Till Done
		while(!(TWCR & (1<<TWINT)));

		//Check status
		if((TWSR & 0xF8) != 0x08)
			return FALSE;

		//Now write SLA+W
		//EEPROM @ 00h
		TWDR=0b10100000;	

		//Initiate Transfer
		TWCR=(1<<TWINT)|(1<<TWEN);

		//Poll Till Done
		while(!(TWCR & (1<<TWINT)));
	
	}while((TWSR & 0xF8) != 0x18);
		

//	//Now write ADDRH
//	TWDR=(address>>8);
//
//	//Initiate Transfer
//	TWCR=(1<<TWINT)|(1<<TWEN);
//
//	//Poll Till Done
//	while(!(TWCR & (1<<TWINT)));
//
//	//Check status
//	if((TWSR & 0xF8) != 0x28)
//		return FALSE;

	//Now write ADDRL
	TWDR=(address);

	//Initiate Transfer
	TWCR=(1<<TWINT)|(1<<TWEN);

	//Poll Till Done
	while(!(TWCR & (1<<TWINT)));

	//Check status
	if((TWSR & 0xF8) != 0x28)
		return FALSE;

	//*************************DUMMY WRITE SEQUENCE END **********************


	
	//Put Start Condition on TWI Bus
	TWCR=(1<<TWINT)|(1<<TWSTA)|(1<<TWEN);

	//Poll Till Done
	while(!(TWCR & (1<<TWINT)));

	//Check status
	if((TWSR & 0xF8) != 0x10)
		return FALSE;

	//Now write SLA+R
	//EEPROM @ 00h
	TWDR=0b10100001;	

	//Initiate Transfer
	TWCR=(1<<TWINT)|(1<<TWEN);

	//Poll Till Done
	while(!(TWCR & (1<<TWINT)));

	//Check status
	if((TWSR & 0xF8) != 0x40)
		return FALSE;

	//Now enable Reception of data by clearing TWINT
	TWCR=(1<<TWINT)|(1<<TWEN);

	//Wait till done
	while(!(TWCR & (1<<TWINT)));

	//Check status
	if((TWSR & 0xF8) != 0x58)
		return FALSE;

	//Read the data
	data=TWDR;

	//Put Stop Condition on bus
	TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
	
	//Wait for STOP to finish
	while(TWCR & (1<<TWSTO));

	//Return TRUE
	return data;
}


		


