/* lpc313x.h

   written by Durgesh Pattamatta
   10 Oct 2008

   Copyright (C) 2008 NXP Semiconductors

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#if !defined (__LPC313X_H__)
#    define   __LPC313X_H__

/* ----- Includes */
#include <apex.h>

#include <asm/reg.h>

/* ----- Types */

/* ----- Globals */
#define _BIT(n)	  ((1) << (n))
/* shift bit field */
#define _SBF(f,v) ((v) << (f))

/* ----- Prototypes */

	/* Registers */
/***********************************************************************
 * WDT register definitions
 **********************************************************************/
#define WDT_PHYS      (0x13002400)
#define WDT_IR       __REG (WDT_PHYS + 0x00)
#define WDT_TCR      __REG (WDT_PHYS + 0x04)
#define WDT_TC       __REG (WDT_PHYS + 0x08)
#define WDT_PR       __REG (WDT_PHYS + 0x0c)
#define WDT_MCR      __REG (WDT_PHYS + 0x14)
#define WDT_MR0      __REG (WDT_PHYS + 0x18)
#define WDT_MR1      __REG (WDT_PHYS + 0x1c)
#define WDT_EMR      __REG (WDT_PHYS + 0x3c)

#define WDT_IR_MR1        _BIT(1)
#define WDT_IR_MR0        _BIT(0)
#define WDT_TCR_CNT_RESET _BIT(1)
#define WDT_TCR_CNT_EN    _BIT(0)
#define WDT_MCR_STOP_MR1  _BIT(5)
#define WDT_MCR_RESET_MR1 _BIT(4)
#define WDT_MCR_INT_MR1   _BIT(3)
#define WDT_MCR_STOP_MR0  _BIT(2)
#define WDT_MCR_RESET_MR0 _BIT(1)
#define WDT_MCR_INT_MR0   _BIT(0)
#define WDT_EMR_CTRL0(n)  _SBF(4,((n) &0x3))
#define WDT_EMR_CTRL1(n)  _SBF(6,((n) &0x3))
#define WDT_EMR_M1        _BIT(1)
#define WDT_EMR_M0        _BIT(0)

/***********************************************************************
 * Timer register definitions
 **********************************************************************/
#define TIMER0_PHYS     (0x13008000)
#define TIMER1_PHYS     (0x13008400)
#define TIMER2_PHYS     (0x13008800)
#define TIMER3_PHYS     (0x13008c00)
#define TIMER_LOAD      0x00
#define TIMER_VALUE     0x04
#define TIMER_CONTROL   0x08
#define TIMER_CLEAR     0x0c

#define TM_CTRL_ENABLE    _BIT(7)
#define TM_CTRL_MODE      _BIT(6)
#define TM_CTRL_PERIODIC  _BIT(6)
#define TM_CTRL_PS1       _SBF(2, 0)
#define TM_CTRL_PS16      _SBF(2, 1)
#define TM_CTRL_PS256     _SBF(2, 2)
#define TM_CTRL_PS_MASK   _SBF(2, 0x3)

/***********************************************************************
 * UART register definitions
 **********************************************************************/
#define UART_PHYS     (0x15001000)
#define UART_DLL      __REG (UART_PHYS + 0x00)
#define UART_FIFO     __REG (UART_PHYS + 0x00)
#define UART_IE       __REG (UART_PHYS + 0x04)
#define UART_DLM      __REG (UART_PHYS + 0x04)
#define UART_IIR      __REG (UART_PHYS + 0x08)
#define UART_FCR      __REG (UART_PHYS + 0x08)
#define UART_LCR      __REG (UART_PHYS + 0x0c)
#define UART_MCR      __REG (UART_PHYS + 0x10)
#define UART_LSR      __REG (UART_PHYS + 0x14)
#define UART_MSR      __REG (UART_PHYS + 0x18)
#define UART_SCR      __REG (UART_PHYS + 0x1c)
#define UART_ACR      __REG (UART_PHYS + 0x20)
#define UART_ICR      __REG (UART_PHYS + 0x24)
#define UART_FDR      __REG (UART_PHYS + 0x28)


#define UART_LOAD_DLL(div)          ((div) & 0xFF)
#define UART_LOAD_DLM(div)          (((div) >> 8) & 0xFF)
#define UART_LCR_DIVLATCH_EN       _BIT(7)
#define UART_LCR_WLEN_8BITS        _SBF(0, 3)
#define UART_FCR_DMA_MODE          _BIT(3)
#define UART_FCR_TXFIFO_FLUSH      _BIT(2)
#define UART_FCR_RXFIFO_FLUSH      _BIT(1)
#define UART_FCR_FIFO_EN           _BIT(0)
#define UART_LSR_FIFORX_ERR        _BIT(7)
#define UART_LSR_TEMT              _BIT(6)
#define UART_LSR_FR                _BIT(3)
#define UART_LSR_PE                _BIT(2)
#define UART_LSR_OE                _BIT(1)
#define UART_LSR_RDR               _BIT(0)

/***********************************************************************
 * SYS_REG register definitions
 **********************************************************************/
#define SYS_PHYS            (0x13002800)
#define SYS_SDMMC_DELAYMODES   __REG (SYS_PHYS + 0x2C)
#define SYS_USB_ATX_PLL_PD_REG __REG (SYS_PHYS + 0x30)
#define SYS_USB_OTG_CFG        __REG (SYS_PHYS + 0x34)
#define SYS_USB_OTG_LED_CTL    __REG (SYS_PHYS + 0x38)
#define SYS_USB_PLL_NDEC       __REG (SYS_PHYS + 0x40)
#define SYS_USB_PLL_MDEC       __REG (SYS_PHYS + 0x44)
#define SYS_USB_PLL_PDEC       __REG (SYS_PHYS + 0x48)
#define SYS_USB_PLL_SELR       __REG (SYS_PHYS + 0x4C)
#define SYS_USB_PLL_SELI       __REG (SYS_PHYS + 0x50)
#define SYS_USB_PLL_SELP       __REG (SYS_PHYS + 0x54)

