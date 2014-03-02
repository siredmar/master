/***********************************************************************
 * $Id:: sram_timing.c 8087 2011-09-14 04:13:00Z ing02124              $
 *
 * Project: SRAM Timing test
 *
 * Description:
 *     SRAM Timing test: bandwidth, memory acces timings.
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
#include "lpc_string.h"
#include "lpc_usb.h"
#include "lpc_irq_fiq.h"
#include "lpc_arm922t_cp15_driver.h"
#include "lpc313x_mpmc.h"
#include "lpc313x_sysreg.h"
#include "ea3131_board.h"
#include "lpc313x_cgu_driver.h"
#include "lpc313x_timer_driver.h"
#include "lpc313x_intc_driver.h"
#include "lpc313x_ioconf_driver.h"
#include "lpc313x_uart_driver.h"
#include "lpc313x_dma_driver.h"
#include "lpc31xx_vmem_driver.h"

extern int usb_test_start(void);

#define PLL_180M  {CGU_FIN_SELECT_FFAST, 770,   8191, 98, 0, 16,   8, 0, 180000000}
#define PLL_178M  {CGU_FIN_SELECT_FFAST, 770,   8191, 98, 0, 16,   8, 0, 178000000}
#define PLL_176M  {CGU_FIN_SELECT_FFAST, 514,  30037, 98, 0, 32,  15, 0, 176000000}
#define PLL_174M  {CGU_FIN_SELECT_FFAST, 514,  30037, 98, 0, 32,  15, 0, 174000000}
#define PLL_172M  {CGU_FIN_SELECT_FFAST, 514,  30037, 98, 0, 32,  15, 0, 172000000}
#define PLL_170M  {CGU_FIN_SELECT_FFAST, 770,   4095, 98, 0, 16,   8, 0, 170000000}
#define PLL_168M  {CGU_FIN_SELECT_FFAST, 770,   4095, 98, 0, 16,   8, 0, 168000000}
#define PLL_166M  {CGU_FIN_SELECT_FFAST, 770,   4095, 98, 0, 16,   8, 0, 166000000}
#define PLL_164M  {CGU_FIN_SELECT_FFAST, 514,  32085, 98, 0, 28,  14, 0, 164000000}
#define PLL_162M  {CGU_FIN_SELECT_FFAST, 514,  32085, 98, 0, 28,  14, 0, 162000000}
#define PLL_160M  {CGU_FIN_SELECT_FFAST, 514,  32085, 98, 0, 28,  14, 0, 160000000}
#define PLL_158M  {CGU_FIN_SELECT_FFAST, 770,   2047, 98, 0, 16,   7, 0, 158000000}
#define PLL_156M  {CGU_FIN_SELECT_FFAST, 770,   2047, 98, 0, 16,   7, 0, 156000000}
#define PLL_154M  {CGU_FIN_SELECT_FFAST, 770,   2047, 98, 0, 16,   7, 0, 154000000}
#define PLL_152M  {CGU_FIN_SELECT_FFAST, 514,  32597, 98, 0, 28,  13, 0, 152000000}
#define PLL_150M  {CGU_FIN_SELECT_FFAST, 514,  32597, 98, 0, 28,  13, 0, 150000000}
#define PLL_148M  {CGU_FIN_SELECT_FFAST, 514,  32597, 98, 0, 28,  13, 0, 148000000}
#define PLL_146M  {CGU_FIN_SELECT_FFAST, 770,   1023, 98, 0, 16,   7, 0, 146000000}
#define PLL_144M  {CGU_FIN_SELECT_FFAST, 770,   1023, 98, 0, 16,   7, 0, 144000000}
#define PLL_142M  {CGU_FIN_SELECT_FFAST, 770,   1023, 98, 0, 16,  07, 0, 142000000}
#define PLL_140M  {CGU_FIN_SELECT_FFAST, 514,  32725, 98, 0, 24,  12, 0, 140000000}
#define PLL_138M  {CGU_FIN_SELECT_FFAST, 514,  32725, 98, 0, 24,  12, 0, 138000000}
#define PLL_136M  {CGU_FIN_SELECT_FFAST, 514,  19660, 66, 0, 48,  23, 0, 136000000}
#define PLL_134M  {CGU_FIN_SELECT_FFAST, 514,  19660, 66, 0, 48,  23, 0, 134000000}
#define PLL_132M  {CGU_FIN_SELECT_FFAST, 770,  32746, 66, 0, 24,  12, 0, 132000000}
#define PLL_130M  {CGU_FIN_SELECT_FFAST, 514,  21299, 66, 0, 44,  22, 0, 130000000}
#define PLL_128M  {CGU_FIN_SELECT_FFAST, 514,  21299, 66, 0, 44,  22, 0, 128000000}
#define PLL_126M  {CGU_FIN_SELECT_FFAST, 770,  32757, 66, 0, 24,  11, 0, 126000000}
#define PLL_124M  {CGU_FIN_SELECT_FFAST, 514,  21708, 66, 0, 44,  21, 0, 124000000}
#define PLL_122M  {CGU_FIN_SELECT_FFAST, 514,  21708, 66, 0, 44,  21, 0, 122000000}
#define PLL_120M  {CGU_FIN_SELECT_FFAST, 770,  32762, 66, 0, 24,  11, 0, 120000000}
#define PLL_114M  {CGU_FIN_SELECT_FFAST, 770,  32765, 66, 0, 20,  10, 0, 114000000}
#define PLL_108M  {CGU_FIN_SELECT_FFAST, 770,  32766, 66, 0, 20,  10, 0, 108000000}
#define PLL_102M  {CGU_FIN_SELECT_FFAST, 770,  32767, 66, 0, 20,   9, 0, 102000000}
#define PLL_96M   {CGU_FIN_SELECT_FFAST, 770,  16383, 66, 0, 20,   9, 0, 96000000}

#define MAX_CLKS 35
static CGU_HPLL_SETUP_T g_pll[MAX_CLKS] =
{
  PLL_180M,
  PLL_178M,
  PLL_176M,
  PLL_174M,
  PLL_172M,
  PLL_170M,
  PLL_168M,
  PLL_166M,
  PLL_164M,
  PLL_162M,
  PLL_160M,
  PLL_158M,
  PLL_156M,
  PLL_154M,
  PLL_152M,
  PLL_150M,
  PLL_148M,
  PLL_146M,
  PLL_144M,
  PLL_142M,
  PLL_140M,
  PLL_138M,
  PLL_136M,
  PLL_134M,
  PLL_132M,
  PLL_130M,
  PLL_128M,
  PLL_126M,
  PLL_124M,
  PLL_122M,
  PLL_120M,
  PLL_114M,
  PLL_108M,
  PLL_102M,
  PLL_96M,
};
/* PLL index */
static UNS_32 pll_id = 0;

