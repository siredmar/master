/* mx31-pins.h
     $Id$

   written by Marc Singer
   22 Dec 2006

   Copyright (C) 2006 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   The format of this file was inspired by the Freescale Linux kernel
   code.  We've changed the format significantly in order to aid
   debugging.

*/

#if !defined (__MX31_PINS_H__)
#    define   __MX31_PINS_H__


/* MUX is bits 9-0. Byte address of MUX field.
   PAD is bits 25-16.  Word address of PAD field, field index in MSBs.
   GPIO register index is bits 31-26.
     0 (GPIO1) is 0xcc >> 2
     1 (GPIO2) is 0xd0 >> 2
     2 (GPIO3) is 0xa4 >> 2
     none is 0
   GPIO field is bits 15-11.
 */

/** Pin definition macro.  This is the general macro for declaring a
    pin's encoding. */
#define __P(mux_r,mux_f,pad_r,pad_f,gpio_r,gpio_f)\
	(((((mux_r) & 0x7f) + (0x00c >>2)) << 2) /* MUX register */\
	|( ((mux_f) & 0x3 )		  << 0)	 /* MUX field w/in register */\
	|((((pad_r) & 0x7f) + (0x154 >>2)) << 18)/* PAD register */\
	|( ((pad_f) & 0x3 )		  << 16) /* PAD field w/in register */\
	|((((gpio_r) + 1) & 0x3 )	  << 28) /* GPIO register */\
	|( ((gpio_f)      & 0x1f)	  << 11))/* GPIO field w/in reg. */\

/** GPIO pin declaration.  Used to declare a pin that has GPIO capabilities. */
#define __G(mux_r,mux_f,pad_r,pad_f,gpio_r,gpio_f)\
	__P (mux_r, mux_f, pad_r, pad_f, gpio_r, gpio_f)
/** Non-GPIO pin declaration.  Used to delcare a ping that has no GPIO
    capabilities.  This is a convenience since the GPIO fields are at
    the end of the parameter list and are zero for non-gpio pins. */
#define __N(mux_r,mux_f,pad_r,pad_f)	  __P(mux_r,mux_f,pad_r,pad_f,0,0)

