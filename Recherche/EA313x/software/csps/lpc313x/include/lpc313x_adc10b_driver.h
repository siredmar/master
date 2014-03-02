/***********************************************************************
 * $Id:: lpc313x_adc10b_driver.h 1180 2008-09-11 19:32:40Z jmirazabal      $
 *
 * Project: LPC3xxx standard 10-bit ADC driver
 *
 * Description:
 *     This file contains driver support for the LPC313X 10-bit ADC.
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

#ifndef LPC313X_ADC_DRIVER_H
#define LPC313X_ADC_DRIVER_H

#include "lpc313x_adc10b.h"
#include "lpc_params.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* Size of the ADC receive buffer */
#define ADC_BUFSIZE 4

/* ADC device configuration structure type */
typedef struct
{
  BOOL_32 init;       			/* Device initialized flag */
  ADC_REGS_T *regptr; 			/* Pointer to ADC registers */
  UNS_16 rx[ADC_BUFSIZE];  		/* ADC data buffer */
} ADC_CFG_T;

/***********************************************************************
 * ADC device configuration commands (IOCTL commands and arguments)
 **********************************************************************/

/* ADC device commands (IOCTL commands) */
typedef enum
{
  ADC_SELECT_VREFP0,				/* Select Voltage reference vrefp0 */
  ADC_SELECT_CONTINUOUS_SCAN_MODE,	/* Select Conversion Mode */
  ADC_SELECT_SINGLE_CONV_MODE,		/* Select Single Conversion Mode */
  ADC_START_CONVERSION,				/* Start ADC conversion */
  ADC_STOP_CONVERSION,				/* Stop ADC conversion */
  ADC_SELECT_CHANNEL_0, 			/* Select the ADC channel 0 */
  ADC_SELECT_CHANNEL_1, 			/* Select the ADC channel 1 */
  ADC_SELECT_CHANNEL_2, 			/* Select the ADC channel 2 */
  ADC_SELECT_CHANNEL_3				/* Select the ADC channel 3 */
} ADC_IOCTL_CMD_T;

/* ADC device arguments for ADC_GET_STATUS command (IOCTL
   arguments) */
typedef enum
{
  ADC_GET_STATUS	/* Get ADC conversion status */
} ADC_IOCTL_STS_T;

/***********************************************************************
 * 10-bit ADC driver API functions
 **********************************************************************/

/* Open the ADC */
INT_32 adc_open(void *ipbase,
                INT_32 arg);

/* Close the ADC */
STATUS adc_close(INT_32 devid);

/* ADC configuration block */
STATUS adc_ioctl(INT_32 devid,
                 INT_32 cmd,
                 INT_32 arg);

/* ADC read function (polled) */
void poll_adc(INT_32 devid);

/***********************************************************************
 * ADC driver miscellaneous functions
 **********************************************************************/

/* ADC interrupt handler */
void adc_interrupt_handler(void);

#ifdef __cplusplus
}
#endif

#endif /* LPC313X_ADC_DRIVER_H */
