/*
 * dio.h
 *
 *  Created on: Nov 6, 2012
 *      Author: armin
 */

#ifndef DIO_H_
#define DIO_H_

#include "../../inc/std_types.h"
#include "dio_lcfg.h"
#include "dio_cfg.h"


#define NUMBER_OF_CONFIGURED_PINS    14
typedef uint8 dio_portValue_ui8;
typedef uint16 dio_portGroupValueType;


/*
Bit numbers in MODE0 and MODE1 registers

   0 -> GPIO1  !!!
   1 -> GPIO0
   2 -> GPIO2
   3 -> GPIO3
   4 -> GPIO4
   5 -> GPIO11
   6 -> GPIO12
   7 -> GPIO13
*/

typedef enum
{
   DIO_CHANNEL_0  = 1,
   DIO_CHANNEL_1  = 0,
   DIO_CHANNEL_2  = 2,
   DIO_CHANNEL_3  = 3,
   DIO_CHANNEL_11 = 5,
   DIO_CHANNEL_12 = 6,
   DIO_CHANNEL_13 = 7,
   DIO_CHANNEL_14 = 8,
   DIO_CHANNEL_15 = 9,
   DIO_CHANNEL_16 = 10,
   DIO_CHANNEL_17 = 11,
   DIO_CHANNEL_18 = 12,
   DIO_CHANNEL_19 = 13,
   DIO_CHANNEL_20 = 14
}dio_channelType;


typedef enum
{
   DIO_PIN_INPUT = 0,
   DIO_PIN_OUTPUT
}dio_pinDirectionType;


typedef enum
{
   DIO_PIN_INIT_LOW = 0,
   DIO_PIN_INIT_HIGH
}dio_pinInitialValueType;

typedef enum
{
   DIO_LOW = 0,
   DIO_HIGH
}dio_pinLevelType;

typedef enum
{
   DIO_PORTGROUP0 = 0,
   DIO_PORTGROUP1,
   DIO_PORTGROUP2
}dio_portGroupNoType;

typedef struct
{
   dio_channelType            dio_pinNumber_e;
   dio_pinDirectionType       dio_pinDirection_e;
   dio_pinLevelType			  dio_pinInitialValue_e;
}dio_pinConfigType;

typedef struct
{
	uint8           dio_numberOfConfiguredPins_ui8;
	dio_channelType dio_portGroupChannel[DIO_MAX_NUMBER_OF_PORTS_PER_GROUP];
}dio_portPerGroupType;

typedef struct
{
    uint8                dio_numberOfConfiguredPortGroups_ui8;
	dio_portPerGroupType dio_portGroup[DIO_MAX_NUMBER_OF_PORT_GROUPS];
}dio_portGroupType;

typedef struct
{
   dio_pinConfigType  dio_pinConfig_s[NUMBER_OF_CONFIGURED_PINS];
   dio_portGroupType dio_portGroupConfig_s;
}dio_configType;

void dio_init(void);

void dio_writeChannel(dio_channelType dio_pin_ui8, dio_pinLevelType dio_output_ui8);
dio_pinLevelType dio_readChannel(dio_channelType dio_pin_ui8);


void dio_writePortGroup(uint16 dio_data_ui16);

dio_portGroupValueType dio_readPortGroup(dio_portGroupNoType dio_portGroup_e);



#endif /* DIO_H_ */
