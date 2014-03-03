/*
   written by Durgesh Pattamatta
   10 Oct 2008

   Copyright (C) 2008 NXP Semiconductors

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

 */

#include <apex.h>
#include "lpc313x.h"



/*Below are the default configurations
 * FFAST : 12MHz
 * MASTER PLL Freq: 180MHz;
 * AUDIOPLL Freq:   1024Fs, Fs = 44.1kHz
 *
 * Domain                                 Input           Subdomain     Divider Ratio
 * 0 - CLK_SWITCHBOX_SYS_BASE0            MASTER PLL      DIV0          1/2
 *                                                        DIV1          1
 *                                                        DIV2          1/2
 *                                                        DIV3          1/4
 *                                                        DIV4          1/4
 *                                                        DIV5          1/4
 *                                                        DIV6          1/2
 *
 * 1 - CLK_SWITCHBOX_AHB0_APB0_BASE1      FFAST           DIV7          1/2
 *                                                        DIV8          1/2
 *
 * 2 - CLK_SWITCHBOX_AHB0_APB1_BASE2      FFAST           DIV9          1/2
 *                                                        DIV10         1/2
 *
 * 3 - CLK_SWITCHBOX_AHB0_APB2_BASE3      MASTER PLL      DIV11         1/2
 *                                                        DIV12         1/4
 *                                                        DIV13         not used
 *
 * 4 - CLK_SWITCHBOX_AHB0_APB2_BASE3      MASTER PLL      DIV14         1/2
 *
 * 5 - CLK_SWITCHBOX_IPINT0_BASE5         MASTER PLL      DIV15         1/2
 *
 * 6 - CLK_SWITCHBOX_UARTCLK_BASE6        MASTER PLL      DIV16         0
 *
 * 7 - CLK_SWITCHBOX_CLK1024FS_BASE7      AUDIO PLL       DIV17         1/1024
 *                                                        DIV18         1/16
 *                                                        DIV19         1/4
 *                                                        DIV20         1/16
 *                                                        DIV21         1/128
 *                                                        DIV22         1/8
 *
 * 8 - CLK_SWITCHBOX_DAI_BCK0_BASE8        DAI_BCK0          -                -
 *
 * 9 - CLK_SWITCHBOX_DAI_BCK1_BASE9        DAI_BCK1          -                -
 *
 * 10 - CLK_SWITCHBOX_SPI_BASE10           MASTER PLL       DIV23        1/2
 *
 * 11 - CLK_SWITCHBOX_SYSCLK0_BASE11       FFAST              -                 -
 *
 * End of configuration table*/

/*------------------------------------------------------------------------------------------
  Group the clocks 0 - 29 belonging to SYS_BASE domain into 7 different sub-domains using
  following macro. Clocks not defined in the macros will be sourced with SYS_BASE_CLK.
 ------------------------------------------------------------------------------------------*/
/* define clocks belonging to subdomain DOMAIN0_DIV0 */
#define CGU_DEF_DOMAIN0_DIV0	( D0_BIT(CGU_SB_APB0_CLK_ID) | D0_BIT(CGU_SB_APB1_CLK_ID) | \
                               D0_BIT(CGU_SB_APB2_CLK_ID) | D0_BIT(CGU_SB_APB3_CLK_ID) | \
                               D0_BIT(CGU_SB_APB4_CLK_ID) | D0_BIT(CGU_SB_AHB2INTC_CLK_ID) | \
                               D0_BIT(CGU_SB_AHB0_CLK_ID) | D0_BIT(CGU_SB_DMA_PCLK_ID) | \
                               D0_BIT(CGU_SB_DMA_CLK_GATED_ID) | D0_BIT(CGU_SB_NANDFLASH_S0_CLK_ID) | \
                               D0_BIT(CGU_SB_NANDFLASH_PCLK_ID) | D0_BIT(CGU_SB_ARM926_BUSIF_CLK_ID) | \
                               D0_BIT(CGU_SB_SD_MMC_HCLK_ID) | D0_BIT(CGU_SB_USB_OTG_AHB_CLK_ID) | \
                               D0_BIT(CGU_SB_ISRAM0_CLK_ID) | D0_BIT(CGU_SB_ISRAM1_CLK_ID) | \
                               D0_BIT(CGU_SB_ISROM_CLK_ID) | D0_BIT(CGU_SB_MPMC_CFG_CLK_ID) | \
                               D0_BIT(CGU_SB_MPMC_CFG_CLK2_ID) | D0_BIT(CGU_SB_INTC_CLK_ID) )

