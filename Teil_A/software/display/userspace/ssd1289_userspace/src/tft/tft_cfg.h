/**
 *  COPYRIGHT: Armin Schlegel
 *  \file tft.h
 *  \brief Display Graphic Driver
 *
 *  This header file contains all tft specific datatypes and defines.
 *
 *******************************************************************************/
#ifndef TFT_CFG_H
#define TFT_CFG_H
/* ***************************** includes < > ******************************* */

/* ***************************** includes " " ******************************* */

/* ***************************** typedefs *********************************** */
/* ***************************** defines ************************************ */

#define TFT_WIDTH_UI16                       (uint16)240
#define TFT_HEIGHT_UI16                      (uint16)320

#define TFT_ZERO_X                           (uint16)0
#define TFT_ZERO_Y                           (uint16)0
#define TFT_PIXELS_UI16                     (uint16)38400


#define TFT_BACKLIGHT_PIN_UI8             DIO_CHANNEL_20

#define TFT_RESET_PIN_UI8                 DIO_CHANNEL_19

#define SSD1289_REG_OSCILLATION      0x00
#define SSD1289_REG_DRIVER_OUT_CTRL  0x01
#define SSD1289_REG_LCD_DRIVE_AC     0x02
#define SSD1289_REG_POWER_CTRL_1     0x03
#define SSD1289_REG_DISPLAY_CTRL     0x07
#define SSD1289_REG_FRAME_CYCLE      0x0b
#define SSD1289_REG_POWER_CTRL_2     0x0c
#define SSD1289_REG_POWER_CTRL_3     0x0d
#define SSD1289_REG_POWER_CTRL_4     0x0e
#define SSD1289_REG_GATE_SCAN_START  0x0f
#define SSD1289_REG_SLEEP_MODE       0x10
#define SSD1289_REG_ENTRY_MODE       0x11
#define SSD1289_REG_POWER_CTRL_5     0x1e
#define SSD1289_REG_GDDRAM_DATA      0x22
#define SSD1289_REG_WR_DATA_MASK_1   0x23
#define SSD1289_REG_WR_DATA_MASK_2   0x24
#define SSD1289_REG_FRAME_FREQUENCY  0x25
#define SSD1289_REG_GAMMA_CTRL_1     0x30
#define SSD1289_REG_GAMME_CTRL_2     0x31
#define SSD1289_REG_GAMMA_CTRL_3     0x32
#define SSD1289_REG_GAMMA_CTRL_4     0x33
#define SSD1289_REG_GAMMA_CTRL_5     0x34
#define SSD1289_REG_GAMMA_CTRL_6     0x35
#define SSD1289_REG_GAMMA_CTRL_7     0x36
#define SSD1289_REG_GAMMA_CTRL_8     0x37
#define SSD1289_REG_GAMMA_CTRL_9     0x3a
#define SSD1289_REG_GAMMA_CTRL_10    0x3b
#define SSD1289_REG_V_SCROLL_CTRL_1  0x41
#define SSD1289_REG_V_SCROLL_CTRL_2  0x42
#define SSD1289_REG_H_RAM_ADR_POS    0x44
#define SSD1289_REG_V_RAM_ADR_START  0x45
#define SSD1289_REG_V_RAM_ADR_END    0x46
#define SSD1289_REG_FIRST_WIN_START  0x48
#define SSD1289_REG_FIRST_WIN_END    0x49
#define SSD1289_REG_SECND_WIN_START  0x4a
#define SSD1289_REG_SECND_WIN_END    0x4b
#define SSD1289_REG_GDDRAM_X_ADDR    0x4e
#define SSD1289_REG_GDDRAM_Y_ADDR    0x4f

#define SCREENWIDTH                      TFT_WIDTH_UI16
#define SCREENHEIGHT                     TFT_HEIGHT_UI16

/* Hardware stuff */
#define SRAM0_BASE       (0x20000000U)
#define SRAM0_CTRL       (0x00000U)
#define SRAM0_DATA       (0x10000U)

#define MPMC_BASE        (0x17008000U)
#define MPMC_CONTROL     0x00
#define MPMC_STATUS      0x01
#define MPMC_STCONFIG0   0x80//(0x200U)
#define MPMC_STWTWEN0    0x81//(0x204U)
#define MPMC_STWTOEN0    0x82//(0x208U)
#define MPMC_STWTRD0     0x83//(0x20CU)
#define MPMC_STWTPG0     0x84//(0x210U)
#define MPMC_STWTWR0     0x85//(0x214U)
#define MPMC_STWTTURN0   0x86//(0x218U)
/* ***************************** enums ************************************** */

#endif /* TFT_CFG_H */