/* Timer device handles */
INT_32 g_timer0_dev, g_uart_id;

/* Number of milliSeconds */
static volatile UNS_32 dma_ints;
static UNS_16 ALIGNED(32) virt_dma_buff[1024];
static UNS_16* dma_buff = virt_dma_buff;
static volatile INT_32 dmach;
static DMAC_REGS_T *dmaregs;
static volatile UNS_32 bandwidth = 0;
static volatile PFV curr_test;
static UNS_32 arm_tx_hwords;
static volatile UNS_16* arm_mem_ptr = (volatile UNS_16*)EXT_SRAM0_0_BASE;
static UNS_32 do_verify = 0;
static char tmp_buff[64];

void verify(void);
void stop_test(void);

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
void uart_string_write(char *pbuff, UNS_32 len)
{
  UNS_32 written;

  if (g_uart_id != 0)
  {
    if (len == 0)
      len = str_size(pbuff);

    while (len > 0)
    {
      written = uart_write(g_uart_id, pbuff, len);
      len -= written;
      pbuff += written;
    }
  }
}

/***********************************************************************
 *
 * Function: timer0_user_interrupt
 *
 * Purpose: Timer 0 interrupt handler
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
void timer0_user_interrupt(void)
{
  /* Clear latched timer interrupt */
  timer_ioctl(g_timer0_dev, TMR_CLEAR_INTS, 1);

  bandwidth = (dma_ints >> 3);
  dma_ints = 0;
}


