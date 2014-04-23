#include "gpio.h"
#include "gpio_cfg.h"
#include "gpio_lcfg.h"
#include "../inc/std_types.h"

static volatile gpio_ConfigType *localConfig;

volatile const gpio_RegisterAddresstype gpio_RegisterAdress_as[MAX_NUM_OF_PORTS] =
{
      {
            (uint8*) GPIO_PORTB_ADDRESS,
            (uint8*) GPIO_DDRB_ADDRESS,
            (uint8*) GPIO_PINB_ADDRESS
      },
      {
            (uint8*) GPIO_PORTC_ADDRESS,
            (uint8*) GPIO_DDRC_ADDRESS,
            (uint8*) GPIO_PINC_ADDRESS
      },
      {
            (uint8*) GPIO_PORTD_ADDRESS,
            (uint8*) GPIO_DDRD_ADDRESS,
            (uint8*) GPIO_PIND_ADDRESS
      }
};


void gpio_init()
{
   uint8 forCounter1_ui8;
   gpio_PinInOutType gpio_LocalPinInOut;
   gpio_PinInitialType gpio_LocalPinInitial;
   gpio_PortType port_e;
   gpio_PinType pin_e;
   uint8 channel_ui16;

   localConfig = (gpio_ConfigType*) gpio_getlcfgdata();

   for(forCounter1_ui8 = 0; forCounter1_ui8 < MAX_NUM_OF_PORTS * MAX_NUM_OF_PINS; forCounter1_ui8++)
   {
      if(localConfig->gpio_PinConfig[forCounter1_ui8].gpio_ConfireAllowed == GPIO_TRUE)
      {
         gpio_LocalPinInOut = localConfig->gpio_PinConfig[forCounter1_ui8].gpio_PinInOut;
         gpio_LocalPinInitial = localConfig->gpio_PinConfig[forCounter1_ui8].gpio_PinInitial;
         channel_ui16 = localConfig->gpio_PinConfig[forCounter1_ui8].gpio_Pin;
         port_e = (uint8)(channel_ui16 >> 8);
         pin_e = (uint8)((channel_ui16 & 0xFF));

         *(gpio_RegisterAdress_as[port_e].gpio_DirectionRegister_pui8) |= (uint8)(gpio_LocalPinInOut << pin_e);
         if(gpio_LocalPinInOut == GPIO_OUTPUT)
         {
        	 *(gpio_RegisterAdress_as[port_e].gpio_PortRegister_pui8) |= (uint8)(gpio_LocalPinInitial << pin_e);
         }
      }
   }
}


void gpio_WriteChannel(gpio_ChannelType channel, gpio_PinState state)
{
   gpio_PortType port_ui8 = (channel >> 8);
   gpio_PinType pin_ui8 = (channel & 0xFF);

   if(state == GPIO_HIGH)
      *(gpio_RegisterAdress_as[port_ui8].gpio_PortRegister_pui8) |= (uint8)(state << pin_ui8);
   else
      *(gpio_RegisterAdress_as[port_ui8].gpio_PortRegister_pui8) &= (uint8)(state << pin_ui8);
}

void gpio_ToggleChannel(gpio_ChannelType channel)
{
   gpio_PortType port_ui8 = (channel >> 8);
   gpio_PinType pin_ui8 = (channel & 0xFF);

   *(gpio_RegisterAdress_as[port_ui8].gpio_PortRegister_pui8) ^= (uint8)(1 << pin_ui8);
}

gpio_PinState gpio_ReadChannel(gpio_ChannelType channel)
{
   gpio_PortType port_ui8 = (channel >> 8);
   gpio_PinType pin_ui8 = (channel & 0xFF);

   return *(gpio_RegisterAdress_as[port_ui8].gpio_InputRgister_pui8) & (uint8)(1 << pin_ui8);
}