/* define clocks belonging to subdomain DOMAIN0_DIV1 */
#define CGU_DEF_DOMAIN0_DIV1	(D0_BIT(CGU_SB_ARM926_CORE_CLK_ID))

/* define clocks belonging to subdomain DOMAIN0_DIV2 */
#define CGU_DEF_DOMAIN0_DIV2	( D0_BIT(CGU_SB_NANDFLASH_AES_CLK_ID) | \
                               D0_BIT(CGU_SB_NANDFLASH_NAND_CLK_ID))

/* define clocks belonging to subdomain DOMAIN0_DIV3 */
#define CGU_DEF_DOMAIN0_DIV3	(D0_BIT(CGU_SB_NANDFLASH_ECC_CLK_ID))

/* define clocks belonging to subdomain DOMAIN0_DIV4 */
#define CGU_DEF_DOMAIN0_DIV4	(D0_BIT(CGU_SB_SD_MMC_CCLK_IN_ID))
/* define clocks belonging to subdomain DOMAIN0_DIV5 */
#define CGU_DEF_DOMAIN0_DIV5	(D0_BIT(CGU_SB_CLOCK_OUT_ID))
/* define clocks belonging to subdomain DOMAIN0_DIV6 */
#define CGU_DEF_DOMAIN0_DIV6	(D0_BIT(CGU_SB_EBI_CLK_ID))

/*------------------------------------------------------------------------------------------
  Group the clocks 30 - 39 belonging to AHB_APB0_BASE domain into 2 different sub-domains using
  following macro. Clocks not defined in the macros will be sourced with AHB_APB0_BASE_CLK.
 ------------------------------------------------------------------------------------------*/
/* define clocks belonging to subdomain DOMAIN1_DIV7 */
#define CGU_DEF_DOMAIN1_DIV7	(D1_BIT(CGU_SB_ADC_CLK_ID))
/* define clocks belonging to subdomain DOMAIN1_DIV8 */
#define CGU_DEF_DOMAIN1_DIV8	( D1_BIT(CGU_SB_AHB2APB0_ASYNC_PCLK_ID) | \
                               D1_BIT(CGU_SB_EVENT_ROUTER_PCLK_ID) | D1_BIT(CGU_SB_ADC_PCLK_ID) | \
                               D1_BIT(CGU_SB_WDOG_PCLK_ID) | D1_BIT(CGU_SB_IOCONF_PCLK_ID) | \
                               D1_BIT(CGU_SB_CGU_PCLK_ID) | D1_BIT(CGU_SB_SYSCREG_PCLK_ID) | \
                               D1_BIT(CGU_SB_OTP_PCLK_ID) | D1_BIT(CGU_SB_RNG_PCLK_ID) )

/*------------------------------------------------------------------------------------------
  Group the clocks 40 - 49 belonging to AHB_APB1_BASE domain into 2 different sub-domains using
  following macro. Clocks not defined in the macros will be sourced with AHB_APB1_BASE_CLK.
 ------------------------------------------------------------------------------------------*/