enum {
  MX31_PIN_CSPI3_MISO     = __N ( 0, 3,   1, 2),
  MX31_PIN_CSPI3_SCLK	  = __N ( 0, 2,   1, 1),
  MX31_PIN_CSPI3_SPI_RDY  = __N ( 0, 1,   1, 0),
  MX31_PIN_TTM_PAD	  = __N ( 0, 0,   0, 2),
  MX31_PIN_ATA_RESET_B	  = __G ( 1, 3,   3, 0,	2, 31),
  MX31_PIN_CE_CONTROL	  = __N ( 1, 2,   2, 2),
  MX31_PIN_CLKSS	  = __N ( 1, 1,   2, 1),
  MX31_PIN_CSPI3_MOSI	  = __N ( 1, 0,   2, 0),
  MX31_PIN_ATA_CS1	  = __G ( 2, 3,   4, 1,	2, 27),
  MX31_PIN_ATA_DIOR	  = __G ( 2, 2,   4, 0,	2, 28),
  MX31_PIN_ATA_DIOW	  = __G ( 2, 1,   3, 2,	2, 29),
  MX31_PIN_ATA_DMACK	  = __G ( 2, 0,   3, 1,	2, 30),
  MX31_PIN_SD1_DATA1	  = __G ( 3, 3,   5, 2,	1, 29),
  MX31_PIN_SD1_DATA2	  = __G ( 3, 2,   5, 1,	1, 30),
  MX31_PIN_SD1_DATA3	  = __G ( 3, 1,   5, 0,	1, 31),
  MX31_PIN_ATA_CS0	  = __G ( 3, 0,   4, 2,	2, 26),
  MX31_PIN_D3_SPL	  = __N ( 4, 3,   7, 0),
  MX31_PIN_SD1_CMD	  = __G ( 4, 2,   6, 2,	1, 26),
  MX31_PIN_SD1_CLK	  = __G ( 4, 1,   6, 1,	1, 27),
  MX31_PIN_SD1_DATA0	  = __G ( 4, 0,   6, 0,	1, 28),
  MX31_PIN_VSYNC3	  = __N ( 5, 3,   8, 1),
  MX31_PIN_CONTRAST	  = __N ( 5, 2,   8, 0),
  MX31_PIN_D3_REV	  = __N ( 5, 1,   7, 2),
  MX31_PIN_D3_CLS	  = __N ( 5, 0,   7, 1),
  MX31_PIN_SER_RS	  = __G ( 6, 3,   9, 2,	2, 25),
  MX31_PIN_PAR_RS	  = __N ( 6, 2,   9, 1),
  MX31_PIN_WRITE	  = __N ( 6, 1,   9, 0),
  MX31_PIN_READ		  = __N ( 6, 0,   8, 2),
  MX31_PIN_SD_D_IO	  = __G ( 7, 3,  11, 0,	2, 21),
  MX31_PIN_SD_D_CLK	  = __G ( 7, 2,  10, 2,	2, 22),
  MX31_PIN_LCS0		  = __G ( 7, 1,  10, 1,	2, 23),
  MX31_PIN_LCS1		  = __G ( 7, 0,  10, 0,	2, 24),
  MX31_PIN_HSYNC	  = __N ( 8, 3,  12, 1),
  MX31_PIN_FPSHIFT	  = __N ( 8, 2,  12, 0),
  MX31_PIN_DRDY0	  = __N ( 8, 1,  11, 2),
  MX31_PIN_SD_D_I	  = __G ( 8, 0,  11, 1,	2, 20),
  MX31_PIN_LD15		  = __N ( 9, 3,  13, 2),
  MX31_PIN_LD16		  = __N ( 9, 2,  13, 1),
  MX31_PIN_LD17		  = __N ( 9, 1,  13, 0),
  MX31_PIN_VSYNC0	  = __N ( 9, 0,  12, 2),
  MX31_PIN_LD11		  = __N (10, 3,  15, 0),
  MX31_PIN_LD12		  = __N (10, 2,  14, 2),
  MX31_PIN_LD13		  = __N (10, 1,  14, 1),
  MX31_PIN_LD14		  = __N (10, 0,  14, 0),
  MX31_PIN_LD7		  = __N (11, 3,  16, 1),
  MX31_PIN_LD8		  = __N (11, 2,  16, 0),
  MX31_PIN_LD9		  = __N (11, 1,  15, 2),
  MX31_PIN_LD10		  = __N (11, 0,  15, 1),
  MX31_PIN_LD3		  = __N (12, 3,  17, 2),
  MX31_PIN_LD4		  = __N (12, 2,  17, 1),
  MX31_PIN_LD5		  = __N (12, 1,  17, 0),
  MX31_PIN_LD6		  = __N (12, 0,  16, 2),
  MX31_PIN_USBH2_DATA1	  = __N (13, 3,  19, 0),
  MX31_PIN_LD0		  = __N (13, 2,  18, 2),
  MX31_PIN_LD1		  = __N (13, 1,  18, 1),
  MX31_PIN_LD2		  = __N (13, 0,  18, 0),
  MX31_PIN_USBH2_DIR	  = __N (14, 3,  20, 1),
  MX31_PIN_USBH2_STP	  = __N (14, 2,  20, 0),
  MX31_PIN_USBH2__NXT	  = __N (14, 1,  19, 2),
  MX31_PIN_USBH2_DATA0	  = __N (14, 0,  19, 1),
  MX31_PIN_USBOTG_DATA5	  = __N (15, 3,  21, 2),
  MX31_PIN_USBOTG_DATA6	  = __N (15, 2,  21, 1),
  MX31_PIN_USBOTG_DATA7	  = __N (15, 1,  21, 0),
  MX31_PIN_USBH2_CLK	  = __N (15, 0,  20, 2),
  MX31_PIN_USBOTG_DATA1	  = __N (16, 3,  23, 0),
  MX31_PIN_USBOTG_DATA2	  = __N (16, 2,  22, 2),
  MX31_PIN_USBOTG_DATA3	  = __N (16, 1,  22, 1),
  MX31_PIN_USBOTG_DATA4	  = __N (16, 0,  22, 0),
  MX31_PIN_USBOTG_DIR	  = __N (17, 3,  24, 1),
  MX31_PIN_USBOTG_STP	  = __N (17, 2,  24, 0),
  MX31_PIN_USBOTG__NXT	  = __N (17, 1,  23, 2),
  MX31_PIN_USBOTG_DATA0	  = __N (17, 0,  23, 1),
  MX31_PIN_USB_PWR	  = __G (18, 3,  25, 2,	0, 29),
  MX31_PIN_USB_OC	  = __G (18, 2,  25, 1,	0, 30),
  MX31_PIN_USB_BYP	  = __G (18, 1,  25, 0,	0, 31),
  MX31_PIN_USBOTG_CLK	  = __N (18, 0,  24, 2),
  MX31_PIN_TDO		  = __N (19, 3,  27, 0),
  MX31_PIN_TRSTB	  = __N (19, 2,  26, 2),
  MX31_PIN_DE_B		  = __N (19, 1,  26, 1),
  MX31_PIN_SJC_MOD	  = __N (19, 0,  26, 0),
  MX31_PIN_RTCK		  = __N (20, 3,  28, 1),
  MX31_PIN_TCK		  = __N (20, 2,  28, 0),
  MX31_PIN_TMS		  = __N (20, 1,  27, 2),
  MX31_PIN_TDI		  = __N (20, 0,  27, 1),
  MX31_PIN_KEY_COL4	  = __G (21, 3,  29, 2,	1, 22),
  MX31_PIN_KEY_COL5	  = __G (21, 2,  29, 1,	1, 23),
  MX31_PIN_KEY_COL6	  = __G (21, 1,  29, 0,	1, 24),
  MX31_PIN_KEY_COL7	  = __G (21, 0,  28, 2,	1, 25),
  MX31_PIN_KEY_COL0	  = __N (22, 3,  31, 0),
  MX31_PIN_KEY_COL1	  = __N (22, 2,  30, 2),
  MX31_PIN_KEY_COL2	  = __N (22, 1,  30, 1),
  MX31_PIN_KEY_COL3	  = __N (22, 0,  30, 0),
  MX31_PIN_KEY_ROW4	  = __G (23, 3,  32, 1,	1, 18),
  MX31_PIN_KEY_ROW5	  = __G (23, 2,  32, 0,	1, 19),
  MX31_PIN_KEY_ROW6	  = __G (23, 1,  31, 2,	1, 20),
  MX31_PIN_KEY_ROW7	  = __G (23, 0,  31, 1,	1, 21),
  MX31_PIN_KEY_ROW0	  = __N (24, 3,  33, 2),
  MX31_PIN_KEY_ROW1	  = __N (24, 2,  33, 1),
  MX31_PIN_KEY_ROW2	  = __N (24, 1,  33, 0),
  MX31_PIN_KEY_ROW3	  = __N (24, 0,  32, 2),
  MX31_PIN_TXD2		  = __G (25, 3,  35, 0,	0, 28),
  MX31_PIN_RTS2		  = __N (25, 2,  34, 2),
  MX31_PIN_CTS2		  = __N (25, 1,  34, 1),
  MX31_PIN_BATT_LINE	  = __G (25, 0,  34, 0,	1, 17),
  MX31_PIN_RI_DTE1	  = __G (26, 3,  36, 1,	1, 14),
  MX31_PIN_DCD_DTE1	  = __G (26, 2,  36, 0,	1, 15),
  MX31_PIN_DTR_DCE2	  = __G (26, 1,  35, 2,	1, 16),
  MX31_PIN_RXD2		  = __G (26, 0,  35, 1,	0, 27),
  MX31_PIN_RI_DCE1	  = __G (27, 3,  37, 2,	1, 10),
  MX31_PIN_DCD_DCE1	  = __G (27, 2,  37, 1,	1, 11),
  MX31_PIN_DTR_DTE1	  = __G (27, 1,  37, 0,	1, 12),
  MX31_PIN_DSR_DTE1	  = __G (27, 0,  36, 2,	1, 13),
  MX31_PIN_RTS1		  = __G (28, 3,  39, 0,	1,  6),
  MX31_PIN_CTS1		  = __G (28, 2,  38, 2,	1,  7),
  MX31_PIN_DTR_DCE1	  = __G (28, 1,  38, 1,	1,  8),
  MX31_PIN_DSR_DCE1	  = __G (28, 0,  38, 0,	1,  9),
  MX31_PIN_CSPI2_SCLK	  = __N (29, 3,  40, 1),
  MX31_PIN_CSPI2_SPI_RDY  = __N (29, 2,  40, 0),
  MX31_PIN_RXD1		  = __G (29, 1,  39, 2,	1,  4),
  MX31_PIN_TXD1		  = __G (29, 0,  39, 1,	1,  5),
  MX31_PIN_CSPI2_MISO	  = __N (30, 3,  41, 2),
  MX31_PIN_CSPI2_SS0	  = __N (30, 2,  41, 1),
  MX31_PIN_CSPI2_SS1	  = __N (30, 1,  41, 0),
  MX31_PIN_CSPI2_SS2	  = __N (30, 0,  40, 2),
  MX31_PIN_CSPI1_SS2	  = __N (31, 3,  43, 0),
  MX31_PIN_CSPI1_SCLK	  = __N (31, 2,  42, 2),
  MX31_PIN_CSPI1_SPI_RDY  = __N (31, 1,  42, 1),
  MX31_PIN_CSPI2_MOSI	  = __N (31, 0,  42, 0),
  MX31_PIN_CSPI1_MOSI	  = __N (32, 3,  44, 1),
  MX31_PIN_CSPI1_MISO	  = __N (32, 2,  44, 0),
  MX31_PIN_CSPI1_SS0	  = __N (32, 1,  43, 2),
  MX31_PIN_CSPI1_SS1	  = __N (32, 0,  43, 1),
  MX31_PIN_STXD6	  = __G (33, 3,  45, 2,	0, 23),
  MX31_PIN_SRXD6	  = __G (33, 2,  45, 1,	0, 24),
  MX31_PIN_SCK6		  = __G (33, 1,  45, 0,	0, 25),
  MX31_PIN_SFS6		  = __G (33, 0,  44, 2,	0, 26),
  MX31_PIN_STXD5	  = __G (34, 3,  47, 0,	0, 21),
  MX31_PIN_SRXD5	  = __G (34, 2,  46, 2,	0, 22),
  MX31_PIN_SCK5		  = __N (34, 1,  46, 1),
  MX31_PIN_SFS5		  = __N (34, 0,  46, 0),
  MX31_PIN_STXD4	  = __G (35, 3,  48, 1,	0, 19),
  MX31_PIN_SRXD4	  = __G (35, 2,  48, 0,	0, 20),
  MX31_PIN_SCK4		  = __N (35, 1,  47, 2),
  MX31_PIN_SFS4		  = __N (35, 0,  47, 1),
  MX31_PIN_STXD3	  = __G (36, 3,  49, 2,	0, 17),
  MX31_PIN_SRXD3	  = __G (36, 2,  49, 1,	0, 18),
  MX31_PIN_SCK3		  = __N (36, 1,  49, 0),
  MX31_PIN_SFS3		  = __N (36, 0,  48, 2),
  MX31_PIN_CSI_HSYNC	  = __G (37, 3,  51, 0,	2, 18),
  MX31_PIN_CSI_PIXCLK	  = __G (37, 2,  50, 2,	2, 19),
  MX31_PIN_I2C_CLK	  = __N (37, 1,  50, 1),
  MX31_PIN_I2C_DAT	  = __N (37, 0,  50, 0),
  MX31_PIN_CSI_D14	  = __G (38, 3,  52, 1,	2, 14),
  MX31_PIN_CSI_D15	  = __G (38, 2,  52, 0,	2, 15),
  MX31_PIN_CSI_MCLK	  = __G (38, 1,  51, 2,	2, 16),
  MX31_PIN_CSI_VSYNC	  = __G (38, 0,  51, 1,	2, 17),
  MX31_PIN_CSI_D10	  = __G (39, 3,  53, 2,	2, 10),
  MX31_PIN_CSI_D11	  = __G (39, 2,  53, 1,	2, 11),
  MX31_PIN_CSI_D12	  = __G (39, 1,  53, 0,	2, 12),
  MX31_PIN_CSI_D13	  = __G (39, 0,  52, 2,	2, 13),
  MX31_PIN_CSI_D6	  = __G (40, 3,  55, 0,	2,  6),
  MX31_PIN_CSI_D7	  = __G (40, 2,  54, 2,	2,  7),
  MX31_PIN_CSI_D8	  = __G (40, 1,  54, 1,	2,  8),
  MX31_PIN_CSI_D9	  = __G (40, 0,  54, 0,	2,  9),
  MX31_PIN_M_REQUEST	  = __N (41, 3,  56, 1),
  MX31_PIN_M__GRANT	  = __N (41, 2,  56, 0),
  MX31_PIN_CSI_D4	  = __G (41, 1,  55, 2,	2,  4),
  MX31_PIN_CSI_D5	  = __G (41, 0,  55, 1,	2,  5),
  MX31_PIN_PC_RST	  = __N (42, 3,  57, 2),
  MX31_PIN_IOIS16	  = __N (42, 2,  57, 1),
  MX31_PIN_PC_RW_B	  = __N (42, 1,  57, 0),
  MX31_PIN_PC_POE	  = __N (42, 0,  56, 2),
  MX31_PIN_PC_VS1	  = __N (43, 3,  59, 0),
  MX31_PIN_PC_VS2	  = __N (43, 2,  58, 2),
  MX31_PIN_PC_BVD1	  = __N (43, 1,  58, 1),
  MX31_PIN_PC_BVD2	  = __N (43, 0,  58, 0),
  MX31_PIN_PC_CD2_B	  = __N (44, 3,  60, 1),
  MX31_PIN_PC_WAIT_B	  = __N (44, 2,  60, 0),
  MX31_PIN_PC_READY	  = __N (44, 1,  59, 2),
  MX31_PIN_PC_PWRON	  = __N (44, 0,  59, 1),
  MX31_PIN_D2		  = __N (45, 3,  61, 2),
  MX31_PIN_D1		  = __N (45, 2,  61, 1),
  MX31_PIN_D0		  = __N (45, 1,  61, 0),
  MX31_PIN_PC_CD1_B	  = __N (45, 0,  60, 2),
  MX31_PIN_D6		  = __N (46, 3,  63, 0),
  MX31_PIN_D5		  = __N (46, 2,  62, 2),
  MX31_PIN_D4		  = __N (46, 1,  62, 1),
  MX31_PIN_D3		  = __N (46, 0,  62, 0),
  MX31_PIN_D10		  = __N (47, 3,  64, 1),
  MX31_PIN_D9		  = __N (47, 2,  64, 0),
  MX31_PIN_D8		  = __N (47, 1,  63, 2),
  MX31_PIN_D7		  = __N (47, 0,  63, 1),
  MX31_PIN_D14		  = __N (48, 3,  65, 2),
  MX31_PIN_D13		  = __N (48, 2,  65, 1),
  MX31_PIN_D12		  = __N (48, 1,  65, 0),
  MX31_PIN_D11		  = __N (48, 0,  64, 2),
  MX31_PIN_NFWP_B	  = __G (49, 3,  67, 0,	0, 14),
  MX31_PIN_NFCE_B	  = __G (49, 2,  66, 2,	0, 15),
  MX31_PIN_NFRB		  = __G (49, 1,  66, 1,	0, 16),
  MX31_PIN_D15		  = __N (49, 0,  66, 0),
  MX31_PIN_NFWE_B	  = __G (50, 3,  68, 1,	0, 10),
  MX31_PIN_NFRE_B	  = __G (50, 2,  68, 0,	0, 11),
  MX31_PIN_NFALE	  = __G (50, 1,  67, 2,	0, 12),
  MX31_PIN_NFCLE	  = __G (50, 0,  67, 1,	0, 13),
  MX31_PIN_SDQS0	  = __N (51, 3,  69, 2),
  MX31_PIN_SDQS1	  = __N (51, 2,  69, 1),
  MX31_PIN_SDQS2	  = __N (51, 1,  69, 0),
  MX31_PIN_SDQS3	  = __N (51, 0,  68, 2),
  MX31_PIN_SDCKE0	  = __N (52, 3,  71, 0),
  MX31_PIN_SDCKE1	  = __N (52, 2,  70, 2),
  MX31_PIN_SDCLK	  = __N (52, 1,  70, 1),
  MX31_PIN_SDCLK_B	  = __N (52, 0,  70, 0),
  MX31_PIN_RW		  = __N (53, 3,  72, 1),
  MX31_PIN_RAS		  = __N (53, 2,  72, 0),
  MX31_PIN_CAS		  = __N (53, 1,  71, 2),
  MX31_PIN_SDWE		  = __N (53, 0,  71, 1),
  MX31_PIN_CS5		  = __N (54, 3,  73, 2),
  MX31_PIN_ECB		  = __N (54, 2,  73, 1),
  MX31_PIN_LBA		  = __N (54, 1,  73, 0),
  MX31_PIN_BCLK		  = __N (54, 0,  72, 2),
  MX31_PIN_CS1		  = __N (55, 3,  75, 0),
  MX31_PIN_CS2		  = __N (55, 2,  74, 2),
  MX31_PIN_CS3		  = __N (55, 1,  74, 1),
  MX31_PIN_CS4		  = __N (55, 0,  74, 0),
  MX31_PIN_EB0		  = __N (56, 3,  76, 1),
  MX31_PIN_EB1		  = __N (56, 2,  76, 0),
  MX31_PIN_OE		  = __N (56, 1,  75, 2),
  MX31_PIN_CS0		  = __N (56, 0,  75, 1),
  MX31_PIN_DQM0		  = __N (57, 3,  77, 2),
  MX31_PIN_DQM1		  = __N (57, 2,  77, 1),
  MX31_PIN_DQM2		  = __N (57, 1,  77, 0),
  MX31_PIN_DQM3		  = __N (57, 0,  76, 2),
  MX31_PIN_SD28		  = __N (58, 3,  79, 0),
  MX31_PIN_SD29		  = __N (58, 2,  78, 2),
  MX31_PIN_SD30		  = __N (58, 1,  78, 1),
  MX31_PIN_SD31		  = __N (58, 0,  78, 0),
  MX31_PIN_SD24		  = __N (59, 3,  80, 1),
  MX31_PIN_SD25		  = __N (59, 2,  80, 0),
  MX31_PIN_SD26		  = __N (59, 1,  79, 2),
  MX31_PIN_SD27		  = __N (59, 0,  79, 1),
  MX31_PIN_SD20		  = __N (60, 3,  81, 2),
  MX31_PIN_SD21		  = __N (60, 2,  81, 1),
  MX31_PIN_SD22		  = __N (60, 1,  81, 0),
  MX31_PIN_SD23		  = __N (60, 0,  80, 2),
  MX31_PIN_SD16		  = __N (61, 3,  83, 0),
  MX31_PIN_SD17		  = __N (61, 2,  82, 2),
  MX31_PIN_SD18		  = __N (61, 1,  82, 1),
  MX31_PIN_SD19		  = __N (61, 0,  82, 0),
  MX31_PIN_SD12		  = __N (62, 3,  84, 1),
  MX31_PIN_SD13		  = __N (62, 2,  84, 0),
  MX31_PIN_SD14		  = __N (62, 1,  83, 2),
  MX31_PIN_SD15		  = __N (62, 0,  83, 1),
  MX31_PIN_SD8		  = __N (63, 3,  85, 2),
  MX31_PIN_SD9		  = __N (63, 2,  85, 1),
  MX31_PIN_SD10		  = __N (63, 1,  85, 0),
  MX31_PIN_SD11		  = __N (63, 0,  84, 2),
  MX31_PIN_SD4		  = __N (64, 3,  87, 0),
  MX31_PIN_SD5		  = __N (64, 2,  86, 2),
  MX31_PIN_SD6		  = __N (64, 1,  86, 1),
  MX31_PIN_SD7		  = __N (64, 0,  86, 0),
  MX31_PIN_SD0		  = __N (65, 3,  88, 1),
  MX31_PIN_SD1		  = __N (65, 2,  88, 0),
  MX31_PIN_SD2		  = __N (65, 1,  87, 2),
  MX31_PIN_SD3		  = __N (65, 0,  87, 1),
  MX31_PIN_A24		  = __N (66, 3,  89, 2),
  MX31_PIN_A25		  = __N (66, 2,  89, 1),
  MX31_PIN_SDBA1	  = __N (66, 1,  89, 0),
  MX31_PIN_SDBA0	  = __N (66, 0,  88, 2),
  MX31_PIN_A20		  = __N (67, 3,  91, 0),
  MX31_PIN_A21		  = __N (67, 2,  90, 2),
  MX31_PIN_A22		  = __N (67, 1,  90, 1),
  MX31_PIN_A23		  = __N (67, 0,  90, 0),
  MX31_PIN_A16		  = __N (68, 3,  92, 1),
  MX31_PIN_A17		  = __N (68, 2,  92, 0),
  MX31_PIN_A18		  = __N (68, 1,  91, 2),
  MX31_PIN_A19		  = __N (68, 0,  91, 1),
  MX31_PIN_A12		  = __N (69, 3,  93, 2),
  MX31_PIN_A13		  = __N (69, 2,  93, 1),
  MX31_PIN_A14		  = __N (69, 1,  93, 0),
  MX31_PIN_A15		  = __N (69, 0,  92, 2),
  MX31_PIN_A9		  = __N (70, 3,  95, 0),
  MX31_PIN_A10		  = __N (70, 2,  94, 2),
  MX31_PIN_MA10		  = __N (70, 1,  94, 1),
  MX31_PIN_A11		  = __N (70, 0,  94, 0),
  MX31_PIN_A5		  = __N (71, 3,  96, 1),
  MX31_PIN_A6		  = __N (71, 2,  96, 0),
  MX31_PIN_A7		  = __N (71, 1,  95, 2),
  MX31_PIN_A8		  = __N (71, 0,  95, 1),
  MX31_PIN_A1		  = __N (72, 3,  97, 2),
  MX31_PIN_A2		  = __N (72, 2,  97, 1),
  MX31_PIN_A3		  = __N (72, 1,  97, 0),
  MX31_PIN_A4		  = __N (72, 0,  96, 2),
  MX31_PIN_DVFS1	  = __N (73, 3,  99, 0),
  MX31_PIN_VPG0		  = __N (73, 2,  98, 2),
  MX31_PIN_VPG1		  = __N (73, 1,  98, 1),
  MX31_PIN_A0		  = __N (73, 0,  98, 0),
  MX31_PIN_CKIL		  = __N (74, 3, 100, 1),
  MX31_PIN_POWER_FAIL	  = __N (74, 2, 100, 0),
  MX31_PIN_VSTBY	  = __N (74, 1,  99, 2),
  MX31_PIN_DVFS0	  = __N (74, 0,  99, 1),
  MX31_PIN_BOOT_MODE1	  = __N (75, 3, 101, 2),
  MX31_PIN_BOOT_MODE2	  = __N (75, 2, 101, 1),
  MX31_PIN_BOOT_MODE3	  = __N (75, 1, 101, 0),
  MX31_PIN_BOOT_MODE4	  = __N (75, 0, 100, 2),
  MX31_PIN_RESET_IN_B	  = __N (76, 3, 103, 0),
  MX31_PIN_POR_B	  = __N (76, 2, 102, 2),
  MX31_PIN_CLKO		  = __N (76, 1, 102, 1),
  MX31_PIN_BOOT_MODE0	  = __N (76, 0, 102, 0),
  MX31_PIN_STX0		  = __G (77, 3, 104, 1,	1,  1),
  MX31_PIN_SRX0		  = __G (77, 2, 104, 0,	1,  2),
  MX31_PIN_SIMPD0	  = __G (77, 1, 103, 2,	1,  3),
  MX31_PIN_CKIH		  = __N (77, 0, 103, 1),
  MX31_PIN_GPIO3_1	  = __G (78, 3, 105, 2,	2,  1),
  MX31_PIN_SCLK0	  = __G (78, 2, 105, 1,	2,  2),
  MX31_PIN_SRST0	  = __G (78, 1, 105, 0,	2,  3),
  MX31_PIN_SVEN0	  = __G (78, 0, 104, 2,	1,  0),
  MX31_PIN_GPIO1_4	  = __G (79, 3, 107, 0,	0,  4),
  MX31_PIN_GPIO1_5	  = __G (79, 2, 106, 2,	0,  5),
  MX31_PIN_GPIO1_6	  = __G (79, 1, 106, 1,	0,  6),
  MX31_PIN_GPIO3_0	  = __G (79, 0, 106, 0,	2,  0),
  MX31_PIN_GPIO1_0	  = __G (80, 3, 108, 1,	0,  0),
  MX31_PIN_GPIO1_1	  = __G (80, 2, 108, 0,	0,  1),
  MX31_PIN_GPIO1_2	  = __G (80, 1, 107, 2,	0,  2),
  MX31_PIN_GPIO1_3	  = __G (80, 0, 107, 1,	0,  3),
  MX31_PIN_CAPTURE	  = __G (81, 3, 109, 2,	0,  7),
  MX31_PIN_COMPARE	  = __G (81, 2, 109, 1,	0,  8),
  MX31_PIN_WATCHDOG_RST	  = __N (81, 1, 109, 0),
  MX31_PIN_PWMO		  = __G (81, 0, 108, 2,	0,  9),
};

