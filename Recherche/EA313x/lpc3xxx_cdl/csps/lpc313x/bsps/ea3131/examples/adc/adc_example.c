/***********************************************************************
 * $Id:: adc_example.c 2555 2009-11-25 21:14:53Z pdurgesh              $
 *
 * Project: LPC313x ADC driver example
 *
 * Description:
 *     the main program tests the different ADC modes
 *     - single scan (Interrupt driven and polled)
 *     - continous scan (Interrupt driven and polled)
 *     Messages are displayed in Serial Terminal
 *     Settings: 115,200 - 8n1 - No HW flow control
 *
 *     I  - Single conversion examples (interrupt mode and polled):
 *          perform a conversion of the 4 ADC's (one scan)
 *          and display the results
 *
 *     II - Continuous scan examples:
 *          a) Interrupt Mode - Channel 0 and Channel 1:
 *             The ADC is connected to the accelerometer
 *             (X and Y axis only)
 *             Continous scan of Channel 0 and 1 is performed until a
 *             movement in the X-axis or Y-axis is detected.
 *             When detected,  the continuous scan stops, the
 *             conversion values and the detected direction are
 *             displayed.
 *             This is for demo purpose only and not guaranteed to be
 *             correctly calibrated.
 *             Jumper configuration:
 *     		   AIN0 = Accel
 *      	   AIN1 = Accel
 *
 *          b) Polled Mode - Channel 2:
 *             The ADC is connected to the potentiometer.
 *             Continous scan on Channel 2 is performed as long as the
 *             A to D result is different from 0x3FF. When 0x3FF is detected,
 *             conversions stop and the number of conversions performed is
 *             displayed.
 *             0x3FF assumes that 10-bit resolution is selected.
 *             Rotating the pot clockwise increases the input voltage.
 *             Jumper configuration:
 *			   AIN2 = Trim pot
 *
 *     To select the mode, set one of the following to 1
 *     #define SINGLE_SCAN_INT			0
 *     #define CONTINUOUS_SCAN_INT		1
 *     #define SINGLE_SCAN_POLLED		0
 *     #define CONTINUOUS_SCAN_POLLED	0
 *     See below right after the include section
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
#include <stdio.h>
#include <string.h>

#include "lpc_types.h"
#include "lpc_irq_fiq.h"
#include "lpc_arm922t_cp15_driver.h"
#include "ea3131_board.h"
#include "lpc313x_cgu_driver.h"
#include "lpc313x_intc_driver.h"
#include "lpc313x_wdt.h"
#include "lpc313x_uart_driver.h"
#include "lpc313x_adc10b_driver.h"

/* Switches to select the demo */
#define SINGLE_SCAN_INT			1
#define CONTINUOUS_SCAN_INT		0
#define SINGLE_SCAN_POLLED		0
#define CONTINUOUS_SCAN_POLLED	0

INT_32 uartdev;
char buff[512];
UART_CONTROL_T ucntl;

/* ADC flags - see lpc313x_adc_driver.c file */
extern volatile INT_32 convdone;
extern volatile INT_32 adc_int_conv_ctr;
extern INT_32 adc_polled_conv_ctr;

/* Flags for the ADC example */
INT_32 x_movement = 0;
INT_32 y_movement = 0;
INT_32 z_movement = 0;
INT_32 movement_detected = 0;
INT_32 cycle_finished = 0;

/* ADC device handle */
STATIC INT_32 adcdev;

/**********************************************************************
* Send a string to the UART
**********************************************************************/
static void uart_string_write(INT_32 devid, char *pbuff)
{
  UNS_32 len = strlen(pbuff);
  UNS_32 written;

  while (len > 0)
  {
    written = uart_write(devid, pbuff, len);
    len -= written;
    pbuff += written;
  }
}

void print(char line[])
{
  sprintf(buff, line);
  uart_string_write(uartdev, buff);
}