/***********************************************************************
 *
 * Function: dma_callback
 *
 * Purpose: Count bandwidth transfer
 *
 * Processing:
 *     For each call to this handler, the DMA circuilar buffer half
 *     that isn't active is refilled. Once all data is transferred,
 *     a flag is set to end the program.
 *
 * Parameters:
 *    pdmaregs: Pointer to DMA registers
 *
 * Outputs: None
 *
 * Returns: Always returns 1
 *
 * Notes: None
 *
 **********************************************************************/
void dma_callback(INT_32 ch, DMA_IRQ_TYPE_T itype, 
    void *pdmaregs)
{
  UNS_32 irqmask = dmaregs->irq_status_clear & (0x3 << dmach);

  dmaregs->irq_status_clear = irqmask;
  dma_ints++;
  if (do_verify)
  {
    stop_test();
    verify();
  }
}

/***********************************************************************
 *
 * Function: mem_init
 *
 * Purpose:
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
void read_param(void)
{
  UNS_8 ch;
  UNS_32 reg = 0, val = 0;
  UNS_8 buff[16];
  INT_32 bread = 0;

  buff[10] = ')';
  buff[11] = ':';
  buff[12] = ' ';
  buff[13] = 0;

  uart_string_write("\r\n Enter config reg (", 0);
  str_makehex(buff, MPMC->static_mem[0].config, 8);
  buff[10] = ')';
  uart_string_write((char *)buff, 0);
  uart_string_write("\r\n 0 - 16 ", 0);
  uart_string_write("\r\n 1 - 16, buf ", 0);
  uart_string_write("\r\n 2 - 16, PM ", 0);
  uart_string_write("\r\n 3 - 16, PM, buf ", 0);
  uart_string_write("\r\n 4 - 8 ", 0);
  uart_string_write("\r\n 5 - 8, buf ", 0);
  uart_string_write("\r\n 6 - 8, PM  ", 0);
  uart_string_write("\r\n 7 - 8, PM, buf ", 0);
  uart_string_write("\r\n : ", 0);
  while (uart_read(g_uart_id, &ch, 1) < 1);
  uart_string_write((char *)&ch, 1);
  uart_string_write("\r\n", 0);
  hex_char_to_val(ch, &val);
  if ((val & _BIT(2)) == 0)
    reg = 1;

  if (val & _BIT(0))
    reg |= _BIT(19);

  if (val & _BIT(1))
    reg |= _BIT(3);

  MPMC->static_mem[0].config = reg | MPMC_STATIC_CFG_BLS;
  sprintf(tmp_buff, "\r \n New config reg: 0x%x \r\n", (int)MPMC->static_mem[0].config);
  uart_string_write(tmp_buff, 0);

  uart_string_write("\r\n Enter wtwen reg (", 0);
  str_makehex(buff, MPMC->static_mem[0].wtwen, 8);
  buff[10] = ')';
  uart_string_write((char *)buff, 0);
  while (uart_read(g_uart_id, &ch, 1) < 1);
  uart_string_write((char *)&ch, 1);
  uart_string_write("\r\n", 0);
  hex_char_to_val(ch, &val);
  MPMC->static_mem[0].wtwen = val;
  sprintf(tmp_buff, "\r\n New wtwen reg: 0x%x \r\n", (int)MPMC->static_mem[0].wtwen);
  uart_string_write(tmp_buff, 0);

  uart_string_write("\r\n Enter wtoen reg (", 0);
  str_makehex(buff, MPMC->static_mem[0].wtoen, 8);
  buff[10] = ')';
  uart_string_write((char *)buff, 0);
  while (uart_read(g_uart_id, &ch, 1) < 1);
  uart_string_write((char *)&ch, 1);
  uart_string_write("\r\n", 0);
  hex_char_to_val(ch, &val);
  MPMC->static_mem[0].wtoen = val;
  sprintf(tmp_buff, "\r\n New wtoen reg: 0x%x \r\n", (int)MPMC->static_mem[0].wtoen);
  uart_string_write(tmp_buff, 0);

  uart_string_write("\r\n Enter wtrd reg (", 0);
  str_makehex(buff, MPMC->static_mem[0].wtrd, 8);
  buff[10] = ')';
  uart_string_write((char *)buff, 0);
  while (uart_read(g_uart_id, &ch, 1) < 1);
  uart_string_write((char *)&ch, 1);
  uart_string_write("\r\n", 0);
  hex_char_to_val(ch, &val);
  MPMC->static_mem[0].wtrd = val;
  sprintf(tmp_buff, "\r\n New wtrd reg: 0x%x \r\n", (int)MPMC->static_mem[0].wtrd);
  uart_string_write(tmp_buff, 0);

  uart_string_write("\r\n Enter wtpg reg (", 0);
  str_makehex(buff, MPMC->static_mem[0].wtpg, 8);
  buff[10] = ')';
  uart_string_write((char *)buff, 0);
  while (uart_read(g_uart_id, &ch, 1) < 1);
  uart_string_write((char *)&ch, 1);
  uart_string_write("\r\n", 0);
  hex_char_to_val(ch, &val);
  MPMC->static_mem[0].wtpg = val;
  sprintf(tmp_buff, "\r\n New wtpg reg: 0x%x \r\n", (int)MPMC->static_mem[0].wtpg);
  uart_string_write(tmp_buff, 0);

  uart_string_write("\r\n Enter wtwr reg (", 0);
  str_makehex(buff, MPMC->static_mem[0].wtwr, 8);
  buff[10] = ')';
  uart_string_write((char *)buff, 0);
  while (uart_read(g_uart_id, &ch, 1) < 1);
  uart_string_write((char *)&ch, 1);
  uart_string_write("\r\n", 0);
  hex_char_to_val(ch, &val);
  MPMC->static_mem[0].wtwr = val;
  sprintf(tmp_buff, "\r\n New wtwr reg: 0x%x \r\n", (int)MPMC->static_mem[0].wtwr);
  uart_string_write(tmp_buff, 0);

  uart_string_write("\r\n Enter wtturn reg (", 0);
  str_makehex(buff, MPMC->static_mem[0].wtturn, 8);
  buff[10] = ')';
  uart_string_write((char *)buff, 0);
  while (uart_read(g_uart_id, &ch, 1) < 1);
  uart_string_write((char *)&ch, 1);
  uart_string_write("\r\n", 0);
  hex_char_to_val(ch, &val);
  MPMC->static_mem[0].wtturn = val;
  sprintf(tmp_buff, "\r\n New wtturn reg: 0x%x \r\n", (int)MPMC->static_mem[0].wtturn);
  uart_string_write(tmp_buff, 0);

  uart_string_write("\r\n Split read wait (reg:", 0);
  str_makehex(buff, SYS_REGS->mpmc_waitread_delay0, 8);
  buff[10] = ')';
  uart_string_write((char *)buff, 0);
  while (uart_read(g_uart_id, &ch, 1) < 1);
  uart_string_write((char *)&ch, 1);
  uart_string_write("\r\n", 0);
  hex_char_to_val(ch, &val);
  if (val)
    SYS_REGS->mpmc_waitread_delay0 = _BIT(5) | val;
  else
    SYS_REGS->mpmc_waitread_delay0 = 0;
  sprintf(tmp_buff, "\r\n New waitread_delay0 reg: 0x%x \r\n", (int)SYS_REGS->mpmc_waitread_delay0);
  uart_string_write(tmp_buff, 0);

  uart_string_write("\r\n Verify (0 - 1):", 0);
  while (uart_read(g_uart_id, &ch, 1) < 1);
  uart_string_write((char *)&ch, 1);
  uart_string_write("\r\n", 0);
  hex_char_to_val(ch, &do_verify);
  sprintf(tmp_buff, "\r\n New do_verify: 0x%x \r\n", (int)do_verify);
  uart_string_write(tmp_buff, 0);

  uart_string_write("\r\n Current PLL freq ", 0);
  str_makedec(buff, g_pll[pll_id].freq);
  uart_string_write((char *)buff, 0);
  uart_string_write("\r\n Enter PLL option (0 - 34) ", 0);
  bread = 0;
  while (bread < 2)
  {
    bread += uart_read(g_uart_id, &buff[bread], 2 - bread);
  }
  buff[2] = '\0';
  uart_string_write((char *)buff, 0);
  uart_string_write("\r\n", 0);
  str_dec_to_val(buff, &val);
  if (val >= MAX_CLKS)
    val = MAX_CLKS - 1;
  uart_string_write("\r\n Setting PLL freq [", 0);
  str_makedec(buff, val);
  uart_string_write((char *)buff, 0);
  uart_string_write("]: ", 0);
  str_makedec(buff, g_pll[val].freq);
  uart_string_write((char *)buff, 0);
  uart_string_write("\r\n", 0);

  /* set HPLL1 - main PLL to default speed */
  cgu_hpll_config(CGU_HPLL1_ID, &g_pll[val]);
  pll_id = val;
}
void stop_test(void)
{
  curr_test = (PFV)0;
  arm_tx_hwords = bandwidth = dma_ints = 0;
  arm_mem_ptr = (volatile UNS_16*)EXT_SRAM0_0_BASE;
  /* disable timers (starts counting) */
  timer_ioctl(g_timer0_dev, TMR_ENABLE, 0);
  int_disable(IRQ_TIMER0);
  /* Disable channel */
  dmaregs->irq_mask |= (0x3 << (2 * dmach));
  dmaregs->channel[dmach].enable = 0;
  uart_string_write("\r\n Stopping test \r\n", 0);
}