/* define clocks belonging to subdomain DOMAIN2_DIV9 */
#define CGU_DEF_DOMAIN2_DIV9	( D2_BIT(CGU_SB_AHB2APB1_ASYNC_PCLK_ID) | \
                               D2_BIT(CGU_SB_TIMER0_PCLK_ID) | D2_BIT(CGU_SB_TIMER1_PCLK_ID) | \
                               D2_BIT(CGU_SB_TIMER2_PCLK_ID) | D2_BIT(CGU_SB_TIMER3_PCLK_ID) | \
                               D2_BIT(CGU_SB_PWM_PCLK_ID) | D2_BIT(CGU_SB_PWM_PCLK_REGS_ID) | \
                               D2_BIT(CGU_SB_I2C0_PCLK_ID) | D2_BIT(CGU_SB_I2C1_PCLK_ID) )

/* define clocks belonging to subdomain DOMAIN2_DIV10 */
#define CGU_DEF_DOMAIN2_DIV10	(D2_BIT(CGU_SB_PWM_CLK_ID))

/*------------------------------------------------------------------------------------------
  Group the clocks 50 - 57 belonging to AHB_APB2_BASE domain into 3 different sub-domains using
  following macro. Clocks not defined in the macros will be sourced with AHB_APB2_BASE_CLK.
 ------------------------------------------------------------------------------------------*/
/* define clocks belonging to subdomain DOMAIN3_DIV11 */
#define CGU_DEF_DOMAIN3_DIV11	( D3_BIT(CGU_SB_AHB2APB2_ASYNC_PCLK_ID) | \
                                D3_BIT(CGU_SB_PCM_PCLK_ID) | D3_BIT(CGU_SB_PCM_APB_PCLK_ID) | \
                                D3_BIT(CGU_SB_UART_APB_CLK_ID) | D3_BIT(CGU_SB_LCD_PCLK_ID) | \
                                D3_BIT(CGU_SB_SPI_PCLK_ID) | D3_BIT(CGU_SB_SPI_PCLK_GATED_ID) )

/* define clocks belonging to subdomain DOMAIN3_DIV12 */
#define CGU_DEF_DOMAIN3_DIV12	(D3_BIT(CGU_SB_LCD_CLK_ID))
/* Currently no clocks are connected to this subdomain */
#define CGU_DEF_DOMAIN3_DIV13	(0)

/*------------------------------------------------------------------------------------------
  Group the clocks 58 - 70 belonging to AHB_APB3_BASE domain into a sub-domains using
  following macro. Clocks not defined in the macros will be sourced wih AHB_APB3_BASE_CLK.
 ------------------------------------------------------------------------------------------*/
#define CGU_DEF_DOMAIN4_DIV14	( D4_BIT(CGU_SB_AHB2APB3_ASYNC_PCLK_ID) | \
                                D4_BIT(CGU_SB_I2S_CFG_PCLK_ID) | D4_BIT(CGU_SB_EDGE_DET_PCLK_ID) | \
                                D4_BIT(CGU_SB_I2STX_FIFO_0_PCLK_ID) | D4_BIT(CGU_SB_I2STX_IF_0_PCLK_ID) | \
                                D4_BIT(CGU_SB_I2STX_FIFO_1_PCLK_ID) | D4_BIT(CGU_SB_I2STX_IF_1_PCLK_ID) | \
                                D4_BIT(CGU_SB_I2SRX_FIFO_0_PCLK_ID) | D4_BIT(CGU_SB_I2SRX_IF_0_PCLK_ID) | \
                                D4_BIT(CGU_SB_I2SRX_FIFO_1_PCLK_ID) | D4_BIT(CGU_SB_I2SRX_IF_1_PCLK_ID) )

/*------------------------------------------------------------------------------------------
  Define whether CGU_SB_PCM_CLK_IP_ID clock uses the FDC_15 fractional divider or not. If
  the following macro is set 0 then CGU_SB_PCM_CLK_IP_ID clock is sourced with PCM_BASE_CLK.
 ------------------------------------------------------------------------------------------*/
#define CGU_DEF_DOMAIN5_DIV15	(D5_BIT(CGU_SB_PCM_CLK_IP_ID))

