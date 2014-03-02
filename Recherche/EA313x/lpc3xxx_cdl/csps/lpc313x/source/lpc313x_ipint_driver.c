/***********************************************************************
* $Id:: lpc313x_ipint_driver.c 1529 2009-01-06 02:08:40Z pdurgesh      $
*
* Project: LPC313X IPINT controller implementation
*
* Description:
*     This file contains the implementation method and some test
*     constants for the LPC313X chip family component:
*         Pulse Code Modulation (PCM) & ISDN Oriented Modular (IOM)
*         Interface (IPINT)
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

#include "lpc313x_ipint_driver.h"
#include "lpc313x_intc_driver.h"
#include "lpc313x_cgu_driver.h"
#include "lpc313x_sysreg.h"

/* Device Configuration Structure Type */
typedef struct
{
  BOOL_32 		init;		/* Flag to check whether initialising 	*/
  IPINT_REGS_T   	*regptr;	/* Pointer to IPINT module registers 	*/
} IPINT_CFG_T;

/* Global variable for IPINT Configuration structure Type 	*/
static IPINT_CFG_T ipintdat = {FALSE, NULL};

/***********************************************************************
 *
 * Function: ipint_open
 *
 * Purpose: Open the IPINT module
 *
 * Processing:
 *     If the passed register base is valid and the IPINT
 *     driver isn't already initialized, then setup the IPINT
 *     into a initialized state specified in arg parameter. Return
 *     a pointer to the driver's data structure or NULL if driver
 *     initialization failed.
 *
 * Parameters:
 *     ipbase: Pointer to a IPINT peripheral block
 *     arg   : structure pointer
 *
 * Outputs: None
 *
 * Returns: The pointer to a IPINT config structure or NULL
 *
 * Notes: None
 *
 **********************************************************************/
INT_32 ipint_open(void *ipbase, INT_32 arg)
{
  INT_32 tptr = (INT_32) NULL;
  /* cast the arg address to IPINT_COMPOSITE_T structure type */
  IPINT_COMPOSITE_T *parg = (IPINT_COMPOSITE_T*)arg;
  UNS_32 error_code = ipint_init(parg->type, parg->pin,
                                 parg->fsc, parg->dck, parg->en_slots_used, parg->dir_slots_used, parg->loop_mode);
  /* if no error, get the address of the ipintdata */
  if (error_code == IPINT_OK)
  {
    tptr = (INT_32) & ipintdat;
  }
  /* return the address of ipintdata */
  return tptr;
}
/***********************************************************************
 *
 * Function: ipint_close
 *
 * Purpose: Close the IPINT
 *
 * Processing:
 *
 *
 * Parameters:
 *     unitId: ID number for IPINT module (==0)
 *
 * Outputs: None
 *
 * Returns: The status of the close operation
 *
 * Notes: None
 *
 **********************************************************************/
UNS_32 ipint_close(INT_32 unitId)
{
  return ipint_deinit();
}
/***********************************************************************
 *
 * Function: ipint_init
 *
 * Purpose:
 *		This function is used to initialise the IPINT. The application
 * 		programmer has the ability to setup the IPINT for different
 * 		configurations.
 * Processing:
 *		check the initialising flag and if have not initialised, assign the
 *		pointer to IPINT Base Address and set IPINT registers to value passing
 *
 * Parameters:
 *     	type: device type is master or slave
 *		pin: open-drain or push-pull
 *		fsc: data frame sync type
 *		dck: data shift out clock
 *		slots_used : the slots used on IPINT bus,refer to slots_used definition
 * Outputs: None
 *
 * Returns:
 *      Returns an error status code
 *      IPINT_OK :In case of success
 *      IPINT_ERR_INIT_FAILED : initialisation failed
 *      IPINT_ERR_BAD_PARAMETER : In case the input device type is invalid.
 *      IPINT_ERR_RESOURCE_OWNED :If IPINT is already initialised
 * Notes: None
 *
 **********************************************************************/


