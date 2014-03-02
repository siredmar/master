/*
 * dio.c
 *
 *  Created on: Nov 6, 2012
 *      Author: armin
 */
#include "dio.h"

char *gpio_memory;

static dio_configType* lcfgConfigPtr_ps;

/*
 * M1/M0 combinations

M1   M0    function
0    0     input/output driver disabled
0    1     output is controlled by the device
1    0     output is driven low
1    1     output is driven high
*/

void dio_init(void)
{

	int mem_fd;

	    //atexit(byebye);

	    if ((mem_fd = open("/dev/mem", O_RDWR)) < 0) {
	        printf("can't open /dev/mem \n");
	        exit(-1);
	    }

	    printf("mem_fd = %d\n", mem_fd);
	    printf("getpagesize() -> %d\n", getpagesize());

	    gpio_memory = mmap(0,
	                 getpagesize(),
	                 PROT_READ|PROT_WRITE,
	                 //MAP_PRIVATE,
	                 MAP_SHARED,
	                 mem_fd,
	                 IOCONFIG);

	    if (gpio_memory == MAP_FAILED) {
	        printf("mmap error %d\n", errno);
	        exit(-1);
	    }
	    else {
	        printf("gpio_memory = 0x%x\n", (unsigned int)gpio_memory);
	    }

   dio_channelType            dio_pinNumberLocal_ui8 = DIO_CHANNEL_0;
   dio_pinDirectionType       dio_pinDirectionLocal_ui8 = DIO_PIN_OUTPUT;
   dio_pinInitialValueType    dio_pinInitialValueLocal_ui8 = DIO_LOW;
   uint8                      dio_iterator_ui8 = 0;


  lcfgConfigPtr_ps = (dio_configType*)dio_getlcfgdata();


   for(dio_iterator_ui8 = 0; dio_iterator_ui8 < NUMBER_OF_CONFIGURED_PINS; dio_iterator_ui8++)
      {
         dio_pinNumberLocal_ui8       = lcfgConfigPtr_ps->dio_pinConfig_s[dio_iterator_ui8].dio_pinNumber_e;
         dio_pinDirectionLocal_ui8    = lcfgConfigPtr_ps->dio_pinConfig_s[dio_iterator_ui8].dio_pinDirection_e;
         dio_pinInitialValueLocal_ui8 = lcfgConfigPtr_ps->dio_pinConfig_s[dio_iterator_ui8].dio_pinInitialValue_e;

         if(dio_pinDirectionLocal_ui8 == DIO_PIN_OUTPUT)
         {
        	 *(unsigned int *)(gpio_memory + FBLO_GPIO + MODE1_SET) = (1 << dio_pinNumberLocal_ui8);
        	 if(dio_pinInitialValueLocal_ui8 == DIO_PIN_INIT_LOW)
        	 {
        		 *(unsigned int *)(gpio_memory + FBLO_GPIO + MODE0_RESET) = (1 << dio_pinNumberLocal_ui8);
        		 //printf("dio_init(): %d is OUTPUT LOW\n", dio_pinNumberLocal_ui8);
        	 }
        	 else
        	 {
        		 *(unsigned int*)(gpio_memory + FBLO_GPIO + MODE0_SET) = (1 << dio_pinNumberLocal_ui8);
        		 //printf("dio_init(): %d is OUTPUT HIGH\n", dio_pinNumberLocal_ui8);
        	 }
         }
         else
         {
        	 //gpio_direction_input(dio_pinNumberLocal_ui8);
         }
      }
}

void dio_writeChannel(dio_channelType dio_pin_ui8, dio_pinLevelType dio_output_ui8)
{
	if(dio_output_ui8 == DIO_HIGH)
	{
		 *(unsigned int*)(gpio_memory + FBLO_GPIO + MODE0_SET) = (1 << dio_pin_ui8);
		 //printf("dio_writeChannel(): %d is HIGH\n", dio_pin_ui8);
	}
	else
	{
		*(unsigned int*)(gpio_memory + FBLO_GPIO + MODE0_RESET) = (1 << dio_pin_ui8);
		//printf("dio_writeChannel(): %d is LOW\n", dio_pin_ui8);
	}

}


void dio_uninit()
{
	/* cleaning up */
	munmap(gpio_memory, getpagesize());
}

void dio_writePortGroup(uint16 dio_data_ui16)
{
	dio_writeChannel(DIO_CHANNEL_16, (dio_data_ui16 >> 0) & 0x01);
	dio_writeChannel(DIO_CHANNEL_15, (dio_data_ui16 >> 1) & 0x01);
	dio_writeChannel(DIO_CHANNEL_14, (dio_data_ui16 >> 2) & 0x01);
	dio_writeChannel(DIO_CHANNEL_13, (dio_data_ui16 >> 3) & 0x01);
	dio_writeChannel(DIO_CHANNEL_12, (dio_data_ui16 >> 4) & 0x01);
	dio_writeChannel(DIO_CHANNEL_11, (dio_data_ui16 >> 5) & 0x01);
	dio_writeChannel(DIO_CHANNEL_2,  (dio_data_ui16 >> 6) & 0x01);
	dio_writeChannel(DIO_CHANNEL_3,  (dio_data_ui16 >> 7) & 0x01);
}

