/*
   written by Durgesh Pattamatta
   10 Oct 2008

   Copyright (C) 2008 NXP Semiconductors

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

*/

#include <linux/types.h>
#include <linux/string.h>
#include <apex.h>
#include <command.h>
#include <error.h>
#include <linux/kernel.h>
#include <config.h>
#include "lpc313x.h"

/***********************************************************************
* CGU driver package data
***********************************************************************/
u32* const g_clkin_freq = (u32*) (0x11029000 - CGU_FIN_SELECT_MAX * sizeof(u32));

/***********************************************************************
* CGU driver private functions
**********************************************************************/
/***********************************************************************
*
* Function: cgu_fdiv_num_bits
*
* Purpose: calculate int(log2(i))+1
*
* Processing:
*     This algorithm is used to calculate the above said formula.
*     This is used to calculate the madd and msub width in frac div
*     registers.Reasonably fast.
*
* Parameters:
*     i : Number
*
* Outputs: None
*
* Returns: int(log2(i))+1
*
* Notes: None
*
**********************************************************************/
static u32 __section (.bootstrap) cgu_fdiv_num_bits(unsigned int i)
{
  u32 x = 0, y = 16;

  for (; y > 0; y = y >> 1)
  {
    if (i >> y)
    {
      x += y;
      i = i >> y;
    }
  }

  if (i)
    x++;

  return x;
}

/***********************************************************************
*
* Function: cgu_clkid2esrid
*
* Purpose: Finds ESR Id corresponding to the requested clock Id
*
* Processing:
*     Finds ESR index corresponding to the requested clock Id.
*
* Parameters:
*     clkid : clock Id.
*
* Outputs: None
*
* Returns: ESR Id corresponding to clock Id
*
* Notes: None
*
**********************************************************************/
u32 __section (.bootstrap) cgu_clkid2esrid(CGU_CLOCK_ID_T clkid)
{
  u32 esrIndex = (u32)clkid;

  switch (clkid)
  {
    case CGU_SB_I2SRX_BCK0_ID:
    case CGU_SB_I2SRX_BCK1_ID:
    case CGU_SB_SYSCLK_O_ID:
      /* invalid esr index. No ESR register for these clocks */
      esrIndex = CGU_INVALID_ID;
      break;

    case CGU_SB_SPI_CLK_ID:
    case CGU_SB_SPI_CLK_GATED_ID:
      esrIndex = esrIndex - 2;
      break;
    default:
      /* do nothing */
      break;
  }

  return esrIndex;
}
/***********************************************************************
*
* Function: cgu_DomainId2bcrid
*
* Purpose: Finds BCR Id corresponding to the requested domain Id
*
* Processing:
*     Finds BCR index corresponding to the requested domain Id.
*
* Parameters:
*     domainid : Domain ID
*
* Outputs: None
*
* Returns: BCR Id corresponding to domain Id
*
* Notes: None
*
**********************************************************************/
u32 __section (.bootstrap) cgu_DomainId2bcrid(CGU_DOMAIN_ID_T domainid)
{
  u32 bcridx = CGU_INVALID_ID;
  switch (domainid)
  {
    case CGU_SB_SYS_BASE_ID:
    case CGU_SB_AHB0_APB0_BASE_ID:
    case CGU_SB_AHB0_APB1_BASE_ID:
    case CGU_SB_AHB0_APB2_BASE_ID:
      bcridx = domainid;
      break;
    case CGU_SB_CLK1024FS_BASE_ID:
      bcridx = CGU_SB_NR_BCR - 1;
      break;
    default:
      bcridx = CGU_INVALID_ID;
      break;
  }
  return bcridx;
}
/***********************************************************************
*
* Function: cgu_ClkId2DomainId
*
* Purpose: Finds domain & sub-domain Ids for requested clock
*
* Processing:
*     Finds domain index and fractional divider index for the requested
*	   clock.
*
* Parameters:
*     clkid : clock Id.
*     pDomainId : pointer to domain index.
*     pSubdomainId : pointer to fractional divider index.
*
* Outputs:
*     pDomainId : pointer to domain index.
*     pSubdomainId : pointer to fractional divider index.
*
* Returns: None
*
* Notes: None
*
**********************************************************************/
void __section (.bootstrap) cgu_ClkId2DomainId(CGU_CLOCK_ID_T clkid, CGU_DOMAIN_ID_T* pDomainId,
                        u32* pSubdomainId)
{
  u32 esrIndex, esrReg;
  u32 fracdiv_base = CGU_INVALID_ID;

  /*    1. Get the domain ID */

  if (clkid <= CGU_SYS_LAST)
  {
    *pDomainId = CGU_SB_SYS_BASE_ID;
    fracdiv_base = CGU_SB_BASE0_FDIV_LOW_ID;

  }
  else 	if (clkid <= CGU_AHB0APB0_LAST)
  {
    *pDomainId = CGU_SB_AHB0_APB0_BASE_ID;
    fracdiv_base = CGU_SB_BASE1_FDIV_LOW_ID;

  }
  else 	if (clkid <= CGU_AHB0APB1_LAST)
  {
    *pDomainId = CGU_SB_AHB0_APB1_BASE_ID;
    fracdiv_base = CGU_SB_BASE2_FDIV_LOW_ID;

  }
  else 	if (clkid <= CGU_AHB0APB2_LAST)
  {
    *pDomainId = CGU_SB_AHB0_APB2_BASE_ID;
    fracdiv_base = CGU_SB_BASE3_FDIV_LOW_ID;

  }
  else 	if (clkid <= CGU_AHB0APB3_LAST)
  {
    *pDomainId = CGU_SB_AHB0_APB3_BASE_ID;
    fracdiv_base = CGU_SB_BASE4_FDIV_LOW_ID;

  }
  else 	if (clkid == CGU_PCM_LAST)
  {
    *pDomainId = CGU_SB_PCM_BASE_ID;
    fracdiv_base = CGU_SB_BASE5_FDIV_LOW_ID;

  }
  else 	if (clkid == CGU_UART_LAST)
  {
    *pDomainId = CGU_SB_UART_BASE_ID;
    fracdiv_base = CGU_SB_BASE6_FDIV_LOW_ID;

  }
  else 	if (clkid <= CGU_CLK1024FS_LAST)
  {
    *pDomainId = CGU_SB_CLK1024FS_BASE_ID;
    fracdiv_base = CGU_SB_BASE7_FDIV_LOW_ID;

  }
  else 	if (clkid == CGU_I2SRX_BCK0_LAST)
  {
    *pDomainId = CGU_SB_I2SRX_BCK0_BASE_ID;
    fracdiv_base = CGU_INVALID_ID;

  }
  else 	if (clkid == CGU_I2SRX_BCK1_LAST)
  {
    *pDomainId = CGU_SB_I2SRX_BCK1_BASE_ID;
    fracdiv_base = CGU_INVALID_ID;

  }
  else 	if (clkid <= CGU_SPI_LAST)
  {
    *pDomainId = CGU_SB_SPI_CLK_BASE_ID;
    fracdiv_base = CGU_SB_BASE10_FDIV_LOW_ID;

  }
  else
  {
    *pDomainId = CGU_SB_SYSCLK_O_BASE_ID;
    fracdiv_base = CGU_INVALID_ID;
  }

  *pSubdomainId = CGU_INVALID_ID;

  /* read the clocks ESR to get the fractional divider */
  esrIndex = cgu_clkid2esrid(clkid);

  if (CGU_INVALID_ID != esrIndex)
  {
    /* read the clocks ESR to get the fractional divider */
    esrReg = CGU_SB->clk_esr[esrIndex];

    /* A clock may not be connected to any sub-domain and it might be
    connected directly to domain. This is also a valid combination. So,
    errror should not be returned */
    if (esrReg & CGU_SB_ESR_ENABLE)
    {
      *pSubdomainId = CGU_SB_ESR_SEL_GET(esrReg) + fracdiv_base;
    }
  }

}

