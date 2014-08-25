#ifndef _EEPROM_H_
#define _EEPROM_H_

#define FALSE 0
#define TRUE 1

void eeprom_init();
uint8 write_eeprom_byte(uint16,uint8);
uint8 read_eeprom_byte(uint16 address);

#endif
