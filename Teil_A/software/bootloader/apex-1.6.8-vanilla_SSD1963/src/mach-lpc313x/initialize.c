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

#if (CONFIG_USES_SSD1963)
#define SSD_PHYS        (EXT_SRAM0_PHYS)
#define SSD_PHYS_CTRL   (SSD_PHYS + 0)
#define SSD_PHYS_DATA   (SSD_PHYS + 0x10000)

#define SSD1963_SOFT_RESET            0x0001
#define SSD1963_SET_PLL               0x00E0
#define SSD1963_SET_PLL_MN            0x00E2


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

#endif

//{fin_select; ndec; mdec; pdec; selr; seli; selp; mode; freq;}
#if (CONFIG_OSC_FREQ == 24000000)
#define PLL_180M  {CGU_FIN_SELECT_FFAST, 514,  8191, 98, 0, 16,  8, 0, 180000000}
#define PLL_270M  {CGU_FIN_SELECT_FFAST,   2, 19660, 98, 0, 48, 23, 0, 180000000}
#else
#define PLL_24M   {CGU_FIN_SELECT_FFAST, 770,  1023, 10, 0, 16,  7, 0, 24000000}
#define PLL_60M   {CGU_FIN_SELECT_FFAST, 770,  8191,  1, 0, 16,  8, 0, 60000000}
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

#elif defined(CONFIG_PLL_90)

#define CUR_PLL_SET     PLL_60M
#define HCLK          (30 * 1000000ull)
#define PLLCLK        (60 * 1000000ull)

extern const CGU_CLKS_INIT_T g_cgu_default_clks_1;
#define DEFAULT_CFG   &g_cgu_default_clks_1

#endif

#if(CONFIG_USES_SSD1963)
struct ssd1963 {
   volatile u16* ctrl;
   volatile u16* data;
};


static struct ssd1963 ssd1963;

static void ssd1963_send_cmd(u16 cmd)
{
   *ssd1963.ctrl = 0x00FF & cmd;
}

static void ssd1963_send_data(u16 data)
{
   *ssd1963.data = data;
}

u16 ssd1963_read_data(void)
{
   usleep(1000);
   return *ssd1963.data;
}

