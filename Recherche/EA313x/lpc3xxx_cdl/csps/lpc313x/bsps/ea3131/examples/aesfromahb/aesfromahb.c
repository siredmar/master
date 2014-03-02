/***********************************************************************
 * $Id:: aesfromahb.c 2554 2009-11-24 23:06:41Z pdurgesh               $
 *
 * Project: AES from AHB example
 *
 * Description:
 *     A simple example to access AES engine from AHB interface. This
 * example works only on LPC3143 and LPC3154 chips.
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
#include "lpc313x_uart_driver.h"
#include "lpc313x_cgu_driver.h"
#include "lpc313x_nandc.h"

INT_32 g_uartdev;
char g_buff[512];


UNS_32 encrypted_data[128] =
{
  0x1F4E3A7F, 0xE4771EA4, 0x98DD1E85, 0xDC53F035, 0x5D61D6D5, 0x4C03DAB3, 0x57CCF329, 0x46BB5A11, 
  0xDE1FC5E9, 0x06750119, 0xF18A405F, 0x5C67CD34, 0x2481D5BD, 0x3EEF6E46, 0x5C3C5E34, 0x7DE40BC6, 
  0x71982CE3, 0xF144961C, 0x23B7CF66, 0xB305515B, 0x277EC23C, 0xF81C611E, 0x39E8E649, 0x33601E62, 
  0x7B90FDF0, 0x10ADD1EA, 0xD2F69207, 0x2D10BED5, 0x250F8CD2, 0x32C3918C, 0x0B644D00, 0x5D05E884, 
  0xF9A791B9, 0x0960916F, 0x0360BC6B, 0x7C91F09B, 0x0452E5A7, 0x1203065E, 0xEA44380C, 0x617056D2, 
  0x2AFB2457, 0xF23D7D12, 0x52CD7990, 0x753028BE, 0x783C36E9, 0x647CAD4D, 0xC4C9A530, 0xE434F9AA, 
  0x7A7D7554, 0x7E866260, 0xAD35667E, 0xC7A4CF9D, 0x1BE12574, 0x92963A2E, 0x86D2CA3D, 0x4241E2D8, 
  0x2ECBB52D, 0xFB07A91B, 0x578595C7, 0x4BF30B32, 0x7720D7AC, 0x3C476B75, 0xCD59442C, 0x4C7FDF7C, 
  0xF29EEF8B, 0xC768C3B1, 0xB7D2B411, 0x53F52551, 0xD37C53DF, 0x3FF8135B, 0x227868D6, 0x960C8CBC, 
  0xC1D90B74, 0xB421A99A, 0xEC1FF8CB, 0xA645CB67, 0xFD58E27A, 0xF4A52E2A, 0x391EA514, 0xF95099E0, 
  0x4367975D, 0xF274536A, 0x1142B93D, 0x13F3BCA7, 0x6FCA1B38, 0x27A9C24F, 0x4409A5BB, 0x9146A199, 
  0xF5F8BA15, 0x792EA8A0, 0xE7C25D73, 0x2874F7A4, 0xF02BCB82, 0x672B263F, 0x386D72EA, 0x8625E310, 
  0x4E10FB2D, 0xD0EB694E, 0x1C8A59FA, 0xD0821962, 0xC4A42A86, 0xD52CFA5E, 0xD3F678D0, 0x93810B20, 
  0xB0C4A7D0, 0x2CCC7D3A, 0xB20757CC, 0x460F9E8E, 0xAD0591E6, 0xFE0346A7, 0xE31492F6, 0x8FC532E0, 
  0xBE70BB9A, 0x40F05D1C, 0xA2CFC009, 0xD55D817E, 0x578ACCC7, 0x8F24CE52, 0x802827A5, 0x0A817019, 
  0x605A2F2E, 0xC4D688D7, 0x1029B01E, 0xF4399198, 0xDBBAB158, 0x89BFEBB2, 0x0963E229, 0x7C1BED3A, 
};

UNS_32 decrypted_data[128] =
{
  0x03020100, 0x07060504, 0x03020100, 0x07060504, 0x03020100, 0x07060504, 0x03020100, 0x07060504,
  0x03020100, 0x07060504, 0x03020100, 0x07060504, 0x03020100, 0x07060504, 0x03020100, 0x07060504,
  0x03020100, 0x07060504, 0x03020100, 0x07060504, 0x03020100, 0x07060504, 0x03020100, 0x07060504,
  0x03020100, 0x07060504, 0x03020100, 0x07060504, 0x03020100, 0x07060504, 0x03020100, 0x07060504,
  0x03020100, 0x07060504, 0x03020100, 0x07060504, 0x03020100, 0x07060504, 0x03020100, 0x07060504,
  0x03020100, 0x07060504, 0x03020100, 0x07060504, 0x03020100, 0x07060504, 0x03020100, 0x07060504,
  0x03020100, 0x07060504, 0x03020100, 0x07060504, 0x03020100, 0x07060504, 0x03020100, 0x07060504,
  0x03020100, 0x07060504, 0x03020100, 0x07060504, 0x03020100, 0x07060504, 0x03020100, 0x07060504,
  0x03020100, 0x07060504, 0x03020100, 0x07060504, 0x03020100, 0x07060504, 0x03020100, 0x07060504,
  0x03020100, 0x07060504, 0x03020100, 0x07060504, 0x03020100, 0x07060504, 0x03020100, 0x07060504,
  0x03020100, 0x07060504, 0x03020100, 0x07060504, 0x03020100, 0x07060504, 0x03020100, 0x07060504,
  0x03020100, 0x07060504, 0x03020100, 0x07060504, 0x03020100, 0x07060504, 0x03020100, 0x07060504,
  0x03020100, 0x07060504, 0x03020100, 0x07060504, 0x03020100, 0x07060504, 0x03020100, 0x07060504,
  0x03020100, 0x07060504, 0x03020100, 0x07060504, 0x03020100, 0x07060504, 0x03020100, 0x07060504,
  0x03020100, 0x07060504, 0x03020100, 0x07060504, 0x03020100, 0x07060504, 0x03020100, 0x07060504,
  0x03020100, 0x07060504, 0x03020100, 0x07060504, 0x03020100, 0x07060504, 0x03020100, 0x07060504,
};

/***********************************************************************
 *
 * Function: uart_string_write
 *
 * Purpose: UART string output helper function
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
 ***********************************************************************/
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
 * Function: image_decrypt_init
 *
 * Purpose: Initialise AES engine for access from AHB.
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
 ***********************************************************************/
