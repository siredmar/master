/***********************************************************************
 * $Id:: pf_main.c 2530 2009-11-17 00:39:08Z pdurgesh                  $
 *
 * Project: UART driver example
 *
 * Description:
 *     A simple UART driver example.
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
#include <string.h>
#include "lpc_types.h"
#include "lpc_arm922t_cp15_driver.h"
#include "lpc_irq_fiq.h"
#include "lpc313x_chip.h"
#include "ea3131_board.h"
#include "lpc313x_intc_driver.h"
#include "lpc313x_cgu_driver.h"
#include "lpc313x_i2c_driver.h"
#include "lpc313x_pca9532_driver.h"
#include "lpc313x_uart_driver.h"
#include "lpc315x_otp_driver.h"

/* register offset and bit position of OTP control in PCA9532 regs*/
#define PCA9532_OTP_REG     0x07
#define PCA9532_OTP_BITPOS  2



OTP_DATA_T g_otp_data;
/* device handles */
static INT_32 dev_i2c0;
static INT_32 uartdev;

static UNS_32 fill_lpc3143_otp_data(OTP_DATA_T* pdata);


/* UART device handles */
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
/***********************************************************************
 *
 * Function: ea3143_set_otp_read_voltage
 *
 * Purpose: Set appropriate VPP voltage
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 **********************************************************************/
static void ea3143_set_otp_read_voltage(void)
{
  UNS_8 reg_addr;
  INT_32 data;
  /* read the value of LED reg */
  reg_addr = PCA9532_OTP_REG;
  pca9532_ioctl(dev_i2c0, PCA9532_GET_LED, (INT_32)&reg_addr);
  reg_addr &= ~(0x03 << PCA9532_OTP_BITPOS);
  data = (PCA9532_OTP_REG << 8) | (reg_addr & 0xFF);
  pca9532_ioctl(dev_i2c0, PCA9532_SET_LED, data);

  uart_string_write(uartdev, "Read voltage!!! \r\n");

}
/***********************************************************************
 *
 * Function: ea3143_set_otp_write_voltage
 *
 * Purpose: Set appropriate VPP voltage
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 **********************************************************************/
static void ea3143_set_otp_write_voltage(void)
{
  UNS_8 reg_addr;
  INT_32 data;
  /* read the value of LED reg */
  reg_addr = PCA9532_OTP_REG;
  pca9532_ioctl(dev_i2c0, PCA9532_GET_LED, (INT_32)&reg_addr);
  reg_addr &= ~(0x03 << PCA9532_OTP_BITPOS);
  reg_addr |= (0x01 << PCA9532_OTP_BITPOS);
  data = (PCA9532_OTP_REG << 8) | (reg_addr & 0xFF);
  pca9532_ioctl(dev_i2c0, PCA9532_SET_LED, data);
  uart_string_write(uartdev, "Write voltage!!! \r\n");
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
  INT_32 otpdev;
  UART_CONTROL_T ucntl;
  UNS_32 bytes = 0;
  OTP_OPEN_ARG_T otpArgs;
  UNS_32 wr_protect_mask;
  CGU_FDIV_SETUP_T fdiv_cfg;

  /* Disable interrupts in ARM core */
  disable_irq_fiq();

  /* Setup miscellaneous board functions */
  ea3131_board_init();

  /* Set virtual address of MMU table */
  cp15_set_vmmu_addr((void *)ISROM_MMU_TTB_BASE);

  /* Enable I2C system clock */
  cgu_clk_en_dis(CGU_SB_I2C0_PCLK_ID, TRUE);

  /* set base frequency for the domain AHB0_APB1 belongs to PLL1*/
  cgu_set_base_freq(CGU_SB_AHB0_APB1_BASE_ID, CGU_FIN_SELECT_HPPLL1);
  /* set I2C clock between 100KHz & 400KHz for write */
  fdiv_cfg.stretch = 1;
  fdiv_cfg.n = 1;
  fdiv_cfg.m = 4;

  /* change the frac divider corresponding to the clock.
  Note this changes clock fre for all clocks connect to
  this frac divider. */
  cgu_set_subdomain_freq(CGU_SB_I2C0_PCLK_ID, fdiv_cfg);

  /* Initialize interrupt system */
  int_initialize(0xFFFFFFFF);

  /* install default I2C0 & I2C1 interrupt handlers */
  int_install_irq_handler(IRQ_I2C0, (PFV)i2c0_user_interrupt);

  /* Enable IRQ interrupts in the ARM core */
  enable_irq();

  /* open I2C0 and I2C1 */
  dev_i2c0 = pca9532_open((void *)I2C0, 0);

  /* Setup UART for 115.2K, 8 data bits, no parity, 1 stop bit */
  ucntl.baud_rate = 115200;
  ucntl.parity = UART_PAR_NONE;
  ucntl.databits = 8;
  ucntl.stopbits = 1;
  uartdev = uart_open((void *) UART_BASE, (INT_32) & ucntl);
  if (uartdev != 0)
  {
    uart_string_write(uartdev, "LPC314x programming OTP fuses\r\n");

    /* open OTP device */
    otpArgs.set_read_voltage = ea3143_set_otp_read_voltage;
    otpArgs.set_write_voltage = ea3143_set_otp_write_voltage;
    otpdev = otp_open((void*)OTP_BASE, (INT_32) & otpArgs);

    if (otpdev != 0)
    {
      /* set base frequency for the domain OTP_CLK belongs to FFAST.*/
      cgu_set_base_freq(CGU_SB_AHB0_APB0_BASE_ID, CGU_FIN_SELECT_FFAST);

      /* set OTP clock between 100KHz & 500KHz for write */
      fdiv_cfg.stretch = 1;
      fdiv_cfg.n = 1;
      fdiv_cfg.m = 128;
      /* change the frac divider corresponding to the clock.
      Note this changes clock fre for all clocks connect to
      this frac divider. */
      cgu_set_subdomain_freq(CGU_SB_OTP_PCLK_ID, fdiv_cfg);

      /* fill the OTP data structure with desired value */
      wr_protect_mask = fill_lpc3143_otp_data(&g_otp_data);

      if (wr_protect_mask != 0xFFFF)
      {
        /* remove write protection */
        otp_ioctl(otpdev, OTP_SET_WRITE_PROTECT, wr_protect_mask);

        /* write the data to OTP fuses */
        bytes = otp_write(otpdev, (void*) & g_otp_data, sizeof(g_otp_data));

        /* set write protection back */
        otp_ioctl(otpdev, OTP_SET_WRITE_PROTECT, OTP_WRPORT_PROTECT_ALL | 0xFFFF);

        /* set OTP clock back to 6MHz */
        fdiv_cfg.stretch = 1;
        fdiv_cfg.n = 1;
        fdiv_cfg.m = 2;
        cgu_set_subdomain_freq(CGU_SB_OTP_PCLK_ID, fdiv_cfg);

        /* remove read protection */
        otp_ioctl(otpdev, OTP_SET_READ_PROTECT, 0);
        /* re-read OTP fuses */
        otp_ioctl(otpdev, OTP_REREAD_FUSES, 0);
        /* set read protection back */
        otp_ioctl(otpdev, OTP_SET_READ_PROTECT, OTP_RPORT_PROTECT_ALL | 0xFFFF);
      }
    }
    uart_string_write(uartdev, "Done!!! \r\n");

  }
  uart_close(uartdev);
  return 1;
}

static UNS_32 fill_lpc3143_otp_data(OTP_DATA_T* pdata)
{
  /* write protect all except chip_cfg & aes key fields*/
  UNS_32 wrp_mask = 0xFFFF;
  UNS_32 chip_id = OTP->d.map.chip_cfg & 0x3F;

  /* allow AES key programming for LPC3143 & LPC3154 only */
  if ((chip_id == 0x0B) || (chip_id == 0x07)) 
  {
    if (!(OTP->d.map.chip_cfg & OTPD_CFG_AES_VALID))
    {
      wrp_mask = 0x7F0F;
      /* set the AES key values */
      pdata->aes_key[0] = 0x0FC14139;
      pdata->aes_key[1] = 0x00215B47;
      pdata->aes_key[2] = 0xAF9E139D;
      pdata->aes_key[3] = 0x1650EA23;
      /* set chip config values */
      pdata->chip_cfg = OTPD_CFG_AES_VALID;
    }
    else
    {
      uart_string_write(uartdev, "AES key already programmed!!! \r\n");
    }
  }
  else
  {
    uart_string_write(uartdev, "AES key programing not allowed on this chip!!! \r\n");
  }
  return wrp_mask;
}