/***********************************************************************
*
* Function: cgu_fdiv_config
*
* Purpose:
*
* Processing:
*     .
*
* Parameters:
*     i : Number
*
* Outputs: None
*
* Returns:
*
* Notes: None
*
**********************************************************************/
/* frac divider config function */
u32 __section (.bootstrap) cgu_fdiv_config(u32 fdId, 
					   CGU_FDIV_SETUP_T fdivCfg, 
					   u32 enable,
					   u32 dyn_fdc)
{
  u32 conf, maddw, msubw, maxw, fdWidth;
  int madd, msub;

  /* calculating program values to see if they fit in fractional divider*/
  madd = fdivCfg.m - fdivCfg.n;
  msub = -fdivCfg.n;

  /* Find required bit width of madd & msub:*/
  maddw = cgu_fdiv_num_bits((u32)madd);
  msubw = cgu_fdiv_num_bits((u32)fdivCfg.n);
  maxw = (maddw > msubw) ? maddw : msubw;
  fdWidth = CGU_SB_BASE0_FDIV0_W;

  if (fdId == CGU_SB_BASE7_FDIV_LOW_ID)
  {
    /* for Frac divider 17 the bit width is 13 */
    fdWidth = CGU_SB_BASE7_FDIV0_W;
  }

  /* Calculate Configure parameter:*/
  conf = ((((1 << fdWidth) - 1) &
           (msub << (fdWidth - maxw))) <<
          (fdWidth + CGU_SB_FDC_MADD_POS)) |
         (madd << (fdWidth - maxw + CGU_SB_FDC_MADD_POS));

  /* check whther 50% duty cycle is needed for this divider*/
  if (fdivCfg.stretch)
    conf |= CGU_SB_FDC_STRETCH;
  /* check whehter to enable the divider immediately */
  if (enable)
    conf |= CGU_SB_FDC_RUN;

  /* finally configure the divider*/
  if (0 == dyn_fdc)
  {
	CGU_SB->base_fdc[fdId] = conf;
  }
  else
  {
	  if (fdId < CGU_SB_NR_DYN_FDIV)
		CGU_SB->base_dyn_fdc[fdId] = conf | CGU_SB_DYN_FDC_ALLOW;
  }

  return conf;
}
/***********************************************************************
*
* Function: cgu_init_domain_clks
*
* Purpose:
*
* Processing:
*     .
*
* Parameters:
*     i : Number
*
* Outputs: None
*
* Returns:
*
* Notes: None
*
**********************************************************************/
void __section (.bootstrap) cgu_init_domain_clks(CGU_DOMAIN_CFG_T* pDomain)
{
  u32 fd_id, clk_id, esr_id;
  CGU_SUB_DOMAIN_CFG_T* psub = pDomain->sub;
  u32 bcr_id = cgu_DomainId2bcrid(pDomain->id);
  u32 fdiv_conf = 0;

  if (bcr_id != CGU_INVALID_ID)
  {
    /* disable BCR for domain */
    CGU_SB->base_bcr[bcr_id] = 0;
  }

  /* configure frac divider */
  for (fd_id = 0; fd_id < pDomain->fdiv_cnt; fd_id++, psub++)
  {
    /* set fractional dividers but don't enable them yet*/
    fdiv_conf = cgu_fdiv_config(fd_id + pDomain->fdiv_min, psub->fdiv_cfg, 0, 0);

    /* enable frac divider only if it has valid settings. Or else it may be unused*/
    if (fdiv_conf != 0)
    {
      /* select frac div for each clock in this sub domain*/
      for (clk_id = 0; clk_id <= pDomain->clk_cnt; clk_id++)
      {
        esr_id = cgu_clkid2esrid((CGU_CLOCK_ID_T)(clk_id + pDomain->clk_min));
        if (esr_id != CGU_INVALID_ID)
        {
          /* check whether clock belongs to this sub-domain */
          if (psub->clks & _BIT(clk_id))
          {
            /* finally configure the clock*/
            CGU_SB->clk_esr[esr_id] = CGU_SB_ESR_SELECT(fd_id) | CGU_SB_ESR_ENABLE;
          }
        }
      }
      /* enable frac divider */
      CGU_SB->base_fdc[fd_id + pDomain->fdiv_min] |= CGU_SB_FDC_RUN;
    }
  }
  if (bcr_id != CGU_INVALID_ID)
  {
    /* enable BCR for domain */
    CGU_SB->base_bcr[bcr_id] = CGU_SB_BCR_FD_RUN;
  }
  /* select input for domain*/
  cgu_set_base_freq(pDomain->id, pDomain->fin_sel);
}