void image_decrypt_init(void)
{
  UNS_32 i;

  CGU_CFG->nandflash_ctrl_nand_reset_n_soft = 0x0;
  CGU_CFG->nandflash_ctrl_ecc_reset_n_soft = 0x0;
  CGU_CFG->nandflash_ctrl_aes_reset_n_soft = 0x0;

  for (i = 1;i == 10;i++) {};

  CGU_CFG->nandflash_ctrl_nand_reset_n_soft = 0x1;
  CGU_CFG->nandflash_ctrl_ecc_reset_n_soft = 0x1;
  CGU_CFG->nandflash_ctrl_aes_reset_n_soft = 0x1;

  NAND_CTRL->set_ce = NAND_SETCE_WP | NAND_SETCE_CV(0);

  NAND_CTRL->config  = 0;

  NAND_CTRL->aes_key[0] = 0x0FC14139;
  NAND_CTRL->aes_key[1] = 0x00215B47;
  NAND_CTRL->aes_key[2] = 0xAF9E139D;
  NAND_CTRL->aes_key[3] = 0x1650EA23;

  NAND_CTRL->aes_iv[0]  = 0xDEADBEEF;
  NAND_CTRL->aes_iv[1]  = 0xFADCABED;
  NAND_CTRL->aes_iv[2]  = 0xABADDEED;
  NAND_CTRL->aes_iv[3]  = 0x12345678;
  //NAND_CTRL->config |= NAND_CFG_AO;
}

/***********************************************************************
 *
 * Function: load_enc_data
 *
 * Purpose: Load encrypted data to NAND buffer for test.
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
 ***********************************************************************/
void load_enc_data(int start)
{
  int i;
  volatile UNS_32* pram = (volatile UNS_32*)start;

  sprintf(g_buff, "Fill Memory starting at address: %x ...\r\n", start);
  uart_string_write(g_uartdev, g_buff);
  for (i = 0;i < 128;i++)
  {
    *pram = encrypted_data[i];
    pram++;
  }
}

/***********************************************************************
 *
 * Function: check_dec_data
 *
 * Purpose: Verify the decrypted data matches the original data.
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
 ***********************************************************************/
void check_dec_data(int start)
{
  int i;
  volatile UNS_32* pram = (volatile UNS_32*)start;

  sprintf(g_buff, "Checking decrypted data: %x ...\r\n", start);
  uart_string_write(g_uartdev, g_buff);
  for (i = 0;i < 128;i++)
  {

    if (*pram != decrypted_data[i])
    {
      sprintf(g_buff, "Read error at %d, data is: %x, should be: %x\r\n", i, *pram, decrypted_data[i]);
      uart_string_write(g_uartdev, g_buff);
    }

    pram++;
  }
  sprintf(g_buff, "Check completed\r\n");
  uart_string_write(g_uartdev, g_buff);
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
  UART_CONTROL_T ucntl;
  volatile UNS_32 irq_status;
  /* Disable interrupts in ARM core */
  disable_irq_fiq();

  /* Set virtual address of MMU table */
  cp15_set_vmmu_addr((void *)ISROM_MMU_TTB_BASE);

  /* enable nand clocks */
  cgu_clk_en_dis(CGU_SB_NANDFLASH_S0_CLK_ID, TRUE);
  cgu_clk_en_dis(CGU_SB_NANDFLASH_NAND_CLK_ID, TRUE);
  cgu_clk_en_dis(CGU_SB_NANDFLASH_AES_CLK_ID, TRUE);
  cgu_clk_en_dis(CGU_SB_NANDFLASH_PCLK_ID, TRUE);

  /* init AES controller for AHB access */
  image_decrypt_init();
  /* Setup UART for 115.2K, 8 data bits, no parity, 1 stop bit */
  ucntl.baud_rate = 115200;
  ucntl.parity = UART_PAR_NONE;
  ucntl.databits = 8;
  ucntl.stopbits = 1;
  g_uartdev = uart_open((void *) UART_BASE, (INT_32) & ucntl);
  if (g_uartdev != 0)
  {
    sprintf(g_buff, "LPC3143/54 AES AHB test\r\n");
    uart_string_write(g_uartdev, g_buff);

    /* Set NAND Controller in AES mode */
    NAND_CTRL->aes_from_ahb = NAND_AES_AHB_EN;

    /* copy encrypted data to RAM0 */
    load_enc_data(0x70000000);

    /* Start decoding */
    NAND_CTRL->aes_from_ahb |= NAND_AES_AHB_DCRYPT_RAM0;

    do
    {
      irq_status = NAND_CTRL->irq_status_raw;
    }
    while ((irq_status & NAND_IRQ_AES_DONE_RAM0) == 0);

    NAND_CTRL->irq_status_raw = NAND_IRQ_AES_DONE_RAM0;

    check_dec_data(0x70000000);
  }
  return 1;
}
