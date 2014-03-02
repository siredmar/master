/***********************************************************************
 * $Id:: lpc313x_adc_driver.c 1310 2008-11-04 01:47:18Z jmiirazabal $
 *
 * Project: LPC313x standard 10-bit ADC driver
 *
 * Description:
 *     This file contains driver support for the LPC313x 10-bitADC
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
 *********************************************************************/

#include "lpc313x_adc10b_driver.h"
#include "lpc313x_intc_driver.h"
#include "lpc313x_cgu_driver.h"

/* Conversion Done flag */
volatile INT_32 convdone = 0;

/* Counters - for test purpose */
volatile INT_32 adc_int_conv_ctr = 0;
INT_32 adc_polled_conv_ctr = 0;

/***********************************************************************
 * ADC driver private data and types
 **********************************************************************/


/* ADC device configuration structure */
STATIC ADC_CFG_T adccfg;

/***********************************************************************
 * ADC driver public functions
 **********************************************************************/

/***********************************************************************
 *
 * Function: adc_default
 *
 * Purpose: Places the ADC interface and controller in a default state
 *
 * Processing:
 *     Setup default state of ADC:
 *     - Vref0 selected
 *     - Single conversion mode selected
 *     - ADC enabled
 *
 * Parameters:
 *     adcregs: Pointer to an ADC register set
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
STATIC void adc_default(ADC_REGS_T *adcregs)
{

  UNS_32 dummy;
  /* Setup default state of ADC:
    - ADC enabled
  - Vrefp0 selected
  - Single Conversion mode selected */

  /* set ADC to default state */
  adcregs->adc_con_reg = ADC_CON_DEFAULT_STATE;
  adcregs->adc_csel_reg = ADC_CSEL_DEFAULT_STATE;
  adcregs->adc_int_enable_reg = ADC_INT_EN_DEFAULT_STATE;
  adcregs->adc_int_clear_reg |= (ADC_INT_CLEAR);
  /* Dummy Interrupt Status Register Read */
  dummy = adcregs->adc_int_status_reg ;
  dummy = dummy;	/* To get rid of a warning */
  while (adcregs->adc_int_status_reg & 0x01);

  /* Enable ADC Interrupt */
  adcregs->adc_int_enable_reg |= ADC_INT_ENABLE;

  /* Select Vrefp0 */
  adcregs->adc_con_reg |= ADC_SELREF;
  /* Note JMI: setting this bit to 1 or 0 doesn't seem to do much
     Check with MCU folks what Vrefp0 and Vrefp1 are */

  /* set ADC power up */
  adcregs->adc_con_reg |= ADC_ENABLE;
}


/***********************************************************************
 * ADC driver public functions
 **********************************************************************/

/***********************************************************************
 *
 * Function: adc_open
 *
 * Purpose: Open the ADC
 *
 * Processing:
 *     If init is not FALSE, return 0x00000000 to the caller. Otherwise,
 *     set init to TRUE, save the ADC peripheral register set address,
 *     and initialize the ADC interface and controller to a default
 *     state by calling adc_default(), and return a pointer to the ADC
 *     config structure to the caller.
 *
 * Parameters:
 *     ipbase: ADC descriptor device address
 *     arg   : Not used
 *
 * Outputs: None
 *
 * Returns: The pointer to a ADC config structure or 0
 *
 * Notes: None
 *
 **********************************************************************/
INT_32 adc_open(void *ipbase,
                INT_32 arg)
{
  INT_32 status = 0;

  if ((adccfg.init == FALSE) && ((ADC_REGS_T *) ipbase == ADC))
  {
    /* Device is valid and not previously initialized */
    adccfg.init = TRUE;

    /* Save and return address of peripheral block */
    adccfg.regptr = (ADC_REGS_T *) ipbase;

    /* Enable ADC system clock */
    cgu_clk_en_dis(CGU_SB_ADC_CLK_ID, TRUE);
    cgu_clk_en_dis(CGU_SB_ADC_PCLK_ID, TRUE);

    /* Place ADC in a default state */
    adc_default(adccfg.regptr);

    /* Return pointer to ADC configuration structure */
    status = (INT_32) & adccfg;
  }
  return status;
}

/***********************************************************************
 *
 * Function: adc_close
 *
 * Purpose: Close the ADC
 *
 * Processing:
 *     If init is not TRUE, then return _ERROR to the caller as the
 *     device was not previously opened. Otherwise, disable the ADC
 *     interface and controller by setting the control register
 *     to its default state, and return _NO_ERROR to the caller.
 *
 * Parameters:
 *     devid: Pointer to ADC config structure
 *
 * Outputs: None
 *
 * Returns: The status of the close operation
 *
 * Notes: None
 *
 **********************************************************************/
