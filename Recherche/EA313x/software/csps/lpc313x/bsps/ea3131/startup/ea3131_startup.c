/***********************************************************************
 * $Id:: ea3131_startup.c 3674 2010-06-05 01:53:13Z usb10131          $
 *
 * Project: Embedded Artist ea3131 board startup code
 *
 * Description:
 *     This file contains startup code used with the NXP ea3131 board.
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

#include "lpc_arm922t_cp15_driver.h"
#include "ea3131_board.h"
#include "lpc313x_mpmc.h"
#include "lpc313x_sysreg.h"
#include "lpc313x_cgu_driver.h"
#include "lpc313x_timer_driver.h"

/***********************************************************************
 * Startup code private data
 **********************************************************************/

 
/***********************************************************************
 * Private functions
 **********************************************************************/

/***********************************************************************
 *
 * Function: ea3131_gpio_setup
 *
 * Purpose: Setup GPIO and MUX states
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
static void ea3131_gpio_setup(void)
{
}

/***********************************************************************
 *
 * Function: ea3131_clock_setup
 *
 * Purpose: Setup system clocking
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
static void ea3131_clock_setup(void)
{
  UNS_32 clkin_freq[CGU_FIN_SELECT_MAX]; 
  CGU_HPLL_SETUP_T pll_180 = 
	{
        CGU_FIN_SELECT_FFAST, //fin_select;
        770,		//ndec;
        8191,		//mdec;
        98,			//pdec;
        0,			//selr;
        16,			//seli;
        8,			//selp;
        0,			//mode;
        180000000	//freq; 
    };

  CGU_HPLL_SETUP_T pll_270 = 
	{
        CGU_FIN_SELECT_FFAST, //fin_select;
        514,			//ndec;
        19660,		//mdec;
        98,			//pdec;
        0,			//selr;
        48,			//seli;
        23,			//selp;
        0,			//mode;
        270000000	//freq; 
    };
  CGU_HPLL_SETUP_T pll0 = {
        CGU_FIN_SELECT_FFAST, //fin_select;
        131,			//ndec;
        29784,		//mdec;
        7,			//pdec;
        0,			//selr;
        8,			//seli;
        31,			//selp;
        0,			//mode;
        406425600	//freq; 
    };
  UNS_32 chip_id = *((volatile UNS_32*)CHIP_ID_REG) & CHIP_ID_MSK;

  /* init clock frequencies */
  clkin_freq[0] = XTAL_IN; /* CGU_FIN_SELECT_FFAST */
  clkin_freq[1] = 0; /*	CGU_FIN_SELECT_XT_DAI_BCK0 */
  clkin_freq[2] = 0; /*	CGU_FIN_SELECT_XT_DAI_WS0 */
  clkin_freq[3] = 0; /*	CGU_FIN_SELECT_XT_DAI_BCK1 */
  clkin_freq[4] = 0; /*	CGU_FIN_SELECT_XT_DAI_WS1 */
  clkin_freq[5] = 0; /*	CGU_FIN_SELECT_HPPLL0     */
  clkin_freq[6] = 0; /*	CGU_FIN_SELECT_HPPLL1     */

  /* init CGU driver */
  cgu_init(clkin_freq);
  cgu_reset_all_clks();
  /* set HPLL0 - Audio PLL to default speed */
  cgu_hpll_config (CGU_HPLL0_ID, &pll0);
  /* set HPLL1 - main PLL to default speed */
  /* Check if we are running on LPC3141 or LPC3143. If yes use 270/90
  settings. For all other chips use 180/90
  */
  if ( (chip_id == CHIP_ID_LPC3141) || (chip_id == CHIP_ID_LPC3143))
  {
    cgu_hpll_config (CGU_HPLL1_ID, &pll_270);
    /* use the default clock tree distribution structure */
    cgu_init_clks(&g_lpc314x_cgu_default_clks);	
  }
  else
  {
    cgu_hpll_config (CGU_HPLL1_ID, &pll_180);
    /* use the default clock tree distribution structure */
    cgu_init_clks(&g_cgu_default_clks);	
  }

}


