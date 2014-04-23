/* initialize.c

   written by Durgesh Pattamatta
   10 Oct 2008

   Copyright (C) 2008 NXP Semiconductors

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.


   -----------
   DESCRIPTION
   -----------

   Hardware initializations for the EA313x boards.  Some
   initializations may be left to drivers, such as the serial
   interface and timer.

 */

#include <config.h>
#include <asm/bootstrap.h>
#include <service.h>
#include <sdramboot.h>

#include "hardware.h"

#include <debug_ll.h>

//#define DEBUG_TIMEOUT 1000000       // 1s
//#define DEBUG_TIMEOUT 500000        // 0.5s
#define DEBUG_TIMEOUT 5000          // 5ms

//#if (CONFIG_USES_SSD1963)
#define SSD_PHYS        (EXT_SRAM0_PHYS)
#define SSD_PHYS_CTRL   (SSD_PHYS + 0)
#define SSD_PHYS_DATA   (SSD_PHYS + 0x8000)

#define LED_ENABLE_PIN  20
#define RESET_PIN       19
#define SSD1963_SCREENSIZE   130560      // 480*272
#define SSD1963_DEFAULT_COLOR 0x0000

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
//#endif
#define HDP                                 (479U)           // 479
#define HT                                  (531U)           // 525
#define HPS                                 (43U)            // 40
#define LPS                                 (8U)             // 6
#define HPW                                 (10U)            // 39

#define VDP                                 (271U)           // 271
#define VT                                  (288U)           // 288
#define VPS                                 (12U)            // 12
#define FPS                                 (4U)             // 8
#define VPW                                 (10U)            // 11


//#if (CONFIG_USES_SSD1963)



struct ssd1963 {
   volatile u16* ctrl;
   volatile u16* data;
};

static struct ssd1963 ssd1963;


static void ssd1963_send_cmd_slow(u16 cmd)
{
   *ssd1963.ctrl = cmd;
   udelay(DEBUG_TIMEOUT);
}

static void ssd1963_send_data_slow(u16 data)
{
   *ssd1963.data = data;
   udelay(DEBUG_TIMEOUT);
}

static u16 ssd1963_read_data_slow()
{
   udelay(DEBUG_TIMEOUT);
   return *ssd1963.ctrl;
}

static void ssd1963_send_cmd(u16 cmd)
{
   *ssd1963.ctrl = 0x00FF & cmd;
   //udelay(1000);
}

static void ssd1963_send_data(u16 data)
{
   *ssd1963.data = data;
   //udelay(1000);
}


static void ssd1963_set_window(u16 xs, u16 xe, u16 ys, u16 ye)
{
   ssd1963_send_cmd(SSD1963_SET_COLUMN_ADDRESS);
   *ssd1963.data = ((xs >> 8) & 0xFF);
   *ssd1963.data = (xs & 0xFF);
   *ssd1963.data = ((xe  >> 8 ) & 0xFF);
   *ssd1963.data = (xe & 0xFF);

   ssd1963_send_cmd(SSD1963_SET_PAGE_ADDRESS);
   *ssd1963.data = ((ys >> 8) & 0xFF);
   *ssd1963.data = (ys & 0xFF);
   *ssd1963.data = ((ye  >> 8 ) & 0xFF);
   *ssd1963.data = (ye & 0xFF);

   ssd1963_send_cmd(SSD1963_WRITE_MEMORY_START);
}

void ssd1963_draw_rectangle(u16 xs, u16 ys, u16 width, u16 height, u16 color)
{
   u32 cnt;
   u16 xe;
   u16 ye;
   u32 size;

   xe = xs + width;
   ye = ys + height;

//   if(xe > (SCREENWIDTH - 1))
//   {
//      xe = SCREENWIDTH - 1;
//   }
//   if(ye > (SCREENHEIGHT - 1))
//   {
//      ye = SCREENHEIGHT - 1;
//   }
   size = width * height;
   ssd1963_set_window(xs, xe, ys, ye);
   ssd1963_send_cmd(SSD1963_WRITE_MEMORY_START);
   for(cnt = 0; cnt < size; cnt++)
   {
//      *ssd1963.data = ((color >> 9) & 0xF8);
//      *ssd1963.data = ((color >> 3) & 0xFC);
//      *ssd1963.data = ((color << 3) & 0xF8);

      *ssd1963.data = (color);
   }


}