/***********************************************************************
* CGU driver public functions
***********************************************************************/
/***********************************************************************
*
* Function: cgu_init
*
* Purpose:
*
* Processing:
*     .
*
* Parameters:
*     i : Number
*
* Outputs: None
*
* Returns:
*
* Notes: None
*
**********************************************************************/
void __section (.bootstrap) cgu_init(void)
{
    g_clkin_freq[0] = FFAST_CLOCK;
    g_clkin_freq[1] = 0;
    g_clkin_freq[2] = 0;
    g_clkin_freq[3] = 0;
    g_clkin_freq[4] = 0;
    g_clkin_freq[5] = 0;
    g_clkin_freq[6] = 0;
}

/***********************************************************************
*
* Function: cgu_reset_all_clks
*
* Purpose:
*
* Processing:
*     .
*
* Parameters:
*     i : Number
*
* Outputs: None
*
* Returns:
*
* Notes: None
*
**********************************************************************/
void __section (.bootstrap) cgu_reset_all_clks(void)
{
  CGU_DOMAIN_ID_T domainId;
  u32 bcrId, esr_id;
  u32 enable, i;

  /* switch all clocks to FFAST */
  for (domainId = CGU_SB_BASE_FIRST; domainId < CGU_SB_NR_BASE; domainId++)
  {
    /* switch reference clock in to FFAST */
    cgu_set_base_freq(domainId, CGU_FIN_SELECT_FFAST);
    /* check if the domain has a BCR*/
    bcrId = cgu_DomainId2bcrid(domainId);
    /* disable all BCRs */
    if (bcrId != CGU_INVALID_ID)
    {
      CGU_SB->base_bcr[bcrId] = 0;
    }
  }
  /* disable all clocks except the needed ones */
  for (i = 0; i < (CGU_SYSCLK_O_LAST + 1); i++)
  {
    /* Clear the clocks ESR to deselect fractional divider */
    esr_id = cgu_clkid2esrid((CGU_CLOCK_ID_T)i);

    if (CGU_INVALID_ID != esr_id)
    {
      CGU_SB->clk_esr[esr_id] = 0;
    }

    if (i < 31)
    {
      enable = CGU_DEF_CLKS_0_31 & _BIT(i);
    }
    else
    {
      if (i < 63)
      {
        enable = CGU_DEF_CLKS_32_63 & _BIT(i - 32);
      }
      else
      {
        enable = CGU_DEF_CLKS_64_92 & _BIT(i - 64);
      }
    }
    /**********************************************************/
    /* initiliase external enables and ext_outs  */
    /**********************************************************/
    /* set external enable for all possible clocks to conserve
    power. cgu_clk_set_exten() function sets CGU_SB_PCR_EXTEN_EN
    for allowed clocks only. */
    cgu_clk_set_exten((CGU_CLOCK_ID_T)i, 1);

    /* set enable-out�s for only the following clocks */
    if ((i == CGU_SB_ARM926_BUSIF_CLK_ID) ||
        (i == CGU_SB_MPMC_CFG_CLK_ID))
    {
      CGU_SB->clk_pcr[i] |= CGU_SB_PCR_ENOUT_EN;
    }
    else
    {
      CGU_SB->clk_pcr[i] &= ~CGU_SB_PCR_ENOUT_EN;
    }

    /* disable all clocks except the needed ones */
    if (enable)
    {
      CGU_SB->clk_pcr[i] |= CGU_SB_PCR_RUN;
    }
    else
    {
      CGU_SB->clk_pcr[i] &= ~CGU_SB_PCR_RUN;
    }
  }
  /* disable all fractional dividers */
  for (i = 0; i < CGU_SB_NR_FRACDIV; i++)
  {
    CGU_SB->base_fdc[i] &= ~CGU_SB_FDC_RUN;
  }
}