UNS_32 ipint_init(IpInt_DevType_t type, IpInt_DevPinType_t pin,
                  IpInt_FSCType_t fsc, IpInt_DCKFreq_t dck,
                  UNS_32 en_slots_used, UNS_32 dir_slots_used, UNS_32 loop_mode)
{
  UNS_32 errorStatus = IPINT_OK;
  SYSCREG_REGS_T *pSysRegBase = SYS_REGS;

  if (ipintdat.init == FALSE)
  {
    /* read the base address */
    ipintdat.regptr = (IPINT_REGS_T *)IPINT_BASE;
    /* check for errors */
    if ((ipintdat.regptr == NULL) /*(IPINT_REGS_T *)VHNOBASE) ||
			(ipintdat.regptr == (IPINT_REGS_T *)VHCIERROR)*/)
    {
      errorStatus = IPINT_ERR_INIT_FAILED;
    }
    else
    {
      /* Enable the CLK_IP and VPB_PCLK clk for the IPINT module:  	*/
      /* PCLK	: clk for DMA transfer (will be turned later)       */
      /* VPB_CLK: access IPINT registers							*/
      /* CLK_IP	: synchronyze IPINT data(24MHz, init in ClkInitDomain*/

      cgu_clk_en_dis(CGU_SB_PCM_PCLK_ID, TRUE);
      cgu_clk_en_dis(CGU_SB_PCM_APB_PCLK_ID, TRUE);
      cgu_clk_en_dis(CGU_SB_PCM_CLK_IP_ID, TRUE);

      /* Set device to normal mode  	*/
      ipintdat.regptr->global = 0;
      ipintdat.regptr->global |= 0x00000004;

      /* Switch IPINT signals to IC's pin, replace for DAO signals */
      pSysRegBase->mux_dao_ipint_sel = 0x01;

      if (type == MASTER)
      {
        //CNTL0 register
        ipintdat.regptr->cntl0 |= IPINT_Dev_Master | pin | dck | fsc | loop_mode;
        //CNTL1 register ->enable slots
        ipintdat.regptr->cntl1 = en_slots_used;
        //CNTL2 register ->slot direction
        ipintdat.regptr->cntl2 = dir_slots_used;
        ipintdat.init = TRUE;
      }
      else if (type == SLAVE)
      {
        //CNTL0 register
        ipintdat.regptr->cntl0 &= IPINT_Dev_Slave;
        ipintdat.regptr->cntl0 |= pin | dck | fsc;
        //CNTL1 register ->enable slots
        ipintdat.regptr->cntl1 = en_slots_used;
        //CNTL2 register ->slot direction
        ipintdat.regptr->cntl2 = dir_slots_used;
        ipintdat.init = TRUE;
      }
      else
      {
        errorStatus = IPINT_ERR_BAD_PARAMETER;
      }
    }
  }
  else
  {
    errorStatus = IPINT_ERR_RESOURCE_OWNED;
  }

  return errorStatus;
}

/***********************************************************************
 *
 * Function:	ipint_deinit
 *
 * Purpose:
 *			close the IPINT module, not use IPINT function anymore
 * Processing:
 *			de-activate the module, stop feeding clock, restore pin
 *			function to function 0 for pins.
 *
 * Parameters:
 *
 *
 * Outputs: None
 *
 * Returns:	Returns an error status code
 *			IPINT_OK always
 * Notes: None
 *
 **********************************************************************/
UNS_32 ipint_deinit(void)
{
  UNS_8 i;
  SYSCREG_REGS_T *pSysRegBase = SYS_REGS;

  ipintdat.regptr->global = 0x00000000;
  ipintdat.regptr->cntl0 = 0x00000000;
  ipintdat.regptr->cntl1 = 0x00000000;
  ipintdat.regptr->cntl2 = 0x00000000;
  for (i = 0;i < 6;i++)
    ipintdat.regptr->hpout[i] = 0x00000000;
  /* Clock switch to be disable */
  cgu_clk_en_dis(CGU_SB_PCM_PCLK_ID, FALSE);
  cgu_clk_en_dis(CGU_SB_PCM_APB_PCLK_ID, FALSE);
  cgu_clk_en_dis(CGU_SB_PCM_CLK_IP_ID, FALSE);
  /* the basis function restore for pins */
  pSysRegBase->mux_dao_ipint_sel = 0x00;

  ipintdat.regptr = (IPINT_REGS_T *)NULL;

  ipintdat.init = FALSE;

  return IPINT_OK;
}