//static void ssd1963_init_pll (void)
//{
//
//
//   GPIO_OUT_LOW(IOCONF_GPIO, _BIT(13)); //GPIO19 is nRESET
//   udelay(1000);
//   GPIO_OUT_HIGH(IOCONF_GPIO, _BIT(13)); //GPIO19 is nRESET
//   udelay(1000);
//
//   GPIO_OUT_LOW(IOCONF_GPIO, _BIT(14)); //GPIO20 is LED_ENABLE
//
//   ssd1963_send_cmd_slow(SSD1963_SOFT_RESET);
//   udelay(200000);
//
//
//   ssd1963_send_cmd_slow(SSD1963_SET_PLL); // PLL config - continued
//   ssd1963_send_data_slow(0x0001);
//
//   ssd1963_send_cmd_slow(SSD1963_SET_PLL); // PLL config - continued
//   ssd1963_send_data_slow(0x0001);
//   udelay(9000);
//
//   ssd1963_send_cmd_slow(SSD1963_SET_PLL_MN); // PLL config - continued
//   //udelay(2000);
//   ssd1963_send_data_slow(0x001D);
//   //udelay(2000);
//   ssd1963_send_data_slow(0x0002);
//   //udelay(2000);
//   ssd1963_send_data_slow(0x0054);
//   udelay(100000);
//
//   do
//   {
//      ssd1963_send_cmd_slow(SSD1963_GET_PLL_STATUS);
//   } while (ssd1963_read_data_slow()!=0x04);
//
//
//   ssd1963_send_cmd_slow(SSD1963_SET_PLL); // PLL config - continued
//   ssd1963_send_data_slow(0x0003);
//   udelay(50000);
//}

