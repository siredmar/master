/***********************************************************************
* $Id:: lpc313x_adc.h 1180 2008-09-11 19:32:40Z jmirazabal             $
*
* Project: LPC313X 10bit ADC definitions
*
* Description:
*     This file contains the structure definitions and manifest
*     constants for the LPC313X chip family component:
*         10-bit Analog Digital Converter (ADC)
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

#ifndef LPC313X_ADC_H
#define LPC313X_ADC_H

#include "lpc_types.h"
#include "lpc313x_chip.h"

#ifdef __cplusplus
extern "C"
{
#endif

/***********************************************************************
* 10-bit ADC Register Structures
**********************************************************************/

/* 10-bit ADC Module Register Structure */
typedef volatile struct
{
  volatile const UNS_32 adc_r0_reg;				/* Digital conversion Channel 0*/
  volatile const UNS_32 adc_r1_reg;				/* Digital conversion Channel 1*/
  volatile const UNS_32 adc_r2_reg;				/* Digital conversion Channel 2*/
  volatile const UNS_32 adc_r3_reg;				/* Digital conversion Channel 3*/
  volatile const UNS_32 unused1[4];				/* Unused registers */
  volatile UNS_32 adc_con_reg;     				/* ADC operation modes and status */
  volatile UNS_32 adc_csel_reg;					/* ADC input channel selection and resolution */
  volatile UNS_32 adc_int_enable_reg;			/* ADC interrupt request generation */
  volatile const UNS_32 adc_int_status_reg;		/* ADC interrupt status */
  volatile UNS_32 adc_int_clear_reg;			/* ADC interrupt clear */
  volatile const UNS_32 unused2[1];				/* Unused register */

} ADC_REGS_T;

#define ADCDAT_VALUE_MASK           0x000003FF

/**********************************************************************
* ADC_CON_REG register definitions
**********************************************************************/
/* Bit for monitoring conversion status */
#define ADC_STATUS					_BIT(4)
/* Bit for starting the ADC conversion */
#define ADC_START					_BIT(3)
/* Bit for stoping the ADC conversion */
#define ADC_STOP					0xFFFFFFF7
/* Bit for enabling continuous conversion scan */
#define ADC_CSCAN					_BIT(2)
/* Bit for enabling single conversion */
#define ADC_SINGLE_CONV				0xFFFFFFB
/* Bit for enabling the ADC */

#define ADC_ENABLE					_BIT(1)
/* Bit for selecting the reference voltage input */
#define ADC_SELREF					_BIT(0)

#define ADC_CON_DEFAULT_STATE		0

/**********************************************************************
* ADC_CSEL_RES_REG register definitions
**********************************************************************/
#define ADC_CHSEL_RES(n,m)			_SBF(4*n,m)		 /* n = channel (0,1,2,3) m = resolution (2 to 10) */
#define ADC_CSEL_DEFAULT_STATE		0

/**********************************************************************
* ADC_INT_ENABLE_REG register definitions
**********************************************************************/
/* Bit for contolling Interrupt Enable / Disable */
#define ADC_INT_ENABLE        		_BIT(0)
#define ADC_INT_EN_DEFAULT_STATE	0

/**********************************************************************
* ADC_INT_STATUS_REG register definitions
**********************************************************************/
/* Bit containing Interrupt Status */
#define ADC_INT_STATUS        		_BIT(0)

/**********************************************************************
* ADC_INT_CLEAR_REG register definitions
**********************************************************************/
/* Bit clearing interrupt status */
#define ADC_INT_CLEAR        		_BIT(0)

/**********************************************************************
* Macro to access 10-bit ADC registers
**********************************************************************/
#define ADC    ((ADC_REGS_T*)ADC_BASE)

#ifdef __cplusplus
}
#endif

#endif /* LPC313X_ADC_H */
