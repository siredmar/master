/* linux/arch/arm/mach-lpc313x/include/mach/irqs.h
 *
 *  Author:	Durgesh Pattamatta
 *  Copyright (C) 2009 NXP semiconductors
 *
 *  IRQ defines for LPC313x and LPC315x SoCs.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */


#ifndef __ASM_ARCH_IRQS_H
#define __ASM_ARCH_IRQS_H



# define IRQ_EVT_ROUTER0  1        /*interrupts from Event router 0*/
# define IRQ_EVT_ROUTER1  2        /*interrupts from Event router 1*/
# define IRQ_EVT_ROUTER2  3        /*interrupts from Event router 2*/
# define IRQ_EVT_ROUTER3  4        /*interrupts from Event router 3*/
# define IRQ_TIMER0       5        /*Timer 0 IRQ */
# define IRQ_TIMER1       6        /*Timer 1 IRQ */
# define IRQ_TIMER2       7        /*Timer 2 IRQ */
# define IRQ_TIMER3       8        /*Timer 3 IRQ */
# define IRQ_ADC          9        /*10bit ADC irq*/
# define IRQ_UART         10       /*UART irq */
# define IRQ_I2C0         11       /*I2C 0 IRQ */
# define IRQ_I2C1         12       /*I2C 1 IRQ */
# define IRQ_I2S0_OUT     13       /*I2S 0 out IRQ */
# define IRQ_I2S1_OUT     14       /*I2S 1 out IRQ */
# define IRQ_I2S0_IN      15       /*I2S 0 IN IRQ */
# define IRQ_I2S1_IN      16       /*I2S 1 IN IRQ */
# define IRQ_LCD          18       /*LCD irq */
# define IRQ_SPI_SMS      19       /*SPI SMS IRQ */
# define IRQ_SPI_TX       20       /*SPI Transmit IRQ */
# define IRQ_SPI_RX       21       /*SPI Receive IRQ */
# define IRQ_SPI_OVR      22       /*SPI overrun IRQ */
# define IRQ_SPI          23       /*SPI interrupt IRQ */
# define IRQ_DMA          24       /*DMA irq */
# define IRQ_NAND_FLASH   25       /*NAND flash irq */
# define IRQ_MCI          26       /*MCI irq */
# define IRQ_USB          27       /*USB irq */
# define IRQ_ISRAM0       28       /*ISRAM0 irq */
# define IRQ_ISRAM1       29       /*ISRAM1 irq */


# define NR_IRQ_CPU	  30	/* IRQs directly recognized by CPU */
#define IRQ_BOARD_START   NR_IRQ_CPU


  /* System specific IRQs */
#include "event_router.h"

#if defined (CONFIG_MACH_FHS3143)
#define IRQ_SDMMC_CD	        (IRQ_BOARD_START + 0)	/* SD card detect */
#define IRQ_Touch_PenDown 	  (IRQ_BOARD_START + 1) /* Touchscreen pen down */

#define NR_IRQ_BOARD		2

#define BOARD_IRQ_EVENT_MAP	{ \
	{IRQ_SDMMC_CD, EVT_mI2STX_BCK0, EVT_ACTIVE_HIGH}, \
  {IRQ_Touch_PenDown, EVT_GPIO17, EVT_FALLING_EDGE},  \
	}


#define IRQ_EVTR0_START        IRQ_SDMMC_CD
#define IRQ_EVTR0_END          IRQ_SDMMC_CD
#define IRQ_EVTR1_START        0
#define IRQ_EVTR1_END          0
#define IRQ_EVTR2_START        IRQ_Touch_PenDown
#define IRQ_EVTR2_END          IRQ_Touch_PenDown
#define IRQ_EVTR3_START        0
#define IRQ_EVTR3_END          0

#elif defined (CONFIG_MACH_VAL3153) 

# define IRQ_CS8900_ETH_INT	IRQ_BOARD_START	/* Ethernet chip */
# define IRQ_SDMMC_CD0	        (IRQ_BOARD_START + 1)	/* SD card detect */
# define IRQ_SDMMC_CD1	        (IRQ_BOARD_START + 2)	/* SD card detect */
# define NR_IRQ_BOARD		3

/* now define board irq to event pin map */
#define BOARD_IRQ_EVENT_MAP	{ \
	{IRQ_CS8900_ETH_INT, EVT_GPIO16, EVT_ACTIVE_HIGH}, \
	{IRQ_SDMMC_CD0, EVT_GPIO12, EVT_ACTIVE_HIGH}, \
	{IRQ_SDMMC_CD1, EVT_GPIO13, EVT_ACTIVE_HIGH}, \
	}
/* Following defines group the board IRQs into 4 IRQ_EVNTR groups.
   IRQ_EVT_ROUTERx IRQ is generated when event in the corresponding 
   group triggers.
*/
#define IRQ_EVTR0_START        IRQ_CS8900_ETH_INT
#define IRQ_EVTR0_END          IRQ_CS8900_ETH_INT
#define IRQ_EVTR1_START        IRQ_SDMMC_CD0
#define IRQ_EVTR1_END          IRQ_SDMMC_CD0
#define IRQ_EVTR2_START        IRQ_SDMMC_CD1
#define IRQ_EVTR2_END          IRQ_SDMMC_CD1
#define IRQ_EVTR3_START        0
#define IRQ_EVTR3_END          0