static void ssd1963_init (void)
{


   u32 cnt;
   static u16 color = 0;
   //   /* LCD display, 16 bit */
   //   MPMC_STCONFIG0 = 0x81;
   //   MPMC_STWTWEN0  = 0;
   //   MPMC_STWTOEN0  = 0;
   //   MPMC_STWTRD0   = 31;
   //   MPMC_STWTPG0   = 0;
   //   MPMC_STWTWR0   = 3;
   //   MPMC_STWTTURN0 = 0;



   *ssd1963.ctrl = (SSD1963_SET_LSHIFT_FREQ);          udelay(DEBUG_TIMEOUT);
   *ssd1963.data = (0x01);                             udelay(DEBUG_TIMEOUT);
   *ssd1963.data = (0x70);                             udelay(DEBUG_TIMEOUT);
   *ssd1963.data = (0xA2);                             udelay(DEBUG_TIMEOUT);

   *ssd1963.ctrl = (SSD1963_SET_LCD_MODE);             udelay(DEBUG_TIMEOUT);
   udelay(100);
   *ssd1963.data = (0x38);                             udelay(DEBUG_TIMEOUT);
   *ssd1963.data = (0x00);                             udelay(DEBUG_TIMEOUT);
   *ssd1963.data = (0x01);                             udelay(DEBUG_TIMEOUT);
   *ssd1963.data = (0xDF);                             udelay(DEBUG_TIMEOUT);
   *ssd1963.data = (0x01);                             udelay(DEBUG_TIMEOUT);
   *ssd1963.data = (0x0F);                             udelay(DEBUG_TIMEOUT);
   *ssd1963.data = (0x00);                             udelay(DEBUG_TIMEOUT);
   udelay(5000);

   *ssd1963.ctrl = (SSD1963_SET_HORI_PERIOD);         udelay(DEBUG_TIMEOUT);
   *ssd1963.data = (((HT >> 8U) & 0x00FF));           udelay(DEBUG_TIMEOUT);
   *ssd1963.data = ((HT & 0x00FF));                   udelay(DEBUG_TIMEOUT);
   *ssd1963.data = (((HPS >> 8U) & 0x00FF));          udelay(DEBUG_TIMEOUT);
   *ssd1963.data = ((HPS & 0x00FF));                  udelay(DEBUG_TIMEOUT);
   *ssd1963.data = (HPW);                             udelay(DEBUG_TIMEOUT);
   *ssd1963.data = (((LPS >> 8U) & 0x00FF));          udelay(DEBUG_TIMEOUT);
   *ssd1963.data = ((LPS & 0x00FF));                  udelay(DEBUG_TIMEOUT);
   *ssd1963.data = (0x0000);

   *ssd1963.ctrl = (SSD1963_SET_VERT_PERIOD);         udelay(DEBUG_TIMEOUT);
   *ssd1963.data = (((VT >> 8U) & 0x00FF));           udelay(DEBUG_TIMEOUT);
   *ssd1963.data = ((VT & 0x00FF));                   udelay(DEBUG_TIMEOUT);
   *ssd1963.data = ((VPS >> 8U) & 0x00FF);            udelay(DEBUG_TIMEOUT);
   *ssd1963.data = ((VPS & 0x00FF));                  udelay(DEBUG_TIMEOUT);
   *ssd1963.data = (VPW);                             udelay(DEBUG_TIMEOUT);
   *ssd1963.data = (((FPS >> 8U) & 0x00FF));          udelay(DEBUG_TIMEOUT);
   *ssd1963.data = ((FPS & 0x00FF));                  udelay(DEBUG_TIMEOUT);

   *ssd1963.ctrl = (SSD1963_SET_ADDRESS_MODE);        udelay(DEBUG_TIMEOUT);
   *ssd1963.data = (0x00);                            udelay(DEBUG_TIMEOUT);

   *ssd1963.ctrl = (SSD1963_SET_PIXEL_DATA_INTERFACE);  udelay(DEBUG_TIMEOUT);
   *ssd1963.data = (0x03);                              udelay(DEBUG_TIMEOUT);
   udelay(5000);

   *ssd1963.ctrl = (SSD1963_SET_DISPLAY_ON);          udelay(DEBUG_TIMEOUT);

   *ssd1963.ctrl = (SSD1963_SET_PWM_CONF);            udelay(DEBUG_TIMEOUT);
   *ssd1963.data = (0x06);                            udelay(DEBUG_TIMEOUT);
   *ssd1963.data = (0xF0);                            udelay(DEBUG_TIMEOUT);
   *ssd1963.data = (0x01);                            udelay(DEBUG_TIMEOUT);
   *ssd1963.data = (0xF0);                            udelay(DEBUG_TIMEOUT);
   *ssd1963.data = (0x00);                            udelay(DEBUG_TIMEOUT);
   *ssd1963.data = (0x00);                            udelay(DEBUG_TIMEOUT);

   *ssd1963.ctrl = (SSD1963_SET_DBC_CONF);            udelay(DEBUG_TIMEOUT);
   *ssd1963.data = (0x0D);                            udelay(DEBUG_TIMEOUT);

   *ssd1963.ctrl = (SSD1963_SET_GPIO_CONF);          udelay(DEBUG_TIMEOUT);
   *ssd1963.data = (0x00);                           udelay(DEBUG_TIMEOUT);
   *ssd1963.data = (0x00);

   *ssd1963.ctrl = (SSD1963_SET_GPIO_VALUE);         udelay(DEBUG_TIMEOUT);
   *ssd1963.data = (0x00);                           udelay(DEBUG_TIMEOUT);

  // GPIO_OUT_HIGH(IOCONF_GPIO, _BIT(14)); //GPIO20 is LED_ENABLE

   ssd1963_draw_rectangle(0,0,200,200, 0xAAAA);

}


//{fin_select; ndec; mdec; pdec; selr; seli; selp; mode; freq;}
#if (CONFIG_OSC_FREQ == 24000000)
#define PLL_180M  {CGU_FIN_SELECT_FFAST, 514,  8191, 98, 0, 16,  8, 0, 180000000}
#define PLL_270M  {CGU_FIN_SELECT_FFAST,   2, 19660, 98, 0, 48, 23, 0, 180000000}
#else
#define PLL_90M   {CGU_FIN_SELECT_FFAST, 770,  8191, 66, 0, 16,  8, 0, 90000000}
#define PLL_180M  {CGU_FIN_SELECT_FFAST, 770,  8191, 98, 0, 16,  8, 0, 180000000}
#define PLL_270M  {CGU_FIN_SELECT_FFAST, 514, 19660, 98, 0, 48, 23, 0, 270000000}
#endif