/***********************************************************************
*
* Function: cgu_init_clks
*
* Purpose:
*
* Processing:
*     .
*
* Parameters:
*     i : Number
*
* Outputs: None
*
* Returns:
*
* Notes: None
*
**********************************************************************/
/* select frac div for each clk */
int __section (.bootstrap) cgu_init_clks(const CGU_CLKS_INIT_T* pClksCfg)
{
  CGU_DOMAIN_CFG_T domain;
  int i;

  /* reset all clocks and connect them to FFAST */
  cgu_reset_all_clks();

  /**********************************************************/
  /* initiliase Domain0 = SYS_BASE clocks                   */
  /**********************************************************/
  domain.id = CGU_SB_SYS_BASE_ID;
  domain.fin_sel = pClksCfg->domain0.fin_sel;
  domain.clk_min = CGU_SYS_FIRST;
  domain.clk_cnt = (CGU_SYS_LAST - CGU_SYS_FIRST) + 1;
  domain.fdiv_min = CGU_SB_BASE0_FDIV_LOW_ID;
  domain.fdiv_cnt = CGU_SB_BASE0_FDIV_CNT;
  domain.sub = (CGU_SUB_DOMAIN_CFG_T*) & pClksCfg->domain0.sub[0];
  /* initialize all clocks and frac dividers for this domain */
  cgu_init_domain_clks(&domain);

  /**********************************************************/
  /* initiliase Domain1 = AHB0APB0_BASE clocks */
  /**********************************************************/
  domain.id = CGU_SB_AHB0_APB0_BASE_ID;
  domain.fin_sel = pClksCfg->domain1.fin_sel;
  domain.clk_min = CGU_AHB0APB0_FIRST;
  domain.clk_cnt = (CGU_AHB0APB0_LAST - CGU_AHB0APB0_FIRST) + 1;
  domain.fdiv_min = CGU_SB_BASE1_FDIV_LOW_ID;
  domain.fdiv_cnt = CGU_SB_BASE1_FDIV_CNT;
  domain.sub = (CGU_SUB_DOMAIN_CFG_T*) & pClksCfg->domain1.sub[0];
  /* initialize all clocks and frac dividers for this domain */
  cgu_init_domain_clks(&domain);

  //cgu_set_base_freq(CGU_SB_AHB0_APB0_BASE_ID, CGU_FIN_SELECT_HPPLL1);
  /**********************************************************/
  /* initiliase Domain2 = AHB0APB1_BASE clocks */
  /**********************************************************/
  domain.id = CGU_SB_AHB0_APB1_BASE_ID;
  domain.fin_sel = pClksCfg->domain2.fin_sel;
  domain.clk_min = CGU_AHB0APB1_FIRST;
  domain.clk_cnt = (CGU_AHB0APB1_LAST - CGU_AHB0APB1_FIRST) + 1;
  domain.fdiv_min = CGU_SB_BASE2_FDIV_LOW_ID;
  domain.fdiv_cnt = CGU_SB_BASE2_FDIV_CNT;
  domain.sub = (CGU_SUB_DOMAIN_CFG_T*) & pClksCfg->domain2.sub[0];
  /* initialize all clocks and frac dividers for this domain */
  cgu_init_domain_clks(&domain);

  /**********************************************************/
  /* initiliase Domain3 = AHB0APB2_BASE clocks */
  /**********************************************************/
  domain.id = CGU_SB_AHB0_APB2_BASE_ID;
  domain.fin_sel = pClksCfg->domain3.fin_sel;
  domain.clk_min = CGU_AHB0APB2_FIRST;
  domain.clk_cnt = (CGU_AHB0APB2_LAST - CGU_AHB0APB2_FIRST) + 1;
  domain.fdiv_min = CGU_SB_BASE3_FDIV_LOW_ID;
  domain.fdiv_cnt = CGU_SB_BASE3_FDIV_CNT;
  domain.sub = (CGU_SUB_DOMAIN_CFG_T*) & pClksCfg->domain3.sub[0];
  /* initialize all clocks and frac dividers for this domain */
  cgu_init_domain_clks(&domain);

  /**********************************************************/
  /* initiliase Domain4 = AHB0APB3_BASE clocks */
  /**********************************************************/
  domain.id = CGU_SB_AHB0_APB3_BASE_ID;
  domain.fin_sel = pClksCfg->domain4.fin_sel;
  domain.clk_min = CGU_AHB0APB3_FIRST;
  domain.clk_cnt = (CGU_AHB0APB3_LAST - CGU_AHB0APB3_FIRST) + 1;
  domain.fdiv_min = CGU_SB_BASE4_FDIV_LOW_ID;
  domain.fdiv_cnt = CGU_SB_BASE4_FDIV_CNT;
  domain.sub = (CGU_SUB_DOMAIN_CFG_T*) & pClksCfg->domain4.sub[0];
  /* initialize all clocks and frac dividers for this domain */
  cgu_init_domain_clks(&domain);

  /**********************************************************/
  /* initiliase Domain5 = PCM_BASE clocks */
  /**********************************************************/
  domain.id = CGU_SB_PCM_BASE_ID;
  domain.fin_sel = pClksCfg->domain5.fin_sel;
  domain.clk_min = CGU_PCM_FIRST;
  domain.clk_cnt = 1;
  domain.fdiv_min = CGU_SB_BASE5_FDIV_LOW_ID;
  domain.fdiv_cnt = CGU_SB_BASE5_FDIV_CNT;
  domain.sub = (CGU_SUB_DOMAIN_CFG_T*) & pClksCfg->domain5.sub[0];
  /* initialize all clocks and frac dividers for this domain */
  cgu_init_domain_clks(&domain);

  /**********************************************************/
  /* initiliase Domain6 = UART_BASE clocks */
  /**********************************************************/
  domain.id = CGU_SB_UART_BASE_ID;
  domain.fin_sel = pClksCfg->domain6.fin_sel;
  domain.clk_min = CGU_UART_FIRST;
  domain.clk_cnt = 1;
  domain.fdiv_min = CGU_SB_BASE6_FDIV_LOW_ID;
  domain.fdiv_cnt = CGU_SB_BASE6_FDIV_CNT;
  domain.sub = (CGU_SUB_DOMAIN_CFG_T*) & pClksCfg->domain6.sub[0];
  /* initialize all clocks and frac dividers for this domain */
  cgu_init_domain_clks(&domain);

  /**********************************************************/
  /* initiliase Domain7 = CLK1024FS_BASE clocks */
  /**********************************************************/
  domain.id = CGU_SB_CLK1024FS_BASE_ID;
  domain.fin_sel = pClksCfg->domain7.fin_sel;
  domain.clk_min = CGU_CLK1024FS_FIRST;
  domain.clk_cnt = (CGU_CLK1024FS_LAST - CGU_CLK1024FS_FIRST) + 1;
  domain.fdiv_min = CGU_SB_BASE7_FDIV_LOW_ID;
  domain.fdiv_cnt = CGU_SB_BASE7_FDIV_CNT;
  domain.sub = (CGU_SUB_DOMAIN_CFG_T*) & pClksCfg->domain7.sub[0];
  /* initialize all clocks and frac dividers for this domain */
  cgu_init_domain_clks(&domain);

  /**********************************************************/
  /* initiliase Domain8 = BCK0_BASE clocks */
  /**********************************************************/
  /* select input for domain*/
  cgu_set_base_freq(CGU_SB_I2SRX_BCK0_BASE_ID, pClksCfg->domain8.fin_sel);

  /**********************************************************/
  /* initiliase Domain9 = BCK1_BASE clocks */
  /**********************************************************/
  /* select input for domain*/
  cgu_set_base_freq(CGU_SB_I2SRX_BCK1_BASE_ID, pClksCfg->domain9.fin_sel);

  /**********************************************************/
  /* initiliase Domain10 = SPI_BASE clocks */
  /**********************************************************/
  domain.id = CGU_SB_SPI_CLK_BASE_ID;
  domain.fin_sel = pClksCfg->domain10.fin_sel;
  domain.clk_min = CGU_SPI_FIRST;
  domain.clk_cnt = (CGU_SPI_LAST - CGU_SPI_FIRST) + 1;
  domain.fdiv_min = CGU_SB_BASE10_FDIV_LOW_ID;
  domain.fdiv_cnt = CGU_SB_BASE10_FDIV_CNT;
  domain.sub = (CGU_SUB_DOMAIN_CFG_T*) & pClksCfg->domain10.sub[0];
  /* initialize all clocks and frac dividers for this domain */
  cgu_init_domain_clks(&domain);

  /**********************************************************/
  /* initiliase Domain11 = SYSCLK_O_BASE clocks */
  /**********************************************************/
  /* select input for domain*/
  cgu_set_base_freq(CGU_SB_SYSCLK_O_BASE_ID, pClksCfg->domain11.fin_sel);

  /**********************************************************/
  /* initiliase Dynamic fractional dividers TBD */
  /**********************************************************/
#if defined(CONFIG_DYNAMIC_CLOCKS)
  /* disable BCR for domain */
  CGU_SB->base_bcr[0] = 0;
  for (i = 0; i < CGU_SB_NR_DYN_FDIV; i++) 
  {
	  if (pClksCfg->dyn_fdiv_cfg[i].cfg.n != 0)
	  {
		  CGU_SB->base_dyn_sel[i] = pClksCfg->dyn_fdiv_cfg[i].sel;
		  cgu_fdiv_config(i, pClksCfg->dyn_fdiv_cfg[i].cfg, 1, 1);
	  }
	  else
	  {
		  CGU_SB->base_dyn_fdc[i] = 0;
	  }

  }

  CGU_SB->base_bcr[0] = CGU_SB_BCR_FD_RUN;
#endif

  return 0;
}