/*------------------------------------------------------------------------------------------
  Define whether CGU_SB_UART_U_CLK_ID clock uses the FDC_16 fractional divider or not. If
  the following macro is set 0 then CGU_SB_UART_U_CLK_ID clock is sourced with UART_BASE_CLK.
 ------------------------------------------------------------------------------------------*/
#define CGU_DEF_DOMAIN6_DIV16	(0)

/*------------------------------------------------------------------------------------------
  Group the clocks 73 - 86 belonging to CLK1024FS_BASE domain into 6 different sub-domains using
  following macro. Clocks not defined in the macros will be sourced with CLK1024FS_BASE_CLK.
 ------------------------------------------------------------------------------------------*/
/* define clocks belonging to subdomain DOMAIN7_DIV17. This divider has 13 bit resolution
   for madd & msub values compared to other dividers which have 8 bit only.*/
#define CGU_DEF_DOMAIN7_DIV17	( D7_BIT(CGU_SB_I2S_EDGE_DETECT_CLK_ID) | \
                                D7_BIT(CGU_SB_I2STX_WS0_ID) | D7_BIT(CGU_SB_I2STX_WS1_ID) | \
                                D7_BIT(CGU_SB_I2SRX_WS0_ID) | D7_BIT(CGU_SB_I2SRX_WS1_ID) )

/* define clocks belonging to subdomain DOMAIN7_DIV18 */
#define CGU_DEF_DOMAIN7_DIV18	( D7_BIT(CGU_SB_I2STX_BCK0_N_ID) | \
                                D7_BIT(CGU_SB_I2STX_BCK1_N_ID) )

/* define clocks belonging to subdomain DOMAIN7_DIV19 */
#define CGU_DEF_DOMAIN7_DIV19	( D7_BIT(CGU_SB_I2STX_CLK0_ID) | \
                                D7_BIT(CGU_SB_CLK_256FS_ID) )

/* define clocks belonging to subdomain DOMAIN7_DIV20 */
#define CGU_DEF_DOMAIN7_DIV20	( D7_BIT(CGU_SB_I2SRX_BCK0_N_ID) | \
                                D7_BIT(CGU_SB_I2SRX_BCK1_N_ID) )

/* define clocks belonging to subdomain DOMAIN7_DIV21 */
#define CGU_DEF_DOMAIN7_DIV21	(0)
/* define clocks belonging to subdomain DOMAIN7_DIV22 */
#define CGU_DEF_DOMAIN7_DIV22	(0)

/* I2SRX_BCK0_BASE and I2SRX_BCK1_BASE domains are directly connected. So, no entry exists here */

/*------------------------------------------------------------------------------------------
  Group the clocks 89 - 90 belonging to SPI_CLK_BASE domain into a sub-domain using
  following macro. Clocks not defined in the macros will be sourced wih SPI_CLK_BASE_CLK.
 ------------------------------------------------------------------------------------------*/
#define CGU_DEF_DOMAIN10_DIV23	( D10_BIT(CGU_SB_SPI_CLK_ID) | \
                                 D10_BIT(CGU_SB_SPI_CLK_GATED_ID) )

/* SYSCLK_O_BASE is directly connected. So, no entry exists here */

/*------------------------------------------------------------------------------------------
  Global variables:
 ------------------------------------------------------------------------------------------*/
#if defined(CONFIG_PLL_270)

