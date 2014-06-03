#ifndef _EEPROM_H_
#define _EEPROM_H_

#define FALSE 0
#define TRUE 1

void eeprom_init();
uint8_t write_eeprom_byte(uint16_t,uint8_t);
uint8_t read_eeprom_byte(uint16_t address);

#endif