STATUS adc_close(INT_32 devid)
{
  ADC_CFG_T *adccfgptr = (ADC_CFG_T *) devid;
  STATUS status = _ERROR;

  if (adccfgptr->init == TRUE)
  {
    /* Make sure there is no A to D conversion in progress */
    while (adccfgptr->regptr->adc_int_status_reg & 0x00000010);
    /* set ADC to default state */
    while (adccfgptr->regptr->adc_int_status_reg & 0x00000010);
    adccfgptr->regptr->adc_con_reg = ADC_CON_DEFAULT_STATE;
    adccfgptr->regptr->adc_csel_reg = ADC_CSEL_DEFAULT_STATE;
    adccfgptr->regptr->adc_int_enable_reg = ADC_INT_EN_DEFAULT_STATE;
    adccfgptr->regptr->adc_int_clear_reg |= (ADC_INT_CLEAR);
    /* Reset Conversion Done flag */
    convdone = 0;

    /* Disable ADC system clock */
    cgu_clk_en_dis(CGU_SB_ADC_CLK_ID, FALSE);
    cgu_clk_en_dis(CGU_SB_ADC_PCLK_ID, FALSE);

    status = _NO_ERROR;
    adccfgptr->init = FALSE;
  }
  return status;
}

/***********************************************************************
 *
 * Function: adc_ioctl
 *
 * Purpose: ADC configuration block
 *
 * Processing:
 *     This function is a large case block. Based on the passed function
 *     and option values, set or get the appropriate ADC parameter.
 *
 * Parameters:
 *     devid: Pointer to ADC config structure
 *     cmd:   ioctl command
 *     arg:   ioctl argument
 *
 * Outputs: None
 *
 * Returns: The status of the ioctl operation
 *
 * Notes: None
 *
 **********************************************************************/
STATUS adc_ioctl(INT_32 devid,
                 INT_32 cmd,
                 INT_32 arg)
{
  ADC_REGS_T *adcregsptr;
  ADC_CFG_T *adccfgptr = (ADC_CFG_T *) devid;
  STATUS status = _ERROR;

  if (adccfgptr->init == TRUE)
  {
    status = _NO_ERROR;
    adcregsptr = adccfgptr->regptr;

    switch (cmd)
    {
      case ADC_SELECT_VREFP0:
        /*   Select Vrefp0 ad reference voltage input */
        adcregsptr->adc_con_reg |= (ADC_SELREF);
        break;

      case ADC_SELECT_CONTINUOUS_SCAN_MODE:
        /*  Select Continuous Scan mode */
        adcregsptr->adc_con_reg |= (ADC_CSCAN);
        break;

      case ADC_SELECT_SINGLE_CONV_MODE:
        /* Select single conversion mode */
        /* Make sure there is no pending ADC interrupt */
        if ((adcregsptr->adc_int_status_reg  & 0x00000001))
        {
          /* If yes, clear it */
          adcregsptr->adc_int_clear_reg |= (ADC_INT_CLEAR);
          while (adcregsptr->adc_int_status_reg  & 0x00000001);
        }
        /* Select Single Conversion Mode */
        adcregsptr->adc_con_reg &= (ADC_SINGLE_CONV);
        break;

      case ADC_START_CONVERSION:
        /* Start the A to D conversion */
        convdone = 0;	/* Clear the Conversion Done Flag */
        adc_int_conv_ctr = 0;
        adc_polled_conv_ctr = 0;
        adcregsptr->adc_con_reg |= (ADC_START);
        break;

      case ADC_STOP_CONVERSION:
        /* Stop the ADC conversion */
        /* Make sure there is no pending ADC interrupt */
        if ((adcregsptr->adc_int_status_reg  & 0x00000001))
        {
          /* If yes, clear it */
          adcregsptr->adc_int_clear_reg |= (ADC_INT_CLEAR);
          while (adcregsptr->adc_int_status_reg  & 0x00000001);
        }
        adcregsptr->adc_con_reg &= (ADC_STOP);
        break;

      case ADC_SELECT_CHANNEL_0:
        /* Select Channel 0 and resolution ( = arg) for this channel */
        adcregsptr->adc_csel_reg |= (ADC_CHSEL_RES(0, arg));
        break;

      case ADC_SELECT_CHANNEL_1:
        /* Select Channel 1 and resolution ( = arg) for this channel */
        adcregsptr->adc_csel_reg |= (ADC_CHSEL_RES(1, arg));
        break;

      case ADC_SELECT_CHANNEL_2:
        /* Select Channel 2 and resolution ( = arg) for this channel */
        adcregsptr->adc_csel_reg |= (ADC_CHSEL_RES(2, arg));
        break;

      case ADC_SELECT_CHANNEL_3:
        /* Select Channel 3 and resolution ( = arg) for this channel */
        adcregsptr->adc_csel_reg |= (ADC_CHSEL_RES(3, arg));
        break;

      default:
        /* Unsupported parameter */
        status = SMA_BAD_PARAMS;
    }
  }
  return status;
}