const CGU_CLKS_INIT_T const g_cgu_default_clks_3 =
{
  /* domain0 [7] - SYS_BASE Domain*/
  {
    CGU_FIN_SELECT_HPPLL1,
    {
      {{1, 1, 3}, CGU_DEF_DOMAIN0_DIV0},  /*FracDiv0 */
      {{0, 0, 0}, CGU_DEF_DOMAIN0_DIV1},  /*FracDiv1 */
      {{1, 1, 3}, CGU_DEF_DOMAIN0_DIV2},  /*FracDiv2 */
      {{1, 1, 6}, CGU_DEF_DOMAIN0_DIV3},  /*FracDiv3 */
      {{1, 1, 6}, CGU_DEF_DOMAIN0_DIV4},  /*FracDiv4 */
      {{1, 1, 3}, CGU_DEF_DOMAIN0_DIV5}, /*FracDiv5*/
      {{1, 1, 3}, CGU_DEF_DOMAIN0_DIV6}   /*FracDiv6 */
    }
  },
  /* domain1 [2] - AHB0_APB0 ClockDomain*/
  {
    CGU_FIN_SELECT_FFAST,
    {
      {{1, 1, 38}, CGU_DEF_DOMAIN1_DIV7}, /*FracDiv7 */
      {{1, 1, 2}, CGU_DEF_DOMAIN1_DIV8}   /*FracDiv8 */
    }
  },
  /* domain2 [2] - AHB0_APB1 ClockDomain*/
  {
    CGU_FIN_SELECT_FFAST,
    {
      {{1, 1, 2}, CGU_DEF_DOMAIN2_DIV9}, /*FracDiv9  */
      {{1, 1, 2}, CGU_DEF_DOMAIN2_DIV10} /*FracDiv10 */
    }
  },
  /* domain3 [3] - AHB0_APB2 ClockDomain*/
  {
    CGU_FIN_SELECT_HPPLL1,
    {
      {{1, 1, 3}, CGU_DEF_DOMAIN3_DIV11}, /*FracDiv11 */
      {{1, 1, 40}, CGU_DEF_DOMAIN3_DIV12},/*FracDiv12 */
      {{0, 0, 0}, CGU_DEF_DOMAIN3_DIV13}  /*FracDiv13 */
    }
  },
  /* domain4 [1] - AHB0_APB3 ClockDomain*/
  {
    CGU_FIN_SELECT_FFAST,
    {
      {{1, 1, 2}, CGU_DEF_DOMAIN4_DIV14} /*FracDiv14 */
    }
  },
  /* domain5 [1] - PCM ClockDomain*/
  {
    CGU_FIN_SELECT_HPPLL1,
    {
      {{1, 1, 3}, CGU_DEF_DOMAIN5_DIV15} /*FracDiv15 */
    }
  },
  /* domain6 [1] - UART ClockDomain*/
  {
    CGU_FIN_SELECT_FFAST,
    {
      {{0, 0, 0}, CGU_DEF_DOMAIN6_DIV16} /*FracDiv16 */
    }
  },
  /* domain7 [6] - CLK1024FS ClockDomain*/
  {
    CGU_FIN_SELECT_HPPLL0,
    {
      {{1, 1, 256}, CGU_DEF_DOMAIN7_DIV17},/*FracDiv17 */
      {{1, 1, 4}, CGU_DEF_DOMAIN7_DIV18},  /*FracDiv18 */
      {{1, 1, 2}, CGU_DEF_DOMAIN7_DIV19},  /*FracDiv19 */
      {{1, 1, 4}, CGU_DEF_DOMAIN7_DIV20},  /*FracDiv20 */
      {{1, 1, 32}, CGU_DEF_DOMAIN7_DIV21}, /*FracDiv21 */
      {{1, 1, 2}, CGU_DEF_DOMAIN7_DIV22}   /*FracDiv22 */
    }
  },
  /* domain8  - I2SRX_BCK0 ClockDomain*/
  {
    CGU_FIN_SELECT_XT_I2SRX_BCK0
  },

  /* domain9  - I2SRX_BCK1 ClockDomain*/
  {
    CGU_FIN_SELECT_XT_I2SRX_BCK1
  },
  /* domain10 [1] - SPI ClockDomain*/
  {
    CGU_FIN_SELECT_HPPLL1,
    {
      {{1, 1, 3}, CGU_DEF_DOMAIN10_DIV23} /*FracDiv23 */
    }
  },
  /* domain11  - SYSCLK_O ClockDomain*/
  {
    CGU_FIN_SELECT_FFAST
  },

  /* dyn_fdiv_cfg[7]  */
  {
    /*For DYN_SEL, the bits below are all configured as 1.*/
    /*
       bit 	  Name							    Description:

    	0	simple_dma_trans  			 dma transfers can enable high-speed
    	1	simple_dma_ready				 dma last transfers can enable high-speed
    	2	arm926_i_trans  	       arm926ejs_i transfers can enable high-speed
    	3	arm926_i_ready		       arm926ejs_i_ready last transfers can enable high-speed
    	4	arm926_d_trans  	       arm926ejs_d transfers can enable high-speed
    	5	arm926_d_ready		       arm926ejs_d transfers can enable high-speed
    	6	usb_otg_mst_trans   		 USB OTG transfers can enable high spee
    	7	usb_otg_mst_ready				 USB OTG last transfers can enable high-speed
    	8	mpmc_refresh_hispeed_req External SDRAM refresh generator transfers can enable high speed
    */
    {
      0x1FF,
      {1, 1, 64} /*DynFD0*/
    },
    {
      0x1FF,
      {0, 0, 0}	/*DynFD1*/
    },
    {
      0x1FF,
      {1, 1, 3}	/*DynFD2*/
    },
    {
      0x1FF,
      {1, 1, 6}	/*DynFD3*/
    },
    {
      0x1FF,
      {0, 0, 0}	/*DynFD4 - SD_MMC clock doesn't like dynamic switching*/
    },
    {
      0x1FF,
      {1, 1, 6}	/*DynFD5*/
    },
    {
      0x1FF,
      {1, 1, 3}	/*DynFD6*/
    }
  }
};