/***********************************************************************
 *
 * Function:	ipint_hpout
 *
 * Purpose:
 *
 * Processing:
 *
 *
 * Parameters:
 *         num : the HpIn register number
 *         data : data used to copy to HpOut register
 *
 * Outputs: None
 *
 * Returns: Returns an error status code
 *          IPINT_OK in case of success
 *          IPINT_ERR_BAD_PARAMETER : bad parameter
 *         	IPINT_ERR_NOT_INITIALIZED : In case the driver is not initialised.
 * Notes: None
 *
 **********************************************************************/
UNS_32 ipint_hpout(UNS_8 num, UNS_16 data)
{
  UNS_32 errorStatus = IPINT_OK;
  if (ipintdat.init == FALSE)
  {
    errorStatus = IPINT_ERR_NOT_INITIALIZED;
  }
  else
  {
    if (num >= 6) errorStatus = IPINT_ERR_BAD_PARAMETER;
    else
      ipintdat.regptr->hpout[num] = data;
  }
  return errorStatus;
}


/***********************************************************************
 *
 * Function:	ipint_hpin
 *
 * Purpose:
 *			write the data point by data to hpin numth register
 * Processing:
 *
 *
 * Parameters:
 *           num : the HpIn register number
 *           data : pointer to store HpIn content
 *
 * Outputs: None
 *
 * Returns: Returns an error status code
 *          IPINT_OK in case of success
 *          IPINT_ERR_BAD_PARAMETER : bad parameter
 *         	IPINT_ERR_NOT_INITIALIZED : In case the driver is not initialised.
 * Notes: None
 *
 **********************************************************************/
UNS_32 ipint_hpin(UNS_8 num, UNS_16 *data)
{
  UNS_32 errorStatus = IPINT_OK;
  if (ipintdat.init == FALSE)
  {
    errorStatus = IPINT_ERR_NOT_INITIALIZED;
  }
  else
  {
    if (num >= 6) errorStatus = IPINT_ERR_BAD_PARAMETER;
    else
      *data = (UNS_16)ipintdat.regptr->hpin[num];
  }
  return errorStatus;
}

/***********************************************************************
 *
 * Function:	ipint_start
 *
 * Purpose:
 * 				begin to activate the IPINT module, the signal send to pins
 *
 * Processing:
 *				if IPINT module have been initialised, write the START status
 *				to the global register.
 *
 * Parameters:
 *
 *
 * Outputs: None
 *
 * Returns:	Returns an error status code
 *          IPINT_OK in case of success
 *         	IPINT_ERR_NOT_INITIALIZED : In case the driver is not initialised.
 * Notes: None
 *
 **********************************************************************/
UNS_32 ipint_start(void)
{
  UNS_32 errorStatus = IPINT_OK;

  if (ipintdat.init == FALSE)
    errorStatus = IPINT_ERR_NOT_INITIALIZED;
  else
    ipintdat.regptr->global |= IPINT_Start;

  return errorStatus;
}

/***********************************************************************
 *
 * Function:ipint_stop
 *
 * Purpose:
 * 		De-activate the IPINT module, nothing send to outside
 *
 * Processing:
 *		if the IPINT module have been initialised, write to
 *		global register the STOP status.
 *
 * Parameters:
 *
 *
 * Outputs: None
 *
 * Returns:	Returns an error status code
 *         IPINT_OK in case of success
 *         IPINT_ERR_NOT_INITIALIZED : In case the driver is not initialised.
 * Notes: None
 *
 **********************************************************************/
UNS_32 ipint_stop(void)
{
  UNS_32 errorStatus = IPINT_OK;

  if (ipintdat.init == FALSE)
    errorStatus = IPINT_ERR_NOT_INITIALIZED;
  else
    ipintdat.regptr->global &= IPINT_Stop;

  return errorStatus;
}