/***********************************************************************
 *
 * Function: adc_read_registers
 *
 * Purpose: Move data from the ADC DAT to the rx buffer
 *
 * Processing:
 *     While there is ADC Data, copy an entry from the ADC DAT into
 *     the ADC driver rx buffer.
 *
 * Parameters:
 *     adccfgptr: Pointer to ADC config structure
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
STATIC void adc_read_registers(ADC_CFG_T *adccfgptr)
{
  INT_32 dummy_ctr = 0;
  UNS_32 dummy;

  ADC_REGS_T *adcregsptr = (ADC_REGS_T *) adccfgptr->regptr;

  /* Read ADC Status interrupt */
  dummy = adcregsptr->adc_int_status_reg;
  dummy = dummy;		/* To get rid of a warning */
  /* Clear Interrupt */
  adcregsptr->adc_int_clear_reg |= (ADC_INT_CLEAR);
  convdone = 1;
  adc_int_conv_ctr++;
  /* read in the 10 bit value of the converted channel */
  if (adcregsptr->adc_csel_reg & 0x0000000F)
    adccfgptr->rx[0] = (UNS_16) adcregsptr->adc_r0_reg & ADCDAT_VALUE_MASK;
  if (adcregsptr->adc_csel_reg & 0x000000F0)
    adccfgptr->rx[1] = (UNS_16) adcregsptr->adc_r1_reg & ADCDAT_VALUE_MASK;
  if (adcregsptr->adc_csel_reg & 0x00000F00)
    adccfgptr->rx[2] = (UNS_16) adcregsptr->adc_r2_reg & ADCDAT_VALUE_MASK;
  if (adcregsptr->adc_csel_reg & 0x0000F000)
    adccfgptr->rx[3] = (UNS_16) adcregsptr->adc_r3_reg & ADCDAT_VALUE_MASK;

  for (dummy_ctr = 0; dummy_ctr < 1000 ; dummy_ctr ++);
  /* Note JMI: I don't really understand this but that dummy
   counter seems to be required.
   If not implemented, the ISR is executed twice per conversion */

  /* wait until the Interrupt Status bit is cleared */
  while (adcregsptr->adc_int_status_reg  & 0x00000001);
}

/***********************************************************************
 *
 * Function: poll_adc
 *
 * Purpose: poll ADC and move data from the ADC DAT to the rx buffer
 *          when conversion is done
 *
 * Processing:
 *     Check for conversion status and wait until conversion done.
 *     While there is ADC Data, copy an entry from the ADC DAT into
 *     the ADC driver rx buffer.
 *
 * Parameters:
 *     devid: Pointer to ADC config structure
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void poll_adc(INT_32 devid)
{
  ADC_REGS_T *adcregsptr;
  ADC_CFG_T *adccfgptr = (ADC_CFG_T *) devid;
  adcregsptr = adccfgptr->regptr;
  /* Wait until a conversion is done*/
  while (adcregsptr->adc_int_status_reg == 0);
  /* Clear Interrupt */
  adcregsptr->adc_int_clear_reg |= (ADC_INT_CLEAR);
  convdone = 1;
  adc_polled_conv_ctr++;
  /* read in the 10 bit value of the converted channel */
  if (adcregsptr->adc_csel_reg & 0x0000000F)
    adccfgptr->rx[0] = (UNS_16) adcregsptr->adc_r0_reg & ADCDAT_VALUE_MASK;
  if (adcregsptr->adc_csel_reg & 0x000000F0)
    adccfgptr->rx[1] = (UNS_16) adcregsptr->adc_r1_reg & ADCDAT_VALUE_MASK;
  if (adcregsptr->adc_csel_reg & 0x00000F00)
    adccfgptr->rx[2] = (UNS_16) adcregsptr->adc_r2_reg & ADCDAT_VALUE_MASK;
  if (adcregsptr->adc_csel_reg & 0x0000F000)
    adccfgptr->rx[3] = (UNS_16) adcregsptr->adc_r3_reg & ADCDAT_VALUE_MASK;

  /* wait until the Interrupt Status bit is cleared */
  while (adcregsptr->adc_int_status_reg  & 0x00000001);
}

/***********************************************************************
 *
 * Function: adc_interrupt
 *
 * Purpose: ADC interrupt handler
 *
 * Processing:
 *     On an interrupt call the adc_read_registers() to move
 *     the data from the ADC data register to the ADC driver rx buffer.
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
void adc_interrupt_handler(void)
{
  /* Read the ADC result into a buffer */
  adc_read_registers(&adccfg);
}