#define SYS_MPMC_MISC       __REG (SYS_PHYS + 0x64)
#define SYS_MPMC_DELAY      __REG (SYS_PHYS + 0x68)
#define SYS_MPMC_WTD_DEL0   __REG (SYS_PHYS + 0x6C)
#define SYS_MPMC_WTD_DEL1   __REG (SYS_PHYS + 0x70)
#define SYS_MPMC_TESTMODE0  __REG (SYS_PHYS + 0x78)
#define SYS_MPMC_TESTMODE1  __REG (SYS_PHYS + 0x7C)
#define SYS_REMAP_ADDR      __REG (SYS_PHYS + 0x84)
#define SYS_MUX_LCD_EBI     __REG (SYS_PHYS + 0x90)
#define SYS_MUX_GPIO_MCI    __REG (SYS_PHYS + 0x94)
#define SYS_MUX_NAND_MCI    __REG (SYS_PHYS + 0x98)

/***********************************************************************
 * GPIO register definitions
 **********************************************************************/
#define GPIO_PHYS            (0x13003000)
#define GPIO_STATE(port)     __REG (GPIO_PHYS + (port) + 0x00)
#define GPIO_M0_SET(port)    __REG (GPIO_PHYS + (port) + 0x14)
#define GPIO_M0_RESET(port)  __REG (GPIO_PHYS + (port) + 0x18)
#define GPIO_M1_SET(port)    __REG (GPIO_PHYS + (port) + 0x24)
#define GPIO_M1_RESET(port)  __REG (GPIO_PHYS + (port) + 0x28)

#define GPIO_OUT_LOW(port, pin)  do { GPIO_M1_SET(port) = pin; GPIO_M0_RESET(port) = pin;} while(0)
#define GPIO_OUT_HIGH(port, pin) do { GPIO_M1_SET(port) = pin; GPIO_M0_SET(port) = pin;} while(0)
#define GPIO_IN(port, pin)       do { GPIO_M1_RESET(port) = pin; GPIO_M0_RESET(port) = pin;} while(0)
#define GPIO_DRV_IP(port, pin)   do { GPIO_M1_RESET(port) = pin; GPIO_M0_SET(port) = pin;} while(0)

#define IOCONF_EBI_MCI       (0x000)
#define IOCONF_EBI_I2STX_0   (0x040)
#define IOCONF_CGU           (0x080)
#define IOCONF_I2SRX_0       (0x0c0)
#define IOCONF_I2SRX_1       (0x100)
#define IOCONF_I2STX_1       (0x140)
#define IOCONF_EBI           (0x180)
#define IOCONF_GPIO          (0x1c0)
#define IOCONF_I2C1          (0x200)
#define IOCONF_SPI           (0x240)
#define IOCONF_NAND_CTRL     (0x280)
#define IOCONF_PWM           (0x2c0)
#define IOCONF_UART          (0x300)

/***********************************************************************
 * Event router register definitions
 **********************************************************************/
#define EVTR_PHYS            (0x13000000)
#define EVRT_INT_PEND(bank)  __REG (EVTR_PHYS + 0xC00 + ((bank) << 2))
#define EVRT_INT_CLR(bank)   __REG (EVTR_PHYS + 0xC20 + ((bank) << 2))
#define EVRT_INT_SET(bank)   __REG (EVTR_PHYS + 0xC40 + ((bank) << 2))
#define EVRT_MASK(bank)      __REG (EVTR_PHYS + 0xC60 + ((bank) << 2))
#define EVRT_MASK_CLR(bank)  __REG (EVTR_PHYS + 0xC80 + ((bank) << 2))
#define EVRT_MASK_SET(bank)  __REG (EVTR_PHYS + 0xCA0 + ((bank) << 2))
#define EVRT_APR(bank)       __REG (EVTR_PHYS + 0xCC0 + ((bank) << 2))
#define EVRT_ATR(bank)       __REG (EVTR_PHYS + 0xCE0 + ((bank) << 2))
#define EVRT_RSR(bank)       __REG (EVTR_PHYS + 0xD20 + ((bank) << 2))
#define EVRT_OUT_PEND(vec,bank)     __REG (EVTR_PHYS + 0x1000 + ((vec) << 5) + ((bank) << 2))
#define EVRT_OUT_MASK(vec,bank)     __REG (EVTR_PHYS + 0x1400 + ((vec) << 5) + ((bank) << 2))
#define EVRT_OUT_MASK_CLR(vec,bank) __REG (EVTR_PHYS + 0x1800 + ((vec) << 5) + ((bank) << 2))
#define EVRT_OUT_MASK_SET(vec,bank) __REG (EVTR_PHYS + 0x1C00 + ((vec) << 5) + ((bank) << 2))

/***********************************************************************
 * MPMC memory controller register definitions
 **********************************************************************/
