#include "gpio.h"

const gpio_ConfigType gpio_initialConfiguration_s =
{
      {
       /* PORT B */
            {
                  GPIO_TRUE,
                  GPIO_CHANNEL_PB0,
                  GPIO_OUTPUT,
                  GPIO_PIN_INITIAL_HIGH
            },
            {
                  GPIO_TRUE,
                  GPIO_CHANNEL_PB1,
                  GPIO_OUTPUT,
                  GPIO_PIN_INITIAL_HIGH
            },
            {
                  GPIO_TRUE,
                  GPIO_CHANNEL_PB2,
                  GPIO_OUTPUT,
                  GPIO_PIN_INITIAL_HIGH
            },
            {
                  GPIO_TRUE,
                  GPIO_CHANNEL_PB3,
                  GPIO_OUTPUT,
                  GPIO_PIN_INITIAL_HIGH
            },
            {
                  GPIO_TRUE,
                  GPIO_CHANNEL_PB4,
                  GPIO_OUTPUT,
                  GPIO_PIN_INITIAL_HIGH
            },
            {
                  GPIO_TRUE,
                  GPIO_CHANNEL_PB5,
                  GPIO_OUTPUT,
                  GPIO_PIN_INITIAL_HIGH
            },
            {
                  GPIO_TRUE,
                  GPIO_CHANNEL_PB6,
                  GPIO_OUTPUT,
                  GPIO_PIN_INITIAL_HIGH
            },
            {
                  GPIO_TRUE,
                  GPIO_CHANNEL_PB7,
                  GPIO_OUTPUT,
                  GPIO_PIN_INITIAL_HIGH
            },

      /* PORT C */
            {
                  GPIO_TRUE,
                  GPIO_CHANNEL_PC0,
                  GPIO_OUTPUT,
                  GPIO_PIN_INITIAL_HIGH
            },
            {
                  GPIO_TRUE,
                  GPIO_CHANNEL_PC1,
                  GPIO_OUTPUT,
                  GPIO_PIN_INITIAL_HIGH
            },
            {
                  GPIO_TRUE,
                  GPIO_CHANNEL_PC2,
                  GPIO_OUTPUT,
                  GPIO_PIN_INITIAL_HIGH
            },
            {
                  GPIO_TRUE,
                  GPIO_CHANNEL_PC3,
                  GPIO_OUTPUT,
                  GPIO_PIN_INITIAL_HIGH
            },
            {
                  GPIO_TRUE,
                  GPIO_CHANNEL_PC4,
                  GPIO_OUTPUT,
                  GPIO_PIN_INITIAL_HIGH
            },
            {
                  GPIO_TRUE,
                  GPIO_CHANNEL_PC5,
                  GPIO_OUTPUT,
                  GPIO_PIN_INITIAL_HIGH
            },
            {
                  GPIO_TRUE,
                  GPIO_CHANNEL_PC6,
                  GPIO_OUTPUT,
                  GPIO_PIN_INITIAL_HIGH
            },

      /* PORT D */
            {
                  GPIO_TRUE,
                  GPIO_CHANNEL_PD0,
                  GPIO_OUTPUT,
                  GPIO_PIN_INITIAL_HIGH
            },
            {
                  GPIO_TRUE,
                  GPIO_CHANNEL_PD1,
                  GPIO_OUTPUT,
                  GPIO_PIN_INITIAL_HIGH
            },
            {
                  GPIO_TRUE,
                  GPIO_CHANNEL_PD2,
                  GPIO_OUTPUT,
                  GPIO_PIN_INITIAL_HIGH
            },
            {
                  GPIO_TRUE,
                  GPIO_CHANNEL_PD3,
                  GPIO_OUTPUT,
                  GPIO_PIN_INITIAL_HIGH
            },
            {
                  GPIO_TRUE,
                  GPIO_CHANNEL_PD4,
                  GPIO_OUTPUT,
                  GPIO_PIN_INITIAL_HIGH
            },
            {
                  GPIO_TRUE,
                  GPIO_CHANNEL_PD5,
                  GPIO_OUTPUT,
                  GPIO_PIN_INITIAL_HIGH
            },
            {
                  GPIO_TRUE,
                  GPIO_CHANNEL_PD6,
                  GPIO_OUTPUT,
                  GPIO_PIN_INITIAL_HIGH
            },
            {
                  GPIO_TRUE,
                  GPIO_CHANNEL_PD7,
                  GPIO_OUTPUT,
                  GPIO_PIN_INITIAL_HIGH
            },


      }
};





const void *gpio_getlcfgdata(void)
{
   return ((void*) &gpio_initialConfiguration_s);
}