void start_dma_burst_read_test(void)
{
  uart_string_write("\r\n Starting read test \r\n", 0);
  /* Setup DMA channel and circular buffer */
  dmaregs->channel[dmach].source = EXT_SRAM0_0_BASE;
  dmaregs->channel[dmach].destination = (UNS_32) dma_buff;
  dmaregs->channel[dmach].length = 1024 / 16 - 1;
  if (do_verify)
  {
    dmaregs->channel[dmach].config = (DMA_CFG_TX_BURST);
  }
  else
  {
    dmaregs->channel[dmach].config = (DMA_CFG_CIRC_BUF | DMA_CFG_TX_BURST);
  }

  /* enable DMA channel interrupts and start DMA */
  dmaregs->irq_mask &= ~(0x1 << (2 * dmach));
  dmaregs->channel[dmach].enable = 1;

  /* Enable timers (starts counting) */
  timer_ioctl(g_timer0_dev, TMR_ENABLE, 1);
  /* Enable timer interrupts in the interrupt controller */
  int_enable(IRQ_TIMER0);
}
void start_dma_burst_write_test(void)
{
  uart_string_write("\r\n Starting write test \r\n", 0);
  /* Setup DMA channel and circular buffer */
  dmaregs->channel[dmach].source = (UNS_32) dma_buff;
  dmaregs->channel[dmach].destination = EXT_SRAM0_0_BASE;
  dmaregs->channel[dmach].length = 1024 / 16 - 1;
  dmaregs->channel[dmach].config = (DMA_CFG_CIRC_BUF | DMA_CFG_TX_BURST);

  /* enable DMA channel interrupts and start DMA */
  dmaregs->irq_mask &= ~(0x1 << (2 * dmach));
  dmaregs->channel[dmach].enable = 1;
  /* Enable timers (starts counting) */
  timer_ioctl(g_timer0_dev, TMR_ENABLE, 1);
  /* Enable timer interrupts in the interrupt controller */
  int_enable(IRQ_TIMER0);
}