#define MPMC_PHYS           (0x17008000)
#define MPMC_CTRL           __REG (MPMC_PHYS + 0x000)
#define MPMC_STATUS         __REG (MPMC_PHYS + 0x004)
#define MPMC_CONFIG         __REG (MPMC_PHYS + 0x008)
#define MPMC_DYNCTL         __REG (MPMC_PHYS + 0x020)
#define MPMC_DYNREF         __REG (MPMC_PHYS + 0x024)
#define MPMC_DYRDCFG        __REG (MPMC_PHYS + 0x028)
#define MPMC_DYTRP          __REG (MPMC_PHYS + 0x030)
#define MPMC_DYTRAS         __REG (MPMC_PHYS + 0x034)
#define MPMC_DYTSREX        __REG (MPMC_PHYS + 0x038)
#define MPMC_DYTAPR         __REG (MPMC_PHYS + 0x03C)
#define MPMC_DYTDAL         __REG (MPMC_PHYS + 0x040)
#define MPMC_DYTWR          __REG (MPMC_PHYS + 0x044)
#define MPMC_DYTRC          __REG (MPMC_PHYS + 0x048)
#define MPMC_DYTRFC         __REG (MPMC_PHYS + 0x04C)
#define MPMC_DYTXSR         __REG (MPMC_PHYS + 0x050)
#define MPMC_DYTRRD         __REG (MPMC_PHYS + 0x054)
#define MPMC_DYTMRD         __REG (MPMC_PHYS + 0x058)
#define MPMC_STEXDWT        __REG (MPMC_PHYS + 0x080)
#define MPMC_DYCONFIG       __REG (MPMC_PHYS + 0x100)
#define MPMC_DYRASCAS       __REG (MPMC_PHYS + 0x104)
#define MPMC_STCONFIG0      __REG (MPMC_PHYS + 0x200)
#define MPMC_STWTWEN0       __REG (MPMC_PHYS + 0x204)
#define MPMC_STWTOEN0       __REG (MPMC_PHYS + 0x208)
#define MPMC_STWTRD0        __REG (MPMC_PHYS + 0x20C)
#define MPMC_STWTPG0        __REG (MPMC_PHYS + 0x210)
#define MPMC_STWTWR0        __REG (MPMC_PHYS + 0x214)
#define MPMC_STWTTURN0      __REG (MPMC_PHYS + 0x218)
#define MPMC_STCONFIG1      __REG (MPMC_PHYS + 0x220)
#define MPMC_STWTWEN1       __REG (MPMC_PHYS + 0x224)
#define MPMC_STWTOEN1       __REG (MPMC_PHYS + 0x228)
#define MPMC_STWTRD1        __REG (MPMC_PHYS + 0x22C)
#define MPMC_STWTPG1        __REG (MPMC_PHYS + 0x230)
#define MPMC_STWTWR1        __REG (MPMC_PHYS + 0x234)
#define MPMC_STWTTURN1      __REG (MPMC_PHYS + 0x238)

#define NS_TO_MPMCCLK(ns, clk)	(((ns)*((clk + 500)/1000) + 500000)/1000000)
/* MPMC Controller Bit Field constants*/
#define MPMC_CTL_LOW_PWR               _BIT(2)
#define MPMC_CTL_ENABLE                _BIT(0)
/* MPMC status Bit Field constants*/
#define MPMC_STATUS_SA                 _BIT(2)
#define MPMC_STATUS_WR_BUF             _BIT(1)
#define MPMC_STATUS_BUSY               _BIT(0)
/* MPMC config Bit Field constants*/
#define MPMC_CFG_SDCCLK_1_2            _BIT(8)
#define MPMC_CFG_SDCCLK_1_1            (0)
/* SDRAM Controller Bit Field constants*/
#define MPMC_SDRAMC_CTL_DP             _BIT(13)
#define MPMC_SDRAMC_CTL_NORMAL_CMD     _SBF(7,0)
#define MPMC_SDRAMC_CTL_MODE_CMD       _SBF(7,1)
#define MPMC_SDRAMC_CTL_PALL_CMD       _SBF(7,2)
#define MPMC_SDRAMC_CTL_NOP_CMD        _SBF(7,3)
#define MPMC_SDRAMC_CTL_MCC            _BIT(5)
#define MPMC_SDRAMC_CTL_SR             _BIT(2)
#define MPMC_SDRAMC_CTL_CS             _BIT(1)
#define MPMC_SDRAMC_CTL_CE             _BIT(0)
/* SDRAM Config Bit Field constants*/
#define MPMC_SDRAMC_CFG_SDRAM_MD       _SBF(3,0)
#define MPMC_SDRAMC_CFG_LOW_PWR_MD     _SBF(3,1)
#define MPMC_SDRAMC_CFG_SYNC_FLASH     _SBF(3,2)
#define MPMC_SDRAMC_CFG_BUF_EN         _BIT(19)
#define MPMC_SDRAMC_CFG_WP             _BIT(20)
/*16-bit external bus high-performance address mapping */
/*16Mb (2Mx8), 2 banks, row length = 11, column length = 9*/
#define SDRAMC_16HP_2Mx8_2B_R11_C9    (_SBF(7,0x00))
/*16Mb (1Mx16), 2 banks, row length = 11, column length = 8*/
#define SDRAMC_16HP_1Mx16_2B_R11_C8   (_SBF(7,0x01))
/*64Mb (8Mx8), 4 banks, row length = 12, column length = 9*/
#define SDRAMC_16HP_8Mx8_4B_R12_C9    (_SBF(7,0x04))
/*64Mb (4Mx16), 4 banks, row length = 12, column length = 8*/
#define SDRAMC_16HP_4Mx16_4B_R12_C8   (_SBF(7,0x05))
/*128Mb (16Mx8), 4 banks, row length = 12, column length = 10*/
#define SDRAMC_16HP_16Mx8_4B_R12_C10  (_SBF(7,0x08))
/*128Mb (8Mx16), 4 banks, row length = 12, column length = 9*/
#define SDRAMC_16HP_8Mx16_4B_R12_C9   (_SBF(7,0x09))
/*256Mb (32Mx8), 4 banks, row length = 13, column length = 10*/
#define SDRAMC_16HP_32Mx8_4B_R13_C10  (_SBF(7,0x0C))
/*256Mb (16Mx16), 4 banks, row length = 13, column length = 9*/
#define SDRAMC_16HP_16Mx16_4B_R13_C9  (_SBF(7,0x0D))
/*512Mb (64Mx8), 4 banks, row length = 13, column length = 11*/
#define SDRAMC_16HP_64Mx8_4B_R13_C11  (_SBF(7,0x10))
/*512Mb (32Mx16), 4 banks, row length = 13, column length = 10*/
#define SDRAMC_16HP_32Mx16_4B_R13_C10 (_SBF(7,0x11))

