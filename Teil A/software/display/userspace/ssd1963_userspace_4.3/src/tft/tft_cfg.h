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
#define ROTATE 0

#if !ROTATE
   #define TFT_WIDTH_UI16                    (uint16)480
   #define TFT_HEIGHT_UI16                   (uint16)272
#else
   #define TFT_WIDTH_UI16                    (uint16)272
   #define TFT_HEIGHT_UI16                   (uint16)480
#endif
#define TFT_ZERO_X                           (uint16)0
#define TFT_ZERO_Y                           (uint16)0
#define TFT_PIXELS_UI32                      (uint32)130560


#define TFT_BACKLIGHT_PIN_UI8             DIO_CHANNEL_20

#define TFT_RESET_PIN_UI8                 DIO_CHANNEL_19

#define HDP                         479           // 479
#define HT                          531           // 525
#define HPS                         43            // 40
#define LPS                         8             // 6
#define HPW                         10            // 39

#define VDP                         271           // 271
#define VT                          288           // 288
#define VPS                         12            // 12
#define FPS                         4             // 8
#define VPW                         10            // 11

/* TFT Controler Register defines */
/* TFT Controler Register defines */
#define SSD1963_NOP                   0x00
#define SSD1963_SOFT_RESET            0x01
#define SSD1963_GET_POWER_MODE        0x0A
#define SSD1963_GET_ADDRESS_MODE      0x0B
#define SSD1963_GET_PIXEL_FORMAT      0x0C
#define SSD1963_GET_DISPLAY_MODE      0x0D
#define SSD1963_GET_SIGNAL_MODE       0x0E
#define SSD1963_ENTER_SLEEP_MODE      0x10
#define SSD1963_EXIT_SLEEP_MODE       0x11
#define SSD1963_ENTER_PARTIAL_MODE    0x12
#define SSD1963_ENTER_NORMAL_MODE     0x13
#define SSD1963_EXIT_INVERT_MODE      0x20
#define SSD1963_ENTER_INVERT_MODE     0x21
#define SSD1963_SET_GAMMA_CURVE       0x26
#define SSD1963_SET_DISPLAY_OFF       0x28
#define SSD1963_SET_DISPLAY_ON        0x29
#define SSD1963_SET_COLUMN_ADDRESS    0x2A
#define SSD1963_SET_PAGE_ADDRESS      0x2B
#define SSD1963_WRITE_MEMORY_START    0x2C
#define SSD1963_READ_MEMORY_START     0x2E
#define SSD1963_SET_PARTIAL_AERA      0x30
#define SSD1963_SET_SCROLL_AREA       0x33
#define SSD1963_SET_TEAR_OFF          0x34
#define SSD1963_SET_TEAR_ON           0x35
#define SSD1963_SET_ADDRESS_MODE      0x36
#define SSD1963_SET_SCROLL_START      0x37
#define SSD1963_EXIT_IDLE_MODE        0x38
#define SSD1963_ENTER_IDLE_MODE       0x39
#define SSD1963_SET_PIXEL_FORMAT      0x3A
#define SSD1963_WRITE_MEMORY_CONTINUE 0x3C
#define SSD1963_READ_MEMORY_CONTINUE  0x3E
#define SSD1963_SET_TEAR_SCNALINE     0x44
#define SSD1963_GET_SCANLINE          0x45
#define SSD1963_READ_DDB              0xA1
#define SSD1963_SET_LCD_MODE          0xB0
#define SSD1963_GET_LCD_MODE          0xB1
#define SSD1963_SET_HORI_PERIOD       0xB4
#define SSD1963_GET_HORI_PERIOD       0xB5
#define SSD1963_SET_VERT_PERIOD       0xB6
#define SSD1963_GET_VERT_PERIOD       0xB7
#define SSD1963_SET_GPIO_CONF         0xB8
#define SSD1963_GET_GPIO_CONF         0xB9
#define SSD1963_SET_GPIO_VALUE        0xBA
#define SSD1963_GET_GPIO_STATUS       0xBB
#define SSD1963_SET_POST_PROC         0xBC
#define SSD1963_GET_POST_PROC         0xBD
#define SSD1963_SET_PWM_CONF          0xBE
#define SSD1963_GET_PWM_CONF          0xBF
#define SSD1963_SET_LCD_GEN0          0xC0
#define SSD1963_GET_LCD_GEN0          0xC1
#define SSD1963_SET_LCD_GEN1          0xC2
#define SSD1963_GET_LCD_GEN1          0xC3
#define SSD1963_SET_LCD_GEN2          0xC4
#define SSD1963_GET_LCD_GEN2          0xC5
#define SSD1963_SET_LCD_GEN3          0xC6
#define SSD1963_GET_LCD_GEN3          0xC7
#define SSD1963_SET_GPIO0_ROP         0xC8
#define SSD1963_GET_GPIO0_ROP         0xC9
#define SSD1963_SET_GPIO1_ROP         0xCA
#define SSD1963_GET_GPIO1_ROP         0xCB
#define SSD1963_SET_GPIO2_ROP         0xCC
#define SSD1963_GET_GPIO2_ROP         0xCD
#define SSD1963_SET_GPIO3_ROP         0xCE
#define SSD1963_GET_GPIO3_ROP         0xCF
#define SSD1963_SET_DBC_CONF          0xD0
#define SSD1963_GET_DBC_CONF          0xD1
#define SSD1963_SET_DBC_TH            0xD4
#define SSD1963_GET_DBC_TH            0xD5
#define SSD1963_SET_PLL               0xE0
#define SSD1963_SET_PLL_MN            0xE2
#define SSD1963_GET_PLL_MN            0xE3
#define SSD1963_GET_PLL_STATUS        0xE4
#define SSD1963_SET_DEEP_SLEEP        0xE5
#define SSD1963_SET_LSHIFT_FREQ       0xE6
#define SSD1963_GET_LSHIFT_FREQ       0xE7
#define SSD1963_SET_PIXEL_DATA_INTERFACE 0xF0
#define SSD1963_GET_PIXEL_DATA_INTERFACE 0xF1
#define SCREENWIDTH                      480
#define SCREENHEIGHT                     272

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