/***********************************************************************
*
* Function: cgu_get_base_freq
*
* Purpose:
*
* Processing:
*     .
*
* Parameters:
*     i : Number
*
* Outputs: None
*
* Returns:
*
* Notes: None
*
**********************************************************************/
int __section (.bootstrap) cgu_get_base_freq(CGU_DOMAIN_ID_T baseid)
{
  int freq;

  /* get base frequency for the domain */
  freq = g_clkin_freq[CGU_SB_SSR_FS_GET(CGU_SB->base_ssr[baseid])];

  return freq;
}

/***********************************************************************
*
* Function: cgu_set_base_freq
*
* Purpose:
*
* Processing:
*     .
*
* Parameters:
*     i : Number
*
* Outputs: None
*
* Returns:
*
* Notes: None
*
**********************************************************************/
void __section (.bootstrap) cgu_set_base_freq(CGU_DOMAIN_ID_T baseid, u32 fin_sel)
{
  u32 baseSCR;

  /* Switch configuration register*/
  baseSCR = CGU_SB->base_scr[baseid] & ~CGU_SB_SCR_FS_MASK;
  /* If fs1 is currently enabled set refId to fs2 and enable fs2*/
  if (CGU_SB->base_ssr[baseid] & CGU_SB_SCR_EN1)
  {
    /* check if the selcted frequency is same as requested. If not switch.*/
    if (CGU_SB->base_fs1[baseid] != fin_sel)
    {
      CGU_SB->base_fs2[baseid] = fin_sel;

      /* Don't touch stop bit in SCR register*/
      CGU_SB->base_scr[baseid] = baseSCR | CGU_SB_SCR_EN2;
    }
  }
  else
  {
    /* check if the selcted frequency is same as requested. If not switch.*/
    if (CGU_SB->base_fs2[baseid] != fin_sel)
    {
      CGU_SB->base_fs1[baseid] = fin_sel;

      /* Don't touch stop bit in SCR register*/
      CGU_SB->base_scr[baseid] = baseSCR | CGU_SB_SCR_EN1;
    }
  }
}