/*16-bit external bus low power SDRAM address mapping */
/*16Mb (2Mx8), 2 banks, row length = 11, column length = 9*/
#define SDRAMC_16LP_2Mx8_2B_R11_C9    (_SBF(7,0x20))
/*16Mb (1Mx16), 2 banks, row length = 11, column length = 8*/
#define SDRAMC_16LP_1Mx16_2B_R11_C8   (_SBF(7,0x21))
/*64Mb (8Mx8), 4 banks, row length = 12, column length = 9*/
#define SDRAMC_16LP_8Mx8_4B_R12_C9    (_SBF(7,0x24))
/*64Mb (4Mx16), 4 banks, row length = 12, column length = 8*/
#define SDRAMC_16LP_4Mx16_4B_R12_C8   (_SBF(7,0x25))
/*128Mb (16Mx8), 4 banks, row length = 12, column length = 10*/
#define SDRAMC_16LP_16Mx8_4B_R12_C10  (_SBF(7,0x28))
/*128Mb (8Mx16), 4 banks, row length = 12, column length = 9*/
#define SDRAMC_16LP_8Mx16_4B_R12_C9   (_SBF(7,0x29))
/*256Mb (32Mx8), 4 banks, row length = 13, column length = 10*/
#define SDRAMC_16LP_32Mx8_4B_R13_C10  (_SBF(7,0x2C))
/*256Mb (16Mx16), 4 banks, row length = 13, column length = 9*/
#define SDRAMC_16LP_16Mx16_4B_R13_C9  (_SBF(7,0x2D))
/*512Mb (64Mx8), 4 banks, row length = 13, column length = 11*/
#define SDRAMC_16LP_64Mx8_4B_R13_C11  (_SBF(7,0x30))
/*512Mb (32Mx16), 4 banks, row length = 13, column length = 10*/
#define SDRAMC_16LP_32Mx16_4B_R13_C10 (_SBF(7,0x31))


/* SDRAM Read Config Bit Field constants*/
#define MPMC_SDRAMC_RDCFG_CLKOUTDELAY_STG       _SBF(0,0)
#define MPMC_SDRAMC_RDCFG_CMDDELAY_STG          _SBF(0,1)
#define MPMC_SDRAMC_RDCFG_CMDDELAY_P1_STG       _SBF(0,2)
#define MPMC_SDRAMC_RDCFG_CMDDELAY_P2_STG       _SBF(0,3)
/* SDRAM RASCAS Bit Field constants*/
#define MPMC_SDRAMC_RASCAS_CAS0        _SBF(8,0)
#define MPMC_SDRAMC_RASCAS_CAS1        _SBF(8,1)
#define MPMC_SDRAMC_RASCAS_CAS2        _SBF(8,2)
#define MPMC_SDRAMC_RASCAS_CAS3        _SBF(8,3)
#define MPMC_SDRAMC_RASCAS_RAS0        _SBF(0,0)
#define MPMC_SDRAMC_RASCAS_RAS1        _SBF(0,1)
#define MPMC_SDRAMC_RASCAS_RAS2        _SBF(0,2)
#define MPMC_SDRAMC_RASCAS_RAS3        _SBF(0,3)


/***********************************************************************
* NAND Flash controller Register Structures
**********************************************************************/
#define NANDC_PHYS           (0x17000800)
#define NANDC_RAM0_PHYS	     (0x70000000)
#define NANDC_RAM1_PHYS	     (0x70000400)
#define NANDC_BUF_SZ         512
#define NANDC_IRQ_STS        __REG (NANDC_PHYS + 0x000)
#define NANDC_IRQ_MSK        __REG (NANDC_PHYS + 0x004)
#define NANDC_IRQ_STS_RAW    __REG (NANDC_PHYS + 0x008)
#define NANDC_CFG            __REG (NANDC_PHYS + 0x00C)
#define NANDC_IRQ_IO_CFG     __REG (NANDC_PHYS + 0x010)
#define NANDC_IRQ_TIMING1    __REG (NANDC_PHYS + 0x014)
#define NANDC_IRQ_TIMING2    __REG (NANDC_PHYS + 0x018)
#define NANDC_SET_CMD        __REG (NANDC_PHYS + 0x020)
#define NANDC_SET_ADDR       __REG (NANDC_PHYS + 0x024)
#define NANDC_WRITE_DATA     __REG (NANDC_PHYS + 0x028)
#define NANDC_SET_CE         __REG (NANDC_PHYS + 0x02C)
#define NANDC_READ_DATA      __REG (NANDC_PHYS + 0x030)
#define NANDC_CHECK_STS      __REG (NANDC_PHYS + 0x034)
#define NANDC_CTRL_FLOW      __REG (NANDC_PHYS + 0x038)
#define NANDC_GPIO1          __REG (NANDC_PHYS + 0x040)
#define NANDC_GPIO2          __REG (NANDC_PHYS + 0x044)
#define NANDC_IRQ_STS2       __REG (NANDC_PHYS + 0x048)
#define NANDC_IRQ_MSK2       __REG (NANDC_PHYS + 0x04C)
#define NANDC_IRQ_STS_RAW2   __REG (NANDC_PHYS + 0x050)

/* Register description of irq_status */
#define NAND_IRQ_RB3_POS_EDGE       _BIT(31)
#define NAND_IRQ_RB2_POS_EDGE       _BIT(30)
#define NAND_IRQ_RB1_POS_EDGE       _BIT(29)
#define NAND_IRQ_RB0_POS_EDGE       _BIT(28)
#define NAND_IRQ_RB_POS_EDGE(cs)    _BIT(28 + (cs))
#define NAND_IRQ_ERASED_RAM1        _BIT(27)
#define NAND_IRQ_ERASED_RAM0        _BIT(26)
#define NAND_IRQ_WR_RAM1            _BIT(25)
#define NAND_IRQ_WR_RAM0            _BIT(24)
#define NAND_IRQ_RD_RAM1            _BIT(23)
#define NAND_IRQ_RD_RAM0            _BIT(22)
#define NAND_IRQ_ECC_DEC_RAM0       _BIT(21)
#define NAND_IRQ_ECC_ENC_RAM0       _BIT(20)
#define NAND_IRQ_ECC_DEC_RAM1       _BIT(19)
#define NAND_IRQ_ECC_ENC_RAM1       _BIT(18)
#define NAND_IRQ_NOERR_RAM0         _BIT(17)
#define NAND_IRQ_ERR1_RAM0          _BIT(16)
#define NAND_IRQ_ERR2_RAM0          _BIT(15)
#define NAND_IRQ_ERR3_RAM0          _BIT(14)
#define NAND_IRQ_ERR4_RAM0          _BIT(13)
#define NAND_IRQ_ERR5_RAM0          _BIT(12)
#define NAND_IRQ_ERR_UNR_RAM0       _BIT(11)
#define NAND_IRQ_NOERR_RAM1         _BIT(10)
#define NAND_IRQ_ERR1_RAM1          _BIT(9)
#define NAND_IRQ_ERR2_RAM1          _BIT(8)
#define NAND_IRQ_ERR3_RAM1          _BIT(7)
#define NAND_IRQ_ERR4_RAM1          _BIT(6)
#define NAND_IRQ_ERR5_RAM1          _BIT(5)
#define NAND_IRQ_ERR_UNR_RAM1       _BIT(4)
#define NAND_IRQ_AES_DONE_RAM1      _BIT(1)
#define NAND_IRQ_AES_DONE_RAM0      _BIT(0)