#if defined(CONFIG_PLL_180)

#define CUR_PLL_SET     PLL_180M
#define HCLK          (90 * 1000000ull)
#define PLLCLK        (180 * 1000000ull)

extern const CGU_CLKS_INIT_T g_cgu_default_clks_2;
#define DEFAULT_CFG   &g_cgu_default_clks_2

#elif defined(CONFIG_PLL_270)

#if defined(CONFIG_HAS_ANALOG_DIE)
#warning "####!!!@@##@@!!! LPC3152 can't run at this speed. ####!!!@@##@@!!!\n" 
#endif

#define CUR_PLL_SET     PLL_270M
#define HCLK          (90 * 1000000ull)
#define PLLCLK        (270 * 1000000ull)

extern const CGU_CLKS_INIT_T g_cgu_default_clks_3;
#define DEFAULT_CFG   &g_cgu_default_clks_3

#endif


#if defined(CONFIG_RAM_SIZE_8MB)
#define SDRAM_CHIP_MODE   _SBF(13, 0x23)
#elif defined(CONFIG_RAM_SIZE_64MB)
#define SDRAM_CHIP_MODE   _SBF(13, 0x23)
#else
#define SDRAM_CHIP_MODE   _SBF(12, 0x23)
#endif
const CGU_HPLL_SETUP_T const g_pll = CUR_PLL_SET;

void __naked __section (.bootstrap.early) bootstrap_early (void)
                  {
   /* Initialize startup stack pointer at end of ISRAM0*/
   __asm volatile ("mov sp, %0" :: "r" (ISRAM1_PHYS));

   /* Map first 4KB of ARM space to ISRAM area. ie 0x0 - 0x1000 = 0x11028000 - 0x11029000 */
   SYS_REMAP_ADDR = ISRAM0_PHYS;

   __asm volatile ("b bootstrap_early_exit");
                  }

void __naked __section (.bootstrap.early.exit) bootstrap_early_exit (void)
                  {
                  }

/* usleep

   this function accepts a count of microseconds and will wait at
   least that long before returning.  The timer clock must be
   activated by the initialization code before using usleep.

   When in C, this function was one instruction longer than the
   hand-coded assembler.  For some reason, the compiler would reload
   the TIMER1_PHYS at the top of the while loop.

   Note that this function is neither __naked nor static.  It is
   available to the rest of the application as is.

   Keep in mind that it has a limit of about 357913.94125ms.  

   This routine assumes 12MHz clock is used as source clock for timer
   block. Which means 12 ticks for every usec.

 */

void __section (.bootstrap) usleep (unsigned long us)
{
   unsigned long end, current;
   end = (us << 2) + (us << 3);
   __REG (TIMER1_PHYS + TIMER_CONTROL) = TM_CTRL_ENABLE;
   __REG (TIMER1_PHYS + TIMER_LOAD) = end;

   do {
      current = __REG (TIMER1_PHYS + TIMER_VALUE);
   } while( current <= end);

}


/* initialize_bootstrap

   performs vital SDRAM initialization as well as some other memory
   controller initializations.  It will perform no work if we are
   already running from SDRAM.

   The assembly output of this implementation of the initialization is
   more dense than the assembler version using a table of
   initializations.  This is primarily due to the compiler's ability
   to keep track of the register set offsets and value being output.

   The return value is true if SDRAM has been initialized and false if
   this initialization has already been performed.  Note that the
   non-SDRAM initializations are performed regardless of whether or
   not we're running in SDRAM.


 */