#undef __N
#undef __P
#undef __G

#define _GPIO_R(i) "\x00\xcc\xd0\xa4"[i&0x3]
#define _PIN_MUX_R(v)	((v) & 0x1fc)
#define _PIN_MUX_F(v)	((v) & 0x3)
#define _PIN_PAD_R(v)	(((v) >> 16) & 0x3fc)
#define _PIN_PAD_F(v)	(((v) >> 16) & 0x3)
#define _PIN_IS_GPIO(v)	(((v) & (3<<28)) != 0)
#define _PIN_GPIO_R(v)	(((v) >> 28) & 0x3)		/* See comments */
#define _PIN_GPIO_F(v)	(((v) >> 11) & 0x1f)

  /* These registers are indexed from bases in the PIN macros.  There
     we store a real index of the register instead of an address
     offset from the base--which is why the MUX and PAD macros are
     identical.  The GPIO indices include the bits needed to find each
     of the GPIO regions. */
#define _SW_MUX_CTL(r)	__REG(PHYS_IOMUXC + (r))
#define _SW_PAD_CTL(r)	__REG(PHYS_IOMUXC + (r))
#define _GPIO_BASE(r)	((PHYS_GPIO1 & 0xfff00000) | (_GPIO_R(r)<<12))
#define _GPIO_DR(r)	__REG(_GPIO_BASE(r) + 0x00)
#define _GPIO_GDIR(r)	__REG(_GPIO_BASE(r) + 0x04)
#define _GPIO_PSR(r)	__REG(_GPIO_BASE(r) + 0x08)
#define _GPIO_ICR1(r)	__REG(_GPIO_BASE(r) + 0x0c)
#define _GPIO_ICR2(r)	__REG(_GPIO_BASE(r) + 0x10)
#define _GPIO_IMR(r)	__REG(_GPIO_BASE(r) + 0x14)
#define _GPIO_ISR(r)	__REG(_GPIO_BASE(r) + 0x18)

  /* Configure ping as GPIO. */