#elif defined (CONFIG_MACH_EA313X)
# define IRQ_DM9000_ETH_INT	IRQ_BOARD_START	/* Ethernet chip */
# define IRQ_SDMMC_CD	        (IRQ_BOARD_START + 1)	/* SD card detect */
# define IRQ_Touch_PenDown 	(IRQ_BOARD_START + 2) /* Touchscreen pen down */
# define NR_IRQ_BOARD		3

/* now define board irq to event pin map */
#define BOARD_IRQ_EVENT_MAP	{ \
	{IRQ_DM9000_ETH_INT, EVT_mNAND_RYBN3, EVT_ACTIVE_HIGH}, \
	{IRQ_SDMMC_CD, EVT_mI2STX_BCK0, EVT_ACTIVE_LOW}, \
	{IRQ_Touch_PenDown, EVT_GPIO4, EVT_FALLING_EDGE},  \
	}
/* Following defines group the board IRQs into 4 IRQ_EVNTR groups.
   IRQ_EVT_ROUTERx IRQ is generated when event in the corresponding 
   group triggers.
*/
#define IRQ_EVTR0_START        IRQ_DM9000_ETH_INT
#define IRQ_EVTR0_END          IRQ_DM9000_ETH_INT
#define IRQ_EVTR1_START        IRQ_SDMMC_CD
#define IRQ_EVTR1_END          IRQ_SDMMC_CD
#define IRQ_EVTR2_START IRQ_Touch_PenDown 
#define IRQ_EVTR2_END IRQ_Touch_PenDown
#define IRQ_EVTR3_START 0
#define IRQ_EVTR3_END 0

#elif defined (CONFIG_MACH_NB31)

	#define IRQ_DM9000_ETH_INT	IRQ_BOARD_START	/* Ethernet chip */
	#define IRQ_SDMMC_CD	        (IRQ_BOARD_START + 1)	/* SD card detect */
#ifdef CONFIG_NB31_ENABLETOUCH
	#define IRQ_Touch_PenDown 	(IRQ_BOARD_START + 2) /* Touchscreen pen down */
	#define NR_IRQ_BOARD	 3
#else
	#define NR_IRQ_BOARD     2
#endif
	
	
	/* now define board irq to event pin map */

#ifndef CONFIG_NB31_ENABLETOUCH		
	#define BOARD_IRQ_EVENT_MAP	{ \
		{IRQ_DM9000_ETH_INT, EVT_mNAND_RYBN3, EVT_ACTIVE_HIGH}, \
		{IRQ_SDMMC_CD, EVT_mI2STX_BCK0, EVT_ACTIVE_LOW}, \
		}
#else
	#define BOARD_IRQ_EVENT_MAP	{ \
		{IRQ_DM9000_ETH_INT, EVT_mNAND_RYBN3, EVT_ACTIVE_HIGH}, \
		{IRQ_SDMMC_CD, EVT_mI2STX_BCK0, EVT_ACTIVE_LOW}, \
		{IRQ_Touch_PenDown, EVT_GPIO4, EVT_FALLING_EDGE}, \
		}
#endif		
	/* Following defines group the board IRQs into 4 IRQ_EVNTR groups.
	   IRQ_EVT_ROUTERx IRQ is generated when event in the corresponding 
	   group triggers.
	*/
	#define IRQ_EVTR0_START        IRQ_DM9000_ETH_INT
	#define IRQ_EVTR0_END          IRQ_DM9000_ETH_INT
	#define IRQ_EVTR1_START        IRQ_SDMMC_CD
	#define IRQ_EVTR1_END          IRQ_SDMMC_CD
#ifdef CONFIG_NB31_ENABLETOUCH
	#define IRQ_EVTR2_START 		 	IRQ_Touch_PenDown	
	#define IRQ_EVTR2_END 			  	IRQ_Touch_PenDown	
#else
	#define IRQ_EVTR2_START 			0
	#define IRQ_EVTR2_END 				0
#endif
	#define IRQ_EVTR3_START 			0
	#define IRQ_EVTR3_END 				0



#elif defined (CONFIG_MACH_VAL3154)
# define IRQ_SDMMC_CD	        IRQ_BOARD_START 	/* SD card detect */
# define IRQ_SDMMC_CD	        IRQ_BOARD_START 	/* SD card detect */
# define NR_IRQ_BOARD		1

/* now define board irq to event pin map */
#define BOARD_IRQ_EVENT_MAP	{ \
	{IRQ_SDMMC_CD, EVT_mI2STX_BCK0, EVT_ACTIVE_LOW}, \
	}
/* Following defines group the board IRQs into 4 IRQ_EVNTR groups.
   IRQ_EVT_ROUTERx IRQ is generated when event in the corresponding 
   group triggers.
*/
#define IRQ_EVTR0_START        IRQ_SDMMC_CD
#define IRQ_EVTR0_END          IRQ_SDMMC_CD
#define IRQ_EVTR1_START        0
#define IRQ_EVTR1_END          0
#define IRQ_EVTR2_START        0
#define IRQ_EVTR2_END          0
#define IRQ_EVTR3_START        0
#define IRQ_EVTR3_END          0

#else
# define NR_IRQ_BOARD          0
#define IRQ_EVTR0_START        0
#define IRQ_EVTR0_END          0
#define IRQ_EVTR1_START        0
#define IRQ_EVTR1_END          0
#define IRQ_EVTR2_START        0
#define IRQ_EVTR2_END          0
#define IRQ_EVTR3_START        0
#define IRQ_EVTR3_END          0

#endif


#define NR_IRQS		(NR_IRQ_CPU + NR_IRQ_BOARD)

#endif
