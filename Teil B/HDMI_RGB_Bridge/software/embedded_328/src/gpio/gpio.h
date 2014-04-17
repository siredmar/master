//#define _MMIO_BYTE(mem_addr) (mem_addr))
//#define __SFR_OFFSET 0x20
//#define _SFR_IO8(io_addr) _MMIO_BYTE((io_addr) + __SFR_OFFSET)
//
//
#include "../inc/std_types.h"

#ifndef GPIO_H
#define GPIO_H

#define MAX_NUM_OF_PORTS (3U)
#define MAX_NUM_OF_PINS  (8U)

typedef enum
{
   GPIO_CHANNEL_PB0 = 0x0000,
   GPIO_CHANNEL_PB1 = 0x0001,
   GPIO_CHANNEL_PB2 = 0x0002,
   GPIO_CHANNEL_PB3 = 0x0003,
   GPIO_CHANNEL_PB4 = 0x0004,
   GPIO_CHANNEL_PB5 = 0x0005,
   GPIO_CHANNEL_PB6 = 0x0006,
   GPIO_CHANNEL_PB7 = 0x0007,

   GPIO_CHANNEL_PC0 = 0x0100,
   GPIO_CHANNEL_PC1 = 0x0101,
   GPIO_CHANNEL_PC2 = 0x0102,
   GPIO_CHANNEL_PC3 = 0x0103,
   GPIO_CHANNEL_PC4 = 0x0104,
   GPIO_CHANNEL_PC5 = 0x0105,
   GPIO_CHANNEL_PC6 = 0x0106,

   GPIO_CHANNEL_PD0 = 0x0200,
   GPIO_CHANNEL_PD1 = 0x0201,
   GPIO_CHANNEL_PD2 = 0x0202,
   GPIO_CHANNEL_PD3 = 0x0203,
   GPIO_CHANNEL_PD4 = 0x0204,
   GPIO_CHANNEL_PD5 = 0x0205,
   GPIO_CHANNEL_PD6 = 0x0206,
   GPIO_CHANNEL_PD7 = 0x0207,

}gpio_ChannelType;


typedef enum
{
   GPIO_PORTA = (0U),
   GPIO_PORTB,
   GPIO_PORTC,
   GPIO_PORTD,
}gpio_PortType;

typedef enum
{
   GPIO_PIN0 = (0U),
   GPIO_PIN1,
   GPIO_PIN2,
   GPIO_PIN3,
   GPIO_PIN4,
   GPIO_PIN5,
   GPIO_PIN6,
   GPIO_PIN7
}gpio_PinType;

typedef enum
{
   GPIO_FALSE = (0U),
   GPIO_TRUE
}gpio_ConfigureAllowedType;

typedef enum
{
   GPIO_PIN_INITIAL_LOW = (0U),
   GPIO_PIN_INITIAL_HIGH
}gpio_PinInitialType;

typedef enum
{
   GPIO_LOW = (0U),
   GPIO_HIGH
}gpio_PinState;

typedef enum
{
   GPIO_INPUT = (0U),
   GPIO_OUTPUT
}gpio_PinInOutType;

typedef struct
{
   gpio_ConfigureAllowedType  gpio_ConfireAllowed;
   gpio_ChannelType           gpio_Pin;
   gpio_PinInOutType          gpio_PinInOut;
   gpio_PinInitialType        gpio_PinInitial;
}gpio_PinConfigType;


typedef struct
{
   gpio_PinConfigType gpio_PinConfig[MAX_NUM_OF_PINS * MAX_NUM_OF_PORTS -1]; // -1 for PC7 missing!
}gpio_ConfigType;


typedef struct
{
   uint8*       gpio_PortRegister_pui8;
   uint8*       gpio_DirectionRegister_pui8;
   uint8*       gpio_InputRgister_pui8;
} gpio_RegisterAddresstype;

void gpio_init();
void gpio_WriteChannel(gpio_ChannelType channel, gpio_PinState state);
gpio_PinState gpio_ReadChannel(gpio_ChannelType channel);
void gpio_ToggleChannel(gpio_ChannelType channel);

#endif