#define IOMUX_PIN_CONFIG_GPIO(p)\
	MASK_AND_SET(_SW_MUX_CTL(_PIN_MUX_R (p)),\
		     (0x7f<<(_PIN_MUX_F (p)*8)),\
		     ((0<<4)|(1<<0))<<(_PIN_MUX_F (p)*8))

  /* Configure pin as functional. */
#define IOMUX_PIN_CONFIG_FUNC(p)\
	MASK_AND_SET(_SW_MUX_CTL(_PIN_MUX_R (p)),\
		     (0x7f<<(_PIN_MUX_F (p)*8)),\
		     ((1<<4)|(2<<0))<<(_PIN_MUX_F (p)*8))

  /* Configure pin as alternate output (1,2,3,4,5,6). */
#define IOMUX_PIN_CONFIG_ALT_OUT(p,n)\
	MASK_AND_SET(_SW_MUX_CTL(_PIN_MUX_R (p)),\
		     (0x7f<<(_PIN_MUX_F (p)*8)),\
		     (((1+(n))<<4)|(0<<0))<<(_PIN_MUX_F (p)*8))

  /* Configure pin as alternate input (1,2). */
#define IOMUX_PIN_CONFIG_ALT_IN(p,n)\
	MASK_AND_SET(_SW_MUX_CTL(_PIN_MUX_R (p)),\
		     (0x7f<<(_PIN_MUX_F (p)*8)),\
		     ((0<<4)|((n)<<2))<<(_PIN_MUX_F (p)*8))

  /* Configure pin as alternate input (1,2)/output (1,2,3,4,5,6).  It
     is not clear if this macro is necessary as most pins are defined
     either as an output or an input, seldom either. */