/***********************************************************************
 *
 * Function: ea3131_sdram_init
 *
 * Purpose: Setup SDRAM
 *
 * Processing:
 *     ***** Micron Initialization Sequence from their data sheet
 *           for the Micron MT48LC32M16A2 32M x 16 SDRAM chip:
 *   
 *     Initialization
 *   
 *     SDRAMs must be powered up and initialized in a
 *     predefined manner. Operational procedures other than
 *     those specified may result in undefined operation. Once
 *     power is applied to VDD and VDDQ (simultaneously) and
 *     the clock is stable (stable clock is defined as a signal
 *     cycling within timing constraints specified for the clock
 *     pin), the SDRAM requires a 100µs delay prior to issuing
 *     any command other than a COMMAND INHIBIT or NOP.
 *   
 *     Starting at some point during this 100µs period and
 *     continuing at least through the end of this period,
 *     COMMAND INHIBIT or NOP commands should be applied.
 *     Once the 100µs delay has been satisfied with at least
 *     one COMMAND INHIBIT or NOP command having been applied,
 *     a PRECHARGE command should be applied. All banks must
 *     then be precharged, thereby placing the device in the
 *     all banks idle state.
 *   
 *     Once in the idle state, two AUTO REFRESH cycles
 *     must be performed. After the AUTO REFRESH cycles are
 *     complete, the SDRAM is ready for mode register programming.
 *   
 *     Because the mode register will power up in an
 *     unknown state, it should be loaded prior to applying any
 *     operational command.
 *
 *     *****  The JEDEC recommendation for initializing SDRAM is:
 *    
 *      APPLY POWER (Vdd/Vddq equally, and CLK is stable)
 *      Wait 200uS
 *      PRECHARGE all
 *      8 AUTO REFRESH COMMANDS
 *      LOAD MODE REGISTER
 *      SDRAM is ready for operation
 *    
 *     *****  The Micron SDRAM parts will work fine with the JEDEC sequence,
 *      but also allow for a quicker init sequence of:
 *    
 *      APPLY POWER (Vdd/Vddq equally, and CLK is stable)
 *      Wait at least 100uS (during which time start applying and
 *         continue applying NOP or COMMAND INHIBIT)
 *		PRECHARGE all
 *		2 AUTO REFRESH COMMANDS (min requirement, more than 2 is also ok)
 *		LOAD MODE REGISTER
 *		SDRAM is ready for operation
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: 
 *     IF MPMC CLOCK SPEED IS FIXED REPLACE 'current_hclk' WITH DECIMAL
 *     VALUE OF THE CLOCK. SO THAT THE COMPUTATION IS DONE AT COMPILE
 *     TIME. THIS WILL REDUCE THE CODE FOOTPRINT DRASTICALLY or ELSE
 *     LOT OF RUNTIME-LIBRARY CODE WILL BE ADDED TO THE IMAGE DUE TO
 *     64BIT CALCULATIONS DONE BY THE SDRAM TIMING MACROS.
 *
 **********************************************************************/