void start_dma_16b_read_test(void)
{
  uart_string_write("\r\n Starting read test \r\n", 0);
  /* Setup DMA channel and circular buffer */
  dmaregs->channel[dmach].source = EXT_SRAM0_0_BASE;
  dmaregs->channel[dmach].destination = (UNS_32) dma_buff;
  dmaregs->channel[dmach].length = 1024 / 2 - 1;
  if (do_verify)
  {
    dmaregs->channel[dmach].config = (DMA_CFG_TX_HWORD);
  }
  else
  {
    dmaregs->channel[dmach].config = (DMA_CFG_CIRC_BUF | DMA_CFG_TX_HWORD);
  }

  /* enable DMA channel interrupts and start DMA */
  dmaregs->irq_mask &= ~(0x1 << (2 * dmach));
  dmaregs->channel[dmach].enable = 1;

  /* Enable timers (starts counting) */
  timer_ioctl(g_timer0_dev, TMR_ENABLE, 1);
  /* Enable timer interrupts in the interrupt controller */
  int_enable(IRQ_TIMER0);
}
void start_dma_16b_write_test(void)
{
  uart_string_write("\r\n Starting write test \r\n", 0);
  /* Setup DMA channel and circular buffer */
  dmaregs->channel[dmach].source = (UNS_32) dma_buff;
  dmaregs->channel[dmach].destination = EXT_SRAM0_0_BASE;
  dmaregs->channel[dmach].length = 1024 / 2 - 1;
  dmaregs->channel[dmach].config = (DMA_CFG_CIRC_BUF | DMA_CFG_TX_HWORD);

  /* enable DMA channel interrupts and start DMA */
  dmaregs->irq_mask &= ~(0x1 << (2 * dmach));
  dmaregs->channel[dmach].enable = 1;
  /* Enable timers (starts counting) */
  timer_ioctl(g_timer0_dev, TMR_ENABLE, 1);
  /* Enable timer interrupts in the interrupt controller */
  int_enable(IRQ_TIMER0);
}
void arm_burst_read_test(void)
{
#ifdef __arm
  UNS_32 tmp0, tmp1, tmp2, tmp3;
  UNS_16* dest = &dma_buff[arm_tx_hwords];
  volatile UNS_16* src = arm_mem_ptr;
  __asm
  {
    LDMIA src, {tmp0, tmp1, tmp2, tmp3}
    STMIA dest, {tmp0, tmp1, tmp2, tmp3}
  }
#endif
  arm_tx_hwords += 8;
  arm_mem_ptr += 8;
  if (arm_tx_hwords & 0x200)
  {
    dma_ints++;
    arm_tx_hwords = 0;
    arm_mem_ptr = (volatile UNS_16*)EXT_SRAM0_0_BASE;
  }

}
void arm_burst_write_test(void)
{
#ifdef __arm
  UNS_32 tmp0, tmp1, tmp2, tmp3;
  UNS_16* src = &dma_buff[arm_tx_hwords];
  volatile UNS_16* dest = arm_mem_ptr;
  __asm
  {
    LDMIA src, {tmp0, tmp1, tmp2, tmp3}
    STMIA dest, {tmp0, tmp1, tmp2, tmp3}
  }
#endif
  arm_tx_hwords += 8;
  arm_mem_ptr += 8;
  if (arm_tx_hwords & 0x200)
  {
    dma_ints++;
    arm_tx_hwords = 0;
    arm_mem_ptr = (volatile UNS_16*)EXT_SRAM0_0_BASE;
  }
}
void arm_16b_read_test(void)
{
  dma_buff[arm_tx_hwords++] = *arm_mem_ptr++;
  if (arm_tx_hwords & 0x200)
  {
    dma_ints++;
    arm_tx_hwords = 0;
    arm_mem_ptr = (volatile UNS_16*)EXT_SRAM0_0_BASE;
  }

}
void arm_16b_write_test(void)
{
  *arm_mem_ptr++ = dma_buff[arm_tx_hwords++];
  if (arm_tx_hwords & 0x200)
  {
    dma_ints++;
    arm_tx_hwords = 0;
    arm_mem_ptr = (volatile UNS_16*)EXT_SRAM0_0_BASE;
  }
}