/* Register description of config */
#define NAND_CFG_ECGC		    _BIT(13)
#define NAND_CFG_8BIT_ECC           _BIT(12)
#define NAND_CFG_TL_528             _SBF(10, 0x0)
#define NAND_CFG_TL_516             _SBF(10, 0x1)
#define NAND_CFG_TL_512             _SBF(10, 0x2)
#define NAND_CFG_TL_MASK            _SBF(10, 0x3)
#define NAND_CFG_EO                 _BIT(9)
#define NAND_CFG_DC                 _BIT(8)
#define NAND_CFG_M                  _BIT(7)
#define NAND_CFG_LC_0               _SBF(5, 0x0)
#define NAND_CFG_LC_1               _SBF(5, 0x1)
#define NAND_CFG_LC_2               _SBF(5, 0x2)
#define NAND_CFG_LC_MASK            _SBF(5, 0x3)
#define NAND_CFG_ES                 _BIT(4)
#define NAND_CFG_DE                 _BIT(3)
#define NAND_CFG_AO                 _BIT(2)
#define NAND_CFG_WD                 _BIT(1)
#define NAND_CFG_EC                 _BIT(0)

/* Register description of io_config */
#define NAND_IO_CFG_IO_DRIVE        _BIT(24)
#define NAND_IO_CFG_DATA_DEF(n)     _SBF(8, ((n) & 0xFFFF))
#define NAND_IO_CFG_CLE_1           _SBF(6, 0x01)
#define NAND_IO_CFG_ALE_1           _SBF(4, 0x01)
#define NAND_IO_CFG_WE_1            _SBF(2, 0x01)
#define NAND_IO_CFG_RE_1            _SBF(0, 0x01)

/* Register description of timing1 */
#define NAND_TIM1_TSRD(n)           _SBF(20, ((n) & 0x3))
#define NAND_TIM1_TALS(n)           _SBF(16, ((n) & 0x7))
#define NAND_TIM1_TALH(n)           _SBF(12, ((n) & 0x7))
#define NAND_TIM1_TCLS(n)           _SBF(4, ((n) & 0x7))
#define NAND_TIM1_TCLH(n)           ((n) & 0x7)

/* Register description of timing2 */
#define NAND_TIM2_TDRD(n)           _SBF(28, ((n) & 0x7))
#define NAND_TIM2_TEBI(n)           _SBF(24, ((n) & 0x7))
#define NAND_TIM2_TCH(n)            _SBF(20, ((n) & 0x7))
#define NAND_TIM2_TCS(n)            _SBF(16, ((n) & 0x7))
#define NAND_TIM2_TRH(n)            _SBF(12, ((n) & 0x7))
#define NAND_TIM2_TRP(n)            _SBF(8, ((n) & 0x7))
#define NAND_TIM2_TWH(n)            _SBF(4, ((n) & 0x7))
#define NAND_TIM2_TWP(n)            ((n) & 0x7)

/* Register description of set_ce */
#define NAND_SETCE_OVR_EN(n)        _BIT(((n) & 0x3) + 12)
#define NAND_SETCE_OVR_V(n)         _BIT(((n) & 0x3) + 8)
#define NAND_SETCE_WP               _BIT(4)
#define NAND_SETCE_CV_MASK          0x0F
#define NAND_SETCE_CV(n)            (0x0F & ~_BIT(((n) & 0x3)))

/* Register description of check_sts */
#define NAND_CHK_STS_RB3_EDGE       _BIT(8)
#define NAND_CHK_STS_RB2_EDGE       _BIT(7)
#define NAND_CHK_STS_RB1_EDGE       _BIT(6)
#define NAND_CHK_STS_RB0_EDGE       _BIT(5)
#define NAND_CHK_STS_RB_EDGE(n)     _BIT((n) + 5)
#define NAND_CHK_STS_RB4_LVL        _BIT(4)
#define NAND_CHK_STS_RB3_LVL        _BIT(3)
#define NAND_CHK_STS_RB2_LVL        _BIT(2)
#define NAND_CHK_STS_RB1_LVL        _BIT(1)
#define NAND_CHK_STS_APB_BSY        _BIT(0)

/* Resegister description of control_flow */
#define NAND_CTRLFLW_WRITE_RAM1     _BIT(5)
#define NAND_CTRLFLW_WRITE_RAM0     _BIT(4)
#define NAND_CTRLFLW_READ_RAM1      _BIT(1)
#define NAND_CTRLFLW_READ_RAM0      _BIT(0)
/***********************************************************************
 * SD/MMC MCI register definitions
 **********************************************************************/
