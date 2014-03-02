/***********************************************************************
 * $Id:: s1l_sys.c 3364 2010-05-03 20:14:18Z usb10132                  $
 *
 * Project: Miscellaneous functions
 *
 * Description:
 *     Miscellaneous functions
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

#include "lpc_string.h"
#include "lpc_arm922t_arch.h"
#include "s1l_sys.h"
#include "s1l_sys_inf.h"

S1L_VSYSCFG_T sysinfo;
S1L_CFG_T syscfg;
EXCEPT_DATA_T excdata;
UNS_8 secdat[2048 + 64];
BOOL_32 menuexit;

static UNS_8 undef_msg[] = "Undefined instruction";
static UNS_8 dabort_msg[] = "Data abort";
static UNS_8 excunk_msg[] = "Unknown exception";
static UNS_8 excty_msg[] = "Exception: ";
static UNS_8 spsr_msg[] = " CPSR value at exception: ";
static UNS_8 lr_msg[]   = " Exception address      : ";
static UNS_8 rx_msg[]   = " Register value :";
static UNS_8 rx1_msg[]   = " (r";
static UNS_8 rx2_msg[]   = ")";
static UNS_8 sd_msg[] = "System is down. Reset system";

/***********************************************************************
 *
 * Function: cfg_default
 *
 * Purpose: Sets default system configuration
 *
 * Processing:
 *     Sets the system configuration to the defaults.
 *
 * Parameters:
 *     pCfg : Pointer to config structure to populate with defaults
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void cfg_default(S1L_CFG_T *pCfg)
{
	pCfg->prmpt_to = 50000;
	pCfg->baudrate = 115200;
	str_copy(pCfg->prmpt, sysinfo.sysrtcfg.default_prompt);
	pCfg->aboot.abootsrc = SRC_NONE;
	pCfg->fsave.valid = FALSE;
	pCfg->scr.enabled = FALSE;

	/* Also reset user data */
	cfg_user_reset();
}

/***********************************************************************
 *
 * Function: conv_to_sector
 *
 * Purpose: Converts a block/page to a sector number
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     block : Block number
 *     page  : Page number
 *
 * Outputs: None
 *
 * Returns: The sector number for the passed block and page
 *
 * Notes: None
 *
 **********************************************************************/
UNS_32 conv_to_sector(UNS_32 block,
					  UNS_32 page)
{
	return (page + (sysinfo.nandgeom->pages_per_block * block));
}

/***********************************************************************
 *
 * Function: trap_exception
 *
 * Purpose: Displays exception data when an exception occurs
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
void trap_exception(void)
{
	UNS_8 str [16], *pstr;
	int idx;

	term_dat_out_crlf((UNS_8 *) "");

	/* Determine exception type */
	switch (excdata.cpsr & 0x0F)
	{
		case 0xB:
			/* Undefined instruction */
			pstr = undef_msg;
			break;

		case 0x7:
			/* Data abort */
			pstr = dabort_msg;
			break;

		default:
			/* Unknown */
			pstr = excunk_msg;
	}

	/* Exception type */
	term_dat_out(excty_msg);
	term_dat_out_crlf(pstr);

	/* CPSR and exception address */
	term_dat_out(spsr_msg);
	str_makehex(str, excdata.spsr, 8);
	term_dat_out_crlf(str);
	term_dat_out(lr_msg);
	str_makehex(str, excdata.lr, 8);
	term_dat_out_crlf(str);

	/* Dump registers */
	for (idx = 0; idx <= 13; idx++)
	{
		term_dat_out(rx_msg);
		str_makehex(str, excdata.r[idx], 8);
		term_dat_out(str);
		term_dat_out(rx1_msg);
		str_makedec(str, (UNS_32) idx);
		term_dat_out(str);
		term_dat_out_crlf(rx2_msg);
	}

	term_dat_out_crlf(sd_msg);

	while (1);
}