void ea3131_sdram_init(void)
{
  UNS_32 tmp;
  UNS_64 current_hclk;
  
  current_hclk = cgu_get_clk_freq(CGU_SB_MPMC_CFG_CLK2_ID);
  timer_wait_us(NULL, 100);
  
  /* Set command delay startergy */
  MPMC->sdram_rdcfg = MPMC_SDRAMC_RDCFG_CMDDELAY_STG;
  
  /* configure "device config register" nSDCE0 for proper width
     SDRAM */
  MPMC->sdram[0].config = SDRAMC_16HP_32Mx16_4B_R13_C10;
  
  MPMC->sdram[0].rascas = MPMC_SDRAMC_RASCAS_RAS2 | MPMC_SDRAMC_RASCAS_CAS2;
  
  /* min 20ns program 1 so that atleast 2 HCLKs are used */
  MPMC->sdram_rp   = MPMC_SDRAMC_TRP(EA3131_SDRAM_TRP,current_hclk);
  MPMC->sdram_ras  = MPMC_SDRAMC_TRAS(EA3131_SDRAM_TRAS,current_hclk);
  MPMC->sdram_srex = MPMC_SDRAMC_TSREX(EA3131_SDRAM_TREX,current_hclk);
  MPMC->sdram_apr  = EA3131_SDRAM_TARP;
  MPMC->sdram_dal  = MPMC_SDRAMC_TDAL(EA3131_SDRAM_TDAL,current_hclk);
  MPMC->sdram_wr   = MPMC_SDRAMC_TWR(EA3131_SDRAM_TWR,current_hclk);
  MPMC->sdram_rc   = MPMC_SDRAMC_TRC(EA3131_SDRAM_TRC,current_hclk);
  MPMC->sdram_rfc  = MPMC_SDRAMC_TRFC(EA3131_SDRAM_TRFC,current_hclk);
  MPMC->sdram_xsr  = MPMC_SDRAMC_TXSR(EA3131_SDRAM_TXSR,current_hclk);
  MPMC->sdram_rrd  = MPMC_SDRAMC_TRRD(EA3131_SDRAM_TRRD,current_hclk);
  MPMC->sdram_mrd  = MPMC_SDRAMC_TMRD(EA3131_SDRAM_TMRD,current_hclk);
  
  timer_wait_us(NULL, 100);
  
  /* issue continuous NOP commands  */
  MPMC->sdram_ctrl = MPMC_SDRAMC_CTL_CE | MPMC_SDRAMC_CTL_CS | 
    MPMC_SDRAMC_CTL_NOP_CMD;
  
  /* load ~200us delay value to timer1 */
  timer_wait_us(NULL, 200);
  
  /* issue a "pre-charge all" command */
  MPMC->sdram_ctrl = MPMC_SDRAMC_CTL_CE | MPMC_SDRAMC_CTL_CS | 
    MPMC_SDRAMC_CTL_PALL_CMD;
  
  /*******************************************************************
  * Minimum refresh pulse interval (tRFC) for MT48LC32M16A2=80nsec,
  * 100nsec provides more than adequate interval. 
  ******************************************************************/
  MPMC->sdram_ref = MPMC_SDRAMC_REFRESH(EA3131_SDRAM_REFRESH,
                                           current_hclk);
  
  /* load ~250us delay value to timer1 */
  timer_wait_us(NULL, 250);
  
  /*******************************************************************
  *  Recommended refresh interval for normal operation of the Micron
  * MT48LC16LFFG = 7.8125usec (128KHz rate).
  * ((HCLK / 128000) - 1) = refresh counter interval rate, (subtract
  * one for safety margin).
  ******************************************************************/
  MPMC->sdram_ref = MPMC_SDRAMC_REFRESH(EA3131_SDRAM_OPER_REFRESH,
                                           current_hclk);
  
  /* select mode register update mode */
  MPMC->sdram_ctrl = MPMC_SDRAMC_CTL_CE | MPMC_SDRAMC_CTL_CS | 
    MPMC_SDRAMC_CTL_MODE_CMD;
  
  /*******************************************************************
  * Program the SDRAM internal mode registers on bank nSDCE0
  * and reconfigure the SDRAM chips.  Bus speeds up to 90MHz
  * requires use of a CAS latency = 2.
  * To get correct value on address bus CAS cycle, requires a shift
  * by 13 for 16bit mode
  ******************************************************************/
  tmp = *((volatile UNS_32 *)(EXT_SDRAM_BASE | _SBF(13, 0x23)));
  
  MPMC->sdram[0].config = SDRAMC_16HP_32Mx16_4B_R13_C10;
  
  MPMC->sdram[0].rascas = MPMC_SDRAMC_RASCAS_RAS2 | 
    MPMC_SDRAMC_RASCAS_CAS2;
  
  /* select normal operating mode */
  MPMC->sdram_ctrl = MPMC_SDRAMC_CTL_CE | MPMC_SDRAMC_CTL_CS | 
    MPMC_SDRAMC_CTL_NORMAL_CMD;
  
  /* enable buffers */
  MPMC->sdram[0].config |= MPMC_SDRAMC_CFG_BUF_EN;

  MPMC->sdram_ctrl = MPMC_SDRAMC_CTL_NORMAL_CMD | MPMC_SDRAMC_CTL_CS;
}