#define SDMMC_PHYS            (0x18000000)
#define SDMMC_CTRL            __REG (SDMMC_PHYS + 0x000)
#define SDMMC_PWREN           __REG (SDMMC_PHYS + 0x004)
#define SDMMC_CLKDIV          __REG (SDMMC_PHYS + 0x008)
#define SDMMC_CLKSRC          __REG (SDMMC_PHYS + 0x00c)
#define SDMMC_CLKENA          __REG (SDMMC_PHYS + 0x010)
#define SDMMC_TMOUT           __REG (SDMMC_PHYS + 0x014)
#define SDMMC_CTYPE           __REG (SDMMC_PHYS + 0x018)
#define SDMMC_BLKSIZ          __REG (SDMMC_PHYS + 0x01c)
#define SDMMC_BYTCNT          __REG (SDMMC_PHYS + 0x020)
#define SDMMC_INTMASK         __REG (SDMMC_PHYS + 0x024)
#define SDMMC_CMDARG          __REG (SDMMC_PHYS + 0x028)
#define SDMMC_CMD             __REG (SDMMC_PHYS + 0x02c)
#define SDMMC_RESP0           __REG (SDMMC_PHYS + 0x030)
#define SDMMC_RESP1           __REG (SDMMC_PHYS + 0x034)
#define SDMMC_RESP2           __REG (SDMMC_PHYS + 0x038)
#define SDMMC_RESP3           __REG (SDMMC_PHYS + 0x03c)
#define SDMMC_MINTSTS         __REG (SDMMC_PHYS + 0x040)
#define SDMMC_RINTSTS         __REG (SDMMC_PHYS + 0x044)
#define SDMMC_STATUS          __REG (SDMMC_PHYS + 0x048)
#define SDMMC_FIFOTH          __REG (SDMMC_PHYS + 0x04c)
#define SDMMC_TCBCNT          __REG (SDMMC_PHYS + 0x05c)
#define SDMMC_TBBCNT          __REG (SDMMC_PHYS + 0x060)
#define SDMMC_DEBNCE          __REG (SDMMC_PHYS + 0x064)
#define SDMMC_USRID           __REG (SDMMC_PHYS + 0x068)
#define SDMMC_VERID           __REG (SDMMC_PHYS + 0x06c)
#define SDMMC_HCON            __REG (SDMMC_PHYS + 0x070)
#define SDMMC_DATA            __REG (SDMMC_PHYS + 0x100)
#define SDMMC_DATA_ADR        (SDMMC_PHYS + 0x100)

/* Control register defines */
#define SDMMC_CTRL_CEATA_INT_EN   _BIT(11)
#define SDMMC_CTRL_SEND_AS_CCSD   _BIT(10)
#define SDMMC_CTRL_SEND_CCSD      _BIT(9)
#define SDMMC_CTRL_ABRT_READ_DATA _BIT(8)
#define SDMMC_CTRL_SEND_IRQ_RESP  _BIT(7)
#define SDMMC_CTRL_READ_WAIT      _BIT(6)
#define SDMMC_CTRL_DMA_ENABLE     _BIT(5)
#define SDMMC_CTRL_INT_ENABLE     _BIT(4)
#define SDMMC_CTRL_DMA_RESET      _BIT(2)
#define SDMMC_CTRL_FIFO_RESET     _BIT(1)
#define SDMMC_CTRL_RESET          _BIT(0)
/* Clock Enable register defines */
#define SDMMC_CLKEN_LOW_PWR      _BIT(16)
#define SDMMC_CLKEN_ENABLE       _BIT(0)
/* time-out register defines */
#define SDMMC_TMOUT_DATA(n)      _SBF(8, (n))
#define SDMMC_TMOUT_DATA_MSK     0xFFFFFF00
#define SDMMC_TMOUT_RESP(n)      ((n) & 0xFF)
#define SDMMC_TMOUT_RESP_MSK     0xFF
/* card-type register defines */
#define SDMMC_CTYPE_8BIT         _BIT(16)
#define SDMMC_CTYPE_4BIT         _BIT(0)
/* Interrupt status & mask register defines */
#define SDMMC_INT_SDIO           _BIT(16)
#define SDMMC_INT_EBE            _BIT(15)
#define SDMMC_INT_ACD            _BIT(14)
#define SDMMC_INT_SBE            _BIT(13)
#define SDMMC_INT_HLE            _BIT(12)
#define SDMMC_INT_FRUN           _BIT(11)
#define SDMMC_INT_HTO            _BIT(10)
#define SDMMC_INT_DTO            _BIT(9)
#define SDMMC_INT_RTO            _BIT(8)
#define SDMMC_INT_DCRC           _BIT(7)
#define SDMMC_INT_RCRC           _BIT(6)
#define SDMMC_INT_RXDR           _BIT(5)
#define SDMMC_INT_TXDR           _BIT(4)
#define SDMMC_INT_DATA_OVER      _BIT(3)
#define SDMMC_INT_CMD_DONE       _BIT(2)
#define SDMMC_INT_RESP_ERR       _BIT(1)
#define SDMMC_INT_CD             _BIT(0)
#define SDMMC_INT_ERROR          0xbfc2
/* Command register defines */
#define SDMMC_CMD_START         _BIT(31)
#define SDMMC_CMD_CCS_EXP       _BIT(23)
#define SDMMC_CMD_CEATA_RD      _BIT(22)
#define SDMMC_CMD_UPD_CLK       _BIT(21)
#define SDMMC_CMD_INIT          _BIT(15)
#define SDMMC_CMD_STOP          _BIT(14)
#define SDMMC_CMD_PRV_DAT_WAIT  _BIT(13)
#define SDMMC_CMD_SEND_STOP     _BIT(12)
#define SDMMC_CMD_STRM_MODE     _BIT(11)
#define SDMMC_CMD_DAT_WR        _BIT(10)
#define SDMMC_CMD_DAT_EXP       _BIT(9)
#define SDMMC_CMD_RESP_CRC      _BIT(8)
#define SDMMC_CMD_RESP_LONG     _BIT(7)
#define SDMMC_CMD_RESP_EXP      _BIT(6)
#define SDMMC_CMD_INDX(n)       ((n) & 0x1F)
/* Status register defines */
#define SDMMC_GET_FCNT(x)      (((x)>>17) & 0x1FF)
#define SDMMC_FIFO_SZ           128

/***********************************************************************
 * SPI register definitions
 **********************************************************************/