void arm_16bs_read_test(void)
{
  volatile UNS_16* mem_barier = (volatile UNS_16*)EXT_SRAM0_1_BASE;
  dma_buff[arm_tx_hwords] = *mem_barier;
  dma_buff[arm_tx_hwords++] = *arm_mem_ptr++;
  if (arm_tx_hwords & 0x200)
  {
    dma_ints++;
    arm_tx_hwords = 0;
    arm_mem_ptr = (volatile UNS_16*)EXT_SRAM0_0_BASE;
  }

}
void arm_16bs_write_test(void)
{
  *arm_mem_ptr++ = dma_buff[arm_tx_hwords++];
  if (arm_tx_hwords & 0x200)
  {
    dma_ints++;
    arm_tx_hwords = 0;
    arm_mem_ptr = (volatile UNS_16*)EXT_SRAM0_0_BASE;
  }
}

void verify(void)
{
  UNS_16 data, expect;
  int i;
  UNS_8 buff[36];


  uart_string_write("\r\n Starting verification \r\n", 0);
  uart_string_write("\r\n(idx/data/expect) \r\n", 0);

  for (i = 0; i < 512; i++)
  {
    //0-0, 1-3, 2-6, 3-9
    expect = 0;
    if (i & _BIT(0))
      expect |= _BIT(0);
    if (i & _BIT(1))
      expect |= _BIT(3);
    if (i & _BIT(2))
      expect |= _BIT(6);
    if (i & _BIT(3))
      expect |= _BIT(9);

    data = dma_buff[i] & 0x249;

    /*
        expect = (
          (i & _BIT(0)) |
          ((i & _BIT(1)) << 3) |
          ((i & _BIT(2)) << 6) |
          ((i & _BIT(3)) << 9)
          );
    */
    if (data != expect)
    {
      uart_string_write("\r\n", 0);
      str_makehex((UNS_8*)&buff[0], i, 8);
      buff[10] = '/';
      str_makehex((UNS_8*)&buff[11], data, 8);
      buff[21] = '/';
      str_makehex((UNS_8*)&buff[22], expect, 8);
      uart_string_write((char *)buff, 0);

    }

  }
  uart_string_write("\r\n verification Done!!\r\n", 0);
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
  UNS_8 ch;
  UNS_8 buff[16];
  STATUS status;

  /* Disable interrupts in ARM core */
  disable_irq_fiq();

  /* Setup miscellaneous board functions */
  ea3131_board_init();

  /* Set virtual address of MMU table */
  cp15_set_vmmu_addr((void *)ISROM_MMU_TTB_BASE);

  /* Initialize interrupt system */
  int_initialize(0xFFFFFFFF);

  /* Install timer interrupts handlers as a IRQ interrupts */
  int_install_irq_handler(IRQ_TIMER0, (PFV) timer0_user_interrupt);

  /* Open timers - this will enable the clocks for the timers */
  g_timer0_dev = timer_open((void *)TIMER_CNTR0, 0);
  if (g_timer0_dev == (INT_32)NULL)
  {
    /* Error in opening Timer */
    return 1;
  }

  /******************************************************************/
  /* Setup timer 0 for a 10Hz match rate                            */

  /* Set timer load register to 8secs  */
  status = timer_ioctl(g_timer0_dev, TMR_SET_MSECS, 8000);
  if (status == _ERROR)
  {
    return 2;
  }

  status = timer_ioctl(g_timer0_dev, TMR_SET_PERIODIC_MODE, 0);
  if (status == _ERROR)
  {
    return 3;
  }

  /* Clear any latched timer 0 interrupts and enable match
   interrupt */
  status = timer_ioctl(g_timer0_dev, TMR_CLEAR_INTS, 1);
  if (status == _ERROR)
  {
    return 4;
  }

  /* Get DMA Physical address for DMA */
  dma_buff = (UNS_16 *)lpc31xx_va_to_pa((void *)virt_dma_buff);

  /******************************************************************/
  /* Setup DMA - a single circular buffer is used for audio. This buffer
     is 2KBytes in size and is seperated into 2 1K halves. The DMA will
     generate an interrupt when it's halfway and completely done with a
     transfer. These interrupts are used to refill the unused half of
     the buffer with new audio data. */
  dma_init();
  dmaregs = dma_get_base();
  dmach = dma_alloc_channel(0, dma_callback, (void *)dmaregs);

  /******************************************************************/
  /* Setup UART for 115.2K, 8 data bits, no parity, 1 stop bit */
  g_uart_id = uart_open((void *) UART_BASE, 0);
  if (g_uart_id == (INT_32)NULL)
  {
    /* Error in opening UART */
    return 5;
  }

  uart_string_write("\r\nLPC313x SRAM MEM test>", 0);
  uart_string_write("\r\n 0 - Set timings ", 0);
  uart_string_write("\r\n 1 - Start DMA Burst(16 bytes) read test ", 0);
  uart_string_write("\r\n 2 - Start DMA Burst(16 bytes) Write test ", 0);
  uart_string_write("\r\n 3 - Start DMA 16 bit read test ", 0);
  uart_string_write("\r\n 4 - Start DMA 16 bit Write test ", 0);
  uart_string_write("\r\n 5 - Start ARM Burst(16 bytes) read test ", 0);
  uart_string_write("\r\n 6 - Start ARM Burst(16 bytes) Write test ", 0);
  uart_string_write("\r\n 7 - Start ARM 16 bit read test ", 0);
  uart_string_write("\r\n 8 - Start ARM 16 bit Write test ", 0);
  uart_string_write("\r\n 9 - Start ARM 16 bit split read test ", 0);
  uart_string_write("\r\n a - Start ARM 16 bit split Write test ", 0);
  uart_string_write("\r\n b - Start USB bandwidth test ", 0);
  uart_string_write("\r\n : ", 0);

  /* Enable IRQ interrupts in the ARM core */
  enable_irq();

  ch = 0;
  while (ch != 27)
  {
    if (bandwidth)
    {
      str_makedec((UNS_8*)&buff[0], bandwidth);
      bandwidth = 0;
      uart_string_write("\r\nRate KBs:", 0);
      uart_string_write((char *)buff, 0);
    }
    if (curr_test)
      curr_test();

    if (uart_read(g_uart_id, &ch, 1) > 0)
    {
      uart_string_write((char *)&ch, 1);
      uart_string_write("\r\n", 0);
      stop_test();
      if (ch < 'a')
      {
        int i = 512;
        while (i-- > 0)
        {
          dma_buff[i] = 0;
        }
      }
      switch (ch)
      {
        case '0':
          read_param();
          break;
        case '1':
          start_dma_burst_read_test();
          break;
        case '2':
          start_dma_burst_write_test();
          break;
        case '3':
          start_dma_16b_read_test();
          break;
        case '4':
          start_dma_16b_write_test();
          break;
        case '5':
          uart_string_write("\r\n Starting ARM burst(16 bytes) read test \r\n", 0);
          curr_test = arm_burst_read_test;
          break;
        case '6':
          uart_string_write("\r\n Starting ARM burst(16 bytes) write test \r\n", 0);
          curr_test = arm_burst_write_test;
          break;
        case '7':
          uart_string_write("\r\n Starting ARM 16 bit read test \r\n", 0);
          curr_test = arm_16b_read_test;
          break;
        case '8':
          uart_string_write("\r\n Starting ARM 16 bit write test \r\n", 0);
          curr_test = arm_16b_write_test;
          break;
        case '9':
          uart_string_write("\r\n Starting ARM 16 bit split-read test \r\n", 0);
          curr_test = arm_16bs_read_test;
          break;
        case 'a':
          uart_string_write("\r\n Starting ARM 16 bit split-write test \r\n", 0);
          curr_test = arm_16bs_write_test;
          break;
        case 'b':
          uart_string_write("\r\n Starting USB bandwidth test \r\n", 0);
          usb_test_start();
          break;
        default:
          break;
      }
      if ((ch > '4') && (ch < 'b'))
      {
        /* Enable timers (starts counting) */
        timer_ioctl(g_timer0_dev, TMR_ENABLE, 1);
        /* Enable timer interrupts in the interrupt controller */
        int_enable(IRQ_TIMER0);
      }
      uart_string_write("\r\nLPC313x SRAM MEM test>", 0);
    }
  }

  /* Disable timer interrupts in the interrupt controller */
  int_disable(IRQ_TIMER0);

  /* Disable interrupts in ARM core */
  disable_irq_fiq();

  /* Close timers */
  timer_close(g_timer0_dev);

  return 1;
}