/***********************************************************************
*
* Function: cgu_hpll_config
*
* Purpose:
*
* Processing:
*     .
*
* Parameters:
*     i : Number
*
* Outputs: None
*
* Returns:
*
* Notes: None
*
**********************************************************************/
void __section (.bootstrap) cgu_hpll_config(CGU_HPLL_ID_T pllid, CGU_HPLL_SETUP_T* pllsetup)
{
  CGU_HP_CFG_REGS* hppll;
  u32 switched_domains = 0;
  CGU_DOMAIN_ID_T domainId;

  /**********************************************************
  * switch domains connected to HPLL to FFAST automatically
  ***********************************************************/
  for (domainId = CGU_SB_BASE_FIRST; domainId < CGU_SB_NR_BASE; domainId++)
  {
    if (CGU_SB_SSR_FS_GET(CGU_SB->base_ssr[domainId]) ==
        (CGU_FIN_SELECT_HPPLL0 + pllid))
    {
      /* switch reference clock in to FFAST */
      cgu_set_base_freq(domainId, CGU_FIN_SELECT_FFAST);
      /* store the domain id to switch back to HPLL */
      switched_domains |= _BIT(domainId);
    }
  }

  /* get PLL regs */
  hppll = &CGU_CFG->hp[pllid];

  /* disable clock, disable skew enable, power down pll,
  * (dis/en)able post divider, (dis/en)able pre-divider,
  * disable free running mode, disable bandsel,
  * enable up limmiter, disable bypass
  */
  hppll->mode = CGU_HPLL_MODE_PD;

  /* Select fin */
  hppll->fin_select = pllsetup->fin_select;

  /* M divider */
  hppll->mdec = pllsetup->mdec & CGU_HPLL_MDEC_MASK;

  /* N divider */
  hppll->ndec = pllsetup->ndec & CGU_HPLL_NDEC_MSK;

  /* P divider */
  hppll->pdec = pllsetup->pdec & CGU_HPLL_PDEC_MSK;

  /* Set bandwidth */
  hppll->selr = pllsetup->selr;
  hppll->seli = pllsetup->seli;
  hppll->selp = pllsetup->selp;

  /* Power up pll */
  hppll->mode = (pllsetup->mode & ~CGU_HPLL_MODE_PD) | CGU_HPLL_MODE_CLKEN;

  /* store the estimated freq in driver data for future clk calcs */
  g_clkin_freq[CGU_FIN_SELECT_HPPLL0 + pllid] = pllsetup->freq;

  /* wait for PLL to lock */
  while ((hppll->status & CGU_HPLL_STATUS_LOCK) == 0);

  /**********************************************************
  * switch domains back to HPLL
  ***********************************************************/
  for (domainId = CGU_SB_BASE_FIRST; domainId < CGU_SB_NR_BASE; domainId++)
  {
    if (switched_domains & _BIT(domainId))
    {
      /* switch reference clock in to HPLL */
      cgu_set_base_freq(domainId, CGU_FIN_SELECT_HPPLL0 + pllid);
    }
  }

}