/**********************************************************************
* Display A to D conversion result - all examples
**********************************************************************/
void display_ADC_result(INT_32 devid)
{
  ADC_REGS_T *adcregsptr;
  ADC_CFG_T *adccfgptr = (ADC_CFG_T *) devid;
  adcregsptr = adccfgptr->regptr;
  while (convdone == 0);

  if (adcregsptr->adc_csel_reg & 0x0000000F)
  {
    sprintf(buff, "Channel 0 - ADC Value : 0x%03x \r\n", adccfgptr->rx[0]);
    uart_string_write(uartdev, buff);
  }

  if (adcregsptr->adc_csel_reg & 0x000000F0)
  {
    sprintf(buff, "Channel 1 - ADC Value : 0x%03x \r\n", adccfgptr->rx[1]);
    uart_string_write(uartdev, buff);
  }

  if (adcregsptr->adc_csel_reg & 0x00000F00)
  {
#if (SINGLE_SCAN_INT | CONTINUOUS_SCAN_INT | SINGLE_SCAN_POLLED)
    sprintf(buff, "Channel 2 - ADC Value : 0x%03x \r\n", adccfgptr->rx[2]);
#endif
#if (CONTINUOUS_SCAN_POLLED)
    /* In continous scan (poll), we always print in the same line */
    sprintf(buff, "Channel 2 - ADC Value : 0x%03x \r", adccfgptr->rx[2]);
#endif
    uart_string_write(uartdev, buff);
  }

  if (adcregsptr->adc_csel_reg & 0x0000F000)
  {
    sprintf(buff, "Channel 3 - ADC Value : 0x%03x \r\n", adccfgptr->rx[3]);
    uart_string_write(uartdev, buff);
  }

#if (SINGLE_SCAN_POLLED)
  sprintf(buff, "Number of A to D conversions: %d \r\n", adc_polled_conv_ctr);
  uart_string_write(uartdev, buff);
#endif

#if (CONTINUOUS_SCAN_POLLED)
  if (cycle_finished)
  {
    /* Move to the next line */
    sprintf(buff, "\n");
    uart_string_write(uartdev, buff);
    sprintf(buff, "Number of A to D conversions: %d \r\n", adc_polled_conv_ctr);
    uart_string_write(uartdev, buff);
  }
#endif
}

/**********************************************************************
* Display detected movement - Continuous Scan Interrupt mode example
**********************************************************************/
void display_movement_direction(void)
{
  if (x_movement)
  {
    sprintf(buff, "Movement in the X direction detected \r\n");
    uart_string_write(uartdev, buff);
  }
  if (y_movement)
  {
    sprintf(buff, "Movement in the Y direction detected \r\n");
    uart_string_write(uartdev, buff);
  }
}

/**********************************************************************
* Initialize the LPC313x
**********************************************************************/
void init_LPC313x(void)
{
  /* Disable interrupts in ARM core */
  disable_irq_fiq();

  /* Set virtual address of MMU table */
  cp15_set_vmmu_addr((void *)ISROM_MMU_TTB_BASE);

  /* Initialize interrupt system */
  int_initialize(0xFFFFFFFF);

}

/**********************************************************************
* Initialize the UART
* 115,200 - 8n1 - No HW flow control
**********************************************************************/
void init_UART(void)
{
  /* Setup UART for 115.2K, 8 data bits, no parity, 1 stop bit */
  ucntl.baud_rate = 115200;
  ucntl.parity = UART_PAR_NONE;
  ucntl.databits = 8;
  ucntl.stopbits = 1;
  uartdev = uart_open((void *) UART_BASE, (INT_32) & ucntl);
}

/**********************************************************************
* Initialize ADC Interrupt
**********************************************************************/
void init_ADC_interrupt(void)
{
  /* Install ADC interrupt handler */
  int_install_irq_handler(IRQ_ADC, adc_interrupt_handler);

  int_enable(IRQ_ADC);
  int_clear(IRQ_ADC);
  enable_irq();
}

/**********************************************************************
* Perform the ADC conversion(s)
* Program the Control and Channel/Resolution registers
* Parameters: - dev: Pointer to ADC config structure
*             - conv_mode: ADC_SELECT_SINGLE_CONV_MODE or
                           ADC_SELECT_CONTINUOUS_CONV_MODE
*             - channel_x: ADC_SELECT_CHANNEL_x if channel x is part of the ADC scan
*                          "0" if channel x is not part of the ADC scan
*             - resolution_x: resolution of the channel (2 to 10)
*                             value is "don't care" when channel not
*                             part of the ADC scan
**********************************************************************/
void program_adc(INT_32 dev,
                 INT_32 conv_mode,
                 INT_32 channel_0, INT_32 resolution_0,
                 INT_32 channel_1, INT_32 resolution_1,
                 INT_32 channel_2, INT_32 resolution_2,
                 INT_32 channel_3, INT_32 resolution_3)
{
  /* select channel  and resolution */
  if (channel_0 == ADC_SELECT_CHANNEL_0) adc_ioctl(dev, ADC_SELECT_CHANNEL_0, resolution_0);
  if (channel_1 == ADC_SELECT_CHANNEL_1) adc_ioctl(dev, ADC_SELECT_CHANNEL_1, resolution_1);
  if (channel_2 == ADC_SELECT_CHANNEL_2) adc_ioctl(dev, ADC_SELECT_CHANNEL_2, resolution_2);
  if (channel_3 == ADC_SELECT_CHANNEL_3) adc_ioctl(dev, ADC_SELECT_CHANNEL_3, resolution_3);

  /* select scan mode */
  if (conv_mode == ADC_SELECT_SINGLE_CONV_MODE) adc_ioctl(dev, ADC_SELECT_SINGLE_CONV_MODE, 0);
  if (conv_mode == ADC_SELECT_CONTINUOUS_SCAN_MODE) adc_ioctl(dev, ADC_SELECT_CONTINUOUS_SCAN_MODE, 0);

  /* start A/D convert */
  adc_ioctl(dev, ADC_START_CONVERSION, 0);

  /* stop A/D convert */
  adc_ioctl(dev, ADC_STOP_CONVERSION, 0);
}