/***********************************************************************
 *
 * Function: ea3131_mem_setup
 *
 * Purpose: Setup memory
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
static void ea3131_mem_setup(void)
{
  /* configure the LCD pins in EBI memory mode. */
  SYS_REGS->mux_lcd_ebi_sel = 1;
  
  /* enable EBI clock */
  cgu_clk_en_dis( CGU_SB_EBI_CLK_ID, TRUE);
  
  /* enable MPMC controller clocks */
  cgu_clk_en_dis( CGU_SB_MPMC_CFG_CLK_ID, TRUE);
  cgu_clk_en_dis( CGU_SB_MPMC_CFG_CLK2_ID, TRUE);
  cgu_clk_en_dis( CGU_SB_MPMC_CFG_CLK3_ID, TRUE);

  /* enable External Memory controller */
  MPMC->control = MPMC_CTL_ENABLE;
  /* Force HCLK to MPMC_CLK to 1:1 ratio */
  MPMC->config = MPMC_CFG_SDCCLK_1_1;
  /* set MPMC delay gates appropriately based on trace lengths between
  SDRAM and the chip. Also based on the delay startergy used for SDRAM. */
  SYS_REGS->mpmp_delaymodes = EA3131_MPMC_DELAY;
  
  /* init sdram */
  ea3131_sdram_init();

  /* Static Memory Configuration for nCS0 */
  MPMC->static_mem[0].config = MPMC_STATIC_CFG_MW16 | MPMC_STATIC_CFG_BLS ;
  /* Static Memory Write Enable Delay for nCS0 */
  MPMC->static_mem[0].wtwen  = 2;
  /* Static Memory Output Enable Delay for nCS0 */
  MPMC->static_mem[0].wtoen  = 1;
  /* Static Memory Read Delay for nCS0 */
  MPMC->static_mem[0].wtrd   = 15;
  /* Static Memory Page Mode Read Delay for nCS0 */
  MPMC->static_mem[0].wtpg   = 2;
  /* Static Memory Write Delay for nCS0 */
  MPMC->static_mem[0].wtwr   = 4 /*2*/;
  /* Static Memory Turn Around Delay for nCS0 */
  MPMC->static_mem[0].wtturn = 2;
}

/***********************************************************************
 *
 * Function: ea3131_misc_setup
 *
 * Purpose: Miscellaneous board setup
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void ea3131_misc_setup(void)
{
  /* Map first 4KB of ARM space to ISRAM area. ie 0x0 - 0x1000 = 0x11028000 - 0x11029000 */
  SYS_REGS->arm926_shadow_pointer = ISRAM_ESRAM0_BASE;
}

/***********************************************************************
 * Public functions
 **********************************************************************/

/***********************************************************************
 *
 * Function: ea3131_init
 *
 * Purpose: Main startup code entry point, called from reset entry code
 *
 * Processing:
 *     Call the ea3131 individual board init functions to setup the
 *     board systems.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void ea3131_init(void)
{
    /* Setup GPIO and MUX states */
    ea3131_gpio_setup();

    /* Setup system clocks and run mode */
    ea3131_clock_setup();

    /* Setup memory */
    ea3131_mem_setup();

    /* Setup miscellaneous functions */
    ea3131_misc_setup();
}
