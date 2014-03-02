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
#define SSD1963_NOP                   0x0000
#define SSD1963_SOFT_RESET            0x0001
#define SSD1963_GET_POWER_MODE        0x000A
#define SSD1963_GET_ADDRESS_MODE      0x000B
#define SSD1963_GET_PIXEL_FORMAT      0x000C
#define SSD1963_GET_DISPLAY_MODE      0x000D
#define SSD1963_GET_SIGNAL_MODE       0x000E
#define SSD1963_ENTER_SLEEP_MODE      0x0010
#define SSD1963_EXIT_SLEEP_MODE       0x0011
#define SSD1963_ENTER_PARTIAL_MODE    0x0012
#define SSD1963_ENTER_NORMAL_MODE     0x0013
#define SSD1963_EXIT_INVERT_MODE      0x0020
#define SSD1963_ENTER_INVERT_MODE     0x0021
#define SSD1963_SET_GAMMA_CURVE       0x0026
#define SSD1963_SET_DISPLAY_OFF       0x0028
#define SSD1963_SET_DISPLAY_ON        0x0029
#define SSD1963_SET_COLUMN_ADDRESS    0x002A
#define SSD1963_SET_PAGE_ADDRESS      0x002B
#define SSD1963_WRITE_MEMORY_START    0x002C
#define SSD1963_READ_MEMORY_START     0x002E
#define SSD1963_SET_PARTIAL_AERA      0x0030
#define SSD1963_SET_SCROLL_AREA       0x0033
#define SSD1963_SET_TEAR_OFF          0x0034
#define SSD1963_SET_TEAR_ON           0x0035
#define SSD1963_SET_ADDRESS_MODE      0x0036
#define SSD1963_SET_SCROLL_START      0x0037
#define SSD1963_EXIT_IDLE_MODE        0x0038
#define SSD1963_ENTER_IDLE_MODE       0x0039
#define SSD1963_SET_PIXEL_FORMAT      0x003A
#define SSD1963_WRITE_MEMORY_CONTINUE 0x003C
#define SSD1963_READ_MEMORY_CONTINUE  0x003E
#define SSD1963_SET_TEAR_SCNALINE     0x0044
#define SSD1963_GET_SCANLINE          0x0045
#define SSD1963_READ_DDB              0x00A1
#define SSD1963_SET_LCD_MODE          0x00B0
#define SSD1963_GET_LCD_MODE          0x00B1
#define SSD1963_SET_HORI_PERIOD       0x00B4
#define SSD1963_GET_HORI_PERIOD       0x00B5
#define SSD1963_SET_VERT_PERIOD       0x00B6
#define SSD1963_GET_VERT_PERIOD       0x00B7
#define SSD1963_SET_GPIO_CONF         0x00B8
#define SSD1963_GET_GPIO_CONF         0x00B9
#define SSD1963_SET_GPIO_VALUE        0x00BA
#define SSD1963_GET_GPIO_STATUS       0x00BB
#define SSD1963_SET_POST_PROC         0x00BC
#define SSD1963_GET_POST_PROC         0x00BD
#define SSD1963_SET_PWM_CONF          0x00BE
#define SSD1963_GET_PWM_CONF          0x00BF
#define SSD1963_SET_LCD_GEN0          0x00C0
#define SSD1963_GET_LCD_GEN0          0x00C1
#define SSD1963_SET_LCD_GEN1          0x00C2
#define SSD1963_GET_LCD_GEN1          0x00C3
#define SSD1963_SET_LCD_GEN2          0x00C4
#define SSD1963_GET_LCD_GEN2          0x00C5
#define SSD1963_SET_LCD_GEN3          0x00C6
#define SSD1963_GET_LCD_GEN3          0x00C7
#define SSD1963_SET_GPIO0_ROP         0x00C8
#define SSD1963_GET_GPIO0_ROP         0x00C9
#define SSD1963_SET_GPIO1_ROP         0x00CA
#define SSD1963_GET_GPIO1_ROP         0x00CB
#define SSD1963_SET_GPIO2_ROP         0x00CC
#define SSD1963_GET_GPIO2_ROP         0x00CD
#define SSD1963_SET_GPIO3_ROP         0x00CE
#define SSD1963_GET_GPIO3_ROP         0x00CF
#define SSD1963_SET_DBC_CONF          0x00D0
#define SSD1963_GET_DBC_CONF          0x00D1
#define SSD1963_SET_DBC_TH            0x00D4
#define SSD1963_GET_DBC_TH            0x00D5
#define SSD1963_SET_PLL               0x00E0
#define SSD1963_SET_PLL_MN            0x00E2
#define SSD1963_GET_PLL_MN            0x00E3
#define SSD1963_GET_PLL_STATUS        0x00E4
#define SSD1963_SET_DEEP_SLEEP        0x00E5
#define SSD1963_SET_LSHIFT_FREQ       0x00E6
#define SSD1963_GET_LSHIFT_FREQ       0x00E7
#define SSD1963_SET_PIXEL_DATA_INTERFACE 0x00F0
#define SSD1963_GET_PIXEL_DATA_INTERFACE 0x00F1
#define SCREENWIDTH                      480
#define SCREENHEIGHT                     272

/* Hardware stuff */
#define SRAM0_BASE       (0x20000000U)
#define SRAM0_CTRL       (0x00000U)
#define SRAM0_DATA       (0x18000U)

#define MPMC_BASE        (0x17008000U)
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