#define IOMUX_PIN_CONFIG_ALT_IN_OUT(p,i,o)               \
	MASK_AND_SET(_SW_MUX_CTL(_PIN_MUX_R (p)),\
		     (0x7f<<(_PIN_MUX_F (p)*8)),\
                     (((1+(o))<<4)|((i)<<2))<<(_PIN_MUX_F (p)*8))

#define IOMUX_PIN_REG(p)\
	_SW_PAD_CTL (_PIN_MUX_R (p))

#define IOMUX_PAD_CONFIG(p,v)\
	MASK_AND_SET(_SW_PAD_CTL(_PIN_PAD_R (p)),\
		     (0x3ff<<(_PIN_PAD_F (p)*10)),\
		     (v)   <<(_PIN_MUX_F (p)*10))

#define GPIO_PIN_CONFIG_OUTPUT(p)\
	(_GPIO_GDIR(_PIN_GPIO_R (p)) |=   (1 << _PIN_GPIO_F (p)))
#define GPIO_PIN_CONFIG_INPUT(p)\
	(_GPIO_GDIR(_PIN_GPIO_R (p)) &= ~(1 << _PIN_GPIO_F (p)))

#define GPIO_PIN_SET(p)\
	(_GPIO_DR(_PIN_GPIO_R (p))   |=   1 << _PIN_GPIO_F (p))
#define GPIO_PIN_CLEAR(p)\
	(_GPIO_DR(_PIN_GPIO_R (p))   &= ~(1 << _PIN_GPIO_F (p)))
#define GPIO_PIN_VALUE(p)\
	((_GPIO_DR(_PIN_GPIO_R (p)) & (1 << _PIN_GPIO_F (p))) != 0)

#endif