void __naked __section (.bootstrap) initialize_bootstrap (void)
{
   unsigned long lr;
   __asm volatile ("mov %0, lr" : "=r" (lr));

   /* init CGU block*/
   cgu_init();
   /* reset all clocks to XTAL clock */
   cgu_reset_all_clks();

   /* set all clocks based on the default structure */
   cgu_init_clks(DEFAULT_CFG);
#if defined(CONFIG_HAS_ANALOG_DIE)
   /* before bumping up the frquency adjust core & IO voltages*/
   analog_reg_init();
   psu_set_voltage(ANALOG_1V2_RAIL, DCDC2_1_40);
   /* Set SDRAM voltage rail to 1.8V default is 1.4V */
   psu_set_voltage(ANALOG_1V8_RAIL, PSU_VOUT3_1_80);
   //psu_set_voltage(ANALOG_3V3_RAIL, DCDC1_3_20);
#endif
   /* set HPLL1 - main PLL to default speed */
   cgu_hpll_config (CGU_HPLL1_ID, &g_pll);


   /* configure the LCD pins in EBI memory mode. */
   SYS_MUX_LCD_EBI = 1;

   cgu_clk_en_dis( CGU_SB_TIMER1_PCLK_ID, 1);
   /* enable EBI clock */
   cgu_clk_en_dis( CGU_SB_EBI_CLK_ID, 1);

   /* enable MPMC controller clocks */
   cgu_clk_en_dis( CGU_SB_MPMC_CFG_CLK_ID, 1);
   cgu_clk_en_dis( CGU_SB_MPMC_CFG_CLK2_ID, 1);
   cgu_clk_en_dis( CGU_SB_MPMC_CFG_CLK3_ID, 1);
   cgu_clk_en_dis( CGU_SB_IOCONF_PCLK_ID, 1);

   /* enable External Memory controller */
   MPMC_CTRL = MPMC_CTL_ENABLE;
   /* Force HCLK to MPMC_CLK to 1:1 ratio */
   MPMC_CONFIG = MPMC_CFG_SDCCLK_1_1;
   /* set MPMC delay gates appropriately based on trace lengths between
  SDRAM and the chip. Also based on the delay startergy used for SDRAM. */
   SYS_MPMC_DELAY = 0x826;

   /* check the jumpers are set for EBI mode or not before proceeding
  further.*/
#if defined(CONFIG_MACH_VAL3153)
   /* NOR flash, 16 bit */
   MPMC_STCONFIG0  = 0x81;
   MPMC_STWTWEN0   = 1;
   MPMC_STWTOEN0   = 1;
   MPMC_STWTRD0    = 6;
   MPMC_STWTPG0    = 2;
   MPMC_STWTWR0    = 6;
   MPMC_STWTTURN0  = 1;
   /* ethernet, 16 bit */
   MPMC_STCONFIG1  = 0x81;
   MPMC_STWTWEN1   = 4;
   MPMC_STWTOEN1   = 4;
   MPMC_STWTRD1    = 18;
   MPMC_STWTPG1    = 0xFF;
   MPMC_STWTWR1    = 16;
   MPMC_STWTTURN1  = 6;
   /* enable oe toggle between consecutive reads */
   SYS_MPMC_WTD_DEL0 = _BIT(5) | 6;
   SYS_MPMC_WTD_DEL1 = _BIT(5) | 18;
#elif defined(CONFIG_MACH_EA313x_V1) || defined(CONFIG_MACH_EA313x_V2) || defined(CONFIG_MACH_EA3152)
   /* LCD display, 16 bit */
   MPMC_STCONFIG0  = 0x81;
   MPMC_STWTWEN0   = 2;
   MPMC_STWTOEN0   = 1;
   MPMC_STWTRD0    = 15;
   MPMC_STWTPG0    = 2;
   MPMC_STWTWR0    = 4;
   MPMC_STWTTURN0  = 2;
   /* ethernet DM9000, 16 bit */
   MPMC_STCONFIG1  = 0x81;
   MPMC_STWTWEN1   = 1;
   MPMC_STWTOEN1   = 1;
   MPMC_STWTRD1    = 4;
   MPMC_STWTPG1    = 1;
   MPMC_STWTWR1    = 1;
   MPMC_STWTTURN1  = 2;
   /* enable oe toggle between consecutive reads */
   SYS_MPMC_WTD_DEL0 = _BIT(5) | 15;
   SYS_MPMC_WTD_DEL1 = _BIT(5) | 4;
#elif defined(CONFIG_MACH_EPLPC3131_V1)
   /* LCD display, 16 bit */
   MPMC_STCONFIG0  = 0x81;
   MPMC_STWTWEN0   = 10;
   MPMC_STWTOEN0   = 0;
   MPMC_STWTRD0    = 31;
   MPMC_STWTPG0    = 0;
   MPMC_STWTWR0    = 31;
   MPMC_STWTTURN0  = 0;

#endif
   SYS_MPMC_MISC = 0x100;
   /* SDRAM */
   MPMC_DYRDCFG    = MPMC_SDRAMC_RDCFG_CMDDELAY_STG;



#if defined(CONFIG_MACH_EPLPC3131_V1) && defined(CONFIG_RAM_SIZE_8MB)
   /* FIXME: At this point I must use SDRAMC_16HP_32Mx16_4B_R13_C10.
     In my view the correct value would be SDRAMC_16LP_4Mx16_4B_R12_C8,
     but it does not work.
    */
   MPMC_DYCONFIG   =   SDRAMC_16HP_32Mx16_4B_R13_C10;

#elif defined(CONFIG_MACH_EPLPC3131_V1) && defined(CONFIG_RAM_SIZE_32MB)

   MPMC_DYCONFIG   =    SDRAMC_16HP_16Mx16_4B_R13_C9;

#elif defined(CONFIG_MACH_EPLPC3131_V1) && defined(CONFIG_RAM_SIZE_64MB)

   MPMC_DYCONFIG   =    SDRAMC_16HP_32Mx16_4B_R13_C10;

#else
   MPMC_DYCONFIG   =   SDRAMC_16HP_32Mx16_4B_R13_C10
#endif



   MPMC_DYRASCAS   = MPMC_SDRAMC_RASCAS_RAS2 | MPMC_SDRAMC_RASCAS_CAS2;
   /* min 20ns program 1 so that atleast 2 HCLKs are used */
   MPMC_DYTRP    = NS_TO_MPMCCLK(20, HCLK);
   MPMC_DYTRAS   = NS_TO_MPMCCLK(44, HCLK);
   MPMC_DYTSREX  = NS_TO_MPMCCLK(75, HCLK);
   MPMC_DYTAPR   = 4;
   MPMC_DYTDAL   = NS_TO_MPMCCLK(50, HCLK);
   MPMC_DYTWR    = NS_TO_MPMCCLK(75, HCLK);
   MPMC_DYTRC    = NS_TO_MPMCCLK(66, HCLK);
   MPMC_DYTRFC   = NS_TO_MPMCCLK(66, HCLK);
   MPMC_DYTXSR   = NS_TO_MPMCCLK(75, HCLK);
   MPMC_DYTRRD   = NS_TO_MPMCCLK(15, HCLK);
   MPMC_DYTMRD   = NS_TO_MPMCCLK(20, HCLK);

   usleep (100);
   /* issue continuous NOP commands  */
   MPMC_DYNCTL    = MPMC_SDRAMC_CTL_CE | MPMC_SDRAMC_CTL_CS | MPMC_SDRAMC_CTL_NOP_CMD;
   usleep (200);
   /* issue a "pre-charge all" command */
   MPMC_DYNCTL    = MPMC_SDRAMC_CTL_CE | MPMC_SDRAMC_CTL_CS | MPMC_SDRAMC_CTL_PALL_CMD;
   MPMC_DYNREF     = NS_TO_MPMCCLK(100, HCLK)/16 + 1;
   usleep (250);
   MPMC_DYNREF     = NS_TO_MPMCCLK(7812, HCLK)/16;
   /* select mode register update mode */
   MPMC_DYNCTL    = MPMC_SDRAMC_CTL_CE | MPMC_SDRAMC_CTL_CS | MPMC_SDRAMC_CTL_MODE_CMD;

   /* set mode register by reading the address */
   __REG (EXT_SDRAM_PHYS + SDRAM_CHIP_MODE);

#if defined(CONFIG_MACH_EPLPC3131_V1) && defined(CONFIG_RAM_SIZE_8MB)
   MPMC_DYCONFIG   =   SDRAMC_16LP_4Mx16_4B_R12_C8;
#elif defined(CONFIG_MACH_EPLPC3131_V1) && defined(CONFIG_RAM_SIZE_32MB)
   MPMC_DYCONFIG   =    SDRAMC_16HP_16Mx16_4B_R13_C9;
#elif defined(CONFIG_MACH_EPLPC3131_V1) && defined(CONFIG_RAM_SIZE_64MB)
   MPMC_DYCONFIG   =    SDRAMC_16HP_32Mx16_4B_R13_C10;
#else
   MPMC_DYCONFIG   =  SDRAMC_16HP_32Mx16_4B_R13_C10;
#endif

   /* select normal operating mode */
   MPMC_DYNCTL    = MPMC_SDRAMC_CTL_CE | MPMC_SDRAMC_CTL_CS | MPMC_SDRAMC_CTL_NORMAL_CMD;

   /* enable buffers */
   MPMC_DYCONFIG |= MPMC_SDRAMC_CFG_BUF_EN;
   MPMC_DYNCTL = MPMC_SDRAMC_CTL_NORMAL_CMD | MPMC_SDRAMC_CTL_CS;

#if defined(CONFIG_DYNAMIC_CLOCKS)
   /* program dynamic referesh clock for external refresh generator also.
  Used during dynamic clock scaling done by CGU. */
   SYS_MPMC_TESTMODE0 = (NS_TO_MPMCCLK(7812, PLLCLK)/16) | _BIT(12);
   /*testMode1 = (10+ Trp/(1/MPMC_CLK) + Trfc/(1/MPMC_CLK)) * baseclock/ MPMC_clk */
   SYS_MPMC_TESTMODE1 = (10 + NS_TO_MPMCCLK(20, HCLK) + NS_TO_MPMCCLK(66, HCLK)) * (PLLCLK/HCLK);
#endif
   ssd1963.ctrl = &__REG16 (SSD_PHYS_CTRL);   //&__REG16
   ssd1963.data = &__REG16 (SSD_PHYS_DATA); //&__REG16
   // *ssd1963.ctrl = (SSD1963_SOFT_RESET); udelay(DEBUG_TIMEOUT);
   //   while(1){
   //
   //         *ssd1963.ctrl = (0xE0);             udelay(DEBUG_TIMEOUT);
   //         *ssd1963.data = (0x01);             udelay(DEBUG_TIMEOUT);
   //         *ssd1963.ctrl = (0xE0);             udelay(DEBUG_TIMEOUT);
   //         *ssd1963.data = (0x01);             udelay(DEBUG_TIMEOUT);
   //         *ssd1963.ctrl = (0xE2);             udelay(DEBUG_TIMEOUT);
   //         *ssd1963.data = (0x1D);             udelay(DEBUG_TIMEOUT);
   //         *ssd1963.data = (0x02);             udelay(DEBUG_TIMEOUT);
   //         *ssd1963.data = (0x54);             udelay(2000000);
   //      }
   //        ssd1963_init_pll();
   //

   *ssd1963.ctrl = (SSD1963_SOFT_RESET);        udelay(DEBUG_TIMEOUT);

   *ssd1963.ctrl = (SSD1963_SET_PLL);           udelay(DEBUG_TIMEOUT);
   *ssd1963.data = (0x01);                    udelay(DEBUG_TIMEOUT);

   *ssd1963.ctrl = (SSD1963_SET_PLL_MN);        udelay(DEBUG_TIMEOUT);
   *ssd1963.data = (0x1D);                    udelay(DEBUG_TIMEOUT);
   *ssd1963.data = (0x02);                    udelay(DEBUG_TIMEOUT);
   *ssd1963.data = (0x54);                    udelay(DEBUG_TIMEOUT);

   *ssd1963.ctrl = (SSD1963_SET_PLL);           udelay(DEBUG_TIMEOUT);
   *ssd1963.data = (0x03);                    udelay(DEBUG_TIMEOUT);


   ssd1963_init();
   //   do
   //   {
   //      *ssd1963.ctrl = (SSD1963_GET_PLL_STATUS);
   //      udelay(1000);
   //   } while (*ssd1963.data != 0x04);

   ssd1963_draw_rectangle(50,50,50,50, 0xFFFF);
   ssd1963_draw_rectangle(100,100,50,50, 0x00FF);

   udelay(DEBUG_TIMEOUT*10);


   __asm volatile ("mov r0, #-1\t\n"
         "mov pc, %0" : : "r" (lr));

   ssd1963_draw_rectangle(150,150,50,50, 0x0FF0);

}


/* target_init

   performs the rest of the hardware initialization that didn't have
   to be performed during the bootstrap phase.

 */

static void target_init (void)
{
   ssd1963_draw_rectangle(25,25,50,50, 0x4321);
}

static __service_0 struct service_d lpc313x_target_service = {
      .init    = target_init,
};