/**********************************************************************
* Process the A to D conversion result (Channel 0 and 1 only)
* Set the movement_detected flag to 1 if conversion value > 0x250
**********************************************************************/
void process_adc_value(INT_32 devid)
{
  ADC_CFG_T *adccfgptr = (ADC_CFG_T *) devid;
  while (convdone == 0);

  if (adccfgptr->rx[0] > 0x250)
  {
    x_movement = 1;
    movement_detected = 1;
  }
  if (adccfgptr->rx[1] > 0x250)
  {
    y_movement = 1;
    movement_detected = 1;
  }

  /* 0x3FF assumes that the 10-bit resolution is used */
  /* Change the value accordingly if lower resolution is used */
  if (adccfgptr->rx[2] == 0x3FF)
  {
    cycle_finished = 1;
  }
}

/**********************************************************************
* End continous A to D  conversions
* Program the Control register
**********************************************************************/
void end_continuous_adc_conv(INT_32 dev)
{
  /* stop A/D convert */
  adc_ioctl(dev, ADC_SELECT_SINGLE_CONV_MODE, 0);
}

/**********************************************************************
* The main program starts here
**********************************************************************/
int c_entry(void)
{
  init_LPC313x();
  init_UART();


#if (SINGLE_SCAN_INT | CONTINUOUS_SCAN_INT)
  init_ADC_interrupt();
#endif

#if (SINGLE_SCAN_POLLED | CONTINUOUS_SCAN_POLLED)
  int_disable(IRQ_ADC);
#endif

  sprintf(buff, "\r\nLPC313x 10-bit ADC Example \r\n");
  uart_string_write(uartdev, buff);

  /* Open the ADC device */
  adcdev = adc_open((void *) ADC , 0);
  sprintf(buff, "ADC open \r\n");
  uart_string_write(uartdev, buff);

  /* Example using Single Conversion Mode - Interrupt driven*/
#if SINGLE_SCAN_INT
  sprintf(buff, "Single conversion example - Interrupt Mode \r\n");
  uart_string_write(uartdev, buff);

  /* single conversion on all channels, all with 10-bit resolution */
  program_adc(adcdev,
              ADC_SELECT_SINGLE_CONV_MODE,
              ADC_SELECT_CHANNEL_0, 10,
              ADC_SELECT_CHANNEL_1, 10,
              ADC_SELECT_CHANNEL_2, 10,
              ADC_SELECT_CHANNEL_3, 10);
  display_ADC_result(adcdev);
#endif

#if CONTINUOUS_SCAN_INT
  sprintf(buff, "Continous Scan example - Interrupt Mode \r\n");
  uart_string_write(uartdev, buff);

  /* continuous scan on Channel 0 and Channel 1, both with 10-bit resolution */
  program_adc(adcdev,
              ADC_SELECT_CONTINUOUS_SCAN_MODE,
              ADC_SELECT_CHANNEL_0, 10,
              ADC_SELECT_CHANNEL_1, 10,
              0, 0,
              0, 0);
  while (movement_detected == 0) process_adc_value(adcdev);
  end_continuous_adc_conv(adcdev);
  display_ADC_result(adcdev);
  display_movement_direction();
#endif

#if SINGLE_SCAN_POLLED
  sprintf(buff, "Single conversion example - Polled Mode \r\n");
  uart_string_write(uartdev, buff);

  /* single conversion on all channels, all with 10-bit resolution */
  program_adc(adcdev,
              ADC_SELECT_SINGLE_CONV_MODE,
              ADC_SELECT_CHANNEL_0, 10,
              ADC_SELECT_CHANNEL_1, 10,
              ADC_SELECT_CHANNEL_2, 10,
              ADC_SELECT_CHANNEL_3, 10);
  poll_adc(adcdev);
  display_ADC_result(adcdev);
#endif

#if CONTINUOUS_SCAN_POLLED
  sprintf(buff, "Continous Scan example - Polled Mode \r\n");
  uart_string_write(uartdev, buff);

  /* continuous scan on Channel 0 and Channel 1 with 10-bit resolution */
  program_adc(adcdev,
              ADC_SELECT_CONTINUOUS_SCAN_MODE,
              0, 0,
              0, 0,
              ADC_SELECT_CHANNEL_2, 10,
              0, 0);
  while (cycle_finished == 0)
  {
    poll_adc(adcdev);
    process_adc_value(adcdev);
    display_ADC_result(adcdev);
  }
  end_continuous_adc_conv(adcdev);
#endif

  /* close the ADC  device*/
  adc_close(adcdev);
  sprintf(buff, "ADC Closed \r\nEnd example \r\n");
  uart_string_write(uartdev, buff);

  return 0;

}