/***********************************************************************
*
* Function: cgu_clk_set_exten
*
* Purpose:
*
* Processing:
*     .
*
* Parameters:
*     i : Number
*
* Outputs: None
*
* Returns:
*
* Notes: None
*
**********************************************************************/
void __section (.bootstrap) cgu_clk_set_exten(CGU_CLOCK_ID_T clkid, u32 enable)
{
  switch (clkid)
  {
    case CGU_SB_OTP_PCLK_ID:
    case CGU_SB_PCM_APB_PCLK_ID:
    case CGU_SB_EVENT_ROUTER_PCLK_ID:
    case CGU_SB_ADC_PCLK_ID:
    case CGU_SB_IOCONF_PCLK_ID:
    case CGU_SB_CGU_PCLK_ID:
    case CGU_SB_SYSCREG_PCLK_ID:
    case CGU_SB_DMA_CLK_GATED_ID:
    case CGU_SB_SPI_PCLK_GATED_ID:
    case CGU_SB_SPI_CLK_GATED_ID:
    case CGU_SB_PCM_CLK_IP_ID:
    case CGU_SB_PWM_PCLK_REGS_ID:
    //case CGU_SB_NANDFLASH_ECC_CLK_ID:
      if (enable)
      {
        CGU_SB->clk_pcr[clkid] |= CGU_SB_PCR_EXTEN_EN;
      }
      else
      {
        CGU_SB->clk_pcr[clkid] &= ~CGU_SB_PCR_EXTEN_EN;
      }
      break;
      /* force disable for the following clocks */
    case CGU_SB_I2C0_PCLK_ID:
    case CGU_SB_I2C1_PCLK_ID:
    case CGU_SB_WDOG_PCLK_ID:
    case CGU_SB_UART_APB_CLK_ID:
    case CGU_SB_LCD_PCLK_ID:
      CGU_SB->clk_pcr[clkid] &= ~CGU_SB_PCR_EXTEN_EN;
      break;
    default:
      break;
  }
}

