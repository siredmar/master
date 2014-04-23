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
   #define TFT_WIDTH_UI16                    (uint16)800
   #define TFT_HEIGHT_UI16                   (uint16)480
#else
   #define TFT_WIDTH_UI16                    (uint16)480
   #define TFT_HEIGHT_UI16                   (uint16)800
#endif
#define TFT_ZERO_X                           (uint16)0
#define TFT_ZERO_Y                           (uint16)0
#define TFT_PIXELS_UI32                      (uint32)384000


#define TFT_BACKLIGHT_PIN_UI8             DIO_CHANNEL_20

#define TFT_RESET_PIN_UI8                 DIO_CHANNEL_19

#define HDP 799           // 479
#define HT  928            // 525
#define HPS 46            // 40
#define LPS 15            // 6
#define HPW 48            // 39

#define VDP 479           // 271
#define VT  525            // 288
#define VPS 16            // 12
#define FPS 8             // 8
#define VPW 16            // 11

/* TFT Controler Register defines */
#define MD050SD_SET_BACKLIGHT_PWM        (0x0001)
#define MD050SD_SET_LINE_ADDRESS_START   (0x0002)
#define MD050SD_SET_COLUMN_ADDRESS_START (0x0003)
#define MD050SD_DISPLAY_PAGE             (0x0004)
#define MD050SD_PAGE_RW                  (0x0005)
#define MD050SD_SET_LINE_ADDRESS_END     (0x0006)
#define MD050SD_SET_COLUMN_ADDRESS_END   (0x0007)
#define MD050SD_SET_SLEEP                (0x000B)
#define MD050SD_WRITE_MEMORY_START       (0x000F)






#define SCREENWIDTH                      800
#define SCREENHEIGHT                     480

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