#define SPI_PHYS              (0x15002000)
#define SPI_CONFIG_REG        __REG (SPI_PHYS + 0x00)
#define SPI_SLV_ENAB_REG      __REG (SPI_PHYS + 0x04)
#define SPI_TXF_FLUSH_REG     __REG (SPI_PHYS + 0x08)
#define SPI_FIFO_DATA_REG     __REG (SPI_PHYS + 0x0C)
#define SPI_NHP_POP_REG       __REG (SPI_PHYS + 0x10)
#define SPI_NHP_MODE_REG      __REG (SPI_PHYS + 0x14)
#define SPI_DMA_SET_REG       __REG (SPI_PHYS + 0x18)
#define SPI_STS_REG           __REG (SPI_PHYS + 0x1C)
#define SPI_HWINFO_REG        __REG (SPI_PHYS + 0x20)
#define SPI_SLV_SET1_REG(slv) __REG (SPI_PHYS + 0x24 + (8 * slv))
#define SPI_SLV_SET2_REG(slv) __REG (SPI_PHYS + 0x28 + (8 * slv))
#define SPI_INT_TRSH_REG      __REG (SPI_PHYS + 0xFD4)
#define SPI_INT_CLRE_REG      __REG (SPI_PHYS + 0xFD8)
#define SPI_INT_SETE_REG      __REG (SPI_PHYS + 0xFDC)
#define SPI_INT_STS_REG       __REG (SPI_PHYS + 0xFE0)
#define SPI_INT_ENAB_REG      __REG (SPI_PHYS + 0xFE4)
#define SPI_INT_CLRS_REG      __REG (SPI_PHYS + 0xFE8)
#define SPI_INT_SETS_REG      __REG (SPI_PHYS + 0xFEC)
#define SPI_MOD_ID_REG        __REG (SPI_PHYS + 0xFFC)

/* SPI device contants */
#define SPI_FIFO_DEPTH  64 /* 64 words (16bit) deep */
#define SPI_NUM_SLAVES  3  /* number of slaves supported */
#define SPI_MAX_DIV2    254
#define SPI_MAX_DIVIDER 65024 /* = 254 * (255 + 1) */
#define SPI_MIN_DIVIDER 2

/* SPI Configuration register definitions (SPI_CONFIG_REG) */
#define SPI_CFG_INTER_DLY(n)      _SBF(16, ((n) & 0xFFFF))
#define SPI_CFG_INTER_DLY_GET(n)  (((n) >> 16) & 0xFFFF)
#define SPI_CFG_UPDATE_EN         _BIT(7)
#define SPI_CFG_SW_RESET          _BIT(6)
#define SPI_CFG_SLAVE_DISABLE     _BIT(4)
#define SPI_CFG_MULTI_SLAVE       _BIT(3)
#define SPI_CFG_LOOPBACK          _BIT(2)
#define SPI_CFG_SLAVE_MODE        _BIT(1)
#define SPI_CFG_ENABLE            _BIT(0)

/* SPI slave_enable register definitions (SPI_SLV_ENAB_REG) */
#define SPI_SLV_EN(n)             _SBF(((n) << 1), 0x1)
#define SPI_SLV_SUSPEND(n)        _SBF(((n) << 1), 0x3)

/* SPI tx_fifo_flush register definitions (SPI_TXF_FLUSH_REG) */
#define SPI_TXFF_FLUSH            _BIT(1)

/* SPI dma_settings register definitions (SPI_DMA_SET_REG) */
#define SPI_DMA_TX_EN             _BIT(1)
#define SPI_DMA_RX_EN             _BIT(0)

/* SPI status register definitions (SPI_STS_REG) */
#define SPI_ST_SMS_BUSY           _BIT(5)
#define SPI_ST_BUSY               _BIT(4)
#define SPI_ST_RX_FF              _BIT(3)
#define SPI_ST_RX_EMPTY           _BIT(2)
#define SPI_ST_TX_FF              _BIT(1)
#define SPI_ST_TX_EMPTY           _BIT(0)

/* SPI slv_setting registers definitions (SPI_SLV_SET1_REG) */
#define SPI_SLV1_INTER_TX_DLY(n)  _SBF(24, ((n) & 0xFF))
#define SPI_SLV1_NUM_WORDS(n)     _SBF(16, ((n) & 0xFF))
#define SPI_SLV1_CLK_PS(n)        _SBF(8, ((n) & 0xFF))
#define SPI_SLV1_CLK_PS_GET(n)    (((n) >> 8) & 0xFF)
#define SPI_SLV1_CLK_DIV1(n)      ((n) & 0xFF)
#define SPI_SLV1_CLK_DIV1_GET(n)  ((n) & 0xFF)

/* SPI slv_setting registers definitions (SPI_SLV_SET2_REG) */
#define SPI_SLV2_PPCS_DLY(n)      _SBF(9, ((n) & 0xFF))
#define SPI_SLV2_CS_HIGH          _BIT(8)
#define SPI_SLV2_SSI_MODE         _BIT(7)
#define SPI_SLV2_SPO              _BIT(6)
#define SPI_SLV2_SPH              _BIT(5)
#define SPI_SLV2_WD_SZ(n)         ((n) & 0x1F)

/* SPI int_threshold registers definitions (SPI_INT_TRSH_REG) */
#define SPI_INT_TSHLD_TX(n)       _SBF(8, ((n) & 0xFF))
#define SPI_INT_TSHLD_RX(n)       ((n) & 0xFF)

/* SPI intterrupt registers definitions ( SPI_INT_xxx) */
#define SPI_SMS_INT               _BIT(4)
#define SPI_TX_INT                _BIT(3)
#define SPI_RX_INT                _BIT(2)
#define SPI_TO_INT                _BIT(1)
#define SPI_OVR_INT               _BIT(0)
#define SPI_ALL_INTS              (SPI_SMS_INT | SPI_TX_INT | SPI_RX_INT | SPI_TO_INT | SPI_OVR_INT)


/***********************************************************************
 * I2C register definitions
 **********************************************************************/
