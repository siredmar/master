/***********************************************************************
 * $Id:: ea3152_board.h 6515 2011-02-22 09:40:38Z ing04225             $
 *
 * Project: EA3152 board definitions
 *
 * Description:
 *     This file contains board specific information such as the
 *     chip select wait states, and other board specific information.
 *
 ***********************************************************************
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * NXP Semiconductors assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. NXP Semiconductors
 * reserves the right to make changes in the software without
 * notification. NXP Semiconductors also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
 **********************************************************************/

#ifndef EA3152_BOARD_H
#define EA3152_BOARD_H

#include "lpc_types.h"
#include "lpc313x_chip.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* board crystal frequency */
#define XTAL_IN		        (12000000)


/* on EA3152 SPI NOR flash is connected to chip select 0 */
#define SPI_NOR_SLAVE_ID	  0

/***********************************************************************
 * LCD modules supported by EA3131 board
 **********************************************************************/
#define EA31_LCD_CMD      (*((volatile UNS_16 *) 0x20000000))
#define EA31_LCD_DATA     (*((volatile UNS_16 *) 0x20010000))
#define EA31_LCD_DATA_ADR (0x20010000)

/* QVGA LCD v2.x display */
#define EA_DISP_QVGA_LCD_V2_ID  0x8989
/* QVGA OLED v1.x display */
#define EA_DISP_QVGA_OLED_V1_ID 0x63D6

/***********************************************************************
 * Address of slave device connected to I2C ports
 **********************************************************************/
#define EA3152_I2C0_LEDC_ADR     0xC0
#define EA3152_I2C0_EEPROM_ADR   0xA0
#define EA3152_I2C1_CODEC_ADR    0x34

#define EA3152_SEEPROM_SIZE      0x8000

/***********************************************************************
 * Delay constants in nanosecs for MT48LC32M16LF SDRAM on board
 **********************************************************************/
#define EA3152_SDRAM_TRP            (20)
#define EA3152_SDRAM_TRFC           (66)
#define EA3152_SDRAM_TRAS           (44)
#define EA3152_SDRAM_TREX           (75)
#define EA3152_SDRAM_TARP           4
#define EA3152_SDRAM_TWR            (75)
#define EA3152_SDRAM_TRC            (66)
#define EA3152_SDRAM_TRRD           (15)
#define EA3152_SDRAM_TMRD           (20)
#define EA3152_SDRAM_TXSR           (75)
#define EA3152_SDRAM_TDAL           (50)
#define EA3152_SDRAM_REFRESH        (100)
#define EA3152_SDRAM_OPER_REFRESH   (7812)

#define EA3152_MPMC_DELAY           0x824

/* Miscellaneous board setup functions */
void ea3152_board_init(void);

/* SPI NOR flash driver functions */
STATUS spinor_init(INT_32 spi_id);
STATUS spinor_read(UNS_8 *dst, UNS_32 offset, UNS_32 bytecount);
STATUS spinor_write(UNS_8 *src, UNS_32 offset, UNS_32 bytecount);
STATUS spinor_erase_from_beginning(UNS_32 bytecount);
STATUS spinor_erase_sector(UNS_32 sectorIndex);
INT_32 spinor_write_page(UNS_8 *src, UNS_32 offset);

/* SD/MMC specific functions */
void mci_init(void);

/* LCD functions */
STATUS ea3152_lcd_init(UNS_32 disp_id);
void ea3152_lcd_set_pos(UNS_16 x, UNS_16 y);

#ifdef __cplusplus
}
#endif

#endif /* EA3152_BOARD_H */