static void ssd1963_init_pll(void)
{
   int counter;

   ssd1963.ctrl = &__REG16 (SSD_PHYS_CTRL);   //&__REG16
   ssd1963.data = &__REG16 (SSD_PHYS_DATA); //&__REG16

   GPIO_OUT_LOW(IOCONF_GPIO, _BIT(13)); //GPIO19 is nRESET
   udelay(20000);
   GPIO_OUT_HIGH(IOCONF_GPIO, _BIT(13)); //GPIO19 is nRESET
   udelay(20000);

   GPIO_OUT_LOW(IOCONF_GPIO, _BIT(14)); //GPIO20 is LED_ENABLE

   ssd1963_send_cmd(SSD1963_SOFT_RESET);
   udelay(20000);

   ssd1963_send_cmd(SSD1963_SET_PLL); // PLL config - continued
   udelay(20000);
   ssd1963_send_data(0x0001);
   udelay(20000);

   ssd1963_send_cmd(SSD1963_SET_PLL_MN); // PLL config - continued
   udelay(20000);
   ssd1963_send_data(0x0023);
   udelay(20000);
   ssd1963_send_data(0x0002);
   udelay(20000);
   ssd1963_send_data(0x0054);
   udelay(20000);

   ssd1963_send_cmd(SSD1963_SET_PLL); // PLL config - continued
   udelay(20000);
   ssd1963_send_data(0x0003);
   udelay(20000);

   do
   {
      ssd1963_send_cmd(SSD1963_GET_PLL_STATUS);
   } while (ssd1963_read_data() != 0x04);

   ssd1963_send_cmd(SSD1963_SOFT_RESET);
   udelay(20000);

   ssd1963_send_cmd(SSD1963_SET_LSHIFT_FREQ);
   ssd1963_send_data(0x0003); // LSHIFT freq
   ssd1963_send_data(0x00FF);
   ssd1963_send_data(0x00FF);

   ssd1963_send_cmd(SSD1963_SET_LCD_MODE);
   udelay(100);
   ssd1963_send_data(0x00B0);     //LCD SPECIFICATION
   ssd1963_send_data(0x0027);
   ssd1963_send_data(0x0000);
   ssd1963_send_data((HDP>>8)&0x00FF);  //Set HDP
   ssd1963_send_data(HDP&0x00FF);
   ssd1963_send_data((VDP>>8)&0x00FF);  //Set VDP
   ssd1963_send_data(VDP & 0x00FF);
   ssd1963_send_data(0x0000);
   udelay(5000);

   ssd1963_send_cmd(SSD1963_SET_HORI_PERIOD);
   ssd1963_send_data(((HT >> 8U) & 0x00FF));  // horizontal period
   ssd1963_send_data((HT & 0x00FF));
   ssd1963_send_data(((HPS >> 8U) & 0x00FF));
   ssd1963_send_data((HPS & 0x00FF));
   ssd1963_send_data(HPW);
   ssd1963_send_data(((LPS >> 8U) & 0x00FF));
   ssd1963_send_data((LPS & 0x00FF));
   ssd1963_send_data(0x0000);

   ssd1963_send_cmd(SSD1963_SET_VERT_PERIOD);
   ssd1963_send_data(((VT >> 8U) & 0x00FF)); // vertical period
   ssd1963_send_data((VT & 0x00FF));
   ssd1963_send_data((VPS >> 8U) & 0x00FF);
   ssd1963_send_data((VPS & 0x00FF));
   ssd1963_send_data(VPW);
   ssd1963_send_data(((FPS >> 8U) & 0x00FF));
   ssd1963_send_data((FPS & 0x00FF));





   ssd1963_send_cmd(SSD1963_SET_PWM_CONF);
   ssd1963_send_data(0x0006);  // pwm config
   ssd1963_send_data(0x00F0);
   ssd1963_send_data(0x0001);
   ssd1963_send_data(0x00F0);
   ssd1963_send_data(0x0000);
   ssd1963_send_data(0x0000);

//   ssd1963_send_cmd(SSD1963_SET_DBC_CONF);
//   ssd1963_send_data(0x000D); // pwm coffig continued


   ssd1963_send_cmd(SSD1963_SET_GPIO_VALUE);
      ssd1963_send_data(0x000F); // gpio to 0x00

   ssd1963_send_cmd(SSD1963_SET_GPIO_CONF);
   ssd1963_send_data(0x0007); // gpio config
   ssd1963_send_data(0x0001);

   ssd1963_send_cmd(SSD1963_SET_COLUMN_ADDRESS);
   ssd1963_send_data(0);
   ssd1963_send_data(0);
   ssd1963_send_data((800 >> 8 ) & 0x00FF);
   ssd1963_send_data(800 & 0x00FF);

   ssd1963_send_cmd(SSD1963_SET_PAGE_ADDRESS);
   ssd1963_send_data(0);
   ssd1963_send_data(0);
   ssd1963_send_data((480  >> 8 ) & 0x00FF);
   ssd1963_send_data(480 & 0x00FF);

   ssd1963_send_cmd(SSD1963_SET_ADDRESS_MODE);
   ssd1963_send_data(0x0000);  // rotation - landscape

   ssd1963_send_cmd(SSD1963_SET_PIXEL_DATA_INTERFACE);
   ssd1963_send_data(0x0003);  // pixel data interface 16 bit    0b011 is 16 Bit
   udelay(5000);

   ssd1963_send_cmd(SSD1963_WRITE_MEMORY_START);

   for(counter = 0; counter < 800*480 ; counter++)
      ssd1963_send_data(0x1234);

   ssd1963_send_cmd(SSD1963_SET_DISPLAY_ON); // set display on
}
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
//   cgu_clk_en_dis( CGU_SB_IOCONF_PCLK_ID, 1);

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
   MPMC_STWTWEN0   = 1;
   MPMC_STWTOEN0   = 0;
   MPMC_STWTRD0    = 31;
   MPMC_STWTPG0    = 0;
   MPMC_STWTWR0    = 3;
   MPMC_STWTTURN0  = 0;

#endif

#if(CONFIG_USES_SSD1963)
   /* LCD display, 16 bit
    * slow down frequency for PLL initialization */
   MPMC_STCONFIG0  = 0x81;
   MPMC_STWTWEN0   = 0;
   MPMC_STWTOEN0   = 0;
   MPMC_STWTRD0    = 31;
   MPMC_STWTPG0    = 0;
   MPMC_STWTWR0    = 31;
   MPMC_STWTTURN0  = 0;

   ssd1963_init_pll();

   /* LCD display, 16 bit
    * speed up frequency for normal display operation */
   MPMC_STCONFIG0  = 0x81;
   MPMC_STWTWEN0   = 0;
   MPMC_STWTOEN0   = 0;
   MPMC_STWTRD0    = 31;
   MPMC_STWTPG0    = 0;
   MPMC_STWTWR0    = 1;
   MPMC_STWTTURN0  = 0;
#endif


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

   __asm volatile ("mov r0, #-1\t\n"
         "mov pc, %0" : : "r" (lr));


}


/* target_init

   performs the rest of the hardware initialization that didn't have
   to be performed during the bootstrap phase.

 */

static void target_init (void)
{
}

static __service_0 struct service_d lpc313x_target_service = {
      .init    = target_init,
};