/***********************************************************************
*
* Function: cgu_get_clk_freq
*
* Purpose:
*
* Processing:
*     .
*
* Parameters:
*     i : Number
*
* Outputs: None
*
* Returns:
*
* Notes: None
*
**********************************************************************/
u32 __section (.bootstrap) cgu_get_clk_freq(CGU_CLOCK_ID_T clkid)
{
  u32 freq = 0;
  CGU_DOMAIN_ID_T domainId;
  u32 subDomainId;
  int n, m;
  u32 fdcVal;

  /* get domain and frac div info for the clock */
  cgu_ClkId2DomainId(clkid, &domainId, &subDomainId);

  /* get base frequency for the domain */
  freq = g_clkin_freq[CGU_SB_SSR_FS_GET(CGU_SB->base_ssr[domainId])];

  /* direct connection  has no fraction divider*/
  if (subDomainId == CGU_INVALID_ID)
  {
    return freq;
  }
  /* read frac div control register value */
  fdcVal = CGU_SB->base_fdc[subDomainId];

  if (fdcVal & CGU_SB_FDC_RUN) /* Is the fracdiv enabled ?*/
  {
    /* Yes, so reverse calculation of madd and msub */
    {
      int msub, madd;

      if (subDomainId != CGU_SB_BASE7_FDIV_LOW_ID)
      {
        msub = CGU_SB_FDC_MSUB_GET(fdcVal);
        madd = CGU_SB_FDC_MADD_GET(fdcVal);
      }
      else
      {
        msub = CGU_SB_FDC17_MSUB_GET(fdcVal);
        madd = CGU_SB_FDC17_MADD_GET(fdcVal);
      }

      /* remove trailing zeros */
      while (!(msub & 1)  && !(madd & 1))
      {
        madd = madd >> 1;
        msub = msub >> 1;
      }
      /* compute m and n values */
      n = - msub;
      m = madd + n;
    }
    /* check m and n are non-zero values */
    if ((n == 0) || (m == 0))
    {
      return 0;
    }
    /* calculate the frequency based on m and n values */
    freq = (freq * n) / m ;
  }
  /* else There is no fractional divider in the clocks path */

  return  freq;
}

/***********************************************************************
*
* Function: cgu_set_subdomain_freq
*
* Purpose:
*
* Processing:
*     .
*
* Parameters:
*     i : Number
*
* Outputs: None
*
* Returns:
*
* Notes: None
*
**********************************************************************/
void __section (.bootstrap) cgu_set_subdomain_freq(CGU_CLOCK_ID_T clkid, CGU_FDIV_SETUP_T fdiv_cfg)
{
  CGU_DOMAIN_ID_T domainId;
  u32 subDomainId, base_freq, bcrId;

  /* get domain and frac div info for the clock */
  cgu_ClkId2DomainId(clkid, &domainId, &subDomainId);

  /* direct connection  has no fraction divider*/
  if (subDomainId != CGU_INVALID_ID)
  {
    /* store base freq */
    base_freq = CGU_SB_SSR_FS_GET(CGU_SB->base_ssr[domainId]);
    /* switch domain to FFAST */
    cgu_set_base_freq(domainId, CGU_FIN_SELECT_FFAST);
    /* check if the domain has a BCR*/
    bcrId = cgu_DomainId2bcrid(domainId);
    /* disable all BCRs */
    if (bcrId != CGU_INVALID_ID)
    {
      CGU_SB->base_bcr[bcrId] = 0;
    }
    /* change fractional divider */
    cgu_fdiv_config(subDomainId, fdiv_cfg, 1, 0);
    /* enable BCRs */
    if (bcrId != CGU_INVALID_ID)
    {
      CGU_SB->base_bcr[bcrId] = CGU_SB_BCR_FD_RUN;
    }
    /* switch domain to original base frequency */
    cgu_set_base_freq(domainId, base_freq);
  }
}

/***********************************************************************
*
* Function: cgu_soft_reset_module
*
* Purpose:
*
* Processing:
*     .
*
* Parameters:
*     i : Number
*
* Outputs: None
*
* Returns:
*
* Notes: None
*
**********************************************************************/
void __section (.bootstrap) cgu_soft_reset_module(CGU_MOD_ID_T modId)
{
  volatile u32 i;

  /* clear and set the register */
  CGU_CFG->resetn_soft[modId] = 0;
  /* introduce some delay */
  for (i = 0;i < 1000;i++);

  CGU_CFG->resetn_soft[modId] = CGU_CONFIG_SOFT_RESET;
}

int cmd_clock (int argc, const char** argv)
{
	u32 clk_id = CGU_SYS_FIRST;
	u32 end_id = (CGU_SYSCLK_O_LAST + 1);
	char* str[2] = { "OFF", " ON"}; 

	if (argc > 1) {

		clk_id = simple_strtoul (argv[1], NULL, 0);
		end_id = clk_id + 1;
	}

	while (clk_id < end_id)
	{
		printf ("clock[%02d] %s(PSR)/%s(PCR) : %d\r\n", clk_id, 
			str[(CGU_SB->clk_psr[clk_id] & 0x1)], 
			str[(CGU_SB->clk_pcr[clk_id] & 0x1)], 
			cgu_get_clk_freq(clk_id));
		clk_id++;
	}

	return 0;
}

static __command struct command_d c_clk = {
	.command = "clock",
	.func = cmd_clock,
	COMMAND_DESCRIPTION ("Get clock speed")
	COMMAND_HELP (
	"clock [clk_id]\n"
	" clk_id - Clock index 0 to 91\n"
	" Dumps all clock speeds if nor parameter is passed.\n"
	"\n")
};
