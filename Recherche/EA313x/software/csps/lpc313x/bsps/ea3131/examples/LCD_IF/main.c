/***********************************************************************
 * $Id:: main.c 2289 2009-10-02 20:52:32Z sscaglia                    $
 *
 * Project: LCD Interface example
 *
 * Description:
 *     LCD Interface programming example.
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
#include "lpc313x_intc_driver.h"
#include "lpc313x_sysreg.h"
#include "lpc313x_cgu_driver.h"
#include "lpc313x_lcdc.h"

#include "lcd_config.h"


void LCD_int_handler() {
  int tmp;

  tmp = LCD_IF->status;
  if (tmp & (1<<0))
    LCD_IF->int_clear |= (1<<0);

  if (tmp & (1<<1))
    LCD_IF->int_clear |= (1<<1);

  if (tmp & (1<<2))
    LCD_IF->int_clear |= (1<<2);

  if (tmp & (1<<3))
    LCD_IF->int_clear |= (1<<3);

  int_clear(IRQ_LCD);
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

  /* configure the LCD/EBI pins in LCD mode. */
  SYS_REGS->mux_lcd_ebi_sel = 0;

  /* enable LCD_PCLK clock */
  cgu_clk_en_dis(CGU_SB_LCD_PCLK_ID, TRUE);

  /* enable LCD_CLK clock */
  cgu_clk_en_dis(CGU_SB_LCD_CLK_ID, TRUE);



  /* LCD Control register configuration  */
#if Enable_Serial_Mode
  LCD_IF->control = LCD_CTRL_PS | (MSB_First_serial << 17) | 
                    (Invert_CS_Serial << 15) | (Serial_CLK_Shift << 4);

#endif /* Enable_Serial_Mode */

#if Enable_Parallel_Mode
  LCD_IF->control = (MI_Motorola_6800 << 2) | (Invert_CS_Parallel << 15) | 
                    ((E_RD_Active ^ MI_Motorola_6800) << 16);

#if Data_bus_width == 4
  LCD_IF->control |= (LCD_CTRL_IF_4 | (MSB_First_4_bits << 17));
#endif /* Data_bus_width == 4*/

#if Data_bus_width == 16
  LCD_IF->control |= LCD_CTRL_IF_16;
#endif /* Data_bus_width == 16 */

#endif /* Enable_Parallel_Mode */

  /*  Initialize Interrupts	 */
  int_initialize(0xFFFFFFFF);
  int_install_irq_handler(IRQ_LCD, (PFV) LCD_int_handler);
  int_enable(IRQ_LCD);
  int_clear(IRQ_LCD);
  LCD_IF->int_mask = 0x0F;	// Change mask accordingly
  enable_irq();

  /* Start sending data & commands to the LCD  */
  while(1){

    /* check if there is space in fifo */
    while (!(LCD_STS_COUNT_GET(LCD_IF->status) < 16));
	/* Send Data to the LCD   */
	LCD_IF->data_byte = 0x55;

    /* check if there is space in fifo */
    while (!(LCD_STS_COUNT_GET(LCD_IF->status) < 16));
	/* Send a Command to the LCD   */
	LCD_IF->inst_byte = 0x55;
  }

 
  return 1;
}