#elif defined(CONFIG_PLL_180)

const CGU_CLKS_INIT_T g_cgu_default_clks_2 =
{
  /* domain0 [7] - SYS_BASE Domain*/
  {
    CGU_FIN_SELECT_HPPLL1,
    {
      {{1, 1, 2}, CGU_DEF_DOMAIN0_DIV0},  /*FracDiv0 */
      {{0, 0, 0}, CGU_DEF_DOMAIN0_DIV1},  /*FracDiv1 */
      {{1, 1, 2}, CGU_DEF_DOMAIN0_DIV2},  /*FracDiv2 */
      {{1, 1, 4}, CGU_DEF_DOMAIN0_DIV3},  /*FracDiv3 */
      {{1, 1, 4}, CGU_DEF_DOMAIN0_DIV4},  /*FracDiv4 */
      {{1, 1, 2}, CGU_DEF_DOMAIN0_DIV5}, /*FracDiv5*/
      {{1, 1, 2}, CGU_DEF_DOMAIN0_DIV6}   /*FracDiv6 */
    }
  },
  /* domain1 [2] - AHB0_APB0 ClockDomain*/
  {
    CGU_FIN_SELECT_FFAST,
    {
      {{1, 1, 38}, CGU_DEF_DOMAIN1_DIV7}, /*FracDiv7 */
      {{1, 1, 2}, CGU_DEF_DOMAIN1_DIV8}   /*FracDiv8 */
    }
  },
  /* domain2 [2] - AHB0_APB1 ClockDomain*/
  {
    CGU_FIN_SELECT_FFAST,
    {
      {{1, 1, 2}, CGU_DEF_DOMAIN2_DIV9}, /*FracDiv9  */
      {{1, 1, 2}, CGU_DEF_DOMAIN2_DIV10} /*FracDiv10 */
    }
  },
  /* domain3 [3] - AHB0_APB2 ClockDomain*/
  {
    CGU_FIN_SELECT_HPPLL1,
    {
      {{1, 1, 2}, CGU_DEF_DOMAIN3_DIV11}, /*FracDiv11 */
      {{1, 1, 40}, CGU_DEF_DOMAIN3_DIV12},/*FracDiv12 */
      {{0, 0, 0}, CGU_DEF_DOMAIN3_DIV13}  /*FracDiv13 */
    }
  },
  /* domain4 [1] - AHB0_APB3 ClockDomain*/
  {
    CGU_FIN_SELECT_FFAST,
    {
      {{1, 1, 2}, CGU_DEF_DOMAIN4_DIV14} /*FracDiv14 */
    }
  },
  /* domain5 [1] - PCM ClockDomain*/
  {
    CGU_FIN_SELECT_HPPLL1,
    {
      {{1, 1, 2}, CGU_DEF_DOMAIN5_DIV15} /*FracDiv15 */
    }
  },
  /* domain6 [1] - UART ClockDomain*/
  {
    CGU_FIN_SELECT_FFAST,
    {
      {{0, 0, 0}, CGU_DEF_DOMAIN6_DIV16} /*FracDiv16 */
    }
  },
  /* domain7 [6] - CLK1024FS ClockDomain*/
  {
    CGU_FIN_SELECT_HPPLL0,
    {
      {{1, 1, 256}, CGU_DEF_DOMAIN7_DIV17},/*FracDiv17 */
      {{1, 1, 4}, CGU_DEF_DOMAIN7_DIV18},  /*FracDiv18 */
      {{0, 0, 0}, CGU_DEF_DOMAIN7_DIV19},  /*FracDiv19 */
      {{1, 1, 4}, CGU_DEF_DOMAIN7_DIV20},  /*FracDiv20 */
      {{1, 1, 32}, CGU_DEF_DOMAIN7_DIV21}, /*FracDiv21 */
      {{1, 1, 2}, CGU_DEF_DOMAIN7_DIV22}   /*FracDiv22 */
    }
  },
  /* domain8  - I2SRX_BCK0 ClockDomain*/
  {
    CGU_FIN_SELECT_XT_I2SRX_BCK0
  },

  /* domain9  - I2SRX_BCK1 ClockDomain*/
  {
    CGU_FIN_SELECT_XT_I2SRX_BCK1
  },
  /* domain10 [1] - SPI ClockDomain*/
  {
    CGU_FIN_SELECT_HPPLL1,
    {
      {{1, 1, 2}, CGU_DEF_DOMAIN10_DIV23} /*FracDiv23 */
    }
  },
  /* domain11  - SYSCLK_O ClockDomain*/
  {
    CGU_FIN_SELECT_FFAST
  },

  /* dyn_fdiv_cfg[7]  */
  {
    /*For DYN_SEL, the bits below are all configured as 1.*/
    /*
       bit 	  Name							    Description:

    	0	simple_dma_trans  			 dma transfers can enable high-speed
    	1	simple_dma_ready				 dma last transfers can enable high-speed
    	2	arm926_i_trans  	       arm926ejs_i transfers can enable high-speed
    	3	arm926_i_ready		       arm926ejs_i_ready last transfers can enable high-speed
    	4	arm926_d_trans  	       arm926ejs_d transfers can enable high-speed
    	5	arm926_d_ready		       arm926ejs_d transfers can enable high-speed
    	6	usb_otg_mst_trans   		 USB OTG transfers can enable high spee
    	7	usb_otg_mst_ready				 USB OTG last transfers can enable high-speed
    	8	mpmc_refresh_hispeed_req External SDRAM refresh generator transfers can enable high speed
    */
    {
      0x1FF,
      {1, 1, 64} /*DynFD0*/
    },
    {
      0x1FF,
      {0, 0, 0}	/*DynFD1*/
    },
    {
      0x1FF,
      {1, 1, 2}	/*DynFD2*/
    },
    {
      0x1FF,
      {1, 1, 4}	/*DynFD3*/
    },
    {
      0x1FF,
      {0, 0, 0}	/*DynFD4 - SD_MMC clock doesn't like dynamic switching*/
    },
    {
      0x1FF,
      {1, 1, 4}	/*DynFD5*/
    },
    {
      0x1FF,
      {1, 1, 2}	/*DynFD6*/
    }
  }
};

#endif



