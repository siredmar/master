/***********************************************************************
 * $Id:: i2s_example.c 1659 2009-02-06 17:46:20Z pdurgesh              $
 *
 * Project: I2S driver example
 *
 * Description:
 *     A simple I2S driver example.
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

#include "lpc_types.h"
#include "lpc_irq_fiq.h"
#include "lpc_arm922t_cp15_driver.h"
#include "ea3131_board.h"
#include "lpc313x_cgu_driver.h"
#include "lpc313x_i2s_driver.h"

INT_32 		i2sTxdev, i2sRxdev, index, test_data;
I2S_CFG_T	*pi2sTxdev, *pi2sRxdev;

/***********************************************************************
 *	I2S TX1  interrupt handler
***********************************************************************/
void i2stx1_int_handler(void)
{

  volatile UNS_32 tmp;

  /* save the status  */
  tmp = pi2sTxdev->regptr->int_status;

  /* select the pattern to write  */
  switch (index)
  {
    case 0:
      pi2sTxdev->regptr->interleaved[0] = 0x11118888;
      break;
    case 1:
      pi2sTxdev->regptr->interleaved[1] = 0x22227777;
      break;
    case 2:
      pi2sTxdev->regptr->interleaved[2] = 0x33336666;
      break;
    case 3:
      pi2sTxdev->regptr->interleaved[3] = 0x44445555;
      break;
    case 4:
      pi2sTxdev->regptr->interleaved[4] = 0x55554444;
      break;
    case 5:
      pi2sTxdev->regptr->interleaved[5] = 0x66663333;
      break;
    case 6:
      pi2sTxdev->regptr->interleaved[6] = 0x77772222;
      break;
    case 7:
      pi2sTxdev->regptr->interleaved[7] = 0x88881111;
      break;
  }

  /*  update patterns index   */
  index ++;
  if (index > 7)
    index = 0;

  /* clear the flag if it's set  */
  if (tmp & I2STX_FIFO_LEFT_HALF_EMPTY)
    pi2sTxdev->regptr->int_status &= ~I2STX_FIFO_LEFT_HALF_EMPTY;

  /* clear the interrupt  */
  int_clear(IRQ_I2S1_OUT);
}


/***********************************************************************
 *	I2S RX1  interrupt handler
***********************************************************************/
void i2srx1_int_handler(void)
{

  volatile UNS_32 tmp;

  /* save the status  */
  tmp = pi2sRxdev->regptr->int_status;

  /* clear the flag if it's set  */
  if (tmp & I2SRX_FIFO_LEFT_HALF_FULL)
    pi2sRxdev->regptr->int_status &= ~I2SRX_FIFO_LEFT_HALF_FULL;

  /*  read data from the RX FIFO   */
  test_data = pi2sRxdev->regptr->interleaved[0];

  /* clear the interrupt  */
  int_clear(IRQ_I2S1_IN);
}

/***********************************************************************
 *
 * Function: c_entry
 *
 * Purpose: Application entry point from the startup code
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Always returns 1
 *
 * Notes: None
 *
 **********************************************************************/
int c_entry(void)
{
  /* Disable interrupts in ARM core */
  disable_irq_fiq();

  /* Setup miscellaneous board functions */
  ea3131_board_init();

  /* Set virtual address of MMU table */
  cp15_set_vmmu_addr((void *)ISROM_MMU_TTB_BASE);

  /* Initialize interrupt system */
  int_initialize(0xFFFFFFFF);

  /* enable I2S-related clocks in the CGU */
  /*  enable clock for ADSS_CFG block   */
  cgu_clk_en_dis(CGU_SB_I2S_CFG_PCLK_ID, TRUE);

  /*  enable clocks for I2S_TX1 module  */
  cgu_clk_en_dis(CGU_SB_I2STX_IF_1_PCLK_ID, TRUE);
  cgu_clk_en_dis(CGU_SB_I2STX_FIFO_1_PCLK_ID, TRUE);
  cgu_clk_en_dis(CGU_SB_I2S_EDGE_DETECT_CLK_ID, TRUE);
  cgu_clk_en_dis(CGU_SB_I2STX_BCK1_N_ID, TRUE);
  cgu_clk_en_dis(CGU_SB_I2STX_WS1_ID, TRUE);

  /*  enable 256fs system clock for external ref */
  cgu_clk_en_dis(CGU_SB_CLK_256FS_ID, TRUE);

  /*  enable clocks for I2S_RX1 module  */
  cgu_clk_en_dis(CGU_SB_I2SRX_IF_1_PCLK_ID, TRUE);
  cgu_clk_en_dis(CGU_SB_I2SRX_FIFO_1_PCLK_ID, TRUE);
  cgu_clk_en_dis(CGU_SB_I2SRX_BCK1_N_ID, TRUE);
  cgu_clk_en_dis(CGU_SB_I2SRX_WS1_ID, TRUE);
  cgu_clk_en_dis(CGU_SB_I2SRX_BCK1_ID, TRUE);

  /* open the I2S TX1 device  */
  i2sTxdev = i2s_open(I2S_TX1, i2stx1_int_handler);
  if (i2sTxdev == 0)
  {
    /*  Can't open device - ERROR  */
    goto close;
  }

  /* get a pointer to the I2S TX config structure  */
  pi2sTxdev = (I2S_CFG_T *)i2sTxdev;

  /* set I2S format for I2S TX - default */
  i2s_ioctl(i2sTxdev, I2S_FORMAT_SETTINGS, I2S);

  /* Configure I2S TX1 FIFO_LEFT_HALFT_EMPTY flag as an interrupt source  */
  i2s_ioctl(i2sTxdev, I2S_INT_UNMASK, I2STX_FIFO_LEFT_HALF_EMPTY);

  /* open the I2S RX1 device  */
  i2sRxdev = i2s_open(I2S_RX1, i2srx1_int_handler);
  if (i2sRxdev == 0)
  {
    /*  Can't open device - ERROR  */
    goto close;
  }

  /* get a pointer to the I2S RX config structure  */
  pi2sRxdev = (I2S_CFG_T *)i2sRxdev;

  /* set I2S format for I2S RX - default */
  i2s_ioctl(i2sRxdev, I2S_FORMAT_SETTINGS, I2S);

  /* Configure I2S RX1 FIFO_LEFT_HALFT_FULL flag as an interrupt source  */
  i2s_ioctl(i2sRxdev, I2S_INT_UNMASK, I2SRX_FIFO_LEFT_HALF_FULL);

  /* Configure RX as Slave - default */
  i2s_ioctl(i2sRxdev, I2S_RX_MASTER_SLAVE_MODE, I2S_RX_SLAVE);

  enable_irq();

  while (1);

close:
  /* close the I2S devices */
  if (i2sTxdev > 0)
    i2s_close(i2sTxdev);

  if (i2sRxdev > 0)
    i2s_close(i2sRxdev);

  /* disable interrupts */
  disable_irq();

  /* Loop here */
  while (1);
}