#define I2C0_PHYS            (0x1300A000)
#define I2C1_PHYS            (0x1300A400)
#define I2C_REG_RX(a)	      __REG ((a) + 0x000)	/* Rx FIFO reg (RO) */
#define I2C_REG_TX(a)	      __REG ((a) + 0x000)		/* Tx FIFO reg (WO) */
#define I2C_REG_STS(a)	      __REG ((a) + 0x04)	/* Status reg (RO) */
#define I2C_REG_CTL(a)	      __REG ((a) + 0x08)	/* Ctl reg */
#define I2C_REG_CKH(a)	      __REG ((a) + 0x0C)	/* Clock divider high */
#define I2C_REG_CKL(a)	      __REG ((a) + 0x10)	/* Clock divider low */
#define I2C_REG_ADR(a)	      __REG ((a) + 0x14)	/* I2C address */
#define I2C_REG_RFL(a)	      __REG ((a) + 0x18)	/* Rx FIFO level (RO) */
#define I2C_REG_TFL(a)	      __REG ((a) + 0x1c)	/* Tx FIFO level (RO) */
#define I2C_REG_RXB(a)	      __REG ((a) + 0x20)	/* Num of bytes Rx-ed (RO) */
#define I2C_REG_TXB(a)	      __REG ((a) + 0x24)	/* Num of bytes Tx-ed (RO) */
#define I2C_REG_TXS(a)	      __REG ((a) + 0x28)	/* Tx slave FIFO (RO) */
#define I2C_REG_STFL(a)	      __REG ((a) + 0x2c)	/* Tx slave FIFO level (RO) */
/* I2C Tx FIFO register definitions*/
#define I2C_TXFF_STOP_CND   _BIT(9)
#define I2C_TXFF_START_CND  _BIT(8)
#define I2C_TXFF_DATA_MSK   0xFF

/* I2C status register definitions*/
#define I2C_STS_TFES        _BIT(13)
#define I2C_STS_TFFS        _BIT(12)
#define I2C_STS_TFE         _BIT(11)
#define I2C_STS_TFF         _BIT(10)
#define I2C_STS_RFE         _BIT(9)
#define I2C_STS_RFF         _BIT(8)
#define I2C_STS_SDA         _BIT(7)
#define I2C_STS_SCL         _BIT(6)
#define I2C_STS_ACTIVE      _BIT(5)
#define I2C_STS_DRSI        _BIT(4)
#define I2C_STS_DRMI        _BIT(3)
#define I2C_STS_NAI         _BIT(2)
#define I2C_STS_AFI         _BIT(1)
#define I2C_STS_TDI         _BIT(0)

/* I2C control register definitions*/
#define I2C_CTL_TFFSIE       _BIT(10)
// #define I2C_CTL_SEVEN        _BIT(9) /* Seven-bit slave address */
#define I2C_CTL_SOFT_RESET   _BIT(8)
#define I2C_CTL_TFFIE        _BIT(7)
#define I2C_CTL_DAIE         _BIT(6)
#define I2C_CTL_RFFIE        _BIT(5)
#define I2C_CTL_DRSIE        _BIT(4)
#define I2C_CTL_DRMIE        _BIT(3)
#define I2C_CTL_NAIE         _BIT(2)
#define I2C_CTL_AFIE         _BIT(1)
#define I2C_CTL_TDIE         _BIT(0)

/***********************************************************************
* LPC315x Analog die I2C constants
**********************************************************************/
#define AD_I2C_WRITE_ADR    (_SBF(1, 0xC) | I2C_TXFF_START_CND)
#define AD_I2C_READ_ADR     (_SBF(1, 0xC) | 0x1 | I2C_TXFF_START_CND)

/* Analog die I2C Register offsets*/
#define AD_REG_OTGDCLIC_RW                            0x0000
#define AD_REG_DCDCLIC_RO                             0x0001
#define AD_REG_CGU_RW                                 0x0002
#define AD_REG_AIN_0_RW                               0x0010
#define AD_REG_AIN_1_RW                               0x0011
#define AD_REG_AOUT_RW                                0x0012
#define AD_REG_DEC_RW                                 0x0013
#define AD_REG_INT_0_RW                               0x0014
#define AD_REG_INT_1_RW                               0x0015
#define AD_REG_DAIOMUX_RW                             0x0016
#define AD_REG_AOUTDECINT_RO                          0x0017
#define AD_REG_RTC_TIME                               0x0020
#define AD_REG_RTC_ALARM_TIME                         0x0021
#define AD_REG_RTC_STATUS                             0x0022
#define AD_REG_RTC_SET_ENA_STAT                       0x0023
#define AD_REG_RTC_CLR_ENA_STAT                       0x0024
#define AD_REG_MOD_ID                                 0x03FF

/* 3v3 IO voltage rail adjustments */
#define DCDC1_3_20                   0
#define DCDC1_3_09                   1
#define DCDC1_2_97                   2
#define DCDC1_2_86                   3
#define DCDC1_2_74                   4
#define DCDC1_2_63                   5
#define DCDC1_2_51                   6
#define DCDC1_2_40                   7
/* 1v2 core voltage rail adjustments */
#define DCDC2_1_40                   0
#define DCDC2_1_33                   1
#define DCDC2_1_26                   2
#define DCDC2_1_19                   3
#define DCDC2_1_11                   4
#define DCDC2_1_04                   5
#define DCDC2_0_97                   6
#define DCDC2_0_90                   7
/* 1v4 io voltage rail adjustments */
#define PSU_VOUT3_1_40               0
#define PSU_VOUT3_1_80               _BIT(16)

/***********************************************************************
 * Memory definitions
 **********************************************************************/
#define EXT_SDRAM_PHYS		0x30000000
#define EXT_SRAM0_PHYS		0x20000000
#define EXT_SRAM1_PHYS		0x20020000
#define ISRAM0_PHYS             0x11028000
#define ISRAM0_LENGTH	        0x00018000
#define ISRAM1_PHYS             0x11040000
#define ISRAM1_LENGTH           0x00018000

/***********************************************************************
 * XTAL clock definitions
 **********************************************************************/
#define XTAL_CLOCK        CONFIG_OSC_FREQ
#define FFAST_CLOCK       XTAL_CLOCK

/* include CGU header */
#include "lpc313x_cgu.h"
#include "event_router.h"


#endif  /* __LPC313X_H__ */
